/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

/* heavily based on CobEngine.h */

#ifndef UNIT_SCRIPT_ENGINE_H
#define UNIT_SCRIPT_ENGINE_H

#include <vector>

#include "System/creg/creg_cond.h"
#include "System/FreeListMap.h"

struct UnitDef;
class CUnit;
class CUnitScript;
class CNullUnitScript;

class CUnitScriptEngine
{
	CR_DECLARE_STRUCT(CUnitScriptEngine)

public:
	size_t AddInstance(CUnitScript* instance);
	void RemoveInstance(size_t id);
	void ReloadScripts(const UnitDef* udef);

	void Tick(int deltaTime);

	void Init();
	void Kill();

	CUnitScript* GetNullUnitScript();

	static void InitStatic();
	static void KillStatic();
private:
	CNullUnitScript* nullUnitScriptPtr = nullptr;
	spring::FreeListMap<CUnitScript*> allUnitScripts;
};

extern CUnitScriptEngine* unitScriptEngine;

#endif // UNIT_SCRIPT_ENGINE_H
