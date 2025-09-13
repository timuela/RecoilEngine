#include "LocalModelPiece.hpp"

#include "3DModelPiece.hpp"
#include "LocalModel.hpp"
#include "Rendering/GL/myGL.h"
#include "System/Misc/TracyDefs.h"
#include "Rendering/Models/LocalModelPieceComponents.hpp"

CR_BIND(LocalModelPiece, )
CR_REG_METADATA(LocalModelPiece, (
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

	CR_MEMBER(modelSpaceMat),

	CR_IGNORED(lodDispLists) //FIXME GL idx!
))

/** ****************************************************************************************************
 * LocalModelPiece
 */

LocalModelPiece::LocalModelPiece(const S3DModelPiece* piece)
	: colvol(piece->GetCollisionVolume())
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
		UpdateFlags, DirtyFlag
	>();

	lmpe.Add<OriginalBakedTransform>(piece->bakedTransform);
	lmpe.Add<ParentChildrenRelationship>();

	dir = piece->GetEmitDir();

	lmpe.Add<PieceSpaceTransform>(CalcPieceSpaceTransform(pos, rot, piece->scale));
	lmpe.Add<CurrModelSpaceTransform, PrevModelSpaceTransform>();
	lmpe.Add<HasAnimation>(); lmpe.Disable<HasAnimation>();
	lmpe.Add<BlockScriptAnims>(); lmpe.Disable<BlockScriptAnims>();

	children.reserve(piece->children.size());
}

void LocalModelPiece::AddChild(LocalModelPiece* c)
{
	assert(c != nullptr);

	using namespace LMP;

	auto& cLmpe = c->GetLocalModelPieceEntity();
	auto cEntId = cLmpe.EntityID();
	auto& cRel = cLmpe.Get<ParentChildrenRelationship>();
	cRel.parent = lmpe.EntityID();

	auto& pRel = lmpe.Get<ParentChildrenRelationship>();
	auto& chVec = pRel.children;

	chVec.insert(std::lower_bound(chVec.begin(), chVec.end(), cEntId), cEntId);

	c->parent = this;
	children.push_back(c);
}

void LocalModelPiece::RemoveChild(LocalModelPiece* c)
{
	assert(c != nullptr);

	using namespace LMP;

	auto& cLmpe = c->GetLocalModelPieceEntity();
	auto& cRel = cLmpe.Get<ParentChildrenRelationship>();
	cRel.parent = Flecs::NullEntity;

	auto& pRel = lmpe.Get<ParentChildrenRelationship>();
	auto it = std::find(pRel.children.begin(), pRel.children.end(), cLmpe.EntityID());
	if (it != pRel.children.end())
		pRel.children.erase(it);

	c->parent = nullptr;
	children.erase(std::find(children.begin(), children.end(), c));
}

// note const is fake here
void LocalModelPiece::SetDirty(bool state) const {
	RECOIL_DETAILED_TRACY_ZONE;

	using namespace LMP;

	// hack to override the constness
	auto& df = lmpe.GetMutable<DirtyFlag>();
	df = state;

	for (LocalModelPiece* child: children) {
		if (child->GetDirty())
			continue;

		child->SetDirty(state);
	}
}

bool LocalModelPiece::GetDirty() const
{
	using namespace LMP;

	return lmpe.Get<const DirtyFlag>().value;
}

const float3& LocalModelPiece::GetPosition() const {
	using namespace LMP;
	return lmpe.Get<const Position>();
}

const float3& LocalModelPiece::GetRotation() const {
	using namespace LMP;
	return lmpe.Get<const Rotation>();
}

void LocalModelPiece::SetPosition(const float3& p, int onAxis) {
	RECOIL_DETAILED_TRACY_ZONE;
	if (GetBlockScriptAnims())
		return;

	using namespace LMP;
	auto& pos = lmpe.Get<Position>();
	if (!GetDirty() && !(p[onAxis] == pos.value[onAxis])) {
		SetDirty(true);
		assert(localModel);
		localModel->SetBoundariesNeedsRecalc();
	}

	pos.value[onAxis] = p[onAxis];
}

void LocalModelPiece::SetRotation(const float3& r, int onAxis) {
	RECOIL_DETAILED_TRACY_ZONE;
	if (GetBlockScriptAnims())
		return;

	using namespace LMP;
	auto& rot = lmpe.Get<Rotation>();
	if (!GetDirty() && !(r[onAxis] == rot.value[onAxis])) {
		SetDirty(true);
		assert(localModel);
		localModel->SetBoundariesNeedsRecalc();
	}

	rot.value[onAxis] = r[onAxis];
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
		lmpe.Enable<BlockScriptAnims>();
	}
	else {
		lmpe.Disable<BlockScriptAnims>();
	}
}

