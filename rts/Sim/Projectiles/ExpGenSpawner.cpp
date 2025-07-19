/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "ExpGenSpawner.h"

#include "ExpGenSpawnableMemberInfo.h"
#include "ExplosionGenerator.h"

#include "System/Misc/TracyDefs.h"

CR_BIND_DERIVED(CExpGenSpawner, CProjectile, )
CR_REG_METADATA(CExpGenSpawner,
(
	CR_MEMBER_BEGINFLAG(CM_Config),
		CR_MEMBER(delay),
		CR_MEMBER(damage),
		CR_IGNORED(explosionGenerator),
	CR_MEMBER_ENDFLAG(CM_Config),
	CR_SERIALIZER(Serialize)
))


CExpGenSpawner::CExpGenSpawner() : CProjectile()
{
	RECOIL_DETAILED_TRACY_ZONE;
	checkCol = false;
	deleteMe = false;
}

void CExpGenSpawner::Serialize(creg::ISerializer* s) {
	RECOIL_DETAILED_TRACY_ZONE;
	int generatorID;

	if (s->IsWriting())
		generatorID = explosionGenerator->GetGeneratorID();

	s->SerializeInt(&generatorID, sizeof(generatorID));

	if (s->IsWriting())
		return;

	// NOTE:
	//   projectiles are serialized, but ExplosionGeneratorHandler itself is not
	//   as such generator-id can be invalid when this spawner gets deserialized
	//   (if additional generators were loaded at runtime before game was saved)
	explosionGenerator = explGenHandler.GetGenerator(generatorID);
}

void CExpGenSpawner::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if ((deleteMe |= ((delay--) <= 0)))
		explosionGenerator->Explosion(
			pos,
			dir,
			damage,
			0.0f,
			0.0f,
			owner(),
			ExplosionHitObject(),
			true
		);
}


bool CExpGenSpawner::GetMemberInfo(SExpGenSpawnableMemberInfo& memberInfo)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (CProjectile::GetMemberInfo(memberInfo))
		return true;

	CHECK_MEMBER_INFO_INT  (CExpGenSpawner, delay );
	CHECK_MEMBER_INFO_FLOAT(CExpGenSpawner, damage);
	// TODO: much nicer to load cegID directly via LoadGeneratorID callback
	CHECK_MEMBER_INFO_PTR  (CExpGenSpawner, explosionGenerator, explGenHandler.LoadGenerator);

	return false;
}
