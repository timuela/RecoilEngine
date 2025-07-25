/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef _LOS_TEXTURE_H
#define _LOS_TEXTURE_H

#include "ModernInfoTexture.h"
#include "Rendering/GL/FBO.h"


namespace Shader {
	struct IProgramObject;
}


class CLosTexture : public CModernInfoTexture
{
public:
	CLosTexture();
	~CLosTexture() override;

public:
	void Update() override;
	bool IsUpdateNeeded() override { return true; }
private:
	GL::Texture2D uploadTex;
};

#endif // _LOS_TEXTURE_H
