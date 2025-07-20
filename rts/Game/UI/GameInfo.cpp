/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */


#include "GameInfo.h"
#include "MouseHandler.h"
#include "Game/GameSetup.h"
#include "Game/GameVersion.h"
#include "Map/MapInfo.h"
#include "Map/ReadMap.h"
#include "Rendering/GL/myGL.h"
#include "Rendering/GL/glExtra.h"
#include "Rendering/Fonts/glFont.h"
#include "Sim/Misc/Team.h"
#include "Sim/Misc/Wind.h"
#include "Sim/Misc/ModInfo.h"
#include "Sim/Path/IPathManager.h"
#include "System/FileSystem/FileSystem.h"
#include "System/StringUtil.h"

#include <SDL_keycode.h>
#include <fmt/printf.h>

#include "System/Misc/TracyDefs.h"

using std::string;
using std::vector;



static std::string boolString(bool value)
{
	RECOIL_DETAILED_TRACY_ZONE;
	return (value)? "True": "False";
}

static std::string floatString(float value)
{
	RECOIL_DETAILED_TRACY_ZONE;
	return fmt::sprintf("%8.3f", value);
}

static void StringListStats(
	const vector<CGameInfo::FontString>& list,
	float& maxWidth,
	float& maxHeight
) {
	RECOIL_DETAILED_TRACY_ZONE;
	maxWidth = 0.0f;
	maxHeight = 0.0f;

	for (const CGameInfo::FontString& fs : list) {
		maxWidth = std::max(maxWidth, fs.width);
		maxHeight = std::max(maxHeight, fs.height);
	}
}



static CGameInfo* instance = nullptr;

void CGameInfo::Enable()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (instance == nullptr)
		instance = new CGameInfo;
}

void CGameInfo::Disable()
{
	RECOIL_DETAILED_TRACY_ZONE;
	delete instance;
	instance = nullptr;
}

bool CGameInfo::IsActive()
{
	RECOIL_DETAILED_TRACY_ZONE;
	return (instance != nullptr);
}



CGameInfo::CGameInfo()
{
	RECOIL_DETAILED_TRACY_ZONE;
	box.x1 = 0.5f;
	box.y1 = 0.5f;
	box.x2 = 0.5f;
	box.y2 = 0.5f;

	labels.reserve(16);
	values.reserve(16);

	if (gameSetup->hostDemo) {
		labels.emplace_back("Playback:");
		values.emplace_back(FileSystem::GetBasename(gameSetup->demoName));
	}

	labels.emplace_back("Spring Version:");
	values.emplace_back(SpringVersion::GetFull());

	labels.emplace_back("Game Speed:");
	values.emplace_back(gs->speedFactor);

	labels.emplace_back("Map Gravity:");
	values.emplace_back(fmt::sprintf("%.2f (%.2f e/f^2)", -(mapInfo->map.gravity * GAME_SPEED * GAME_SPEED), -mapInfo->map.gravity));

	labels.emplace_back("Map Hardness:");
	values.emplace_back(fmt::sprintf("%.2f", mapInfo->map.hardness));

	labels.emplace_back("Map Tidal:");
	values.emplace_back(envResHandler.GetCurrentTidalStrength());

	labels.emplace_back("Map Wind:");
	values.emplace_back(fmt::sprintf("%.2f - %.2f (%.2f)", envResHandler.GetMinWindStrength(), envResHandler.GetMaxWindStrength(), envResHandler.GetAverageWindStrength()));

	labels.emplace_back("Map Size:");
	values.emplace_back(fmt::sprintf("%ix%i", mapDims.mapx / 64, mapDims.mapy / 64));

	labels.emplace_back("Map Name:");
	values.emplace_back(gameSetup->mapName);

	labels.emplace_back("Game Name:");
	values.emplace_back(gameSetup->modName);

	labels.emplace_back("PFS Name:");
	switch (pathManager->GetPathFinderType()) {
		case NOPFS_TYPE: { values.emplace_back("NOPFS"  ); } break;
		case HAPFS_TYPE: { values.emplace_back("HAPFS"  ); } break;
		case QTPFS_TYPE: { values.emplace_back("QTPFS"  ); } break;
		default        : { values.emplace_back("UNKNOWN"); } break; // not reachable
	}

	labels.emplace_back("CHEATS:");
	values.emplace_back(""); // dynamic, set in Draw
}

CGameInfo::~CGameInfo()
{
	RECOIL_DETAILED_TRACY_ZONE;
	instance = nullptr;
}



