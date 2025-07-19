/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "Game/Camera.h"
#include "Game/GameHelper.h"
#include "Map/Ground.h"
#include "MissileProjectile.h"
#include "Rendering/GlobalRendering.h"
#include "Rendering/Env/Particles/Classes/SmokeTrailProjectile.h"
#include "Rendering/Env/Particles/ProjectileDrawer.h"
#include "Rendering/GL/RenderBuffers.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "Sim/Misc/GeometricObjects.h"
#include "Sim/Misc/GlobalSynced.h"
#include "Sim/Projectiles/ExplosionGenerator.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Projectiles/ProjectileMemPool.h"
#include "Sim/Units/Unit.h"
#include "Sim/Weapons/WeaponDefHandler.h"
#include "System/Matrix44f.h"
#include "System/SpringMath.h"

#include "System/Misc/TracyDefs.h"

CR_BIND_DERIVED(CMissileProjectile, CWeaponProjectile, )

CR_REG_METADATA(CMissileProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(ignoreError),
	CR_MEMBER(maxSpeed),
	// CR_MEMBER(ttl),
	CR_MEMBER(age),
	CR_MEMBER(oldSmoke),
	CR_MEMBER(oldDir),
	CR_MEMBER(numParts),
	CR_MEMBER(isWobbling),
	CR_MEMBER(wobbleDir),
	CR_MEMBER(wobbleTime),
	CR_MEMBER(wobbleDif),
	CR_MEMBER(danceMove),
	CR_MEMBER(danceCenter),
	CR_MEMBER(danceTime),
	CR_MEMBER(isDancing),
	CR_MEMBER(extraHeight),
	CR_MEMBER(extraHeightDecay),
	CR_MEMBER(extraHeightTime),
	CR_IGNORED(smokeTrail)
))


CMissileProjectile::CMissileProjectile(const ProjectileParams& params): CWeaponProjectile(params)
	, ignoreError(false)
	, maxSpeed(0.0f)
	, extraHeight(0.0f)
	, extraHeightDecay(0.0f)

	, age(0)
	, numParts(0)
	, extraHeightTime(0)

	, isDancing(false)
	, isWobbling(false)

	, danceTime(1)
	, wobbleTime(1)

	, oldSmoke(pos)
	, oldDir(dir)
	, smokeTrail(nullptr)
{
	RECOIL_DETAILED_TRACY_ZONE;
	projectileType = WEAPON_MISSILE_PROJECTILE;

	mygravity = mix(mygravity, params.gravity, params.gravity != 0.0f);

	if (model != nullptr)
		SetRadiusAndHeight(model);

	if (weaponDef != nullptr) {
		maxSpeed = weaponDef->projectilespeed;
		isDancing = (weaponDef->dance > 0);
		isWobbling = (weaponDef->wobble > 0);

		if (weaponDef->trajectoryHeight > 0.0f) {
			const float dist = pos.distance(targetPos);

			assert(maxSpeed > 0.0f);
			assert((std::max(dist, maxSpeed) / maxSpeed) >= 1.0f);

			extraHeight = (dist * weaponDef->trajectoryHeight);
			extraHeightTime = int(std::max(dist, maxSpeed) / maxSpeed);
			extraHeightDecay = extraHeight / extraHeightTime;
		}
	}

	drawRadius = radius + maxSpeed * 8.0f;
	castShadow = weaponDef ? weaponDef->visuals.castShadow : true;

	CUnit* u = dynamic_cast<CUnit*>(target);
	if (u == nullptr)
		return;

	u->IncomingMissile(this);
}

void CMissileProjectile::Collision()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (weaponDef->visuals.smokeTrail)
		projMemPool.alloc<CSmokeTrailProjectile>(owner(), pos, oldSmoke, dir, oldDir, false, true, GetSmokeSize(), GetSmokeTime(), GetSmokePeriod(), GetSmokeColor(), weaponDef->visuals.texture2, weaponDef->visuals.smokeTrailCastShadow);

	CWeaponProjectile::Collision();
	oldSmoke = pos;
}

void CMissileProjectile::Collision(CUnit* unit)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (weaponDef->visuals.smokeTrail)
		projMemPool.alloc<CSmokeTrailProjectile>(owner(), pos, oldSmoke, dir, oldDir, false, true, GetSmokeSize(), GetSmokeTime(), GetSmokePeriod(), GetSmokeColor(), weaponDef->visuals.texture2, weaponDef->visuals.smokeTrailCastShadow);

	CWeaponProjectile::Collision(unit);
	oldSmoke = pos;
}

