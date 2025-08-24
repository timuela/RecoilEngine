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
using LocalModelPieceEntityRef = ECS::EntityReference<LocalModelPiece>;

/**
 * LocalModel
 * Instance of S3DModel. Container for the geometric properties & piece visibility status of the agent's instance of a 3d model.
 */

struct LocalModelPiece
{
	CR_DECLARE_STRUCT(LocalModelPiece)

	LocalModelPiece()
	{}
	LocalModelPiece(const S3DModelPiece* piece);

	void AddChild(LocalModelPiece* c);
	void RemoveChild(LocalModelPiece* c);
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

	Transform CalcPieceSpaceTransform(const float3& p, const float3& r, float s) const;

	// note: actually OBJECT_TO_WORLD but transform is the same
	float3 GetAbsolutePos() const { return (GetModelSpaceTransform().t * WORLD_TO_OBJECT_SPACE); }

	bool GetEmitDirPos(float3& emitPos, float3& emitDir) const;

	void SetDirty(bool state) const; //needs fake constness;
	bool GetDirty() const;
	void SetPosition(const float3& p, int onAxis); // anim-script only
	void SetRotation(const float3& r, int onAxis); // anim-script only

	void SetRotationNoInterpolation(bool noInterpolate);
	void SetPositionNoInterpolation(bool noInterpolate);
	void SetScalingNoInterpolation (bool noInterpolate);

	void SetBlockScriptAnims(bool b);
	bool GetBlockScriptAnims() const;

	bool GetWasUpdated() const;
	void ResetWasUpdated() const; /*fake*/

	bool SetPieceSpaceMatrix(const CMatrix44f& mat);

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
	const auto& GetLocalModelPieceEntity() const { return lmpe; }
	      auto& GetLocalModelPieceEntity()       { return lmpe; }
private:
	LocalModelPieceEntity lmpe;

	float3 dir;      // cached copy of original->GetEmitDir()

	mutable CMatrix44f modelSpaceMat; // same as above, except matrix

	CollisionVolume colvol;

	bool scriptSetVisible; // TODO: add (visibility) maxradius!
public:
	uint32_t lmodelPieceIndex; // index of this piece into LocalModel::pieces
	uint32_t scriptPieceIndex; // index of this piece into UnitScript::pieces

	const S3DModelPiece* original;
	LocalModelPiece* parent;
	LocalModel* localModel;

	std::vector<LocalModelPiece*> children;
	std::vector<uint32_t> lodDispLists;
};