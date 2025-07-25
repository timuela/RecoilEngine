/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _SMF_GROUND_TEXTURES_H_
#define _SMF_GROUND_TEXTURES_H_

#include <vector>

#include "Map/BaseGroundTextures.h"
#include "Rendering/GL/PBO.h"

class CSMFMapFile;
class CSMFReadMap;

class CSMFGroundTextures: public CBaseGroundTextures
{
public:
	CSMFGroundTextures(CSMFReadMap* rm);
	~CSMFGroundTextures() override;

	void DrawUpdate();
	bool SetSquareLuaTexture(int texSquareX, int texSquareY, int texID);
	bool GetSquareLuaTexture(int texSquareX, int texSquareY, int texID, int texSizeX, int texSizeY, int lodMin, int lodMax);
	void BindSquareTexture(int texSquareX, int texSquareY);

protected:
	void LoadTiles(CSMFMapFile& file);
	void LoadSquareTextures(const int mipLevel);
	void LoadSquareTexturesPersistent();
	void ConvolveHeightMap(const int mapWidth, const int mipLevel);
	bool RecompressTilesIfNeeded();
	void ExtractSquareTiles(const int texSquareX, const int texSquareY, const int mipLevel, GLint* tileBuf) const;
	void LoadSquareTexture(int x, int y, int level);
	void LoadSquareTexturePersistent(int x, int y);

	inline bool TexSquareInView(int, int) const;

	CSMFReadMap* smfMap;

private:
	struct GroundSquare {
		enum {
			RAW_TEX_IDX = 0,
			LUA_TEX_IDX = 1,
		};

		GroundSquare(): textureIDs{0, 0}, texMipLevel(0), texDrawFrame(1) {}
		~GroundSquare();

		bool HasLuaTexture() const { return (textureIDs[LUA_TEX_IDX] != 0); }

		void SetRawTexture(unsigned int id) { textureIDs[RAW_TEX_IDX] = id; }
		void SetLuaTexture(unsigned int id) { textureIDs[LUA_TEX_IDX] = id; }
		void SetMipLevel(unsigned int l) { texMipLevel = l; }
		void SetDrawFrame(unsigned int f) { texDrawFrame = f; }

		unsigned int* GetTextureIDPtr() { return &textureIDs[RAW_TEX_IDX]; }
		unsigned int GetTextureID() const { return textureIDs[HasLuaTexture()]; }
		unsigned int GetMipLevel() const { return texMipLevel; }
		unsigned int GetDrawFrame() const { return texDrawFrame; }

	private:
		unsigned int textureIDs[2];
		unsigned int texMipLevel;
		unsigned int texDrawFrame;
	};

	// note: intentionally declared static (see ReadMap)
	static std::vector<GroundSquare> squares;

	static std::vector<int> tileMap;
	static std::vector<char> tiles;

	// FIXME? these are not updated at runtime
	static std::vector<float> heightMaxima;
	static std::vector<float> heightMinima;
	static std::vector<float> stretchFactors;

	// use Pixel Buffer Objects for async. uploading (DMA)
	PBO pbo;

	unsigned int tileTexFormat = 0;
	// unsigned int pboUnsyncedBit = 0;
	bool smfTextureStreaming = false;
	float smfTextureLodBias = 0.0f;
};

#endif // _BF_GROUND_TEXTURES_H_
