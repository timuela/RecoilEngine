#include "LocalModelPiece.hpp"

#include "3DModelPiece.hpp"
#include "LocalModel.hpp"
#include "Rendering/GL/myGL.h"
#include "System/Misc/TracyDefs.h"
#include "Rendering/Models/LocalModelPieceComponents.hpp"

CR_BIND(LocalModelPiece, )
CR_REG_METADATA(LocalModelPiece, (
	CR_MEMBER(prevModelSpaceTra),
	//CR_MEMBER(pos),
	//CR_MEMBER(rot),
	CR_MEMBER(dir),
	CR_MEMBER(colvol),
	CR_MEMBER(scriptSetVisible),
	CR_MEMBER(lmodelPieceIndex),
	CR_MEMBER(scriptPieceIndex),
	CR_MEMBER(parent),
	CR_MEMBER(localModel),
	CR_MEMBER(children),

	// reload
	CR_IGNORED(original),

	CR_MEMBER(wasUpdated),
	CR_MEMBER(modelSpaceTra),
	CR_MEMBER(pieceSpaceTra),
	CR_MEMBER(modelSpaceMat),

	CR_IGNORED(lodDispLists) //FIXME GL idx!
))

/** ****************************************************************************************************
 * LocalModelPiece
 */

LocalModelPiece::LocalModelPiece(const S3DModelPiece* piece)
	: colvol(piece->GetCollisionVolume())
	, wasUpdated{ true }

	, scriptSetVisible(true)

	, lmodelPieceIndex(-1)
	, scriptPieceIndex(-1)

	, original(piece)
	, parent(nullptr) // set later
{
	assert(piece != nullptr);

	using namespace LMP;
	auto [pos, rot] = lmpe.Add<Position, Rotation>();
	pos = piece->offset;

	lmpe.Add<
		PositionNoInterpolation, RotationNoInterpolation, ScalingNoInterpolation,
		ParentRelationship,
		Dirty
	>();

	dir = piece->GetEmitDir();

	pieceSpaceTra = CalcPieceSpaceTransform(pos, rot, original->scale);
	prevModelSpaceTra = Transform{ };

	children.reserve(piece->children.size());
}

void LocalModelPiece::AddChild(LocalModelPiece* c)
{
	assert(c != nullptr);

	using namespace LMP;
	auto& cr = c->GetLocalModelPieceEntity().Get<ParentRelationship>();
	cr.parent = lmpe.EntityID();

	c->parent = this;
	children.push_back(c);
}

void LocalModelPiece::RemoveChild(LocalModelPiece* c)
{
	assert(c != nullptr);

	using namespace LMP;
	auto& cr = c->GetLocalModelPieceEntity().Get<ParentRelationship>();
	cr.parent = ECS::NullEntity;

	c->parent = nullptr;

	children.erase(std::find(children.begin(), children.end(), c));
}

// note const is fake here
void LocalModelPiece::SetDirty(bool state) const {
	RECOIL_DETAILED_TRACY_ZONE;

	using namespace LMP;

	// hack to override the constness
	auto& dirty = const_cast<LocalModelPieceEntity*>(&lmpe)->Get<Dirty>();
	dirty = state;

	for (LocalModelPiece* child: children) {
		if (child->GetDirty())
			continue;

		child->SetDirty(state);
	}
}

bool LocalModelPiece::GetDirty() const
{
	using namespace LMP;

	return lmpe.Get<Dirty>();
}

const float3& LocalModelPiece::GetPosition() const {
	using namespace LMP;
	return lmpe.Get<Position>();
}

const float3& LocalModelPiece::GetRotation() const {
	using namespace LMP;
	return lmpe.Get<Rotation>();
}

void LocalModelPiece::SetPosition(const float3& p) {
	RECOIL_DETAILED_TRACY_ZONE;
	if (GetBlockScriptAnims())
		return;

	using namespace LMP;
	auto& pos = lmpe.Get<Position>();
	if (!GetDirty() && !p.same(pos)) {
		SetDirty(true);
		assert(localModel);
		localModel->SetBoundariesNeedsRecalc();
	}

	pos = p;
}

