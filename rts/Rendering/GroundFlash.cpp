/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "GroundFlash.h"
#include "Map/Ground.h"
#include "Game/Camera.h"
#include "Rendering/GlobalRendering.h"
#include "Rendering/GL/RenderBuffers.h"
#include "Rendering/Textures/ColorMap.h"
#include "Rendering/Textures/TextureAtlas.h"
#include "Rendering/Env/Particles/ProjectileDrawer.h"
#include "Sim/Projectiles/ExpGenSpawnableMemberInfo.h"
#include "Sim/Projectiles/ProjectileHandler.h"
#include "System/creg/DefTypes.h"
#include "System/SpringMath.h"

CR_BIND_DERIVED(CGroundFlash, CExpGenSpawnable, )
CR_REG_METADATA(CGroundFlash, (
 	CR_MEMBER_BEGINFLAG(CM_Config),
		CR_MEMBER(size),
		CR_MEMBER(depthTest),
		CR_MEMBER(depthMask),
	CR_MEMBER_ENDFLAG(CM_Config)
))

CR_BIND_DERIVED(CStandardGroundFlash, CGroundFlash, )
CR_REG_METADATA(CStandardGroundFlash, (
 	CR_MEMBER_BEGINFLAG(CM_Config),
		CR_MEMBER(ttl),
 		CR_MEMBER(flashSize),
		CR_MEMBER(flashAlpha),
		CR_MEMBER(circleGrowth),
		CR_MEMBER(circleAlpha),
		CR_MEMBER(color),
	CR_MEMBER_ENDFLAG(CM_Config),

	CR_MEMBER(normal),
	CR_MEMBER(side1),
	CR_MEMBER(side2),
	CR_MEMBER(circleSize),
	CR_MEMBER(flashAge),
	CR_MEMBER(flashAgeSpeed),
	CR_MEMBER(circleAlphaDec)
))

CR_BIND_DERIVED(CSeismicGroundFlash, CGroundFlash, (ZeroVector, 1, 0, 1, 1, 1, ZeroVector))
CR_REG_METADATA(CSeismicGroundFlash, (
	CR_MEMBER(side1),
	CR_MEMBER(side2),
	CR_IGNORED(texture),
	CR_MEMBER(sizeGrowth),
	CR_MEMBER(alpha),
	CR_MEMBER(fade),
	CR_MEMBER(ttl),
	CR_MEMBER(color),
	CR_SERIALIZER(Serialize)
))

CR_BIND_DERIVED(CSimpleGroundFlash, CGroundFlash, )
CR_REG_METADATA(CSimpleGroundFlash, (
	CR_MEMBER(side1),
	CR_MEMBER(side2),
	CR_MEMBER(normal),
	CR_MEMBER(age),
	CR_MEMBER(agerate),
 	CR_MEMBER_BEGINFLAG(CM_Config),
		CR_MEMBER(sizeGrowth),
		CR_MEMBER(ttl),
		CR_MEMBER(colorMap),
		CR_IGNORED(texture),
	CR_MEMBER_ENDFLAG(CM_Config),
	CR_SERIALIZER(Serialize)
))



// CREG-only
CGroundFlash::CGroundFlash()
	: size(0.0f)
	, depthTest(true)
	, depthMask(false)
{
	alwaysVisible = false;
}

CGroundFlash::CGroundFlash(const float3& _pos) : CGroundFlash()
{
	pos = _pos;
}

bool CGroundFlash::UpdateAnimParams() {
	UpdateAnimParamsImpl(animParams1, animProgress1);
	return true;
}

