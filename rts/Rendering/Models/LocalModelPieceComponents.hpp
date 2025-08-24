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

	ALIAS_COMPONENT_DEF(Dirty, bool, true);

	struct ParentChildrenRelationship {
		ECS::EntityType parent{ entt::null };
		ECS::EntityType firstChild{ entt::null };
		ECS::EntityType nextSibling{ entt::null };
	};

	struct RelationshipHierarchy {
		ECS::EntityType parent{ entt::null };
		uint32_t hierarchyLevel{ 0 };
	};

	struct WasUpdated {
		bool forCurrFrame = true;
		bool forPrevFrame = true;
	};

	struct BlockScriptAnims {};

	struct MoveTurnAnimation {
		float3 src{};
		float3 spd{};
		float3 dst{}; // means final position when turning or moving, final speed when spinning
		float3 acc{}; // used for spinning, can be negative

		uint32_t doneX : 1;
		uint32_t doneY : 1;
		uint32_t doneZ : 1;

		uint32_t hasWaitingX : 1;
		uint32_t hasWaitingY : 1;
		uint32_t hasWaitingZ : 1;

		uint32_t blockScriptAnim : 1;
		uint32_t noInterpolation : 1;
		uint32_t dirty : 1;

		uint32_t scriptId : 23;
	};

	struct SpinAnimation {
		float3 src{};
		float3 spd{};
		float3 dst{}; // means final position when turning or moving, final speed when spinning

		uint32_t doneX : 1;
		uint32_t doneY : 1;
		uint32_t doneZ : 1;

		uint32_t hasWaitingX : 1;
		uint32_t hasWaitingY : 1;
		uint32_t hasWaitingZ : 1;

		uint32_t blockScriptAnim : 1;
		uint32_t noInterpolation : 1;
		uint32_t dirty : 1;

		uint32_t scriptId : 23;
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