void LocalModelPiece::SetRotation(const float3& r) {
	RECOIL_DETAILED_TRACY_ZONE;
	if (GetBlockScriptAnims())
		return;

	using namespace LMP;
	auto& rot = lmpe.Get<Rotation>();
	if (!GetDirty() && !r.same(rot)) {
		SetDirty(true);
		assert(localModel);
		localModel->SetBoundariesNeedsRecalc();
	}

	rot = r;
}

void LocalModelPiece::SetRotationNoInterpolation(bool noInterpolate)
{
	using namespace LMP;
	lmpe.Set<RotationNoInterpolation>(noInterpolate);
}

void LocalModelPiece::SetPositionNoInterpolation(bool noInterpolate)
{
	using namespace LMP;
	lmpe.Set<PositionNoInterpolation>(noInterpolate);
}

void LocalModelPiece::SetScalingNoInterpolation(bool noInterpolate)
{
	using namespace LMP;
	lmpe.Set<ScalingNoInterpolation>(noInterpolate);
}

void LocalModelPiece::SetBlockScriptAnims(bool b)
{
	using namespace LMP;

	if (b) {
		lmpe.Add<BlockScriptAnims>();
	}
	else if (lmpe.Has<BlockScriptAnims>()) {
		lmpe.Remove<BlockScriptAnims>();
	}
}

bool LocalModelPiece::GetBlockScriptAnims() const
{
	using namespace LMP;

	return lmpe.Has<BlockScriptAnims>();
}

void LocalModelPiece::ResetWasUpdated() const
{
	// wasUpdated needs to trigger twice because otherwise
	// once all animation of piece stops and dirty is no longer triggered
	// UpdateObjectTrasform() would exit too early and wouldn't update
	// prevModelSpaceTra, causing the piece transform to jerk between the
	// up-to-date modelSpaceTra and stale prevModelSpaceTra
	// By passing values from right to left we make sure to trigger
	// wasUpdated[0] || wasUpdated[1] at least twice after such situation
	// happens, thus uploading prevModelSpaceTra in UpdateObjectTrasform() too
	wasUpdated[1] = std::exchange(wasUpdated[0], false);

	// use this call to also reset noInterpolation
	using namespace LMP;
	// hack to override the constness
	auto [pni, rni, sni] = const_cast<LocalModelPieceEntity*>(&lmpe)->Get<PositionNoInterpolation, RotationNoInterpolation, ScalingNoInterpolation>();
	pni = false;
	rni = false;
	sni = false;
}

bool LocalModelPiece::SetPieceSpaceMatrix(const CMatrix44f& mat)
{
	using namespace LMP;

	const bool goodMatrixProvided = mat.GetX() != ZeroVector;
	// In case the matrix is good - block the animations and use the matrix data instead
	// In case the matrix is not good - do not block the animations
	SetBlockScriptAnims(goodMatrixProvided);

	if (!goodMatrixProvided)
		return false;

	pieceSpaceTra = Transform::FromMatrix(mat);

	// neither of these are used outside of animation scripts, and
	// GetEulerAngles wants a matrix created by PYR rotation while
	// <rot> is YPR
	// pos = mat.GetPos();
	// rot = mat.GetEulerAnglesLftHand();
	return true;
}

const Transform& LocalModelPiece::GetModelSpaceTransform() const
{
	if (GetDirty())
		UpdateParentMatricesRec();

	return modelSpaceTra;
}

const CMatrix44f& LocalModelPiece::GetModelSpaceMatrix() const
{
	if (GetDirty())
		UpdateParentMatricesRec();

	return modelSpaceMat;
}

void LocalModelPiece::SetScriptVisible(bool b)
{
	scriptSetVisible = b;
	wasUpdated[0] = true; //update for current frame
}

void LocalModelPiece::SavePrevModelSpaceTransform()
{
	prevModelSpaceTra = GetModelSpaceTransform();
}

