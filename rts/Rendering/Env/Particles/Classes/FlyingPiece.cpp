/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#include "FlyingPiece.h"

#include "Game/GlobalUnsynced.h"
#include "Map/Ground.h"
#include "Map/MapInfo.h"
#include "Rendering/GlobalRendering.h"
#include "Rendering/Common/ModelDrawerHelpers.h"
#include "Rendering/Units/UnitDrawer.h"
#include "Rendering/Models/3DModelPiece.hpp"
#include "Rendering/Models/3DModelMisc.hpp"
#include "Rendering/Textures/S3OTextureHandler.h"
#include "System/SpringMath.h"

#include "System/Misc/TracyDefs.h"


static const float EXPLOSION_SPEED = 2.f;

//TODO: add to creg

/////////////////////////////////////////////////////////////////////
/// NEW S3O,ASSIMP,... IMPLEMENTATION
///

FlyingPiece::FlyingPiece(
	const S3DModelPiece* _piece,
	const CMatrix44f& _pieceMatrix,
	const float3 pos,
	const float3 speed,
	const float2 _pieceParams, // (.x=radius, .y=chance)
	const int2 _renderParams // (.x=texType, .y=team)
)
: pos0(pos)
, bposeMatrix(_pieceMatrix)
, age(0)
, piece(_piece)
{
	assert(piece->GetIndicesVec().size() % 3 == 0); // only triangles

	InitCommon(pos, speed, _pieceParams.x, _renderParams.y, _renderParams.x);

	const S3DModelPiecePart& shatterPiecePart = piece->shatterParts[guRNG.NextInt(piece->shatterParts.size())];
	const auto& shatterPieceData = shatterPiecePart.renderData;

	splitterParts.reserve(shatterPieceData.size());
	for (const auto& cp: shatterPieceData) {
		if (guRNG.NextFloat() > _pieceParams.x)
			continue;

		const float3 flyDir = (cp.dir + (guRNG.NextVector() * 0.3f)).ANormalize();

		splitterParts.emplace_back();
		splitterParts.back().speed                = speed + flyDir * mix<float>(1.f, EXPLOSION_SPEED, guRNG.NextFloat());
		splitterParts.back().rotationAxisAndSpeed = float4(guRNG.NextVector().ANormalize(), guRNG.NextFloat() * 0.1f);
		splitterParts.back().indexCount = cp.indexCount;
		splitterParts.back().indexStart	= cp.indexStart;
	}
}


void FlyingPiece::InitCommon(const float3 _pos, const float3 _speed, const float _radius, int _team, int _texture)
{
	RECOIL_DETAILED_TRACY_ZONE;
	pos   = _pos;
	speed = _speed;

	pieceRadius = _radius;
	drawRadius = _radius + 10.f;

	texture = _texture;
	team    = _team;
}


bool FlyingPiece::Update()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (splitterParts.empty())
		return false;

	++age;

	const float3 dragFactors = GetDragFactors();

	// used for camera frustum checks
	pos        = pos0 + (speed * dragFactors.x) + UpVector * (mapInfo->map.gravity * dragFactors.y);
	drawRadius = pieceRadius + EXPLOSION_SPEED * dragFactors.x + 10.f;

	// check visibility (if all particles are underground -> kill)
	if ((age % GAME_SPEED) == 0)
		return true;

	for (auto& cp: splitterParts) {
		const CMatrix44f& m = GetMatrixOf(cp, dragFactors);
		const float3 p = m.GetPos();
		if ((p.y + pieceRadius * 2.f) >= CGround::GetApproximateHeight(p.x, p.z, false)) {
			return true;
		}
	}

	return false;
}

