#pragma once

#include "System/Ecs/Components/BaseComponents.h"

// Components

namespace LMP {
	ALIAS_COMPONENT(Position, float3);
	ALIAS_COMPONENT(Rotation, float3);

	ALIAS_COMPONENT_DEF(PositionNoInterpolation, bool, false);
	ALIAS_COMPONENT_DEF(RotationNoInterpolation, bool, false);
	ALIAS_COMPONENT_DEF(ScalingNoInterpolation, bool, false);
}