float3 CGroundFlash::CalcNormal(const float3 midPos, const float3 camDir, float quadSize) const
{
	// no degenerate quads, otherwise ANormalize() fails
	// assert(quadSize > 1.0f);
	quadSize = std::max(quadSize, 1.0f);

	float3 p0 = float3(midPos.x + quadSize, 0.0f, midPos.z           );
	float3 p1 = float3(midPos.x - quadSize, 0.0f, midPos.z           );
	float3 p2 = float3(midPos.x,            0.0f, midPos.z + quadSize);
	float3 p3 = float3(midPos.x,            0.0f, midPos.z - quadSize);

	p0.y = CGround::GetApproximateHeight(p0.x, p0.z, false); p0 += camDir;
	p1.y = CGround::GetApproximateHeight(p1.x, p1.z, false); p1 += camDir;
	p2.y = CGround::GetApproximateHeight(p2.x, p2.z, false); p2 += camDir;
	p3.y = CGround::GetApproximateHeight(p3.x, p3.z, false); p3 += camDir;

	const float3 n0 = ((p2 - p0).cross(p3 - p0)).ANormalize();
	const float3 n1 = ((p3 - p1).cross(p2 - p1)).ANormalize();

	return ((n0 + n1).ANormalize());
}


bool CGroundFlash::GetMemberInfo(SExpGenSpawnableMemberInfo& memberInfo)
{
	if (CExpGenSpawnable::GetMemberInfo(memberInfo))
		return true;

	CHECK_MEMBER_INFO_FLOAT(CGroundFlash, size     );
	CHECK_MEMBER_INFO_BOOL (CGroundFlash, depthTest);
	CHECK_MEMBER_INFO_BOOL (CGroundFlash, depthMask);

	return false;
}

CStandardGroundFlash::CStandardGroundFlash()
	: CGroundFlash()
	, normal{}
	, side1{}
	, side2{}
	, ttl{0}
	, flashSize{0.0f}
	, circleSize{0.0f}
	, circleGrowth{0.0f}
	, circleAlpha{0.0f}
	, flashAlpha{0.0f}
	, flashAge{0.0f}
	, flashAgeSpeed{0.0f}
	, circleAlphaDec{0.0f}
	, color{0.0f, 0.0f, 0.0f}
{}

CStandardGroundFlash::CStandardGroundFlash(
	const float3 & _pos,
	float _circleAlpha,
	float _flashAlpha,
	float _flashSize,
	float _circleGrowth,
	float _ttl,
	const float3 & _color)
	: CGroundFlash(_pos)
	, ttl((int)_ttl)
	, flashSize(_flashSize)
	, circleSize(0.0f)
	, circleGrowth(_circleGrowth)
	, circleAlpha(_circleAlpha)
	, flashAlpha(_flashAlpha)
	, color{ 0.0f, 0.0f, 0.0f }
	, flashAge(0)
	, flashAgeSpeed(0.0f)
	, circleAlphaDec(0.0f)
{
	//explicit constructor call, needs Init()
	Init(nullptr, float3{ 0.0 });
}


void CStandardGroundFlash::Init(const CUnit* owner, const float3& offset)
{
	pos += offset;
	pos.y = CGround::GetHeightReal(pos.x, pos.z, false) + 1.0f;

	// flashSize is just backward compatibility
	size = flashSize;

	// moved from the constructor
	circleSize = circleGrowth;
	flashAgeSpeed = ttl ? 1.0f / ttl : 0.0f;
	circleAlphaDec = ttl ? circleAlpha / ttl : 0.0f;

	normal = CalcNormal(pos, camera->GetDir() * -1000.0f, flashSize);

	side1 = (normal.cross(RgtVector)).ANormalize();
	side2 = side1.cross(normal);

	CExpGenSpawnable::Init(owner, offset); // scales rotParams
	projectileHandler.AddGroundFlash(this);
}

bool CStandardGroundFlash::Update()
{
	circleSize += circleGrowth;
	circleAlpha -= circleAlphaDec;
	flashAge += flashAgeSpeed;

	return (std::max(--ttl, 0) > 0);
}