float3 FlyingPiece::GetDragFactors() const
{
	RECOIL_DETAILED_TRACY_ZONE;
	// We started with a naive (iterative) method like this:
	//  pos   += speed;
	//  speed *= airDrag;
	//  speed += gravity;
	// The problem is that pos & speed need to be saved for this (-> memory) and
	// need to be updated each frame (-> cpu usage). Doing so for each polygon is massively slow.
	// So I wanted to replace it with a stateless system, computing the current
	// position just from the params t & individual speed (start pos is 0!).
	//
	// To do so we split the computations in 2 parts: explosion speed & gravity.
	//
	// 1.
	// So the positional offset caused by the explosion speed becomes:
	//  d := drag   & s := explosion start speed
	//  xs(t=1) := s
	//  xs(t=2) := s + s * d
	//  xs(t=3) := s + s * d + s * d^2
	//  xs(t)   := s + s * d + s * d^2 + ... + s * d^t
	//           = s * sum(i=0,t){d^i}
	// That sum is a `geometric series`, the solution is `(1-d^(t+1))/(1-d)`.
	//  => xs(t) = s * (1 - d^(t+1)) / (1 - d)
	//           = s * speedDrag
	//
	// 2.
	// The positional offset caused by gravity is similar but a bit more complicated:
	//  xg(t=1) := g
	//  xg(t=2) := g + (g * d + g)                         = 2g + 1g*d
	//  xg(t=3) := g + (g * d + g) + (g * d^2 + g * d + g) = 3g + 2g*d + 1g*d^2
	//  xg(t)   := g * t + g * (t - 1) * d^2 + ... + g * (t - t) * d^t
	//           = g * sum(i=0,t){(t - i) * d^i}
	//           = g * (t * sum(i=0,t){d^i} - sum(i=0,t){i * d^i})
	//           = g * gravityDrag
	// The first sum is again a geometric series as above, the 2nd one is very similar.
	// The solution can be found here: https://de.wikipedia.org/w/index.php?title=Geometrische_Reihe&oldid=149159222#Verwandte_Summenformel_1
	//
	// The advantage of those 2 drag factors are that they only depend on the time (which even can be interpolated),
	// so we only have to compute them once per frame. And can then get the position of each particle with a simple multiplication,
	// saving memory & cpu time.

	static const float airDrag = 0.995f;
	static const float invAirDrag = 1.f / (1.f - airDrag);

	const float interAge = age + globalRendering->timeOffset;
	const float airDragPowOne = std::pow(airDrag, interAge + 1.f);
	const float airDragPowTwo = airDragPowOne * airDrag; // := std::pow(airDrag, interAge + 2.f);

	float3 dragFactors;

	// Speed Drag
	dragFactors.x = (1.f - airDragPowOne) * invAirDrag;

	// Gravity Drag
	dragFactors.y  = interAge * dragFactors.x; // 1st sum
	dragFactors.y -= (interAge * (airDragPowTwo - airDragPowOne) - airDragPowOne + airDrag) * invAirDrag * invAirDrag; // 2nd sum

	// time interpolation factor
	dragFactors.z = interAge;

	return dragFactors;
}


CMatrix44f FlyingPiece::GetMatrixOf(const SplitterData& cp, const float3 dragFactors) const
{
	RECOIL_DETAILED_TRACY_ZONE;
	const float3 interPos = cp.speed * dragFactors.x + UpVector * mapInfo->map.gravity * dragFactors.y;
	const float4& rot = cp.rotationAxisAndSpeed;

	CMatrix44f m = bposeMatrix;
	m.GetPos() += interPos; //note: not the same as .Translate(pos) which does `m = m * translate(pos)`, but we want `m = translate(pos) * m`
	m.Rotate(rot.w * dragFactors.z, rot.xyz);

	return m;
}


void FlyingPiece::CheckDrawStateChange(const FlyingPiece* prev) const
{
	RECOIL_DETAILED_TRACY_ZONE;
	ScopedModelDrawerImpl<CUnitDrawer> legacy(true, false);

	const auto thisModelType = piece->GetParentModel()->type;

	if (prev == nullptr) {
		CUnitDrawer::SetTeamColor(team);

		if (texture != -1)
			CModelDrawerHelper::BindModelTypeTexture(thisModelType, texture);

		return;
	}

	const auto prevModelType = piece->GetParentModel()->type;

	if (team != prev->team)
		CUnitDrawer::SetTeamColor(team);

	if (texture != -1 && (thisModelType != prevModelType || texture != prev->texture))
		CModelDrawerHelper::BindModelTypeTexture(thisModelType, texture);
}


void FlyingPiece::BeginDraw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	glDisable(GL_CULL_FACE);
	S3DModelHelpers::BindLegacyAttrVBOs();
}

void FlyingPiece::EndDraw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	glEnable(GL_CULL_FACE);
	S3DModelHelpers::UnbindLegacyAttrVBOs();

	CModelDrawerHelper::UnbindModelTypeTexture(MODELTYPE_S3O); // all model types do the same thing
}


void FlyingPiece::Draw(const FlyingPiece* prev) const
{
	RECOIL_DETAILED_TRACY_ZONE;
	CheckDrawStateChange(prev);

	const float3 dragFactors = GetDragFactors(); // speedDrag, gravityDrag, interAge

	for (auto& cp: splitterParts) {
		glPushMatrix();
		glMultMatrixf(GetMatrixOf(cp, dragFactors));
		assert(piece->indxCount != ~0u);
		const uint32_t indxOffset = piece->indxStart + piece->indxCount; //shatter piece indices come after regular indices
		S3DModelPiece::DrawShatterElements(indxOffset + cp.indexStart, cp.indexCount);
		glPopMatrix();
	}
}

