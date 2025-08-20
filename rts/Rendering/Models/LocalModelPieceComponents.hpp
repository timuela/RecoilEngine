#pragma once

#include <vector>
#include "System/Ecs/EcsMain.hpp"
#include "System/Ecs/Components/BaseComponents.h"

// Components

namespace LMP {
	ALIAS_COMPONENT(Position, float3);
	ALIAS_COMPONENT(Rotation, float3);

	ALIAS_COMPONENT_DEF(PositionNoInterpolation, bool, false);
	ALIAS_COMPONENT_DEF(RotationNoInterpolation, bool, false);
	ALIAS_COMPONENT_DEF(ScalingNoInterpolation, bool, false);

	ALIAS_COMPONENT_DEF(Dirty, bool, true);

	struct ParentChildrenRelationship {
		ECS::EntityType parent{ entt::null };
		ECS::EntityType firstChild{ entt::null };
		ECS::EntityType nextSibling{ entt::null };
	};

	struct ParentRelationship {
		ECS::EntityType parent{ entt::null };
	};

	struct BlockScriptAnims {};
}