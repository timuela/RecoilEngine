/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "Game/GameHelper.h"
#include "Map/Ground.h"
#include "Rendering/Colors.h"
#include "Rendering/GL/RenderBuffers.h"
#include "Sim/Features/Feature.h"
#include "Sim/Misc/DamageArray.h"
#include "Sim/Misc/GlobalSynced.h"
#include "Sim/Misc/InterceptHandler.h"
#include "Sim/Misc/QuadField.h"
#include "Sim/Misc/TeamHandler.h"
#include "Sim/Projectiles/ExplosionGenerator.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Projectiles/ProjectileParams.h"
#include "Sim/Projectiles/WeaponProjectiles/WeaponProjectile.h"
#include "Sim/Units/Unit.h"
#include "Sim/Units/UnitHandler.h"
#include "Sim/Weapons/Weapon.h"
#include "Sim/Weapons/WeaponDefHandler.h"
#include "System/Matrix44f.h"
#include "System/SpringMath.h"
#include "System/creg/DefTypes.h"

#include "System/Misc/TracyDefs.h"


CR_BIND_DERIVED_INTERFACE(CWeaponProjectile, CProjectile)

CR_REG_METADATA(CWeaponProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(damages),
	CR_MEMBER(targeted),
	CR_MEMBER(bounced),
	CR_MEMBER(weaponDef),
	CR_MEMBER(target),
	CR_MEMBER(targetPos),
	CR_MEMBER(startPos),
	CR_MEMBER(bounceHitPos),
	CR_MEMBER(bounceParams),
	CR_MEMBER(ttl),
	CR_MEMBER(bounces),
	CR_MEMBER(weaponNum),

	CR_POSTLOAD(PostLoad)
))


CWeaponProjectile::CWeaponProjectile(const ProjectileParams& params)
	: CProjectile(params.pos, params.speed, params.owner, true, true, false, false)

	, damages(nullptr)
	, weaponDef(params.weaponDef)
	, target(params.target)

	, ttl(params.ttl)
	, bounces(0)

	, targeted(false)
	, bounced(false)

	, startPos(params.pos)
	, targetPos(params.end)
{
	projectileType = WEAPON_BASE_PROJECTILE;

	assert(weaponDef != nullptr);

	animParams1 = weaponDef->visuals.animParams[0];
	animParams2 = weaponDef->visuals.animParams[1];
	animParams3 = weaponDef->visuals.animParams[2];
	animParams4 = weaponDef->visuals.animParams[3];

	if (weaponDef->IsHitScanWeapon()) {
		hitscan = true;
		// the else-case (default) is handled in CProjectile::Init
		//
		// ray projectiles must all set this to false because their collision
		// detection is handled by the weapons firing them, ProjectileHandler
		// will skip any tests for these
		checkCol = false;
		// type has not yet been set by derived ctor's at this point
		// useAirLos = (projectileType != WEAPON_LIGHTNING_PROJECTILE);
		useAirLos = true;

		// NOTE:
		//   {BeamLaser, Lightning}Projectile's do NOT actually move (their
		//   speed is never added to pos) and never alter their speed either
		//   they additionally override our ::Update (so CProjectile::Update
		//   is also never called) which means assigning speed a non-zerovec
		//   value should have no side-effects
		SetPosition(startPos);
		SetVelocityAndSpeed(targetPos - startPos);

		// ProjectileDrawer vis-culls by pos == startPos, but we
		// want to see the beam even if camera is near targetPos
		// --> use full distance for drawRadius
		SetRadiusAndHeight((targetPos - startPos).Length(), 0.0f);
	}

	{
		myrange = weaponDef->range;

		collisionFlags = weaponDef->collisionFlags;
		weaponNum = params.weaponNum;

		alwaysVisible = weaponDef->visuals.alwaysVisible;
		ignoreWater = weaponDef->waterweapon;

		validTextures = {
			false,
			IsValidTexture(weaponDef->visuals.texture1),
			IsValidTexture(weaponDef->visuals.texture2),
			IsValidTexture(weaponDef->visuals.texture3),
			IsValidTexture(weaponDef->visuals.texture4)
		};
		//whether to skip the draw call
		validTextures[0] = validTextures[1] || validTextures[2] || validTextures[3] || validTextures[4];
	}

	{
		CSolidObject* so = nullptr;
		CWeaponProjectile* po = nullptr;

		if ((so = dynamic_cast<CSolidObject*>(target)) != nullptr)
			AddDeathDependence(so, DEPENDENCE_WEAPONTARGET);

		if ((po = dynamic_cast<CWeaponProjectile*>(target)) != nullptr) {
			po->SetBeingIntercepted(po->IsBeingIntercepted() || weaponDef->interceptSolo);
			AddDeathDependence(po, DEPENDENCE_INTERCEPTTARGET);
		}
	}

	if (params.model != nullptr) {
		model = params.model;
	} else {
		model = weaponDef->LoadModel();
	}

	if (params.owner == nullptr) {
		// the else-case (default) is handled in CProjectile::Init
		ownerID = params.ownerID;
		teamID = params.teamID;
		allyteamID = teamHandler.IsValidTeam(teamID)? teamHandler.AllyTeam(teamID): -1;
	}

	if (ownerID != -1u && weaponNum != -1u) {
		const CUnit* owner = unitHandler.GetUnit(ownerID);
		const CWeapon* weapon = (owner != nullptr && weaponNum < owner->weapons.size())? owner->weapons[weaponNum]: nullptr;

		if (weapon != nullptr) {
			damages = DynDamageArray::IncRef(weapon->damages);

			myrange = weapon->range;

			// inherit from weapon instance if possible since Lua can change the flags at runtime
			collisionFlags = weapon->collisionFlags;
		}
	}

	if (damages == nullptr)
		damages = DynDamageArray::IncRef(&weaponDef->damages);

	if (params.cegID != -1u) {
		cegID = params.cegID;
	} else {
		cegID = weaponDef->ptrailExplosionGeneratorID;
	}

	// must happen after setting position and velocity
	projectileHandler.AddProjectile(this);
	quadField.AddProjectile(this);

	ASSERT_SYNCED(id);

	if (!weaponDef->targetable)
		return;

	interceptHandler.AddInterceptTarget(this, targetPos);
}