void CStandardGroundFlash::Draw()
{
	UpdateRotation();
	UpdateAnimParams();

	float iAlpha = std::clamp(circleAlpha - (circleAlphaDec * globalRendering->timeOffset), 0.0f, 1.0f);

	const float iSize = circleSize + circleGrowth * globalRendering->timeOffset;
	const float iAge = flashAge + flashAgeSpeed * globalRendering->timeOffset;
	SColor partColor = SColor(color.r, color.g, color.b, 0.0f);

	if (iAlpha > 0.0f && iSize > 0.0f) {
		std::array<float3, 4> bounds = {
			(-side1 - side2) * iSize,
			( side1 - side2) * iSize,
			( side1 + side2) * iSize,
			(-side1 + side2) * iSize
		};

		if (math::fabs(rotVal) > 0.01f) {
			float3::rotate<false>(rotVal, normal, bounds);
		}

		partColor.a = (unsigned char)(iAlpha * 255);
		AddEffectsQuad<0>(
			projectileDrawer->groundringtex->pageNum,
			{ pos + bounds[0], projectileDrawer->groundringtex->xstart, projectileDrawer->groundringtex->ystart, partColor },
			{ pos + bounds[1], projectileDrawer->groundringtex->xend,   projectileDrawer->groundringtex->ystart, partColor },
			{ pos + bounds[2], projectileDrawer->groundringtex->xend,   projectileDrawer->groundringtex->yend,   partColor },
			{ pos + bounds[3], projectileDrawer->groundringtex->xstart, projectileDrawer->groundringtex->yend,   partColor }
		);
	}

	if (iAge < 1.0f) {
		if (iAge < 0.091f) {
			iAlpha = flashAlpha * iAge * 10.0f;
		}
		else {
			iAlpha = flashAlpha * (1.0f - iAge);
		}

		if (iAlpha <= 0.0f)
			return;

		partColor.a = std::clamp(iAlpha, 0.0f, 1.0f) * 255;

		std::array<float3, 4> bounds = {
			(-side1 - side2) * size,
			( side1 - side2) * size,
			( side1 + side2) * size,
			(-side1 + side2) * size
		};

		if (math::fabs(rotVal) > 0.01f) {
			float3::rotate<false>(rotVal, normal, bounds);
		}

		AddEffectsQuad<0>(
			projectileDrawer->groundflashtex->pageNum,
			{ pos + bounds[0], projectileDrawer->groundflashtex->xstart, projectileDrawer->groundflashtex->ystart, partColor },
			{ pos + bounds[1], projectileDrawer->groundflashtex->xend  , projectileDrawer->groundflashtex->ystart, partColor },
			{ pos + bounds[2], projectileDrawer->groundflashtex->xend  , projectileDrawer->groundflashtex->yend  , partColor },
			{ pos + bounds[3], projectileDrawer->groundflashtex->xstart, projectileDrawer->groundflashtex->yend  , partColor }
		);
	}
}



bool CStandardGroundFlash::GetMemberInfo(SExpGenSpawnableMemberInfo& memberInfo)
{
	if (CGroundFlash::GetMemberInfo(memberInfo))
		return true;

	CHECK_MEMBER_INFO_INT   (CStandardGroundFlash, ttl         );
	CHECK_MEMBER_INFO_FLOAT (CStandardGroundFlash, flashSize   );
	CHECK_MEMBER_INFO_FLOAT (CStandardGroundFlash, flashAlpha  );
	CHECK_MEMBER_INFO_FLOAT (CStandardGroundFlash, circleGrowth);
	CHECK_MEMBER_INFO_FLOAT (CStandardGroundFlash, circleAlpha );
	CHECK_MEMBER_INFO_FLOAT3(CStandardGroundFlash, color       );

	return false;
}



CSimpleGroundFlash::CSimpleGroundFlash()
	: sizeGrowth(0.0f)
	, ttl(0)
	, age(0.0f)
	, agerate(0.0f)
	, colorMap(nullptr)
	, texture(nullptr)
{}

void CSimpleGroundFlash::Serialize(creg::ISerializer* s)
{
	std::string name;
	if (s->IsWriting())
		name = projectileDrawer->groundFXAtlas->GetTextureName(texture);
	creg::GetType(name)->Serialize(s, &name);
	if (!s->IsWriting())
		texture = projectileDrawer->groundFXAtlas->GetTexturePtr(name);
}

