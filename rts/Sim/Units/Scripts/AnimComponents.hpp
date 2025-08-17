#pragma once

#include <array>
#include <type_traits>
#include <bitset>

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

//struct AnimTypeBase {};
//struct AnimTypeMove : public AnimTypeBase {};
//struct AnimTypeTurn : public AnimTypeBase {};
//struct AnimTypeSpin : public AnimTypeBase {};

enum AnimType {
	ANone = -1,
	ATurn = 0,
	ASpin = 1,
	AMove = 2,
	ACount = 3
};

//namespace Concepts {
//	template <typename T>
//	concept AnimType = std::is_base_of_v<AnimTypeBase, T>;
//}

struct AnimInfoBase {};

template<std::underlying_type_t<AnimType> T>
struct AnimInfo : public AnimInfoBase {
	std::array<float, 3> speed{};
	std::array<float, 3> dest{};     // means final position when turning or moving
	std::array<float, 3> accel{};    // used for spinning, can be negative
	std::bitset<9> flags{ false };   // flags for validX, validY, validZ, doneX, doneY, doneZ, hasWaitingX, hasWaitingY, hasWaitingZ

	static constexpr std::underlying_type_t<AnimType> animType = T;

	static constexpr size_t validX = 0;
	static constexpr size_t validY = 1;
	static constexpr size_t validZ = 2;
	static constexpr size_t doneX  = 3;
	static constexpr size_t doneY  = 4;
	static constexpr size_t doneZ  = 5;
	static constexpr size_t hasWaitingX = 6;
	static constexpr size_t hasWaitingY = 7;
	static constexpr size_t hasWaitingZ = 8;
};

using AnimInfoMove = AnimInfo<ATurn>;
using AnimInfoTurn = AnimInfo<ASpin>;
using AnimInfoSpin = AnimInfo<AMove>;

/*
struct AnimInfoSpin : public AnimInfoBase {
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
*/

//struct HasAnimation {};

//namespace Concepts {
//	template <typename T>
//	concept AnimComponent = requires(T t) {
//		{ T::Axis } -> std::same_as<std::size_t>;
//	};
//}

namespace Concepts {
	template <typename T>
	concept AnimInfoType = std::is_base_of_v<AnimInfoBase, T>;
}