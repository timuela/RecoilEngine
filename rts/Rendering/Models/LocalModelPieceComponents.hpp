#pragma once

#include <vector>
#include <bitset>
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

	ALIAS_COMPONENT(OriginalBakedTransform, Transform);

	ALIAS_COMPONENT_DEF(PositionNoInterpolation, bool, false);
	ALIAS_COMPONENT_DEF(RotationNoInterpolation, bool, false);
	ALIAS_COMPONENT_DEF(ScalingNoInterpolation, bool, false);

	struct ParentChildrenRelationship {
		Flecs::MyEntityType parent{ Flecs::NullEntity };
		std::vector<Flecs::MyEntityType> children;
	};

	ALIAS_COMPONENT_DEF(DirtyFlag, bool, true);

	struct UpdateFlags {
		bool forCurrFrame = true;
		bool forPrevFrame = true;
	};

	struct BlockScriptAnims {
		inline static const auto flecsTraits = flecs::CanToggle;
	};
	struct HasAnimation {
		inline static const auto flecsTraits = flecs::CanToggle;
	};

	struct CurrTransforms {
		Transform pieceSpaceTransform;
		Transform currModelSpaceTransform;
	};

	struct AuxTransforms {
		Transform origBakedTransform;
		Transform prevModelSpaceTransform;
	};
}