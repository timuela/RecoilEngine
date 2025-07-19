/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "Game/Camera.h"
#include "LightningProjectile.h"
#include "Rendering/GL/RenderBuffers.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "Sim/Misc/GlobalSynced.h"
#include "Sim/Projectiles/ExplosionGenerator.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "Sim/Weapons/WeaponDef.h"

#include "System/Misc/TracyDefs.h"

CR_BIND_DERIVED(CLightningProjectile, CWeaponProjectile, )

CR_REG_METADATA(CLightningProjectile,(
	CR_SETFLAG(CF_Synced),
	CR_MEMBER(color),
	CR_MEMBER(displacements1),
	CR_MEMBER(displacements2)
))


CLightningProjectile::CLightningProjectile(const ProjectileParams& params): CWeaponProjectile(params)
{
	projectileType = WEAPON_LIGHTNING_PROJECTILE;
	useAirLos = false;

	if (weaponDef != nullptr) {
		assert(weaponDef->IsHitScanWeapon());
		color = weaponDef->visuals.color;
	}

	displacements1[0] = 0.0f;
	displacements2[0] = 0.0f;

	for (size_t d = 1; d < displacements_size; ++d) {
		displacements1[d] = (gsRNG.NextFloat() - 0.5f) * drawRadius * 0.05f;
		displacements2[d] = (gsRNG.NextFloat() - 0.5f) * drawRadius * 0.05f;
	}
}

void CLightningProjectile::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (--ttl <= 0) {
		deleteMe = true;
	} else {
		explGenHandler.GenExplosion(
			cegID,
			startPos + ((targetPos - startPos) / ttl),
			targetPos - startPos,
			0.0f,
			displacements1[0],
			0.0f,
			owner(),
			ExplosionHitObject()
		);
	}

	for (size_t d = 1; d < displacements_size; ++d) {
		displacements1[d] += (gsRNG.NextFloat() - 0.5f) * 0.3f;
		displacements2[d] += (gsRNG.NextFloat() - 0.5f) * 0.3f;
	}

	UpdateInterception();
}

void CLightningProjectile::Draw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!validTextures[0])
		return;

	UpdateAnimParams();

	uint8_t col[4] {
		(uint8_t)(color.x * 255),
		(uint8_t)(color.y * 255),
		(uint8_t)(color.z * 255),
		1 //intensity*255;
	};

	const float3 ddir = (targetPos - startPos).Normalize();
	const float3 dif  = (startPos - camera->GetPos()).Normalize();
	const float3 dir1 = (dif.cross(ddir)).Normalize();

	const float* displacements[] = { displacements1, displacements2 };

	for (auto* displacement : displacements) {
		float3 tempPos = startPos;
		for (size_t d = 1; d < displacements_size - 1; ++d) {
			float f = (d + 1) * 0.111f;
			const float3 tempPosO = tempPos;
			tempPos  = (startPos * (1.0f - f)) + (targetPos * f);

			const auto& WDV = weaponDef->visuals;
			AddEffectsQuad<1>(
				WDV.texture1->pageNum,
				{ tempPosO + (dir1 * (displacement[d    ] + WDV.thickness)), WDV.texture1->xstart, WDV.texture1->ystart, col },
				{ tempPos  + (dir1 * (displacement[d + 1] + WDV.thickness)), WDV.texture1->xend,   WDV.texture1->ystart, col },
				{ tempPos  + (dir1 * (displacement[d + 1] - WDV.thickness)), WDV.texture1->xend,   WDV.texture1->yend,   col },
				{ tempPosO + (dir1 * (displacement[d    ] - WDV.thickness)), WDV.texture1->xstart, WDV.texture1->yend,   col }
			);
		}
	}
}

void CLightningProjectile::DrawOnMinimap() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	const SColor lcolor = SColor{
		color[0],
		color[1],
		color[2]
	};

	AddMiniMapVertices({ startPos,  lcolor }, { targetPos, lcolor });
}

int CLightningProjectile::GetProjectilesCount() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	return 2 * displacements_size * validTextures[0];
}