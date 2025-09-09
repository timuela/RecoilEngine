/* This file is part of the Recoil engine (GPL v2 or later), see LICENSE.html */

#pragma once

#include <vector>
#include <cstdint>

#include "3DModelPiece.hpp"
#include "IModelParser.h"

namespace fastgltf {
	class Asset;
}

struct Transform;
struct S3DModel;

struct GLTFPiece : public S3DModelPiece {
	static constexpr size_t INVALID_NODE_INDEX = size_t(-1);
	size_t nodeIndex = INVALID_NODE_INDEX;
};

class CGLTFParser: public IModelParser
{
public:
	void Init() override {};
	void Kill() override {};

	void Load(S3DModel& model, const std::string& name) override;
private:
	GLTFPiece* AllocPiece();
	GLTFPiece* AllocRootEmptyPiece(S3DModel* model, const Transform& parentTransform, const fastgltf::Asset& asset, size_t sceneIndex);
	GLTFPiece* LoadPiece(S3DModel* model, GLTFPiece* parentPiece, const fastgltf::Asset& asset, size_t nodeIndex);

	std::vector<GLTFPiece> piecePool;
	spring::mutex poolMutex;

	uint32_t numPoolPieces = 0;
};
