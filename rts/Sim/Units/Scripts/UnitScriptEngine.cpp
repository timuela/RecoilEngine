/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

/* heavily based on CobEngine.cpp */

#include "UnitScriptEngine.h"

#include "CobEngine.h"
#include "CobFileHandler.h"
#include "UnitScript.h"
#include "UnitScriptFactory.h"
#include "Sim/Units/Unit.h"
#include "Sim/Units/UnitDef.h"
#include "Sim/Units/UnitHandler.h"
#include "System/ContainerUtil.h"
#include "System/SafeUtil.h"
#include "System/Config/ConfigHandler.h"

#include "System/Misc/TracyDefs.h"

CONFIG(bool, AnimationMT).deprecated(true);

static CCobEngine gCobEngine;
static CCobFileHandler gCobFileHandler;
static CUnitScriptEngine gUnitScriptEngine;

CCobEngine* cobEngine = nullptr;
CCobFileHandler* cobFileHandler = nullptr;
CUnitScriptEngine* unitScriptEngine = nullptr;


CR_BIND(CUnitScriptEngine, )

CR_REG_METADATA(CUnitScriptEngine, (
	CR_MEMBER(animating),

	// always null when saving
	CR_IGNORED(currentScript)
))


void CUnitScriptEngine::InitStatic() {
	RECOIL_DETAILED_TRACY_ZONE;
	cobEngine = &gCobEngine;
	cobFileHandler = &gCobFileHandler;
	unitScriptEngine = &gUnitScriptEngine;

	cobEngine->Init();
	cobFileHandler->Init();
	unitScriptEngine->Init();
}

void CUnitScriptEngine::KillStatic() {
	RECOIL_DETAILED_TRACY_ZONE;
	cobEngine->Kill();
	cobFileHandler->Kill();
	unitScriptEngine->Kill();

	cobEngine = nullptr;
	cobFileHandler = nullptr;
	unitScriptEngine = nullptr;
}



void CUnitScriptEngine::ReloadScripts(const UnitDef* udef)
{
	RECOIL_DETAILED_TRACY_ZONE;
	const CCobFile* oldScriptFile = cobFileHandler->GetScriptFile(udef->scriptName);

	if (oldScriptFile == nullptr) {
		LOG_L(L_WARNING, "[UnitScriptEngine::%s] unknown COB script for unit \"%s\": %s", __func__, udef->name.c_str(), udef->scriptName.c_str());
		return;
	}

	CCobFile* newScriptFile = cobFileHandler->ReloadCobFile(udef->scriptName);

	if (newScriptFile == nullptr) {
		LOG_L(L_WARNING, "[UnitScriptEngine::%s] could not load COB script for unit \"%s\" from: %s", __func__, udef->name.c_str(), udef->scriptName.c_str());
		return;
	}

	unsigned int count = 0;

	for (unsigned int i = 0, n = unitHandler.MaxUnits(); i < n; i++) {
		CUnit* unit = unitHandler.GetUnit(i);

		if (unit == nullptr)
			continue;

		CUnitScript*& unitScript = unit->script;
		CCobInstance* cobInstance = dynamic_cast<CCobInstance*>(unitScript);

		if (cobInstance == nullptr || cobInstance->GetFile() != oldScriptFile)
			continue;

		count++;

		spring::SafeDestruct(unitScript);

		unitScript = CUnitScriptFactory::CreateCOBScript(unit, newScriptFile);
		unitScript->Create();
	}

	LOG("[UnitScriptEngine::%s] reloaded COB scripts for %i units", __func__, count);
}


void CUnitScriptEngine::AddInstance(CUnitScript* instance)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (instance == currentScript)
		return;

	spring::VectorInsertUnique(animating, instance/*, true*/);
}

void CUnitScriptEngine::RemoveInstance(CUnitScript* instance)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (instance == currentScript)
		return;

	spring::VectorErase(animating, instance);
}

void CUnitScriptEngine::Tick(int deltaTime)
{
	SCOPED_TIMER("CUnitScriptEngine::Tick");

	cobEngine->Tick(deltaTime);

	/*
	// tick all (COB or LUS) script instances that have registered themselves as animating
	{
		ZoneScopedN("CUnitScriptEngine::Tick(MT)");

		// setting currentScript = animating[i]; is not required here, only in ST section below
		for_mt(0, animating.size(), [&](const int i) {
			animating[i]->TickAllAnims(deltaTime);
		});
	}
	{
		ZoneScopedN("CUnitScriptEngine::Tick(ST)");
		for (size_t i = 0; i < animating.size(); ) {
			currentScript = animating[i];

			if (!currentScript->TickAnimFinished(deltaTime)) {
				animating[i] = animating.back();
				animating.pop_back();
				continue;
			}
			i++;
		}
	}
	*/

	currentScript = nullptr;
	cobEngine->RunDeferredCallins();
}
