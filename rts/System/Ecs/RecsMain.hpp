/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#pragma once

#include <recs/recs.hpp>
#include "System/UnorderedMap.hpp"

namespace recs {
	template<>
	struct UserConfig<void> {
		//static constexpr size_t EntityChunkSize = 127;
		//static constexpr size_t SparseChunkSize = 127;
		//static constexpr size_t DenseChunkSize = 127;
		using SparseMapType = spring::unordered_map<Entity, std::size_t>;
	};
}