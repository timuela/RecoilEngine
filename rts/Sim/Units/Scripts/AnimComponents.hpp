#pragma once

#include <tuple>
#include <type_traits>

#include "System/Ecs/Components/BaseComponents.h"
#include "System/TemplateUtils.hpp"

#include "AnimTypes.hpp"

using AnimTypeInt = std::underlying_type_t<AnimType>;

template<AnimTypeInt AT, size_t AA>
struct AnimInfoBase {
	static constexpr auto compCat = recs::ComponentCategory::Sparse;
	static constexpr auto animType = AT;
	static constexpr auto animAxis = AA;

	size_t scriptId{size_t(-1)}; // script id, used to identify the script instance
	int piece{-1}; // script piece index
	float speed{};
	float dest{};  // means final position when turning or moving, final speed when spinning
	float accel{}; // used for spinning, can be negative
	bool done = false;
	bool hasWaiting = false;
};

static constexpr size_t AnimAxisCount = 3;
static constexpr size_t AnimTypeCount = ACount;

template<size_t AA>
using AnimInfoTurn = AnimInfoBase<ATurn, AA>;

template<size_t AA>
using AnimInfoSpin = AnimInfoBase<ASpin, AA>;

template<size_t AA>
using AnimInfoMove = AnimInfoBase<AMove, AA>;

using AnimInfoTurnX = AnimInfoTurn<0>;
using AnimInfoTurnY = AnimInfoTurn<1>;
using AnimInfoTurnZ = AnimInfoTurn<2>;

using AnimInfoSpinX = AnimInfoSpin<0>;
using AnimInfoSpinY = AnimInfoSpin<1>;
using AnimInfoSpinZ = AnimInfoSpin<2>;

using AnimInfoMoveX = AnimInfoMove<0>;
using AnimInfoMoveY = AnimInfoMove<1>;
using AnimInfoMoveZ = AnimInfoMove<2>;

namespace Concepts {
	template <typename T>
	concept AnimComponent =
		std::same_as<std::remove_const_t<decltype(T::animType)>, AnimTypeInt>&&
		std::same_as<std::remove_const_t<decltype(T::animAxis)>, size_t>;
}

static constexpr auto AnimComponentList = spring::type_list<
	AnimInfoTurnX,
	AnimInfoTurnY,
	AnimInfoTurnZ,
	AnimInfoSpinX,
	AnimInfoSpinY,
	AnimInfoSpinZ,
	AnimInfoMoveX,
	AnimInfoMoveY,
	AnimInfoMoveZ
>;

struct SpinAnimation {
	static constexpr auto compCat = recs::ComponentCategory::Sparse;
	static constexpr auto animType = ASpin;

	float3 src{};
	float3 spd{};
	float3 dst{}; // means final position when turning or moving, final speed when spinning
	float3 acc{}; // used for spinning, can be negative

	uint32_t hasX : 1;
	uint32_t hasY : 1;
	uint32_t hasZ : 1;

	uint32_t doneX : 1;
	uint32_t doneY : 1;
	uint32_t doneZ : 1;

	uint32_t hasWaitingX : 1;
	uint32_t hasWaitingY : 1;
	uint32_t hasWaitingZ : 1;

	uint32_t blockScriptAnim : 1;
	uint32_t noInterpolation : 1;
	uint32_t dirty : 1;

	uint32_t reserved : 4;

	uint32_t pieceNum : 16;

	uint32_t scriptId;
};

template<AnimTypeInt AT>
struct MoveTurnAnimation {
	static constexpr auto compCat = recs::ComponentCategory::Sparse;
	static constexpr auto animType = AT;

	float3 src{};
	float3 spd{};
	float3 dst{}; // means final position when turning or moving, final speed when spinning

	uint32_t hasX : 1;
	uint32_t hasY : 1;
	uint32_t hasZ : 1;

	uint32_t doneX : 1;
	uint32_t doneY : 1;
	uint32_t doneZ : 1;

	uint32_t hasWaitingX : 1;
	uint32_t hasWaitingY : 1;
	uint32_t hasWaitingZ : 1;

	uint32_t blockScriptAnim : 1;
	uint32_t noInterpolation : 1;
	uint32_t dirty : 1;

	uint32_t reserved : 4;

	uint32_t pieceNum : 16;

	uint32_t scriptId;
};

using MoveAnimation = MoveTurnAnimation<AMove>;
using TurnAnimation = MoveTurnAnimation<ATurn>;

struct HasAnimation {
	static constexpr auto compCat = recs::ComponentCategory::Sparse;
};