CWeaponProjectile::~CWeaponProjectile()
{
	RECOIL_DETAILED_TRACY_ZONE;
	DynDamageArray::DecRef(damages);
}


void CWeaponProjectile::Explode(
	CUnit* hitUnit,
	CFeature* hitFeature,
	CWeapon* hitWeapon,
	float3 impactPos,
	float3 impactDir
) {
	const DamageArray& damageArray = damages->GetDynamicDamages(startPos, impactPos);
	const CExplosionParams params = {
		.pos                  = impactPos,
		.dir                  = impactDir.SafeNormalize(),
		.damages              = damageArray,
		.weaponDef            = weaponDef,
		.owner                = owner(),
		.hitObject            = ExplosionHitObject(hitUnit, hitFeature, hitWeapon),
		.craterAreaOfEffect   = damages->craterAreaOfEffect,
		.damageAreaOfEffect   = damages->damageAreaOfEffect,
		.edgeEffectiveness    = damages->edgeEffectiveness,
		.explosionSpeed       = damages->explosionSpeed,
		.gfxMod               = weaponDef->noExplode ? 0.3f : 1.0f,
		.maxGroundDeformation = 0.0f,
		.impactOnly           = weaponDef->impactOnly,
		.ignoreOwner          = weaponDef->noExplode || weaponDef->noSelfDamage,
		.damageGround         = true,
		.projectileID         = static_cast<uint32_t>(id)
	};

	helper->Explosion(params);

	if (weaponDef->noExplode && !TraveledRange())
		return;

	// remove ourselves from the simulation (otherwise
	// keep traveling and generating more explosions)
	CProjectile::Collision();
}

void CWeaponProjectile::Collision()
{
	RECOIL_DETAILED_TRACY_ZONE;

	float3 impactPos = pos;
	float3 impactDir = speed;
	if (hitscan) {
		impactPos = targetPos;
		impactDir = targetPos - startPos;
	};

	Explode(nullptr, nullptr, nullptr, impactPos, impactDir);
}

void CWeaponProjectile::Collision(CFeature* feature)
{
	RECOIL_DETAILED_TRACY_ZONE;

	assert(feature);
	float3 impactPos = pos;
	float3 impactDir = speed;

	if (hitscan) {
		impactPos = feature->pos;
		impactDir = targetPos - startPos;
	}

	if (gsRNG.NextFloat() < weaponDef->fireStarter)
		feature->StartFire();

	Explode(nullptr, feature, nullptr, impactPos, impactDir);
}

void CWeaponProjectile::Collision(CUnit* unit)
{
	RECOIL_DETAILED_TRACY_ZONE;

	assert(unit);
	float3 impactPos = pos;
	float3 impactDir = speed;

	if (hitscan) {
		impactPos = unit->pos;
		impactDir = targetPos - startPos;
	}

	Explode(unit, nullptr, nullptr, impactPos, impactDir);
}

void CWeaponProjectile::Collision(CWeapon* weapon)
{
	RECOIL_DETAILED_TRACY_ZONE;

	assert(weapon);
	float3 impactPos = pos;
	float3 impactDir = speed;

	if (hitscan) {
		impactPos = weapon->owner ? weapon->owner->pos : pos;
		impactDir = targetPos - startPos;
	}

	Explode(nullptr, nullptr, weapon, impactPos, impactDir);
}

void CWeaponProjectile::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	CProjectile::Update();
	UpdateGroundBounce();
	UpdateInterception();
}

