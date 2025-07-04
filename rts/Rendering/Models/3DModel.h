/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _3DMODEL_H
#define _3DMODEL_H

#include <algorithm>
#include <array>
#include <vector>
#include <string>
#include <limits>
#include <type_traits>

#include "ModelsMemStorage.h"
#include "3DModelAnimation.hpp"
#include "Lua/LuaObjectMaterial.h"
#include "Rendering/GL/VBO.h"
#include "Sim/Misc/CollisionVolume.h"
#include "System/Matrix44f.h"
#include "System/Transform.hpp"
#include "System/type2.h"
#include "System/float4.h"
#include "System/SafeUtil.h"
#include "System/SpringMath.h"
#include "System/TemplateUtils.hpp"
#include "System/creg/creg_cond.h"

static constexpr int MAX_MODEL_OBJECTS  = 3840;
static constexpr int AVG_MODEL_PIECES   = 16; // as it used to be
static constexpr int NUM_MODEL_TEXTURES = 2;
static constexpr int NUM_MODEL_UVCHANNS = 2;
static constexpr int MAX_PIECES_PER_MODEL = std::numeric_limits<uint16_t>::max() - 1;
static constexpr int INV_PIECE_NUM = MAX_PIECES_PER_MODEL + 1;

static constexpr float3 DEF_MIN_SIZE( 10000.0f,  10000.0f,  10000.0f);
static constexpr float3 DEF_MAX_SIZE(-10000.0f, -10000.0f, -10000.0f);

enum ModelType {
	MODELTYPE_3DO    = 0,
	MODELTYPE_S3O    = 1,
	MODELTYPE_ASS    = 2, // Assimp
	MODELTYPE_CNT    = 3  // count
};

struct CollisionVolume;
struct S3DModel;
struct S3DModelPiece;
struct LocalModel;
struct LocalModelPiece;
struct Transform;


struct SVertexData {
	SVertexData() {
		pos = float3{};
		normal = UpVector;
		sTangent = RgtVector;
		tTangent = float3{};
		texCoords[0] = float2{};
		texCoords[1] = float2{};
		// boneIDs is initialized afterwards
		boneIDsLow  = DEFAULT_BONEIDS_LOW;
		boneWeights = DEFAULT_BONEWEIGHTS;
		boneIDsHigh = DEFAULT_BONEIDS_HIGH;
	}
	SVertexData(
		const float3& p,
		const float3& n,
		const float3& s,
		const float3& t,
		const float2& uv0,
		const float2& uv1)
	{
		pos = p;
		normal = n;
		sTangent = s;
		tTangent = t;
		texCoords[0] = uv0;
		texCoords[1] = uv1;
		// boneIDs is initialized afterwards
		boneIDsLow  = DEFAULT_BONEIDS_LOW;
		boneWeights = DEFAULT_BONEWEIGHTS;
		boneIDsHigh = DEFAULT_BONEIDS_HIGH;
	}

	static constexpr std::array<uint8_t, 4> DEFAULT_BONEIDS_HIGH = { 255, 255, 255, 255 };
	static constexpr std::array<uint8_t, 4> DEFAULT_BONEIDS_LOW = { 255, 255, 255, 255 };
	static constexpr std::array<uint8_t, 4> DEFAULT_BONEWEIGHTS = { 255, 0  ,   0,   0 };
	static constexpr uint16_t INVALID_BONEID = 0xFFFF;
	static constexpr size_t MAX_BONES_PER_VERTEX = 4;

	float3 pos;
	float3 normal;
	float3 sTangent;
	float3 tTangent;
	float2 texCoords[NUM_MODEL_UVCHANNS];

	std::array<uint8_t, MAX_BONES_PER_VERTEX> boneIDsLow;
	std::array<uint8_t, MAX_BONES_PER_VERTEX> boneWeights;
	std::array<uint8_t, MAX_BONES_PER_VERTEX> boneIDsHigh;

