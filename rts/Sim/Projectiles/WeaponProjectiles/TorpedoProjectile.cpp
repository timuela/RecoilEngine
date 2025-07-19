/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "TorpedoProjectile.h"
#include "Game/Camera.h"
#include "Game/GameHelper.h"
#include "Game/GlobalUnsynced.h"
#include "Map/Ground.h"
#include "Rendering/Env/Particles/ProjectileDrawer.h"
#include "Rendering/GL/RenderBuffers.h"
#include "Rendering/Env/Particles/Classes/BubbleProjectile.h"
#include "Rendering/Env/Particles/Classes/SmokeTrailProjectile.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "Sim/Misc/GlobalSynced.h"
#include "Sim/Projectiles/ExplosionGenerator.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Projectiles/ProjectileMemPool.h"
#include "Sim/Units/Unit.h"
#include "Sim/Weapons/WeaponDef.h"
#include "System/SpringMath.h"

#include "System/Misc/TracyDefs.h"

CR_BIND_DERIVED(CTorpedoProjectile, CWeaponProjectile, )

CR_REG_METADATA(CTorpedoProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(ignoreError),
	CR_MEMBER(tracking),
	CR_MEMBER(maxSpeed),
	CR_MEMBER(nextBubble),
	CR_MEMBER(texx),
	CR_MEMBER(texy)
))


CTorpedoProjectile::CTorpedoProjectile(const ProjectileParams& params): CWeaponProjectile(params)
	, ignoreError(false)

	, tracking(0.0f)
	, maxSpeed(0.0f)

	, nextBubble(4)
	, texx(0.0f)
	, texy(0.0f)
{
	projectileType = WEAPON_TORPEDO_PROJECTILE;

	tracking = params.tracking;

	if (weaponDef != nullptr)
		maxSpeed = weaponDef->projectilespeed;

	drawRadius = maxSpeed * 8.0f;

	texx = projectileDrawer->torpedotex->xstart - (projectileDrawer->torpedotex->xend - projectileDrawer->torpedotex->xstart) * 0.5f;
	texy = projectileDrawer->torpedotex->ystart - (projectileDrawer->torpedotex->yend - projectileDrawer->torpedotex->ystart) * 0.5f;
}



float3 CTorpedoProjectile::UpdateTargetingPos()
{
	RECOIL_DETAILED_TRACY_ZONE;
	float3 targetVel;

	if (target != nullptr) {
		const CSolidObject* to = dynamic_cast<const CSolidObject*>(target);
		const CWeaponProjectile* tp = nullptr;
		const CUnit* tu = nullptr;

		if (to != nullptr) {
			targetPos = to->aimPos;
			targetVel = to->speed;

			if (allyteamID != -1 && !ignoreError) {
				if ((tu = dynamic_cast<const CUnit*>(to)) != nullptr)
					targetPos = tu->GetErrorPos(allyteamID, true);
			}
		} else {
			targetPos = target->pos;

			if ((tp = dynamic_cast<const CWeaponProjectile*>(target)) != nullptr)
				targetVel = tp->speed;

		}
	}

	// set tp.y to 0 if both are false, otherwise no change
	targetPos.y *= (weaponDef->submissile || targetPos.y <= 0.0f);
	return targetVel;
}

float3 CTorpedoProjectile::UpdateTargetingDir(const float3& targetObjVel)
{
	RECOIL_DETAILED_TRACY_ZONE;
	const float3 targetLeadVec = targetObjVel * (pos.distance(targetPos) / maxSpeed) * 0.7f;
	const float3 targetLeadDir = (targetPos + targetLeadVec - pos).Normalize();

	float3 targetDirDif = targetLeadDir - dir;

	if (targetDirDif.Length() < tracking) {
		dir = targetLeadDir;
	} else {
		// <tracking> is the projectile's turn-rate, zero if TorpedoLauncher is non-tracking
		targetDirDif = (targetDirDif - (dir * targetDirDif.dot(dir))).SafeNormalize();
		dir = (dir + (targetDirDif * tracking)).SafeNormalize();
	}

	return (dir * speed.w);
}

