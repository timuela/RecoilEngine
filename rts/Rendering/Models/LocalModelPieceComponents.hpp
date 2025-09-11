#pragma once

#include <vector>
#include <bitset>
#include <functional>
#include "System/float3.h"
#include "System/Transform.hpp"

#include "System/Ecs/RecsMain.hpp"
#include "System/Ecs/Components/BaseComponents.h"

// Components

struct S3DModelPiece;

namespace LMP {
	// translation relative to parent LMP, *INITIALLY* equal to original->offset (SYNCED)
	ALIAS_DENSE_COMPONENT(Position, float3);

	// orientation relative to parent LMP, in radians (updated by scripts)
	ALIAS_DENSE_COMPONENT(Rotation, float3);

	// transform relative to parent LMP (SYNCED), combines <pos> and <rot>
	ALIAS_DENSE_COMPONENT(PieceSpaceTransform, Transform);

	// transform relative to root LMP (SYNCED), chained pieceSpaceMat's
	ALIAS_DENSE_COMPONENT(CurrModelSpaceTransform, Transform);

	// ModelSpaceTransform, but from the previous frame (SYNCED)
	ALIAS_DENSE_COMPONENT(PrevModelSpaceTransform, Transform);

	ALIAS_DENSE_COMPONENT(OriginalBakedTransform, Transform);

	ALIAS_DENSE_COMPONENT_DEF(PositionNoInterpolation, bool, false);
	ALIAS_DENSE_COMPONENT_DEF(RotationNoInterpolation, bool, false);
	ALIAS_DENSE_COMPONENT_DEF(ScalingNoInterpolation, bool, false);

	struct ParentChildrenRelationship {
		static constexpr auto compCat = recs::ComponentCategory::Dense;
		recs::Entity parent{};
		std::vector<recs::Entity> children;
	};

	ALIAS_DENSE_COMPONENT_DEF(DirtyFlag, bool, true);

	struct UpdateFlags {
		static constexpr auto compCat = recs::ComponentCategory::Dense;
		bool forCurrFrame = true;
		bool forPrevFrame = true;
	};

	struct BlockScriptAnims {
		static constexpr auto compCat = recs::ComponentCategory::Sparse;
	};

	struct CurrTransforms {
		static constexpr auto compCat = recs::ComponentCategory::Dense;
		Transform pieceSpaceTransform;
		Transform currModelSpaceTransform;
	};

	struct AuxTransforms {
		static constexpr auto compCat = recs::ComponentCategory::Dense;
		Transform origBakedTransform;
		Transform prevModelSpaceTransform;
	};
}