	template <Concepts::HasSizeAndData C>
	void SetBones(const C& bi) {
		static_assert(std::is_same_v<typename C::value_type, std::pair<uint16_t, float>>);
		assert(bi.size() >= MAX_BONES_PER_VERTEX);

		boneIDsLow = {
			static_cast<uint8_t>((bi[0].first     ) & 0xFF),
			static_cast<uint8_t>((bi[1].first     ) & 0xFF),
			static_cast<uint8_t>((bi[2].first     ) & 0xFF),
			static_cast<uint8_t>((bi[3].first     ) & 0xFF)
		};

		boneIDsHigh = {
			static_cast<uint8_t>((bi[0].first >> 8) & 0xFF),
			static_cast<uint8_t>((bi[1].first >> 8) & 0xFF),
			static_cast<uint8_t>((bi[2].first >> 8) & 0xFF),
			static_cast<uint8_t>((bi[3].first >> 8) & 0xFF)
		};

		// calc sumWeight to normalize the bone weights to cumulative 1.0f
		float sumWeight = bi[0].second + bi[1].second + bi[2].second + bi[3].second;
		sumWeight = (sumWeight <= 0.0f) ? 1.0f : sumWeight;

		boneWeights = {
			(spring::SafeCast<uint8_t>(math::round(bi[0].second / sumWeight * 255.0f))),
			(spring::SafeCast<uint8_t>(math::round(bi[1].second / sumWeight * 255.0f))),
			(spring::SafeCast<uint8_t>(math::round(bi[2].second / sumWeight * 255.0f))),
			(spring::SafeCast<uint8_t>(math::round(bi[3].second / sumWeight * 255.0f)))
		};


	}

	void TransformBy(const Transform& transform);
};

struct S3DModelPiecePart {
public:
	struct RenderData {
		float3 dir;
		uint32_t indexStart;
		uint32_t indexCount;
	};

	static const int SHATTER_MAX_PARTS  = 10;
	static const int SHATTER_VARIATIONS = 2;

	std::vector<RenderData> renderData;
};

struct S3DModelHelpers {
	static void BindLegacyAttrVBOs();
	static void UnbindLegacyAttrVBOs();
};


/**
 * S3DModel
 * A 3D model definition. Holds geometry (vertices/normals) and texture data as well as the piece tree.
 * The S3DModel is static and shouldn't change once created, instead a LocalModel is used by each agent.
 */

struct S3DModelPiece {
	S3DModelPiece() = default;

	virtual void Clear() {
		name.clear();
		children.clear();

		for (S3DModelPiecePart& p : shatterParts) {
			p.renderData.clear();
		}

		vertices.clear();
		indices.clear();
		shatterIndices.clear();

		parent = nullptr;
		colvol = {};

		bposeTransform.LoadIdentity();
		bakedTransform.LoadIdentity();

		offset = ZeroVector;
		goffset = ZeroVector;
		scale = 1.0f;

		mins = DEF_MIN_SIZE;
		maxs = DEF_MAX_SIZE;

		vertIndex = ~0u;
		indxStart = ~0u;
		indxCount = ~0u;

		hasBakedTra = false;
	}

	virtual float3 GetEmitPos() const;
	virtual float3 GetEmitDir() const;

	// internal use
	const float3& GetVertexPos(const int idx) const { return vertices[idx].pos; }
	const float3& GetNormal(const int idx) const { return vertices[idx].normal; }

	virtual void PostProcessGeometry(uint32_t pieceIndex);


	void DrawElements(GLuint prim = GL_TRIANGLES) const;
	static void DrawShatterElements(uint32_t vboIndxStart, uint32_t vboIndxCount, GLuint prim = GL_TRIANGLES);

	bool HasBackedTra() const { return hasBakedTra; }
public:
	void DrawStaticLegacy(bool bind, bool bindPosMat) const;
	void DrawStaticLegacyRec() const;

	void CreateShatterPieces();
	void Shatter(float, int, int, int, const float3, const float3, const CMatrix44f&) const;

	void SetPieceTransform(const Transform& parentTra);
	void SetBakedTransform(const Transform& tra) {
		bakedTransform = tra;
		hasBakedTra = !tra.IsIdentity();
	}

	Transform ComposeTransform(const float3& t, const float3& r, float s) const;

	void SetCollisionVolume(const CollisionVolume& cv) { colvol = cv; }
	const CollisionVolume* GetCollisionVolume() const { return &colvol; }
	      CollisionVolume* GetCollisionVolume()       { return &colvol; }

	bool HasGeometryData() const { return indices.size() >= 3; }
	void SetParentModel(S3DModel* model_) { model = model_; }
	const S3DModel* GetParentModel() const { return model; }

	void ReleaseShatterIndices();

	const std::vector<SVertexData>& GetVerticesVec() const { return vertices; }
	const std::vector<uint32_t>& GetIndicesVec() const { return indices; }
	const std::vector<uint32_t>& GetShatterIndicesVec() const { return shatterIndices; }

