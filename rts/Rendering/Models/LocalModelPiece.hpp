#pragma once

#include <algorithm>
#include <vector>
#include <array>
#include <cstdint>
#include "System/Transform.hpp"
#include "System/Ecs/EntityOwner.hpp"
#include "Sim/Misc/CollisionVolume.h"
#include "System/creg/creg_cond.h"


struct S3DModelPiece;
struct LocalModel;
struct LocalModelPiece;

using LocalModelPieceEntity = ECS::EntityOwner<LocalModelPiece>;

/**
 * LocalModel
 * Instance of S3DModel. Container for the geometric properties & piece visibility status of the agent's instance of a 3d model.
 */

struct LocalModelPiece
{
	CR_DECLARE_STRUCT(LocalModelPiece)

	LocalModelPiece()
		: dirty(true)
		, wasUpdated{ true }
		, noInterpolation { false }
	{}
	LocalModelPiece(const S3DModelPiece* piece);

	void AddChild(LocalModelPiece* c) { children.push_back(c); }
	void RemoveChild(LocalModelPiece* c) { children.erase(std::find(children.begin(), children.end(), c)); }
	void SetParent(LocalModelPiece* p) { parent = p; }
	void SetLocalModel(LocalModel* lm) { localModel = lm; }

	void SetLModelPieceIndex(uint32_t idx) { lmodelPieceIndex = idx; }
	void SetScriptPieceIndex(uint32_t idx) { scriptPieceIndex = idx; }
	uint32_t GetLModelPieceIndex() const { return lmodelPieceIndex; }
	uint32_t GetScriptPieceIndex() const { return scriptPieceIndex; }

	void Draw() const;
	void DrawLOD(uint32_t lod) const;
	void SetLODCount(uint32_t count);


	// on-demand functions
	void UpdateChildTransformRec(bool updateChildMatrices) const;
	void UpdateParentMatricesRec() const;

	Transform CalcPieceSpaceTransformOrig(const float3& p, const float3& r, float s) const;
	Transform CalcPieceSpaceTransform(const float3& p, const float3& r, float s) const;

	// note: actually OBJECT_TO_WORLD but transform is the same
	float3 GetAbsolutePos() const { return (GetModelSpaceTransform().t * WORLD_TO_OBJECT_SPACE); }

	bool GetEmitDirPos(float3& emitPos, float3& emitDir) const;

	void SetDirty();
	void SetPosition(const float3& p); // anim-script only
	void SetRotation(const float3& r); // anim-script only

	void SetRotationNoInterpolation(bool noInterpolate) { noInterpolation[0] = noInterpolate; }
	void SetPositionNoInterpolation(bool noInterpolate) { noInterpolation[1] = noInterpolate; }
	void SetScalingNoInterpolation (bool noInterpolate) { noInterpolation[2] = noInterpolate; }

	auto GetWasUpdated() const { return wasUpdated[0] || wasUpdated[1]; }
	void ResetWasUpdated() const; /*fake*/

	bool SetPieceSpaceMatrix(const CMatrix44f& mat) {
		if ((blockScriptAnims = (mat.GetX() != ZeroVector))) {
			pieceSpaceTra = Transform::FromMatrix(mat);

			// neither of these are used outside of animation scripts, and
			// GetEulerAngles wants a matrix created by PYR rotation while
			// <rot> is YPR
			// pos = mat.GetPos();
			// rot = mat.GetEulerAnglesLftHand();
			return true;
		}

		return false;
	}

	const float3& GetPosition() const;
	const float3& GetRotation() const;

	const float3& GetDirection() const { return dir; }

	const Transform&  GetModelSpaceTransform() const;
	const CMatrix44f& GetModelSpaceMatrix()    const;

	const CollisionVolume* GetCollisionVolume() const { return &colvol; }
	      CollisionVolume* GetCollisionVolume()       { return &colvol; }

	bool GetScriptVisible() const { return scriptSetVisible; }
	void SetScriptVisible(bool b);

	void SavePrevModelSpaceTransform();
	Transform GetEffectivePrevModelSpaceTransform() const;
private:
	Transform prevModelSpaceTra;

	LocalModelPieceEntity lmpe; /*
		Components:
		* Position - translation relative to parent LMP, *INITIALLY* equal to original->offset
		* Rotation - orientation relative to parent LMP, in radians (updated by scripts)
	*/

	float3 dir;      // cached copy of original->GetEmitDir()

	mutable Transform pieceSpaceTra;  // transform relative to parent LMP (SYNCED), combines <pos> and <rot>
	mutable Transform modelSpaceTra;  // transform relative to root LMP (SYNCED), chained pieceSpaceMat's
	mutable CMatrix44f modelSpaceMat; // same as above, except matrix

	CollisionVolume colvol;

	mutable std::array<bool, 2> wasUpdated; // currFrame, prevFrame
	mutable std::array<bool, 3> noInterpolation; // rotate, move, scale
	mutable bool dirty;
	bool scriptSetVisible; // TODO: add (visibility) maxradius!
public:
	bool blockScriptAnims; // if true, Set{Position,Rotation} are ignored for this piece

	uint32_t lmodelPieceIndex; // index of this piece into LocalModel::pieces
	uint32_t scriptPieceIndex; // index of this piece into UnitScript::pieces

	const S3DModelPiece* original;
	LocalModelPiece* parent;
	LocalModel* localModel;

	std::vector<LocalModelPiece*> children;
	std::vector<uint32_t> lodDispLists;
};