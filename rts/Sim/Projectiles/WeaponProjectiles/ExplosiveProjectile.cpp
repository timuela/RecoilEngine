/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "ExplosiveProjectile.h"
#include "Game/Camera.h"
#include "Map/Ground.h"
#include "Rendering/GL/RenderBuffers.h"
#include "Rendering/Textures/ColorMap.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "Sim/Projectiles/ExplosionGenerator.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Weapons/WeaponDef.h"

#include "System/Misc/TracyDefs.h"

CR_BIND_DERIVED(CExplosiveProjectile, CWeaponProjectile, )

CR_REG_METADATA(CExplosiveProjectile, (
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(invttl),
	CR_MEMBER(curTime)
))


CExplosiveProjectile::CExplosiveProjectile(const ProjectileParams& params): CWeaponProjectile(params)
	, invttl(0.0f)
	, curTime(0.0f)
{
	RECOIL_DETAILED_TRACY_ZONE;
	projectileType = WEAPON_EXPLOSIVE_PROJECTILE;

	mygravity = params.gravity;
	useAirLos = true;

	if (weaponDef != nullptr) {
		SetRadiusAndHeight(weaponDef->collisionSize, 0.0f);
		drawRadius = weaponDef->size;
		castShadow = weaponDef->visuals.castShadow;
	}

	if (ttl <= 0) {
		invttl = 1.0f;
	} else {
		invttl = 1.0f / ttl;
	}
}

void CExplosiveProjectile::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	CProjectile::Update();

	if (--ttl == 0) {
		Collision();
	} else {
		if (ttl > 0)
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
	}

	curTime += invttl;
	curTime = std::min(curTime, 1.0f);

	if (weaponDef->noExplode && TraveledRange()) {
		CProjectile::Collision();
		return;
	}

	UpdateGroundBounce();
	UpdateInterception();
}

void CExplosiveProjectile::Draw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	// do not draw if a 3D model has been defined for us
	if (model != nullptr)
		return;

	if (!validTextures[0])
		return;

	UpdateAnimParams();

	uint8_t col[4] = {0};

	const WeaponDef::Visuals& wdVisuals = weaponDef->visuals;
	const auto* tex = wdVisuals.texture1;

	if (wdVisuals.colorMap != nullptr) {
		wdVisuals.colorMap->GetColor(col, curTime);
	} else {
		col[0] = wdVisuals.color.x    * 255;
		col[1] = wdVisuals.color.y    * 255;
		col[2] = wdVisuals.color.z    * 255;
		col[3] = weaponDef->intensity * 255;
	}

	const float  alphaDecay = wdVisuals.alphaDecay;
	const float  sizeDecay  = wdVisuals.sizeDecay;
	const float  separation = wdVisuals.separation;
	const bool   noGap      = wdVisuals.noGap;
	const int    stages     = wdVisuals.stages;
	const float  invStages  = 1.0f / std::max(1, stages);

	const float3 ndir = dir * separation * 0.6f;

	for (int stage = 0; stage < stages; ++stage) {
		const float stageDecay = (stages - (stage * alphaDecay)) * invStages;
		const float stageSize  = drawRadius * (1.0f - (stage * sizeDecay));

		const float3 ydirCam  = camera->GetUp()    * stageSize;
		const float3 xdirCam  = camera->GetRight() * stageSize;
		const float3 stageGap = (noGap)? (ndir * stageSize * stage): (ndir * drawRadius * stage);
		const float3 stagePos = drawPos - stageGap;

		col[0] = stageDecay * col[0];
		col[1] = stageDecay * col[1];
		col[2] = stageDecay * col[2];
		col[3] = stageDecay * col[3];

		AddEffectsQuad<1>(
			tex->pageNum,
			{ stagePos - xdirCam - ydirCam, tex->xstart, tex->ystart, col },
			{ stagePos + xdirCam - ydirCam, tex->xend,   tex->ystart, col },
			{ stagePos + xdirCam + ydirCam, tex->xend,   tex->yend,   col },
			{ stagePos - xdirCam + ydirCam, tex->xstart, tex->yend,   col }
		);
	}
}

int CExplosiveProjectile::ShieldRepulse(const float3& shieldPos, float shieldForce, float shieldMaxSpeed)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (luaMoveCtrl)
		return 0;

	const float3 rdir = (pos - shieldPos).Normalize();

	if (rdir.dot(speed) < shieldMaxSpeed) {
		SetVelocityAndSpeed(speed + (rdir * shieldForce));
		return 2;
	}

	return 0;
}

int CExplosiveProjectile::GetProjectilesCount() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	return weaponDef->visuals.stages * validTextures[0];
}