	std::vector<SVertexData>& GetVerticesVec() { return vertices; }
	std::vector<uint32_t>& GetIndicesVec() { return indices; }
	std::vector<uint32_t>& GetShatterIndicesVec() { return shatterIndices; }
private:
	void CreateShatterPiecesVariation(int num);
public:
	std::string name;
	std::vector<S3DModelPiece*> children;
	std::array<S3DModelPiecePart, S3DModelPiecePart::SHATTER_VARIATIONS> shatterParts;

	S3DModelPiece* parent = nullptr;
	CollisionVolume colvol;

	Transform bposeTransform;    /// bind-pose transform, including baked rots
	Transform bakedTransform;    /// baked local-space rotations

	float3 offset;      /// local (piece-space) offset wrt. parent piece
	float3 goffset;     /// global (model-space) offset wrt. root piece
	float scale{1.0f};  /// baked uniform scaling factor (assimp-only)

	float3 mins = DEF_MIN_SIZE;
	float3 maxs = DEF_MAX_SIZE;

	uint32_t vertIndex = ~0u; // global vertex number offset
	uint32_t indxStart = ~0u; // global Index VBO offset
	uint32_t indxCount = ~0u;
protected:
	std::vector<SVertexData> vertices;
	std::vector<uint32_t> indices;
	std::vector<uint32_t> shatterIndices;

	S3DModel* model;

	bool hasBakedTra;
public:
	friend class CAssParser;
};


struct S3DModel
{
	enum LoadStatus {
		NOTLOADED,
		LOADING,
		LOADED
	};
	S3DModel()
		: id(-1)
		, numPieces(0)
		, textureType(-1)

		, indxStart(~0u)
		, indxCount(0u)

		, type(MODELTYPE_CNT)

		, radius(0.0f)
		, height(0.0f)

		, mins(DEF_MIN_SIZE)
		, maxs(DEF_MAX_SIZE)
		, relMidPos(ZeroVector)

		, loadStatus(NOTLOADED)
		, uploaded(false)

		, animationMap{}

		, traAlloc(ScopedTransformMemAlloc())
	{}

	S3DModel(const S3DModel& m) = delete;
	S3DModel(S3DModel&& m) noexcept { *this = std::move(m); }

	S3DModel& operator = (const S3DModel& m) = delete;
	S3DModel& operator = (S3DModel&& m) noexcept {
		name    = std::move(m.name   );
		texs[0] = std::move(m.texs[0]);
		texs[1] = std::move(m.texs[1]);

		id = m.id;
		numPieces = m.numPieces;
		textureType = m.textureType;

		type = m.type;

		radius = m.radius;
		height = m.height;

		mins = m.mins;
		maxs = m.maxs;
		relMidPos = m.relMidPos;

		indxStart = m.indxStart;
		indxCount = m.indxCount;

		pieceObjects.swap(m.pieceObjects);

		for (auto po : pieceObjects)
			po->SetParentModel(this);

		loadStatus = m.loadStatus;
		uploaded = m.uploaded;

		animationMap = std::move(m.animationMap);

		std::swap(traAlloc, m.traAlloc);

		return *this;
	}

	      S3DModelPiece* FindPiece(const std::string& name);
	const S3DModelPiece* FindPiece(const std::string& name) const;
	size_t FindPieceOffset(const std::string& name) const;

	S3DModelPiece* GetPiece(size_t i) const { assert(i < pieceObjects.size()); return pieceObjects[i]; }
	S3DModelPiece* GetRootPiece() const { return (GetPiece(GetRootPieceIndex())); }
	size_t GetRootPieceIndex() const { return 0; }

	void AddPiece(S3DModelPiece* p) { pieceObjects.push_back(p); }
	void DrawStatic() const {
		S3DModelHelpers::BindLegacyAttrVBOs();

		// draw pieces in their static bind-pose (ie. without script-transforms)
		for (const S3DModelPiece* pieceObj : pieceObjects) {
			pieceObj->DrawStaticLegacy(false, true);
		}

		S3DModelHelpers::UnbindLegacyAttrVBOs();
	}

	void SetPieceMatrices();

	void FlattenPieceTree(S3DModelPiece* root);

	void UpdatePiecesMinMaxExtents();