void CSimpleGroundFlash::Init(const CUnit* owner, const float3& offset)
{
	pos += offset;
	pos.y = CGround::GetHeightReal(pos.x, pos.z, false) + 1.0f;

	age = ttl ? 0.0f : 1.0f;
	agerate = ttl ? 1.0f / ttl : 1.0f;

	normal = CalcNormal(pos, camera->GetDir() * -1000.0f, size + (sizeGrowth * ttl));

	side1 = (normal.cross(RgtVector)).ANormalize();
	side2 = side1.cross(normal);

	CExpGenSpawnable::Init(owner, offset); // scales rotParams

	projectileHandler.AddGroundFlash(this);
}

void CSimpleGroundFlash::Draw()
{
	UpdateRotation();
	UpdateAnimParams();

	unsigned char color[4] = {0, 0, 0, 0};
	colorMap->GetColor(color, age);

	std::array<float3, 4> bounds = {
		(-side1 - side2) * size,
		( side1 - side2) * size,
		( side1 + side2) * size,
		(-side1 + side2) * size
	};

	if (math::fabs(rotVal) > 0.01f) {
		float3::rotate<false>(rotVal, normal, bounds);
	}

	AddEffectsQuad<1>(
		texture->pageNum,
		{ pos + bounds[0], texture->xstart, texture->ystart, color },
		{ pos + bounds[1], texture->xend  , texture->ystart, color },
		{ pos + bounds[2], texture->xend  , texture->yend  , color },
		{ pos + bounds[3], texture->xstart, texture->yend  , color }
	);
}

bool CSimpleGroundFlash::Update()
{
	age += agerate;
	size += sizeGrowth;

	return (age < 1);
}


bool CSimpleGroundFlash::GetMemberInfo(SExpGenSpawnableMemberInfo& memberInfo)
{
	if (CGroundFlash::GetMemberInfo(memberInfo))
		return true;

	CHECK_MEMBER_INFO_FLOAT(CSimpleGroundFlash, sizeGrowth);
	CHECK_MEMBER_INFO_INT  (CSimpleGroundFlash, ttl       );
	CHECK_MEMBER_INFO_PTR  (CSimpleGroundFlash, colorMap, CColorMap::LoadFromDefString                  );
	CHECK_MEMBER_INFO_PTR  (CSimpleGroundFlash, texture , projectileDrawer->groundFXAtlas->GetTexturePtr);

	return false;
}



CSeismicGroundFlash::CSeismicGroundFlash(
	const float3& _pos,
	int _ttl,
	int _fade,
	float _size,
	float _sizeGrowth,
	float _alpha,
	const float3& _color
)
	: CGroundFlash(_pos)
	, texture(projectileDrawer->seismictex)
	, sizeGrowth(_sizeGrowth)
	, alpha(_alpha)
	, fade(_fade)
	, ttl(_ttl)
{
	pos.y = CGround::GetHeightReal(_pos.x, _pos.z, false) + 1.0f;
	// A is set in Draw
	color.r = _color.x * 255.0f;
	color.g = _color.y * 255.0f;
	color.b = _color.z * 255.0f;

	size = _size;
	alwaysVisible = true;

	const float3 normal = CalcNormal(_pos, camera->GetDir() * -1000.0f, size + (sizeGrowth * ttl));

	side1 = (normal.cross(RgtVector)).SafeANormalize();
	side2 = side1.cross(normal);

	projectileHandler.AddGroundFlash(this);
}

void CSeismicGroundFlash::Serialize(creg::ISerializer* s)
{
	if (!s->IsWriting())
		texture = projectileDrawer->seismictex;
}

void CSeismicGroundFlash::Draw()
{
	color.a = mix(255, int(255 * (ttl / (1.0f * fade))), (ttl < fade));

	const float3 p1 = pos + (-side1 - side2) * size;
	const float3 p2 = pos + ( side1 - side2) * size;
	const float3 p3 = pos + ( side1 + side2) * size;
	const float3 p4 = pos + (-side1 + side2) * size;

	AddEffectsQuad<0>(
		texture->pageNum,
		{ p1, texture->xstart, texture->ystart, color },
		{ p2, texture->xend,   texture->ystart, color },
		{ p3, texture->xend,   texture->yend,   color },
		{ p4, texture->xstart, texture->yend,   color }
	);
}

bool CSeismicGroundFlash::Update()
{
	size += sizeGrowth;
	return (--ttl > 0);
}
