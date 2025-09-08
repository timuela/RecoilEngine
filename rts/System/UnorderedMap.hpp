/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#pragma once

//#define USE_EMILIB_HASH_MAP
// boost's implementation also uses LL buckets; see boost/unordered/detail/buckets.hpp
// (we want open addressing, so google's {sparse,dense}_hash_map are better candidates)
#define USE_BOOST_HASH_MAP


#ifndef USE_EMILIB_HASH_MAP
	#ifdef USE_BOOST_HASH_MAP
	#include "EqualTo.hpp"
	#include <boost_unordered/boost_unordered.hpp>
	#include "SpringHash.h"

	namespace spring {
		template<typename K, typename V, typename H = spring::synced_hash<K>, typename C = Recoil::EqualTo<K>>
		using unordered_map = boost::unordered_flat_map<K, V, H, C>;
		template<typename K, typename V, typename H = std::hash<K>, typename C = Recoil::EqualTo<K>>
		using unsynced_map = boost::unordered_flat_map<K, V, H, C>;
	};

	#else
	#include <unordered_map>

	namespace spring {
		using std::unordered_map;
		using std::unordered_multimap;
		using unsynced_map = std::unordered_map;
	};

	#endif
#else
	#include <type_traits>

	#include "SpringHashMap.hpp"
	#include "SpringHash.h"

	// NOTE: no multimap, no emplace
	namespace spring {
		template<typename K, typename V, typename H = spring::synced_hash<K>, typename C = emilib::HashMapEqualTo<K>>
		using unordered_map = emilib::HashMap<K, V, H, C>;
		template<typename K, typename V, typename H = std::hash<K>, typename C = emilib::HashMapEqualTo<K>>
		using unsynced_map = emilib::HashMap<K, V, H, C>;
	};
#endif

namespace Recoil {
	template<typename Map, typename MapPureT = std::remove_cvref_t<Map>>
	auto map_try_get(Map&& map, const typename MapPureT::key_type& key) {
		constexpr bool mapIsConst = std::is_const_v<typename std::remove_reference<Map>::type>;
		using ReturnType = std::conditional_t<mapIsConst, const typename MapPureT::mapped_type, typename MapPureT::mapped_type>;
		if (auto it = map.find(key); it == map.end())
			return static_cast<ReturnType*>(nullptr);
		else
			return static_cast<ReturnType*>(&it->second);
	}
}


namespace spring {
	// Synced unordered maps must be reconstructed (on reload)
	// since clear() may keep the container resized which will
	// lead to differences in iteration order and then desyncs
	template<typename C> void clear_unordered_map(C& cont) { cont = C(); }

	template<typename K, typename V>
	class unordered_bimap {
	public:
		using kv_map_type = spring::unordered_map<K, V>;
		using vk_map_type = spring::unordered_map<V, K>;

		const kv_map_type& first() const { return kv_map; }
		const vk_map_type& second() const { return vk_map; }

		unordered_bimap(std::initializer_list<std::pair<const K, V>> list)
			: kv_map(list)
		{
			vk_map.reserve(list.size());

			for (const auto& pair: list) {
				if (vk_map.find(pair.second) == vk_map.end()) {
					vk_map[pair.second] = pair.first;
				}
			}
		}

	private:
		const kv_map_type kv_map;
		      vk_map_type vk_map;
	};
};