Transform LocalModelPiece::GetEffectivePrevModelSpaceTransform() const
{
	using namespace LMP;

	const auto [pni, rni, sni] = lmpe.Get<const PositionNoInterpolation, const RotationNoInterpolation, const ScalingNoInterpolation>();

	if (!pni && !rni && !sni)
		return prevModelSpaceTra;

	const auto& lmpTransform = GetModelSpaceTransform();
	return Transform {
		rni ? lmpTransform.r : prevModelSpaceTra.r,
		pni ? lmpTransform.t : prevModelSpaceTra.t,
		sni ? lmpTransform.s : prevModelSpaceTra.s
	};
}

void LocalModelPiece::UpdateChildTransformRec(bool updateChildTransform) const
{
	RECOIL_DETAILED_TRACY_ZONE;

	if (GetDirty()) {
		SetDirty(false);
		wasUpdated[0] = true;  //update for current frame
		updateChildTransform = true;

		using namespace LMP;
		auto [pos, rot] = lmpe.Get<Position, Rotation>();
		pieceSpaceTra = CalcPieceSpaceTransform(pos, rot, original->scale);
	}

	if (updateChildTransform) {
		if (parent != nullptr)
			modelSpaceTra = parent->modelSpaceTra * pieceSpaceTra;
		else
			modelSpaceTra = pieceSpaceTra;

		modelSpaceMat = modelSpaceTra.ToMatrix();
	}

	for (auto& child : children) {
		child->UpdateChildTransformRec(updateChildTransform);
	}
}

void LocalModelPiece::UpdateParentMatricesRec() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (parent != nullptr && parent->GetDirty())
		parent->UpdateParentMatricesRec();

	SetDirty(false);
	wasUpdated[0] = true;  //update for current frame

	using namespace LMP;
	auto [pos, rot] = lmpe.Get<Position, Rotation>();
	pieceSpaceTra = CalcPieceSpaceTransform(pos, rot, original->scale);

	if (parent != nullptr)
		modelSpaceTra = parent->modelSpaceTra * pieceSpaceTra;
	else
		modelSpaceTra = pieceSpaceTra;

	modelSpaceMat = modelSpaceTra.ToMatrix();
}

Transform LocalModelPiece::CalcPieceSpaceTransformOrig(const float3& p, const float3& r, float s) const
{
	return original->ComposeTransform(p, r, s);
}

Transform LocalModelPiece::CalcPieceSpaceTransform(const float3& p, const float3& r, float s) const
{
	if (GetBlockScriptAnims())
		return pieceSpaceTra;

	return CalcPieceSpaceTransformOrig(p, r, s);
}


void LocalModelPiece::Draw() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!scriptSetVisible)
		return;

	if (!original->HasGeometryData())
		return;

	assert(original);

	glPushMatrix();
	glMultMatrixf(GetModelSpaceMatrix());
	S3DModelHelpers::BindLegacyAttrVBOs();
	original->DrawElements();
	S3DModelHelpers::UnbindLegacyAttrVBOs();
	glPopMatrix();
}

void LocalModelPiece::DrawLOD(uint32_t lod) const
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (!scriptSetVisible)
		return;

	if (!original->HasGeometryData())
		return;

	glPushMatrix();
	glMultMatrixf(GetModelSpaceMatrix());
	if (const auto ldl = lodDispLists[lod]; ldl == 0) {
		S3DModelHelpers::BindLegacyAttrVBOs();
		original->DrawElements();
		S3DModelHelpers::UnbindLegacyAttrVBOs();
	} else {
		glCallList(ldl);
	}
	glPopMatrix();
}



void LocalModelPiece::SetLODCount(uint32_t count)
{
	RECOIL_DETAILED_TRACY_ZONE;
	// any new LOD's get null-lists first
	lodDispLists.resize(count, 0);

	for (uint32_t i = 0; i < children.size(); i++) {
		children[i]->SetLODCount(count);
	}
}


bool LocalModelPiece::GetEmitDirPos(float3& emitPos, float3& emitDir) const
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (original == nullptr)
		return false;

	// note: actually OBJECT_TO_WORLD but transform is the same
	emitPos = GetModelSpaceTransform() *        original->GetEmitPos()        * WORLD_TO_OBJECT_SPACE;
	emitDir = GetModelSpaceTransform() * float4(original->GetEmitDir(), 0.0f) * WORLD_TO_OBJECT_SPACE;
	return true;
}