void CTorpedoProjectile::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	// tracking only works when we are underwater
	if (!weaponDef->submissile && pos.y > 0.0f) {
		if (!luaMoveCtrl) {
			// must update dir and speed.w here
			SetVelocityAndSpeed(speed + (UpVector * mygravity));
		}
	} else {
		if (--ttl > 0) {
			if (!luaMoveCtrl) {
				speed.w += (std::max(0.2f, tracking) * (speed.w < maxSpeed));

				const float3 targetObjVel = UpdateTargetingPos();
				const float3 targetHitVel = UpdateTargetingDir(targetObjVel);

				// do not need to update dir or speed.w here
				CWorldObject::SetVelocity(targetHitVel);
			}

			explGenHandler.GenExplosion(
				cegID,
				pos,
				speed,
				ttl,
				damages->damageAreaOfEffect,
				0.0f,
				owner(),
				ExplosionHitObject()
			);
		} else {
			if (!luaMoveCtrl) {
				// must update dir and speed.w here
				SetVelocityAndSpeed((speed * 0.98f) + (UpVector * mygravity));
			}
		}
	}

	if (!luaMoveCtrl)
		SetPosition(pos + speed);

	if (pos.y < -2.0f) {
		if ((--nextBubble) == 0) {
			nextBubble = 1 + (int) (gsRNG.NextFloat() * 1.5f);

			projMemPool.alloc<CBubbleProjectile>(
				owner(),
				pos + guRNG.NextVector(),
				guRNG.NextVector() * 0.1f + UpVector * 0.2f,
				guRNG.NextFloat() * GAME_SPEED + 40.0f,
				guRNG.NextFloat() * 2.0f + 1.0f,
				0.01f,
				guRNG.NextFloat() * 0.3f + 0.3f
			);
		}
	}

	UpdateGroundBounce();
	UpdateInterception();
}



void CTorpedoProjectile::Draw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	// do not draw if a 3D model has been defined for us
	if (model != nullptr)
		return;

	//UpdateAnimParams();

	float3 r = dir.cross(UpVector);

	if (r.SqLength() < 0.001f)
		r = RgtVector;

	r.Normalize();
	const float3 u = dir.cross(r);
	const float h = 12;
	const float w = 2;
	const SColor col(60, 60, 100, 255);

	const auto& pageNum = projectileDrawer->torpedotex->pageNum;

	AddEffectsQuad<0>(
		pageNum,
		{ drawPos + (r * w),             texx, texy, col },
		{ drawPos + (u * w),             texx, texy, col },
		{ drawPos + (u * w) + (dir * h), texx, texy, col },
		{ drawPos + (r * w) + (dir * h), texx, texy, col }
	);

	AddEffectsQuad<0>(
		pageNum,
		{ drawPos + (u * w),             texx, texy, col },
		{ drawPos - (r * w),             texx, texy, col },
		{ drawPos - (r * w) + (dir * h), texx, texy, col },
		{ drawPos + (u * w) + (dir * h), texx, texy, col }
	);

	AddEffectsQuad<0>(
		pageNum,
		{ drawPos - (r * w),             texx, texy, col },
		{ drawPos - (u * w),             texx, texy, col },
		{ drawPos - (u * w) + (dir * h), texx, texy, col },
		{ drawPos - (r * w) + (dir * h), texx, texy, col }
	);

	AddEffectsQuad<0>(
		pageNum,
		{ drawPos - (u * w),             texx, texy, col },
		{ drawPos + (r * w),             texx, texy, col },
		{ drawPos + (r * w) + (dir * h), texx, texy, col },
		{ drawPos - (u * w) + (dir * h), texx, texy, col }
	);

	AddEffectsQuad<0>(
		pageNum,
		{ drawPos + (r * w) + (dir * h), texx, texy, col },
		{ drawPos + (u * w) + (dir * h), texx, texy, col },
		{ drawPos + (dir * h * 1.2f),    texx, texy, col },
		{ drawPos + (dir * h * 1.2f),    texx, texy, col }
	);

	AddEffectsQuad<0>(
		pageNum,
		{ drawPos + (u * w) + (dir * h), texx, texy, col },
		{ drawPos - (r * w) + (dir * h), texx, texy, col },
		{ drawPos + (dir * h * 1.2f),    texx, texy, col },
		{ drawPos + (dir * h * 1.2f),    texx, texy, col }
	);

	AddEffectsQuad<0>(
		pageNum,
		{ drawPos - (r * w) + (dir * h), texx, texy, col },
		{ drawPos - (u * w) + (dir * h), texx, texy, col },
		{ drawPos + (dir * h * 1.2f),    texx, texy, col },
		{ drawPos + (dir * h * 1.2f),    texx, texy, col }
	);

	AddEffectsQuad<0>(
		pageNum,
		{ drawPos - (u * w) + (dir * h), texx, texy, col },
		{ drawPos + (r * w) + (dir * h), texx, texy, col },
		{ drawPos + (dir * h * 1.2f),    texx, texy, col },
		{ drawPos + (dir * h * 1.2f),    texx, texy, col }
	);
}

