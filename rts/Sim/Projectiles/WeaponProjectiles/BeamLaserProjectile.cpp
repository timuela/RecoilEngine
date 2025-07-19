/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "BeamLaserProjectile.h"
#include "Game/Camera.h"
#include "Game/CameraHandler.h"
#include "Rendering/GL/RenderBuffers.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "Sim/Projectiles/ExplosionGenerator.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Weapons/WeaponDef.h"
#include <cstring> //memset

#include "System/Misc/TracyDefs.h"

CR_BIND_DERIVED(CBeamLaserProjectile, CWeaponProjectile, )

CR_REG_METADATA(CBeamLaserProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(coreColStart),
	CR_MEMBER(coreColEnd),
	CR_MEMBER(edgeColStart),
	CR_MEMBER(edgeColEnd),
	CR_MEMBER(thickness),
	CR_MEMBER(corethickness),
	CR_MEMBER(flaresize),
	CR_MEMBER(decay),
	CR_MEMBER(midtexx)
))


CBeamLaserProjectile::CBeamLaserProjectile(const ProjectileParams& params): CWeaponProjectile(params)
	, thickness(0.0f)
	, corethickness(0.0f)
	, flaresize(0.0f)
	, decay(0.0f)
	, midtexx(0.0f)
{
	projectileType = WEAPON_BEAMLASER_PROJECTILE;

	if (weaponDef != nullptr) {
		assert(weaponDef->IsHitScanWeapon());

		thickness = weaponDef->visuals.thickness;
		corethickness = weaponDef->visuals.corethickness;
		flaresize = weaponDef->visuals.laserflaresize;
		decay = weaponDef->visuals.beamdecay;

		midtexx =
			(weaponDef->visuals.texture2->xstart +
			(weaponDef->visuals.texture2->xend - weaponDef->visuals.texture2->xstart) * 0.5f);

		coreColStart[0] = (weaponDef->visuals.color2.x * params.startAlpha);
		coreColStart[1] = (weaponDef->visuals.color2.y * params.startAlpha);
		coreColStart[2] = (weaponDef->visuals.color2.z * params.startAlpha);
		coreColStart[3] = 1;
		coreColEnd[0] = (weaponDef->visuals.color2.x * params.endAlpha);
		coreColEnd[1] = (weaponDef->visuals.color2.y * params.endAlpha);
		coreColEnd[2] = (weaponDef->visuals.color2.z * params.endAlpha);
		coreColEnd[3] = 1;
		edgeColStart[0] = (weaponDef->visuals.color.x * params.startAlpha);
		edgeColStart[1] = (weaponDef->visuals.color.y * params.startAlpha);
		edgeColStart[2] = (weaponDef->visuals.color.z * params.startAlpha);
		edgeColStart[3] = 1;
		edgeColEnd[0] = (weaponDef->visuals.color.x * params.endAlpha);
		edgeColEnd[1] = (weaponDef->visuals.color.y * params.endAlpha);
		edgeColEnd[2] = (weaponDef->visuals.color.z * params.endAlpha);
		edgeColEnd[3] = 1;
	}
	else {
		memset(&coreColStart[0], 0, sizeof(coreColStart));
		memset(&coreColEnd[0], 0, sizeof(coreColEnd));
		memset(&edgeColStart[0], 0, sizeof(edgeColStart));
		memset(&edgeColEnd[0], 0, sizeof(edgeColEnd));
	}
}



void CBeamLaserProjectile::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if ((--ttl) <= 0) {
		deleteMe = true;
	} else {
		for (int i = 0; i < 3; i++) {
			coreColStart[i] *= decay;
			coreColEnd[i]   *= decay;
			edgeColStart[i] *= decay;
			edgeColEnd[i]   *= decay;
		}

		explGenHandler.GenExplosion(
			cegID,
			startPos + ((targetPos - startPos) / ttl),
			targetPos - startPos,
			0.0f,
			flaresize,
			0.0f,
			owner(),
			ExplosionHitObject()
		);
	}

	UpdateInterception();
}

