/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#pragma once

#include <recs/recs.hpp>
#include "System/UnorderedMap.hpp"

namespace recs {
	template<>
	struct UserConfig<void> {
		using SparseMapType = spring::unordered_map<Entity, std::size_t, EntityHasher>;
	};
}