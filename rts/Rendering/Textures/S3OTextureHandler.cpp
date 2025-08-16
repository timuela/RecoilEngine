/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "Rendering/GL/myGL.h"
#include "S3OTextureHandler.h"

#include "System/FileSystem/FileHandler.h"
#include "System/FileSystem/SimpleParser.h"
#include "Rendering/ShadowHandler.h"
#include "Rendering/Units/UnitDrawer.h"
#include "Rendering/Models/3DModel.hpp"
#include "Rendering/Models/ModelsLock.h"
#include "Rendering/Textures/Bitmap.h"
#include "System/StringUtil.h"
#include "System/Exceptions.h"
#include "System/Log/ILog.h"
#include "System/Platform/Threading.h"

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>

#include "System/Misc/TracyDefs.h"

#define LOG_SECTION_TEXTURE "Texture"
LOG_REGISTER_SECTION_GLOBAL(LOG_SECTION_TEXTURE)
#ifdef LOG_SECTION_CURRENT
        #undef LOG_SECTION_CURRENT
#endif
#define LOG_SECTION_CURRENT LOG_SECTION_TEXTURE

#define TEX_MAT_UID(pTxID, sTxID) ((std::uint64_t(pTxID) << 32u) | sTxID)


// The S3O texture handler uses two textures.
// The first contains diffuse color (RGB) and teamcolor (A)
// The second contains glow (R), reflectivity (G) and 1-bit Alpha (A).

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CS3OTextureHandler textureHandlerS3O;

void CS3OTextureHandler::Init()
{
	RECOIL_DETAILED_TRACY_ZONE;
	textures.reserve(128);

	// dummies
	textures.emplace_back();
	textures.emplace_back();
}

void CS3OTextureHandler::Kill()
{
	RECOIL_DETAILED_TRACY_ZONE;
	for (S3OTexMat& texture: textures) {
		glDeleteTextures(1, &(texture.tex1));
		glDeleteTextures(1, &(texture.tex2));
	}

	textures.clear();
	textureCache.clear();
	textureTable.clear();
	bitmapCache.clear();
}

void CS3OTextureHandler::Reload()
{
	RECOIL_DETAILED_TRACY_ZONE;
	auto lock = CModelsLock::GetScopedLock(); //needed?

	for (auto& [texName, texData] : textureCache) {
		if (texData.texID == 0)
			continue;

		CBitmap bitmap;
		if (!bitmap.Load(texName) && !bitmap.Load("unittextures/" + texName))
			continue;

		{
			if (texData.invertAlpha)
				bitmap.InvertAlpha();
			if (texData.invertAxis)
				bitmap.ReverseYAxis();

			uint32_t newTexId = bitmap.CreateMipMapTexture(0.0f, 0.0f, 0, texData.texID);
			assert(newTexId == texData.texID);
		}
	}
}


void CS3OTextureHandler::PreloadTexture(S3DModel* model, bool invertAxis, bool invertAlpha)
{
	RECOIL_DETAILED_TRACY_ZONE;
	auto lock = CModelsLock::GetScopedLock();

	LoadAndCacheTexture(model, 0, invertAxis, invertAlpha, true);
	LoadAndCacheTexture(model, 1, invertAxis,       false, true); // never invert alpha for tex2
}


void CS3OTextureHandler::LoadTexture(S3DModel* model)
{
	RECOIL_DETAILED_TRACY_ZONE;
	auto lock = CModelsLock::GetScopedLock();

	const unsigned int tex1ID = LoadAndCacheTexture(model, 0, false, false, false);
	const unsigned int tex2ID = LoadAndCacheTexture(model, 1, false, false, false);

	const auto texTableIter = textureTable.find(TEX_MAT_UID(tex1ID, tex2ID));

	// even if both textures were already loaded as parts of
	// other models, their pair might form a unique material
	if (texTableIter == textureTable.end()) {
		model->textureType = InsertTextureMat(model);
	} else {
		model->textureType = texTableIter->second;
	}
}

unsigned int CS3OTextureHandler::LoadAndCacheTexture(
	const S3DModel* model,
	unsigned int texNum,
	bool invertAxis,
	bool invertAlpha,
	bool preloadCall
) {
	RECOIL_DETAILED_TRACY_ZONE;
	CBitmap* bitmap = nullptr;

	const auto& textureName = model->texs[texNum];
	const auto textureIt = textureCache.find(textureName);

	if (textureIt != textureCache.end() && textureIt->second.texID > 0)
		return textureIt->second.texID;

	const auto bitmapIt = bitmapCache.find(textureName);

	if (bitmapIt != bitmapCache.end()) {
		// bitmap was previously preloaded but not yet loaded
		// if !preloading, we will now turn the bitmap into a
		// texture and cache it
		bitmap = &(bitmapIt->second);
	} else {
		// bitmap was not yet preloaded, meaning we are the
		// first to (all non-3DO model textures are always
		// preloaded)
#ifndef HEADLESS //?
		assert(preloadCall);
#endif

		auto pair = bitmapCache.emplace(textureName, CBitmap{});
		auto iter = pair.first;

		bitmap = &(iter->second);

		if (!bitmap->Load(textureName) && !bitmap->Load("unittextures/" + textureName)) {
			if (texNum == 0)
				LOG_L(L_WARNING, "[%s] could not load primary texture \"%s\" from model \"%s\"", __func__, textureName.c_str(), model->name.c_str());

			// file not found (or headless build), set a single pixel so model is visible
			bitmap->AllocDummy(SColor(255 * (texNum == 0), 0, 0, 255 * (1 - invertAlpha)));
		}

		if (invertAxis)
			bitmap->ReverseYAxis();
		if (invertAlpha)
			bitmap->InvertAlpha();
	}

	const unsigned int texID = preloadCall ? 0 : bitmap->CreateMipMapTexture();
#ifndef HEADLESS
	assert(preloadCall || texID > 0);
#endif

	if (textureIt != textureCache.end() && texID > 0) {
		assert(!preloadCall);
		textureIt->second.texID = texID;
	}
	else {
		//save main params from the preloadCall pass, such that data is stored correctly for Reload()
#ifndef HEADLESS //?
		assert( preloadCall);
#endif
		textureCache[textureName] = {
			texID,
			static_cast<uint32_t>(bitmap->xsize),
			static_cast<uint32_t>(bitmap->ysize),
			invertAxis,
			invertAlpha
		};
	}

	// don't generate a texture yet, just save the bitmap for later
	if (preloadCall)
		return 0;

	bitmapCache.erase(textureName);
	return texID;
}


unsigned int CS3OTextureHandler::InsertTextureMat(const S3DModel* model)
{
	const CachedS3OTex& tex1 = textureCache[ model->texs[0] ];
	const CachedS3OTex& tex2 = textureCache[ model->texs[1] ];

	textures.emplace_back();

	S3OTexMat& texMat = textures.back();

	texMat.num       = textures.size() - 1;
	texMat.tex1      = tex1.texID;
	texMat.tex2      = tex2.texID;
	texMat.tex1SizeX = tex1.xsize;
	texMat.tex1SizeY = tex1.ysize;
	texMat.tex2SizeX = tex2.xsize;
	texMat.tex2SizeY = tex2.ysize;

	textureTable[TEX_MAT_UID(texMat.tex1, texMat.tex2)] = texMat.num;

	return texMat.num;
}

