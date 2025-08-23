#pragma once

#include <vector>
#include <functional>
#include "System/float3.h"
#include "System/Transform.hpp"

#include "System/Ecs/EcsMain.hpp"
#include "System/Ecs/Components/BaseComponents.h"

// Components

struct S3DModelPiece;

namespace LMP {
	// translation relative to parent LMP, *INITIALLY* equal to original->offset (SYNCED)
	ALIAS_COMPONENT(Position, float3);

	// orientation relative to parent LMP, in radians (updated by scripts)
	ALIAS_COMPONENT(Rotation, float3);

	// transform relative to parent LMP (SYNCED), combines <pos> and <rot>
	ALIAS_COMPONENT(PieceSpaceTransform, Transform);

	// transform relative to root LMP (SYNCED), chained pieceSpaceMat's
	ALIAS_COMPONENT(CurrModelSpaceTransform, Transform);

	// ModelSpaceTransform, but from the previous frame (SYNCED)
	ALIAS_COMPONENT(PrevModelSpaceTransform, Transform);

	ALIAS_COMPONENT(OriginalBakedRotation, CQuaternion);

	ALIAS_COMPONENT_DEF(PositionNoInterpolation, bool, false);
	ALIAS_COMPONENT_DEF(RotationNoInterpolation, bool, false);
	ALIAS_COMPONENT_DEF(ScalingNoInterpolation, bool, false);

	ALIAS_COMPONENT_DEF(Dirty, bool, true);

	ALIAS_COMPONENT_DEF(HierarchyLevel, size_t, 0);

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