void CWeaponProjectile::UpdateInterception()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (target == nullptr)
		return;

	CWeaponProjectile* po = dynamic_cast<CWeaponProjectile*>(target);

	if (po == nullptr)
		return;

	// we are the interceptor, point us toward the interceptee pos each frame
	// (normally not needed, subclasses handle it directly in their Update()'s
	// *until* our owner dies)
	if (owner() == nullptr)
		targetPos = po->pos + po->speed;

	static auto GetWeapon = [](const CWeaponProjectile* po) -> CWeapon* {
		const auto* poOwner = po->owner();
		if (!poOwner)
			return nullptr;

		if (po->weaponNum < 0 || po->weaponNum >= poOwner->weapons.size())
			return nullptr;

		return poOwner->weapons[po->weaponNum];
	};

	if (hitscan) {
		if (ClosestPointOnLine(startPos, targetPos, po->pos).SqDistance(po->pos) < Square(weaponDef->collisionSize)) {
			if (auto* weapon = GetWeapon(po); weapon)
				po->Collision(weapon);
			else
				po->Collision();
			Collision();
		}
	} else {
		// FIXME: if (pos.SqDistance(po->pos) < Square(weaponDef->collisionSize)) {
		if (pos.SqDistance(po->pos) < Square(damages->damageAreaOfEffect)) {
			if (auto* weapon = GetWeapon(po); weapon)
				po->Collision(weapon);
			else
				po->Collision();
			Collision();
		}
	}
}


void CWeaponProjectile::UpdateGroundBounce()
{
	RECOIL_DETAILED_TRACY_ZONE;
	#if 1
	// projectile is not allowed to bounce on either surface
	if (!weaponDef->groundBounce && !weaponDef->waterBounce)
		return;

	// maximum number of bounce already reached?
	if (weaponDef->numBounce != -1 // infinite
	&&  bounces >= weaponDef->numBounce)
		return;

	if (luaMoveCtrl)
		return;
	if (ttl <= 0) {
		// //drop scheduled bounce, so HasScheduledBounce() check inside
		// CProjectileHandler::CheckGroundCollisions(ProjectileContainer& pc) is false
		bounced = false;
		return;
	}
	#endif

	if (!bounced) {
		// projectile has already been updated by ProjectileHandler
		// therefore if we detect a ground or water intersection here, it
		// actually happens after this frame and should schedule a bounce
		// for the next
		const float groundDist = (weaponDef->groundBounce)? CGround::LineGroundCol(pos, pos + speed): -1.0f;
		const float  waterDist = (weaponDef->waterBounce)? CGround::LinePlaneCol(pos, dir, speed.w, CGround::GetWaterLevel(pos.x, pos.z)): -1.0f;
		const float bounceDist = std::min(mix(groundDist, speed.w * 10000.0f, groundDist < 0.0f), mix(waterDist, speed.w * 10000.0f, waterDist < 0.0f));

		if ((bounced = (bounceDist >= 0.0f && bounceDist <= speed.w))) {
			bounceHitPos = pos + dir * bounceDist;
			bounceParams = {bounceDist, speed.w, std::min(1.0f, bounceDist / speed.w)};
		}

		return;
	}

	{
		// FIXME: ignoreWater?
		const float3 bounceNormal = mix(CGround::GetNormal(bounceHitPos.x, bounceHitPos.z), UpVector, (bounceHitPos.y <= 0.0f));

		const float moveDistance = bounceParams.y * (1.0f - bounceParams.z);
		const float  normalSpeed = math::fabs(speed.dot(bounceNormal));

		CWorldObject::SetVelocity(speed - (speed + bounceNormal * normalSpeed) * (1 - weaponDef->bounceSlip   ));
		CWorldObject::SetVelocity(         speed + bounceNormal * normalSpeed  * (1 + weaponDef->bounceRebound));
		SetVelocityAndSpeed(speed);
		// the above changes speed.w so this would be marginally incorrect
		// SetPosition(bounceHitPos + speed * (1.0f - bounceParams.z));
		SetPosition(bounceHitPos + dir * moveDistance);

		explGenHandler.GenExplosion(
			weaponDef->bounceExplosionGeneratorID,
			bounceHitPos,
			bounceNormal,
			speed.w,
			1.0f,
			1.0f,
			owner(),
			ExplosionHitObject()
		);

		bounced = false;
	}

	++bounces;
}


void CWeaponProjectile::DrawOnMinimap() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	AddMiniMapVertices({ pos        , color4::yellow }, { pos + speed, color4::yellow });
}


bool CWeaponProjectile::CanBeInterceptedBy(const WeaponDef* wd) const
{
	RECOIL_DETAILED_TRACY_ZONE;
	return ((weaponDef->targetable & wd->interceptor) != 0);
}


void CWeaponProjectile::DependentDied(CObject* o)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (o != target)
		return;

	target = nullptr;
}


void CWeaponProjectile::PostLoad()
{
	RECOIL_DETAILED_TRACY_ZONE;
	assert(weaponDef != nullptr);
	model = weaponDef->LoadModel();
}
