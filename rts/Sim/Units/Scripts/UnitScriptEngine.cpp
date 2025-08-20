/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

/* heavily based on CobEngine.cpp */

#include "UnitScriptEngine.h"

#include <type_traits>

#include "AnimComponents.hpp"
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

	template<Concepts::AnimComponent AnimInfoType>
	inline bool TickMoveAnim(int tickRate, LocalModelPieceEntity& lmpe, const AnimInfoType& ai)
	{
		static constexpr auto animAxis = AnimInfoType::animAxis;

		using namespace LMP;

		auto& [pos, noInterpolate] = lmpe.Get<Position, PositionNoInterpolation>();
		auto cur = pos[animAxis];
		const bool ret = MoveToward(cur, ai.dest, ai.speed / tickRate);
		pos[animAxis] = cur;

		noInterpolate = false;

		return ret;
	}

	template<Concepts::AnimComponent AnimInfoType>
	inline bool TickTurnAnim(int tickRate, LocalModelPieceEntity& lmpe, const AnimInfoType& ai)
	{
		static constexpr auto animAxis = AnimInfoType::animAxis;

		using namespace LMP;

		auto& [rot, noInterpolate] = lmpe.Get<Rotation, PositionNoInterpolation>();
		auto cur = ClampRad(rot[animAxis]);

		const bool ret = TurnToward(rot[ai.axis], ai.dest, ai.speed / tickRate);
		rot[animAxis] = cur;

		noInterpolate = false;

		return ret;
	}

	template<Concepts::AnimComponent AnimInfoType>
	inline bool TickSpinAnim(int tickRate, LocalModelPieceEntity& lmpe, const AnimInfoType& ai)
	{
		static constexpr auto animAxis = AnimInfoType::animAxis;

		using namespace LMP;

		auto& [rot, noInterpolate] = lmpe.Get<Rotation, PositionNoInterpolation>();
		auto cur = ClampRad(rot[animAxis]);

		const bool ret = DoSpin(rot[ai.axis], ai.dest, ai.speed, ai.accel, tickRate);
		rot[animAxis] = cur;

		noInterpolate = false;

		return ret;
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

			static constexpr auto animType = AnimInfoType::animType;
			static constexpr auto animAxis = AnimInfoType::animAxis;

			if constexpr (animType == ATurn) {
				LocalModelPieceEntity::ForEachView<Rotation, RotationNoInterpolation, AnimInfoType>([tickRate](auto&& entityRef, auto& rot, auto& noInterpolate, auto& ai) {
					using namespace LMP;

					auto cur = ClampRad(rot.value[animAxis]);
					ai.done |= Impl::TurnToward(cur, ai.dest, ai.speed / tickRate);
					rot.value[animAxis] = cur;

					noInterpolate = false;
				});
			}
			else if constexpr (animType == ASpin) {
				LocalModelPieceEntity::ForEachView<Rotation, RotationNoInterpolation, AnimInfoType>([tickRate](auto&& entityRef, auto& rot, auto& noInterpolate, auto& ai) {
					using namespace LMP;

					auto cur = ClampRad(rot.value[animAxis]);
					ai.done |= Impl::DoSpin(cur, ai.dest, ai.speed, ai.accel, tickRate);
					rot.value[animAxis] = cur;

					noInterpolate = false;
				});
			}
			else if constexpr (animType == AMove) {
				LocalModelPieceEntity::ForEachView<Position, RotationNoInterpolation, AnimInfoType>([tickRate](auto&& entityRef, auto& pos, auto& noInterpolate, auto& ai) {
					using namespace LMP;

					auto cur = pos.value[animAxis];
					ai.done |= Impl::MoveToward(cur, ai.dest, ai.speed / tickRate);
					pos.value[animAxis] = cur;

					noInterpolate = false;
				});
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
		ZoneScopedN("CUnitScriptEngine::Tick(ST)");

		static const auto FinalizeAnimation = [](auto&& t) {
			using AnimInfoType = std::decay_t<decltype(t)>;

			static constexpr auto animType = AnimInfoType::animType;
			static constexpr auto animAxis = AnimInfoType::animAxis;

			LocalModelPieceEntity::ForEachView<const AnimInfoType>([](auto&& entityRef, const auto& ai) {
				if (!ai.done)
					return;

				if (ai.hasWaiting) {
					//AnimFinished(static_cast<AnimType>(animType), ai.piece, ai.axis);
				}

				entityRef.template Remove<AnimInfoType>();
			});
		};

		std::apply([&](auto&&... args) {
			((FinalizeAnimation(args)), ...);
		}, animTuple);
	}

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