bool LocalModelPiece::GetBlockScriptAnims() const
{
	using namespace LMP;

	return lmpe.Enabled<BlockScriptAnims>();
}

bool LocalModelPiece::GetWasUpdated() const
{
	using namespace LMP;

	auto& wasUpdated = lmpe.Get<UpdateFlags>();
	return wasUpdated.forCurrFrame || wasUpdated.forPrevFrame;
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
	using namespace LMP;

	auto& wasUpdated = lmpe.GetMutable<UpdateFlags>();
	wasUpdated.forPrevFrame = std::exchange(wasUpdated.forCurrFrame, false);

	// use this call to also reset noInterpolation

	// hack to override the constness
	auto [pni, rni, sni] = lmpe.GetMutable<PositionNoInterpolation, RotationNoInterpolation, ScalingNoInterpolation>();
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

	lmpe.Set<PieceSpaceTransform>(Transform::FromMatrix(mat));

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

	using namespace LMP;
	return lmpe.Get<const CurrModelSpaceTransform>();
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

	using namespace LMP;

	auto& wasUpdated = lmpe.Get<UpdateFlags>();
	wasUpdated.forCurrFrame = true; //update for current frame
}

void LocalModelPiece::SavePrevModelSpaceTransform()
{
	using namespace LMP;
	lmpe.Set<PrevModelSpaceTransform>(GetModelSpaceTransform());
}

Transform LocalModelPiece::GetEffectivePrevModelSpaceTransform() const
{
	using namespace LMP;

	const auto [pni, rni, sni, prevModelSpaceTra] = lmpe.Get<const PositionNoInterpolation, const RotationNoInterpolation, const ScalingNoInterpolation, const PrevModelSpaceTransform>();

	if (!pni && !rni && !sni)
		return prevModelSpaceTra;

	const auto& lmpTransform = GetModelSpaceTransform();
	return Transform {
		rni ? lmpTransform.r : prevModelSpaceTra.value.r,
		pni ? lmpTransform.t : prevModelSpaceTra.value.t,
		sni ? lmpTransform.s : prevModelSpaceTra.value.s
	};
}

void LocalModelPiece::UpdateChildTransformRec(bool updateChildTransform) const
{
	RECOIL_DETAILED_TRACY_ZONE;
	using namespace LMP;

	auto& pieceSpaceTra = lmpe.GetMutable<PieceSpaceTransform>();

	if (GetDirty()) {
		SetDirty(false);
		auto& wasUpdated = lmpe.GetMutable<UpdateFlags>();
		wasUpdated.forCurrFrame = true;  //update for current frame
		updateChildTransform = true;

		auto [pos, rot] = lmpe.Get<Position, Rotation>();
		pieceSpaceTra = CalcPieceSpaceTransform(pos, rot, original->scale);
	}

	if (updateChildTransform) {
		auto [modelSpaceTra, rhl] = lmpe.GetMutable<
			CurrModelSpaceTransform,
			const ParentChildrenRelationship
		>();

		if (rhl.parent != Flecs::NullEntity) {
			const LocalModelPieceEntityRef pLmpe(rhl.parent);
			const auto& pModelSpaceTra = pLmpe.Get<CurrModelSpaceTransform>();
			modelSpaceTra = pModelSpaceTra * pieceSpaceTra;
		}
		else {
			modelSpaceTra = pieceSpaceTra;
		}

		modelSpaceMat = modelSpaceTra.value.ToMatrix();
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
	using namespace LMP;

	auto& wasUpdated = lmpe.GetMutable<UpdateFlags>();
	wasUpdated.forCurrFrame = true;  //update for current frame

	auto [pos, rot, modelSpaceTra, pieceSpaceTra, rel] = lmpe.GetMutable<
		const Position,
		const Rotation,
		CurrModelSpaceTransform,
		PieceSpaceTransform,
		const ParentChildrenRelationship
	>();

	pieceSpaceTra = CalcPieceSpaceTransform(pos, rot, original->scale);

	if (rel.parent != Flecs::NullEntity) {
		LocalModelPieceEntityRef pLmpe(rel.parent);
		const auto& pModelSpaceTra = pLmpe.Get<CurrModelSpaceTransform>();
		modelSpaceTra = pModelSpaceTra * pieceSpaceTra;
	}
	else {
		modelSpaceTra = pieceSpaceTra;
	}

	modelSpaceMat = modelSpaceTra.value.ToMatrix();
}

Transform LocalModelPiece::CalcPieceSpaceTransform(const float3& p, const float3& r, float s) const
{
	using namespace LMP;
	if (GetBlockScriptAnims())
		return lmpe.Get<const PieceSpaceTransform>();

	return original->ComposeTransform(p, r, s);
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