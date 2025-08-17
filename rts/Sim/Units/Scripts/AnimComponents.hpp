#pragma once

#include <array>

/*
template<typename AnimTypeTag, size_t AnimAxis>
struct AnimInfo {
	static constexpr size_t Axis = AnimAxis;
	float speed{};
	float dest{};     // means final position when turning or moving, final speed when spinning
	bool done = false;
	bool hasWaiting = false;
};

using AnimInfoMoveX = AnimInfo<struct AnimTypeMove, 0>;
using AnimInfoMoveY = AnimInfo<struct AnimTypeMove, 1>;
using AnimInfoMoveZ = AnimInfo<struct AnimTypeMove, 2>;

using AnimInfoRotateX = AnimInfo<struct AnimTypeRotate, 0>;
using AnimInfoRotateY = AnimInfo<struct AnimTypeRotate, 1>;
using AnimInfoRotateZ = AnimInfo<struct AnimTypeRotate, 2>;

template<size_t AnimAxis>
struct AnimInfoSpin {
	static constexpr size_t Axis = AnimAxis;
	float speed{};
	float dest{};     // means final position when turning or moving, final speed when spinning
	float accel{};    // used for spinning, can be negative
	bool done = false;
	bool hasWaiting = false;
};

using AnimInfoSpinX = AnimInfoSpin<0>;
using AnimInfoSpinY = AnimInfoSpin<1>;
using AnimInfoSpinZ = AnimInfoSpin<2>;
*/

// Components

template<typename AnimTypeTag>
struct AnimInfo {
	std::array<float, 3> speed{};
	std::array<float, 3> dest{};     // means final position when turning or moving
	union {
		uint32_t flagVal = { 0 };
		struct {
			uint32_t validX : 1;
			uint32_t validY : 1;
			uint32_t validZ : 1;
			uint32_t doneX : 1;
			uint32_t doneY : 1;
			uint32_t doneZ : 1;
			uint32_t hasWaitingX : 1;
			uint32_t hasWaitingY : 1;
			uint32_t hasWaitingZ : 1;
		} flags;
	};
};
using AnimInfoMove = AnimInfo<struct AnimTypeMove>;
using AnimInfoRotate = AnimInfo<struct AnimTypeRotate>;

struct AnimInfoSpin {
	std::array<float, 3> speed{};
	std::array<float, 3> dest{};     // means final position when turning or moving, final speed when spinning
	std::array<float, 3> accel{};    // used for spinning, can be negative
	union {
		uint32_t flagVal = {0};
		struct {
			uint32_t validX : 1;
			uint32_t validY : 1;
			uint32_t validZ : 1;
			uint32_t doneX : 1;
			uint32_t doneY : 1;
			uint32_t doneZ : 1;
			uint32_t hasWaitingX : 1;
			uint32_t hasWaitingY : 1;
			uint32_t hasWaitingZ : 1;
		} flags;
	};
};

struct HasAnimation {};

namespace Concepts {
	template <typename T>
	concept AnimComponent = requires(T t) {
		{ T::Axis } -> std::same_as<std::size_t>;
	};
}