void CMissileProjectile::Collision(CFeature* feature)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (weaponDef->visuals.smokeTrail)
		projMemPool.alloc<CSmokeTrailProjectile>(owner(), pos, oldSmoke, dir, oldDir, false, true, GetSmokeSize(), GetSmokeTime(), GetSmokePeriod(), GetSmokeColor(), weaponDef->visuals.texture2, weaponDef->visuals.smokeTrailCastShadow);

	CWeaponProjectile::Collision(feature);
	oldSmoke = pos;
}

void CMissileProjectile::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	const CUnit* own = owner();

	if (--ttl > 0) {
		if (!luaMoveCtrl) {
			speed.w += (weaponDef->weaponacceleration * (speed.w < maxSpeed));

			// FIXME: should go before the targeting update?
			// const float3 orgTargPos = targetPos;
			// const float3 targetDir = (targetPos - pos).SafeNormalize();
			const float3& targetVel = UpdateTargeting();

			UpdateWobble();
			UpdateDance();

			const float3 orgTargPos = targetPos;
			const float3 targetDir = (targetPos - pos).SafeNormalize();
			const float targetDist = pos.distance(targetPos) + 0.1f;

			if (extraHeightTime > 0) {
				extraHeight -= extraHeightDecay;
				--extraHeightTime;

				targetPos.y += extraHeight;

				if (dir.y <= 0.0f) {
					// missile has reached apex, smoothly transition
					// to targetDir (can still overshoot when target
					// is too close or height difference too large)
					const float horDiff = (targetPos - pos).Length2D() + 0.01f;
					const float verDiff = (targetPos.y - pos.y) + 0.01f;
					const float dirDiff = math::fabs(targetDir.y - dir.y);
					const float ratio = math::fabs(verDiff / horDiff);

					// tilt missile up if
					// 1. missile is pointing below target
					// 2. AND missile height is below target
					// This compensates for high wobble zero turnrate missiles aiming at high elevations
					// Prevents these missiles from quickly turing directly downwards if wobble 
					// causes them to undershoot their elevated target 
					if (((targetDir.y - dir.y) > 0.0f) && ((targetPos.y - extraHeight - pos.y) > 0.0f)) {
						dir.y += (dirDiff * ratio);
					}
					else {
						dir.y -= (dirDiff * ratio);
					}

				} else {
					// missile is still ascending
					
					// tilt missile up if
					// 1. missile is pointing below target
					// 2. AND missile height is below target
					// This compensates for high wobble zero turnrate missiles aiming at high elevations
					// Lets these missiles continue ascending to an elevated target
					// even if wobble causes them to temporarily undershoot their elevated target 
					if ( ((targetDir.y - dir.y) > 0.0f) && ((targetPos.y - extraHeight - pos.y) > 0.0f) ) {
						dir.y += (extraHeightDecay / targetDist);
					}
					else {
						dir.y -= (extraHeightDecay / targetDist);
					}
				}
			}

			const float3 targetLeadVec = targetVel * (targetDist / maxSpeed) * 0.7f;
			const float3 targetLeadDir = (targetPos + targetLeadVec - pos).Normalize();

			float3 targetDirDif = targetLeadDir - dir;

			if (targetDirDif.SqLength() < Square(weaponDef->turnrate)) {
				dir = targetLeadDir;
			} else {
				targetDirDif = (targetDirDif - (dir * (targetDirDif.dot(dir)))).SafeNormalize();
				dir = (dir + (targetDirDif * weaponDef->turnrate)).SafeNormalize();
			}

			targetPos = orgTargPos;

			// dir and speed.w have changed, keep speed-vector in sync
			SetDirectionAndSpeed(dir, speed.w);
		}

		explGenHandler.GenExplosion(
			cegID,
			pos,
			dir,
			ttl,
			damages->damageAreaOfEffect,
			0.0f,
			owner(),
			ExplosionHitObject()
		);
	} else {
		if (weaponDef->selfExplode) {
			Collision();
		} else {
			// only when TTL <= 0 do we (missiles)
			// get influenced by gravity and drag
			if (!luaMoveCtrl)
				SetVelocityAndSpeed((speed * 0.98f) + (UpVector * mygravity));
		}
	}

	if (!luaMoveCtrl)
		SetPosition(pos + speed);

	age++;
	numParts++;

	if (weaponDef->visuals.smokeTrail) {
		if (smokeTrail) {
			smokeTrail->UpdateEndPos(pos, dir);
			oldSmoke = pos;
			oldDir = dir;
		}

		if ((age % weaponDef->visuals.smokePeriod) == 0) {
			smokeTrail = projMemPool.alloc<CSmokeTrailProjectile>(
				own,
				pos, oldSmoke,
				dir, oldDir,
				age == weaponDef->visuals.smokePeriod,
				false,
				GetSmokeSize(),
				GetSmokeTime(),
				GetSmokePeriod(),
				GetSmokeColor(),
				weaponDef->visuals.texture2,
				weaponDef->visuals.smokeTrailCastShadow
			);

			numParts = 0;
			useAirLos = smokeTrail->useAirLos;
		}
	}

	UpdateInterception();
	UpdateGroundBounce();
}