void CBeamLaserProjectile::Draw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!validTextures[0])
		return;

	UpdateAnimParams();

	const float3 midPos = (targetPos + startPos) * 0.5f;
	const float3 cameraDir = (midPos - camera->GetPos()).SafeANormalize();

	CCamera* playerCamera = CCameraHandler::GetCamera(CCamera::CAMTYPE_PLAYER);
	const float playerCamDistSq = (midPos - playerCamera->GetPos()).SqLength();

	// beam's coor-system; degenerate if targetPos == startPos
	const float3 zdir = (targetPos - startPos).SafeANormalize();
	const float3 xdir = (cameraDir.cross(zdir)).SafeANormalize();
	const float3 ydir = (cameraDir.cross(xdir));

	const float beamEdgeSize = thickness;
	const float beamCoreSize = beamEdgeSize * corethickness;
	const float flareEdgeSize = thickness * flaresize;
	const float flareCoreSize = flareEdgeSize * corethickness;

	const float3& pos1 = startPos;
	const float3& pos2 = targetPos;

	const auto* WT1 = weaponDef->visuals.texture1;
	const auto* WT2 = weaponDef->visuals.texture2;
	const auto* WT3 = weaponDef->visuals.texture3;

	if (playerCamDistSq < Square(1000.0f)) {
		if (validTextures[2]) {
			AddEffectsQuad<2>(
				WT2->pageNum,
				{ pos1 - xdir * beamEdgeSize,                       midtexx  , WT2->ystart, edgeColStart },
				{ pos1 - xdir * beamEdgeSize - ydir * beamEdgeSize, WT2->xend, WT2->ystart, edgeColStart },
				{ pos1 + xdir * beamEdgeSize - ydir * beamEdgeSize, WT2->xend, WT2->yend  , edgeColStart },
				{ pos1 + xdir * beamEdgeSize,                       midtexx  , WT2->yend  , edgeColStart }
			);
			AddEffectsQuad<2>(
				WT2->pageNum,
				{ pos1 - xdir * beamCoreSize,                       midtexx  , WT2->ystart, coreColStart },
				{ pos1 - xdir * beamCoreSize - ydir * beamCoreSize, WT2->xend, WT2->ystart, coreColStart },
				{ pos1 + xdir * beamCoreSize - ydir * beamCoreSize, WT2->xend, WT2->yend  , coreColStart },
				{ pos1 + xdir * beamCoreSize,                       midtexx  , WT2->yend  , coreColStart }
			);

		}
		if (validTextures[1]) {
			AddEffectsQuad<1>(
				WT1->pageNum,
				{ pos1 - xdir * beamEdgeSize,                       WT1->xstart, WT1->ystart, edgeColStart },
				{ pos2 - xdir * beamEdgeSize,                       WT1->xend  , WT1->ystart, edgeColEnd   },
				{ pos2 + xdir * beamEdgeSize,                       WT1->xend  , WT1->yend  , edgeColEnd   },
				{ pos1 + xdir * beamEdgeSize,                       WT1->xstart, WT1->yend  , edgeColStart }
			);

			AddEffectsQuad<1>(
				WT1->pageNum,
				{ pos1 - xdir * beamCoreSize,                       WT1->xstart, WT1->ystart, coreColStart },
				{ pos2 - xdir * beamCoreSize,                       WT1->xend  , WT1->ystart, coreColEnd   },
				{ pos2 + xdir * beamCoreSize,                       WT1->xend  , WT1->yend  , coreColEnd   },
				{ pos1 + xdir * beamCoreSize,                       WT1->xstart, WT1->yend  , coreColStart }
			);
		}
		if (validTextures[2]) {
			AddEffectsQuad<2>(
				WT2->pageNum,
				{ pos2 - xdir * beamEdgeSize,                       midtexx  , WT2->ystart, edgeColStart },
				{ pos2 - xdir * beamEdgeSize + ydir * beamEdgeSize, WT2->xend, WT2->ystart, edgeColStart },
				{ pos2 + xdir * beamEdgeSize + ydir * beamEdgeSize, WT2->xend, WT2->yend  , edgeColStart },
				{ pos2 + xdir * beamEdgeSize,                       midtexx  , WT2->yend  , edgeColStart }
			);

			AddEffectsQuad<2>(
				WT2->pageNum,
				{ pos2 - xdir * beamCoreSize,                       midtexx  , WT2->ystart, coreColStart },
				{ pos2 - xdir * beamCoreSize + ydir * beamCoreSize, WT2->xend, WT2->ystart, coreColStart },
				{ pos2 + xdir * beamCoreSize + ydir * beamCoreSize, WT2->xend, WT2->yend  , coreColStart },
				{ pos2 + xdir * beamCoreSize,                       midtexx  , WT2->yend  , coreColStart }
			);
		}
	} else {
		if (validTextures[1]) {
			AddEffectsQuad<1>(
				WT1->pageNum,
				{ pos1 - xdir * beamEdgeSize,                       WT1->xstart, WT1->ystart, edgeColStart },
				{ pos2 - xdir * beamEdgeSize,                       WT1->xend  , WT1->ystart, edgeColEnd   },
				{ pos2 + xdir * beamEdgeSize,                       WT1->xend  , WT1->yend  , edgeColEnd   },
				{ pos1 + xdir * beamEdgeSize,                       WT1->xstart, WT1->yend  , edgeColStart }
			);

			AddEffectsQuad<1>(
				WT1->pageNum,
				{ pos1 - xdir * beamCoreSize,                       WT1->xstart, WT1->ystart, coreColStart },
				{ pos2 - xdir * beamCoreSize,                       WT1->xend  , WT1->ystart, coreColEnd   },
				{ pos2 + xdir * beamCoreSize,                       WT1->xend  , WT1->yend  , coreColEnd   },
				{ pos1 + xdir * beamCoreSize,                       WT1->xstart, WT1->yend  , coreColStart }
			);
		}
	}

	// draw flare
	if (validTextures[3]) {
		AddEffectsQuad<3>(
			WT3->pageNum,
			{ pos1 - camera->GetRight() * flareEdgeSize - camera->GetUp() * flareEdgeSize, WT3->xstart, WT3->ystart, edgeColStart },
			{ pos1 + camera->GetRight() * flareEdgeSize - camera->GetUp() * flareEdgeSize, WT3->xend,   WT3->ystart, edgeColStart },
			{ pos1 + camera->GetRight() * flareEdgeSize + camera->GetUp() * flareEdgeSize, WT3->xend,   WT3->yend,   edgeColStart },
			{ pos1 - camera->GetRight() * flareEdgeSize + camera->GetUp() * flareEdgeSize, WT3->xstart, WT3->yend,   edgeColStart }
		);

		AddEffectsQuad<3>(
			WT3->pageNum,
			{ pos1 - camera->GetRight() * flareCoreSize - camera->GetUp() * flareCoreSize, WT3->xstart, WT3->ystart, coreColStart },
			{ pos1 + camera->GetRight() * flareCoreSize - camera->GetUp() * flareCoreSize, WT3->xend,   WT3->ystart, coreColStart },
			{ pos1 + camera->GetRight() * flareCoreSize + camera->GetUp() * flareCoreSize, WT3->xend,   WT3->yend,   coreColStart },
			{ pos1 - camera->GetRight() * flareCoreSize + camera->GetUp() * flareCoreSize, WT3->xstart, WT3->yend,   coreColStart }
		);
	}
}

void CBeamLaserProjectile::DrawOnMinimap() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	const SColor color = { edgeColStart[0], edgeColStart[1], edgeColStart[2], 255u };

	AddMiniMapVertices({ startPos , color }, { targetPos, color });
}

int CBeamLaserProjectile::GetProjectilesCount() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	return
		2 * validTextures[1] +
		4 * validTextures[2] +
		2 * validTextures[3];
}