	// default values set by parsers; radius is also cached in WorldObject::drawRadius (used by projectiles)
	float CalcDrawRadius() const { return ((maxs - mins).Length() * 0.5f); }
	float CalcDrawHeight() const { return (maxs.y - mins.y); }
	float GetDrawRadius() const { return radius; }
	float GetDrawHeight() const { return height; }

	float3 CalcDrawMidPos() const { return ((maxs + mins) * 0.5f); }
	float3 GetDrawMidPos() const { return relMidPos; }

	const ScopedTransformMemAlloc& GetTransformAlloc() const { return traAlloc; }
public:
	std::string name;
	std::array<std::string, NUM_MODEL_TEXTURES> texs;

	// flattened tree; pieceObjects[0] is the root
	std::vector<S3DModelPiece*> pieceObjects;

	int id;                     /// unsynced ID, starting with 1
	int numPieces;
	int textureType;            /// FIXME: MAKE S3O ONLY (0 = 3DO, otherwise S3O or ASSIMP)

	uint32_t indxStart; //global VBO offset, size data
	uint32_t indxCount;

	ModelType type;

	float radius;
	float height;

	float3 mins;
	float3 maxs;
	float3 relMidPos;

	LoadStatus loadStatus;
	bool uploaded;

	ModelAnimation::Map animationMap;
private:
	ScopedTransformMemAlloc traAlloc;
};



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
	{}
	LocalModelPiece(const S3DModelPiece* piece);

	void AddChild(LocalModelPiece* c) { children.push_back(c); }
	void RemoveChild(LocalModelPiece* c) { children.erase(std::find(children.begin(), children.end(), c)); }
	void SetParent(LocalModelPiece* p) { parent = p; }
	void SetLocalModel(LocalModel* lm) { localModel = lm; }

	void SetLModelPieceIndex(unsigned int idx) { lmodelPieceIndex = idx; }
	void SetScriptPieceIndex(unsigned int idx) { scriptPieceIndex = idx; }
	unsigned int GetLModelPieceIndex() const { return lmodelPieceIndex; }
	unsigned int GetScriptPieceIndex() const { return scriptPieceIndex; }

	void Draw() const;
	void DrawLOD(unsigned int lod) const;
	void SetLODCount(unsigned int count);


	// on-demand functions
	void UpdateChildTransformRec(bool updateChildMatrices) const;
	void UpdateParentMatricesRec() const;

	auto CalcPieceSpaceTransformOrig(const float3& p, const float3& r, float s) const { return original->ComposeTransform(p, r, s); }
	auto CalcPieceSpaceTransform(const float3& p, const float3& r, float s) const {
		if (blockScriptAnims)
			return pieceSpaceTra;

		return CalcPieceSpaceTransformOrig(p, r, s);
	}

	// note: actually OBJECT_TO_WORLD but transform is the same
	float3 GetAbsolutePos() const { return (GetModelSpaceTransform().t * WORLD_TO_OBJECT_SPACE); }

	bool GetEmitDirPos(float3& emitPos, float3& emitDir) const;


	void SetDirty();
	void SetPosOrRot(const float3& src, float3& dst); // anim-script only
	void SetPosition(const float3& p) { SetPosOrRot(p, pos); } // anim-script only
	void SetRotation(const float3& r) { SetPosOrRot(r, rot); } // anim-script only

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

	const float3& GetPosition() const { return pos; }
	const float3& GetRotation() const { return rot; }

	const float3& GetDirection() const { return dir; }

	const Transform&  GetModelSpaceTransform() const;
	const CMatrix44f& GetModelSpaceMatrix()    const;

	const CollisionVolume* GetCollisionVolume() const { return &colvol; }
	      CollisionVolume* GetCollisionVolume()       { return &colvol; }

	bool GetScriptVisible() const { return scriptSetVisible; }
	void SetScriptVisible(bool b);

	void SavePrevModelSpaceTransform();
	const auto& GetPrevModelSpaceTransform() const { return prevModelSpaceTra; }
private:
	Transform prevModelSpaceTra;

	float3 pos;      // translation relative to parent LMP, *INITIALLY* equal to original->offset
	float3 rot;      // orientation relative to parent LMP, in radians (updated by scripts)
	float3 dir;      // cached copy of original->GetEmitDir()

	mutable Transform pieceSpaceTra;  // transform relative to parent LMP (SYNCED), combines <pos> and <rot>
	mutable Transform modelSpaceTra;  // transform relative to root LMP (SYNCED), chained pieceSpaceMat's
	mutable CMatrix44f modelSpaceMat; // same as above, except matrix

	CollisionVolume colvol;

	mutable std::array<bool, 2> wasUpdated; // currFrame, prevFrame
	mutable bool dirty;
	bool scriptSetVisible; // TODO: add (visibility) maxradius!