CGameInfo::FontString::FontString(const char* c): msg(c)        { CalcDimensions(); }
CGameInfo::FontString::FontString(const std::string& s): msg(s) { CalcDimensions(); }
CGameInfo::FontString::FontString(bool b): msg(boolString(b))   { CalcDimensions(); }
CGameInfo::FontString::FontString(float f): msg(floatString(f)) { CalcDimensions(); }

void CGameInfo::FontString::CalcDimensions() {
	width  = font->GetSize() * font->GetTextWidth(msg) * globalRendering->pixelX;
	height = font->GetSize() * font->GetLineHeight() * globalRendering->pixelY;
}



/******************************************************************************/

std::string CGameInfo::GetTooltip(int x,int y)
{
	RECOIL_DETAILED_TRACY_ZONE;
	return "Game Information";
}


bool CGameInfo::IsAbove(int x, int y)
{
	RECOIL_DETAILED_TRACY_ZONE;
	const float mx = MouseX(x);
	const float my = MouseY(y);
	return InBox(mx, my, box);
}


bool CGameInfo::MousePress(int x, int y, int button)
{
	RECOIL_DETAILED_TRACY_ZONE;
	return (IsAbove(x, y));
}


void CGameInfo::MouseRelease(int x, int y, int button)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (activeReceiver != this)
		return;

	if (!IsAbove(x, y))
		return;

	delete this;
}


bool CGameInfo::KeyPressed(int keyCode, int scanCode, bool isRepeat)
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (keyCode == SDLK_ESCAPE) {
		delete this;
		return true;
	}
	return false;
}


void CGameInfo::Draw()
{
	RECOIL_DETAILED_TRACY_ZONE;
	if (gs->cheatEnabled) {
		values[values.size() - 1] = "ENABLED";
	} else {
		values[values.size() - 1] = "DISABLED";
	}

	auto& rb = RenderBuffer::GetTypedRenderBuffer<VA_TYPE_C>();
	auto& shader = rb.GetShader();


	// in screen fractions
	const float   split = 10.0f * globalRendering->pixelX;
	const float xBorder =  5.0f * globalRendering->pixelX;
	const float yBorder =  5.0f * globalRendering->pixelY;

	float labelsWidth, labelsHeight;
	float valuesWidth, valuesHeight;
	StringListStats(labels, labelsWidth, labelsHeight);
	StringListStats(values, valuesWidth, valuesHeight);

	const float    width = labelsWidth + valuesWidth + split + 2.0f * xBorder;
	const float rowHeight = std::max(labelsHeight, valuesHeight) + 2.0f * yBorder;
	const float    height = rowHeight * labels.size();

	const float dy = height / labels.size();

	box.x1 = 0.5f - ( width * 0.5f);
	box.x2 = 0.5f + ( width * 0.5f);
	box.y1 = 0.5f - (height * 0.5f);
	box.y2 = 0.5f + (height * 0.5f);


	glEnable(GL_BLEND);

	// draw the boxes
	for (size_t i = 0; i < labels.size(); i++) {
		constexpr float colors[][4]  = {
			{0.6f, 0.3f, 0.3f, 0.8f}, // red
			{0.3f, 0.6f, 0.3f, 0.8f}, // green
			{0.3f, 0.3f, 0.6f, 0.8f}, // blue
		};

		TRectangle<float> b;
		b.x1 = box.x1;
		b.x2 = box.x2;
		b.y2 = box.y2 - (dy * i);
		b.y1 =   b.y2 -  dy;

		gleDrawQuadC(b, SColor(colors[i % 3]), rb);
	}
	{
		shader.Enable();
		rb.DrawElements(GL_TRIANGLES);
		shader.Disable();
	}


	font->SetTextColor(1.0f, 1.0f, 1.0f, 1.0f);

	// draw the strings
	for (size_t i = 0; i < labels.size(); i++) {
		const FontString& lfs = labels[i];
		const FontString& vfs = values[i];

		const float y = box.y2 - (dy * (i + 1)) + yBorder;

		font->glPrint(box.x1 + xBorder, y, 1.0f, FONT_SCALE | FONT_NORM | FONT_BUFFERED, lfs.msg);
		font->glPrint(box.x2 - xBorder, y, 1.0f, FONT_RIGHT | FONT_SCALE | FONT_NORM | FONT_BUFFERED, vfs.msg);
	}

	font->DrawBuffered();
}
