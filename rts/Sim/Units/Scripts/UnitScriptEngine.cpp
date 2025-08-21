/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

/* heavily based on CobEngine.cpp */

#include "UnitScriptEngine.h"

#include <type_traits>

#include "AnimComponents.hpp"
#include "CobEngine.h"
#include "CobFileHandler.h"
#include "UnitScript.h"
#include "NullUnitScript.h"
#include "UnitScriptFactory.h"
#include "Sim/Units/Unit.h"
#include "Sim/Units/UnitDef.h"
#include "Sim/Units/UnitHandler.h"
#include "System/ContainerUtil.h"
#include "System/SafeUtil.h"
#include "System/Config/ConfigHandler.h"
#include "Rendering/Models/LocalModelPieceComponents.hpp"

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
	CR_MEMBER(allUnitScripts),
	CR_IGNORED(nullUnitScriptPtr)
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

void CUnitScriptEngine::Init()
{
	RECOIL_DETAILED_TRACY_ZONE;
	nullUnitScriptPtr = new CNullUnitScript(nullptr);
	allUnitScripts.reserve(1024); // reserve some space for the initial scripts
}

void CUnitScriptEngine::Kill()
{
	spring::SafeDelete(nullUnitScriptPtr);
	allUnitScripts.clear();
}

CUnitScript* CUnitScriptEngine::GetNullUnitScript()
{
	assert(nullUnitScriptPtr);
	return nullUnitScriptPtr;
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


size_t CUnitScriptEngine::AddInstance(CUnitScript* instance)
{
	RECOIL_DETAILED_TRACY_ZONE;

	return allUnitScripts.Add(instance);
}

void CUnitScriptEngine::RemoveInstance(size_t id)
{
	RECOIL_DETAILED_TRACY_ZONE;

	allUnitScripts.Del(id);
}

namespace Impl{
	/**
	 * @brief Updates move animations
	 * @param cur float value to update
	 * @param dest float final value
	 * @param speed float max increment per tick
	 * @return returns true if destination was reached, false otherwise
	 */
	inline bool MoveToward(float& cur, float dest, float speed)
	{
		const float delta = dest - cur;

		if (math::fabsf(delta) <= speed) {
			cur = dest;
			return true;
		}

		cur += (speed * Sign(delta));
		return false;
	}


	/**
	 * @brief Updates turn animations
	 * @param cur float value to update
	 * @param dest float final value
	 * @param speed float max increment per tick
	 * @return returns true if destination was reached, false otherwise
	 */
	inline bool TurnToward(float& cur, float dest, float speed)
	{
		assert(dest < math::TWOPI);
		assert(cur < math::TWOPI);

		float delta = math::fmod(dest - cur + math::THREEPI, math::TWOPI) - math::PI;

		if (math::fabsf(delta) <= speed) {
			cur = dest;
			return true;
		}

		cur = ClampRad(cur + speed * Sign(delta));
		return false;
	}


	/**
	 * @brief Updates spin animations
	 * @param cur float value to update
	 * @param dest float the final desired speed (NOT the final angle!)
	 * @param speed float is updated if it is not equal to dest
	 * @param divisor int is the deltatime, it is not added before the call because speed may have to be updated
	 * @return true if the desired speed is 0 and it is reached, false otherwise
	 */
	inline bool DoSpin(float& cur, float dest, float& speed, float accel, int divisor)
	{
		const float delta = dest - speed;

		// Check if we are not at the final speed and
		// make sure we do not go past desired speed
		if (math::fabsf(delta) <= accel) {
			if ((speed = dest) == 0.0f)
				return true;
		}
		else {
			// accelerations are defined in speed/frame (at GAME_SPEED fps)
			speed += (accel * (GAME_SPEED * 1.0f / divisor) * Sign(delta));
		}

		cur = ClampRad(cur + (speed / divisor));
		return false;
	}
}

void CUnitScriptEngine::Tick(int deltaTime)
{
	SCOPED_TIMER("CUnitScriptEngine::Tick");
	const int tickRate = 1000 / deltaTime;

	cobEngine->Tick(deltaTime);

	using namespace LMP;

	//unused, but needed to deduce the tuple member types
	static constexpr AnimComponentTuple animTuple; 

	{
		ZoneScopedN("CUnitScriptEngine::Tick(MT)");

		const auto ExecuteAnimation = [tickRate](auto&& t) {
			using AnimInfoType = std::decay_t<decltype(t)>;

			using namespace ECS;

			static constexpr auto animType = AnimInfoType::animType;
			static constexpr auto animAxis = AnimInfoType::animAxis;

			if constexpr (animType == ATurn) {
				LocalModelPieceEntity::ForEachGroup<Rotation, RotationNoInterpolation, AnimInfoType>([tickRate](auto&& entityRef, auto&& rot, auto&& noInterpolate, auto&& ai, auto&& dirty) {
					using namespace LMP;

					noInterpolate = false;
					if (entityRef.template Has<BlockScriptAnims>())
						return;

					const auto curValue = rot.value[animAxis];
					auto newValue = ClampRad(curValue);
					ai.done |= Impl::TurnToward(newValue, ai.dest, ai.speed / tickRate);

					if (curValue == newValue)
						return;

					rot.value[animAxis] = newValue;

					// will do recursive propagation of dirty flag later
					dirty = true;
				}, EntityInclude<Dirty>);
			}
			else if constexpr (animType == ASpin) {
				LocalModelPieceEntity::ForEachGroup<Rotation, RotationNoInterpolation, AnimInfoType>([tickRate](auto&& entityRef, auto&& rot, auto&& noInterpolate, auto&& ai, auto&& dirty) {
					using namespace LMP;

					noInterpolate = false;
					if (entityRef.template Has<BlockScriptAnims>())
						return;

					const auto curValue = rot.value[animAxis];
					auto newValue = ClampRad(curValue);
					ai.done |= Impl::DoSpin(newValue, ai.dest, ai.speed, ai.accel, tickRate);

					if (curValue == newValue)
						return;

					rot.value[animAxis] = newValue;

					// will do recursive propagation of dirty flag later
					dirty = true;
				}, EntityInclude<Dirty>);
			}
			else if constexpr (animType == AMove) {
				LocalModelPieceEntity::ForEachGroup<Position, PositionNoInterpolation, AnimInfoType>([tickRate](auto&& entityRef, auto&& pos, auto&& noInterpolate, auto&& ai, auto&& dirty) {
					using namespace LMP;

					noInterpolate = false;
					if (entityRef.template Has<BlockScriptAnims>())
						return;

					const auto curValue = pos.value[animAxis];
					auto newValue = pos.value[animAxis];
					ai.done |= Impl::MoveToward(newValue, ai.dest, ai.speed / tickRate);
					pos.value[animAxis] = newValue;

					if (curValue == newValue)
						return;

					pos.value[animAxis] = newValue;

					// will do recursive propagation of dirty flag later
					dirty = true;
				}, EntityInclude<Dirty>);
			}
			else {
				static_assert(Recoil::always_false_v<AnimInfoType>, "Unknown animation type");
			}
		};

		std::apply([&](auto&&... args) {
			((ExecuteAnimation(args)), ...);
		}, animTuple);
	}
	{
		ZoneScopedN("CUnitScriptEngine::Tick(ST-1)");

		// set dirty flag to all children and further descendants of dirty parent
		bool newDirtyFlags = true;

		while (newDirtyFlags) {
			newDirtyFlags = false;
			LocalModelPieceEntity::ForEachView<Dirty>([&newDirtyFlags](auto&& entityRef, auto&& dirty) {
				if (dirty)
					return;

				using namespace LMP;
				auto& r = entityRef.template Get<ParentRelationship>();
				if (r.parent == ECS::NullEntity)
					return;

				const auto parLmpe = LocalModelPieceEntityRef(r.parent);
				const auto& dirtyParent = parLmpe.Get<const Dirty>();
				if (dirtyParent) {
					// if the parent is dirty, this instance is dirty too
					dirty = true;
					newDirtyFlags = true;
				}
			});
		}
	}
	{
		ZoneScopedN("CUnitScriptEngine::Tick(ST-2)");
		// send AnimFinished calls and pop up done animations
		const auto FinalizeAnimation = [this](auto&& t) {
			using AnimInfoType = std::decay_t<decltype(t)>;

			static constexpr auto animType = AnimInfoType::animType;
			static constexpr auto animAxis = AnimInfoType::animAxis;

			LocalModelPieceEntity::ForEachView<const AnimInfoType>([this](auto&& entityRef, const auto& ai) {
				if (!ai.done)
					return;

				if (ai.hasWaiting) {
					auto* unitScript = std::as_const(allUnitScripts)[ai.scriptId];
					unitScript->AnimFinished(static_cast<AnimType>(ai.animType), ai.piece, animAxis);
				}

				entityRef.template Remove<AnimInfoType>();
			});
		};

		std::apply([&](auto&&... args) {
			((FinalizeAnimation(args)), ...);
		}, animTuple);
	}

	cobEngine->RunDeferredCallins();
}