public:
	bool blockScriptAnims; // if true, Set{Position,Rotation} are ignored for this piece

	unsigned int lmodelPieceIndex; // index of this piece into LocalModel::pieces
	unsigned int scriptPieceIndex; // index of this piece into UnitScript::pieces

	const S3DModelPiece* original;
	LocalModelPiece* parent;
	LocalModel* localModel;

	std::vector<LocalModelPiece*> children;
	std::vector<unsigned int> lodDispLists;
};


struct LocalModel
{
	CR_DECLARE_STRUCT(LocalModel)

	LocalModel() {}
	~LocalModel() { pieces.clear(); }


	bool HasPiece(unsigned int i) const { return (i < pieces.size()); }
	bool Initialized() const { return (!pieces.empty()); }

	const LocalModelPiece* GetPiece(unsigned int i)  const { assert(HasPiece(i)); return &pieces[i]; }
	      LocalModelPiece* GetPiece(unsigned int i)        { assert(HasPiece(i)); return &pieces[i]; }

	const LocalModelPiece* GetRoot() const { return (GetPiece(0)); }
	const CollisionVolume* GetBoundingVolume() const { return &boundingVolume; }

	const LuaObjectMaterialData* GetLuaMaterialData() const { return &luaMaterialData; }
	      LuaObjectMaterialData* GetLuaMaterialData()       { return &luaMaterialData; }

	const float3 GetRelMidPos() const { return (boundingVolume.GetOffsets()); }

	// raw forms, the piece-index must be valid
	const float3 GetRawPiecePos(int pieceIdx) const { return pieces[pieceIdx].GetAbsolutePos(); }

	// used by all SolidObject's; accounts for piece movement
	float GetDrawRadius() const { return (boundingVolume.GetBoundingRadius()); }


	void Draw() const {
		if (!luaMaterialData.Enabled()) {
			DrawPieces();
			return;
		}

		DrawPiecesLOD(luaMaterialData.GetCurrentLOD());
	}

	void SetModel(const S3DModel* model, bool initialize = true);
	void SetLODCount(unsigned int lodCount);
	void UpdateBoundingVolume();

	void GetBoundingBoxVerts(std::vector<float3>& verts) const {
		verts.resize(8 + 2); GetBoundingBoxVerts(&verts[0]);
	}

	void GetBoundingBoxVerts(float3* verts) const {
		const float3 bbMins = GetRelMidPos() - boundingVolume.GetHScales();
		const float3 bbMaxs = GetRelMidPos() + boundingVolume.GetHScales();

		// bottom
		verts[0] = float3(bbMins.x,  bbMins.y,  bbMins.z);
		verts[1] = float3(bbMaxs.x,  bbMins.y,  bbMins.z);
		verts[2] = float3(bbMaxs.x,  bbMins.y,  bbMaxs.z);
		verts[3] = float3(bbMins.x,  bbMins.y,  bbMaxs.z);
		// top
		verts[4] = float3(bbMins.x,  bbMaxs.y,  bbMins.z);
		verts[5] = float3(bbMaxs.x,  bbMaxs.y,  bbMins.z);
		verts[6] = float3(bbMaxs.x,  bbMaxs.y,  bbMaxs.z);
		verts[7] = float3(bbMins.x,  bbMaxs.y,  bbMaxs.z);
		// extrema
		verts[8] = bbMins;
		verts[9] = bbMaxs;
	}

	void SetBoundariesNeedsRecalc()       { needsBoundariesRecalc = true; }
	bool GetBoundariesNeedsRecalc() const { return needsBoundariesRecalc; }
private:
	LocalModelPiece* CreateLocalModelPieces(const S3DModelPiece* mpParent);

	void DrawPieces() const;
	void DrawPiecesLOD(unsigned int lod) const;

public:
	std::vector<LocalModelPiece> pieces;

private:
	// object-oriented box; accounts for piece movement
	CollisionVolume boundingVolume;

	// custom Lua-set material this model should be rendered with
	LuaObjectMaterialData luaMaterialData;

	bool needsBoundariesRecalc = true;
};

#endif /* _3DMODEL_H */