float3 CMissileProjectile::UpdateTargeting() {
	RECOIL_DETAILED_TRACY_ZONE;
	float3 targetVel;

	if (!weaponDef->tracks || target == nullptr)
		return targetVel;

	const CSolidObject* so = dynamic_cast<const CSolidObject*>(target);
	const CUnit* u = nullptr;
	const CWeaponProjectile* po = nullptr;

	if (so != nullptr) {
		// track aim- or error-position for SolidObject's
		targetPos = so->aimPos;
		targetVel = so->speed;

		if (allyteamID != -1 && !ignoreError) {
			if ((u = dynamic_cast<const CUnit*>(so)) != nullptr)
				targetPos = u->GetErrorPos(allyteamID, true);
		}

		targetPos.y = std::max(targetPos.y, targetPos.y * weaponDef->waterweapon);
		return targetVel;
	}

	// track regular target base-position
	targetPos = target->pos;

	if ((po = dynamic_cast<const CWeaponProjectile*>(target)) == nullptr)
		return targetVel;

	return po->speed;
}

void CMissileProjectile::UpdateWobble() {
	RECOIL_DETAILED_TRACY_ZONE;
	if (!isWobbling)
		return;

	if ((--wobbleTime) <= 0) {
		wobbleDif = (gsRNG.NextVector() - wobbleDir) * (1.0f / 16);
		wobbleTime = 16;
	}

	float wobbleFact = weaponDef->wobble;

	if (owner() != nullptr)
		wobbleFact *= CUnit::ExperienceScale(owner()->limExperience, weaponDef->ownerExpAccWeight);

	wobbleDir += wobbleDif;
	dir = (dir + wobbleDir * wobbleFact).Normalize();
}

void CMissileProjectile::UpdateDance() {
	RECOIL_DETAILED_TRACY_ZONE;
	if (!isDancing)
		return;

	if ((--danceTime) <= 0) {
		danceMove = gsRNG.NextVector() * weaponDef->dance - danceCenter;
		danceCenter += danceMove;
		danceTime = 8;
	}

	SetPosition(pos + danceMove);
}

inline float CMissileProjectile::GetSmokeSize() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	return weaponDef->visuals.smokeSize;
}

inline float CMissileProjectile::GetSmokeColor() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	return weaponDef->visuals.smokeColor;
}

inline int CMissileProjectile::GetSmokeTime() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	return weaponDef->visuals.smokeTime;
}

inline int CMissileProjectile::GetSmokePeriod() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	return weaponDef->visuals.smokePeriod;
}

void CMissileProjectile::UpdateGroundBounce() {
	RECOIL_DETAILED_TRACY_ZONE;
	if (luaMoveCtrl)
		return;

	const float4 oldSpeed = speed;

	CWeaponProjectile::UpdateGroundBounce();

	if (oldSpeed == speed)
		return;

	SetVelocityAndSpeed(speed);
}



void CMissileProjectile::Draw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!validTextures[1])
		return;

	UpdateAnimParams();

	// rocket flare
	const SColor lightYellow(255, 210, 180, 1);
	const float fsize = radius * 0.4f;

	const auto* WT1 = weaponDef->visuals.texture1;

	AddEffectsQuad<1>(
		WT1->pageNum,
		{ drawPos - camera->GetRight() * fsize - camera->GetUp() * fsize, WT1->xstart, WT1->ystart, lightYellow },
		{ drawPos + camera->GetRight() * fsize - camera->GetUp() * fsize, WT1->xend,   WT1->ystart, lightYellow },
		{ drawPos + camera->GetRight() * fsize + camera->GetUp() * fsize, WT1->xend,   WT1->yend,   lightYellow },
		{ drawPos - camera->GetRight() * fsize + camera->GetUp() * fsize, WT1->xstart, WT1->yend,   lightYellow }
	);
}

int CMissileProjectile::ShieldRepulse(const float3& shieldPos, float shieldForce, float shieldMaxSpeed)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (luaMoveCtrl)
		return 0;

	if (ttl > 0) {
		const float3 sdir = (pos - shieldPos).SafeNormalize();
		// steer away twice as fast as we can steer toward target
		float3 dif2 = sdir - dir;

		const float tracking = std::max(shieldForce * 0.05f, weaponDef->turnrate * 2);

		if (dif2.SqLength() < Square(tracking)) {
			dir = sdir;
		} else {
			dif2 = (dif2 - (dir * (dif2.dot(dir)))).SafeNormalize();
			dir = (dir + (dif2 * tracking)).SafeNormalize();
		}

		return 2;
	}

	return 0;
}

int CMissileProjectile::GetProjectilesCount() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	return 1 * validTextures[0];
}
