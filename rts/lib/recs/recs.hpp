#pragma once

// Recoil Entity Component System
#include <bit>
#include <cstdint>
#include <concepts>
#include <type_traits>
#include <limits>
#include <vector>
#include <array>
#include <tuple>
#include <bitset>
#include <unordered_map>
#include <utility>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <compare>
#include <iterator>
#include <ranges>

#if __cplusplus >= 202002L
#define recs_likely(x)   (x) [[likely]]
#define recs_unlikely(x) (x) [[unlikely]]
#else
#ifdef __GNUC__
#define recs_likely(x)   (__builtin_expect((x),1))
#define recs_unlikely(x) (__builtin_expect((x),0))
#else
#define recs_likely(x)   (x)
#define recs_unlikely(x) (x)
#endif
#endif

// fwd declarations, types and constants
namespace recs {
	template <typename>
	struct UserConfig;

	struct EntityHasher;
	struct Entity;

	static constexpr size_t DEFAULT_DENSE_CHUNK_SIZE = 1024;
	static constexpr size_t DEFAULT_SPARSE_CHUNK_SIZE = 256;

	enum class ComponentCategory : uint8_t {
		Dense,
		Sparse,
	};

	enum class AllocationType : uint8_t {
		Chunked,
		Contiguous
	};

	// fwd declaration
	template <class T, size_t CHUNK_SIZE>
	class ChunkedArray;

	template<typename TypedRegistry, typename... ICs>
	class ViewImpl;
}

namespace utils {
	using namespace recs;

	template<typename... Ts>
	concept MoreThanOneType = (sizeof...(Ts) > 1);

	template <typename T>
	constexpr bool always_false_v = false;

	template<typename TupleType, typename Type>
	struct tuple_contains_type;

	template<typename Type, typename... TupleElementTypes>
	struct tuple_contains_type<std::tuple<TupleElementTypes...>, Type> : std::disjunction<std::is_same<Type, TupleElementTypes>...> {};

	template<typename TupleType, typename Type>
	constexpr inline bool tuple_contains_type_v = tuple_contains_type<TupleType, Type>::value;

	template<typename T, typename Tuple, std::size_t I = 0>
	static constexpr std::size_t tuple_type_index() {
		if constexpr (I == std::tuple_size_v<Tuple>) {
			static_assert(always_false_v<T>, "Type not found in Tuple");
			return I; // To make compilers happy
		}
		else if constexpr (std::is_same_v<T, std::tuple_element_t<I, Tuple>>) {
			return I;
		}
		else {
			return tuple_type_index<T, Tuple, I + 1>();
		}
	}

	template <typename T, typename Tuple>
	static constexpr size_t tuple_type_index_v = tuple_type_index<T, Tuple>();

	template <typename Fn, typename TupleT>
	void for_each_tuple(Fn&& fn, TupleT&& tp) {
		std::apply(
			[&fn]<typename ...T>(T&& ...args) {
			(fn(std::forward<T>(args)), ...);
		}, std::forward<TupleT>(tp));
	}

	template<typename Tuple, typename Func, std::size_t... Is>
	inline void tuple_exec_at_impl(size_t index, Tuple& t, Func&& f, std::index_sequence<Is...>)
	{
		using FnType = void(*)(Tuple&, Func&&);
		static constexpr FnType table[] = {
			[](Tuple& t, Func&& func) { func(std::get<Is>(t)); }...
		};
		if (index < sizeof...(Is))
			table[index](t, std::forward<Func>(f));
		else
			throw std::out_of_range("tuple index out of range");
	}

	template<typename Tuple, typename Func>
	inline void tuple_exec_at(size_t index, Tuple& t, Func&& f)
	{
		tuple_exec_at_impl(index, t, std::forward<Func>(f), std::make_index_sequence<std::tuple_size_v<Tuple>>{});
	}

	template<typename... Type>
	struct type_list_t {
		using type = type_list_t;
		static constexpr auto size = sizeof...(Type);
	};

	template<typename... Type>
	static constexpr type_list_t<Type...> type_list{};

	template<typename T, typename TypeList>
	struct type_in_list;

	template<typename T, typename... Types>
	struct type_in_list<T, type_list_t<Types...>> {
		static constexpr bool value = (std::is_same_v<T, Types> || ...);
	};

	template<typename T, typename TypeList>
	constexpr bool type_in_list_v = type_in_list<T, TypeList>::value;

	template<typename... Lists>
	struct type_list_concat;

	template<>
	struct type_list_concat<> {
		using type = type_list_t<>;
	};

	template<typename... Ts>
	struct type_list_concat<type_list_t<Ts...>> {
		using type = type_list_t<Ts...>;
	};

	template<typename... Ts, typename... Us, typename... Rest>
	struct type_list_concat<type_list_t<Ts...>, type_list_t<Us...>, Rest...> {
		using type = typename type_list_concat<type_list_t<Ts..., Us...>, Rest...>::type;
	};

	template<typename... Lists>
	using type_list_concat_t = typename type_list_concat<Lists...>::type;

	template<template<class> class Pred, typename List>
	struct type_list_filter;

	template<template<class> class Pred, typename... Ts>
	struct type_list_filter<Pred, type_list_t<Ts...>> {
		using type = type_list_concat_t<
			std::conditional_t<Pred<Ts>::value, type_list_t<Ts>, type_list_t<>>...
		>;
	};

	template<template<class> class Pred, typename List>
	using type_list_filter_t = typename type_list_filter<Pred, List>::type;

	template<std::size_t I, typename List, std::size_t N = List::size>
	struct type_list_at;

	// recursive step
	template<std::size_t I, typename T, typename... Ts, std::size_t N>
	struct type_list_at<I, type_list_t<T, Ts...>, N> : type_list_at<I - 1, type_list_t<Ts...>, N> {};

	// base case
	template<typename T, typename... Ts, std::size_t N>
	struct type_list_at<0, type_list_t<T, Ts...>, N> { using type = T; };

	// out-of-range: empty list reached while I > 0
	template<std::size_t I, std::size_t N>
	struct type_list_at<I, type_list_t<>, N> {
		static_assert(I < N, "type_list_at: index out of range");
	};

	template<std::size_t I, typename List>
	using type_list_at_t = typename type_list_at<I, List>::type;

	template<typename T, typename... Args>
	inline T& construct_and_assign_at_ref(T& place, Args&& ... args) noexcept {
		return place = T{ std::forward<Args>(args)... };
	}

	template <typename T, typename G, bool HasGlobalConfig, bool HasTypedConfig>
	struct SparseMapTypeSelector;

	template <typename T, typename G>
	struct SparseMapTypeSelector<T, G, true, true> {
		using type = typename UserConfig<T>::SparseMapType;
	};

	template <typename T, typename G>
	struct SparseMapTypeSelector<T, G, false, true> {
		using type = typename UserConfig<T>::SparseMapType;
	};

	template <typename T, typename G>
	struct SparseMapTypeSelector<T, G, true, false> {
		using type = typename UserConfig<G>::SparseMapType;
	};

	template <typename T, typename G>
	struct SparseMapTypeSelector<T, G, false, false> {
		using type = std::unordered_map<Entity, std::size_t, EntityHasher>;
	};

	template <typename T>
	concept HasUserConfigEntityChunkSize = requires {
		{ UserConfig<T>::EntityChunkSize } -> std::convertible_to<std::size_t>;
	};

	template <typename T>
	concept HasUserConfigDenseChunkSize = requires {
		{ UserConfig<T>::DenseChunkSize } -> std::convertible_to<std::size_t>;
	};

	template <typename T>
	concept HasUserConfigSparseChunkSize = requires {
		{ UserConfig<T>::SparseChunkSize } -> std::convertible_to<std::size_t>;
	};

	template <typename T>
	concept HasUserConfigSparseMapType = requires {
		typename UserConfig<T>::SparseMapType;
	};

	template <typename T, typename G = std::void_t<T>>
	struct UserConfigResolver {
		static constexpr size_t EntityChunkSize = []() {
			if constexpr (HasUserConfigEntityChunkSize<T>) {
				return static_cast<std::size_t>(UserConfig<T>::EntityChunkSize);
			}
			else if constexpr (HasUserConfigEntityChunkSize<G>) {
				return static_cast<std::size_t>(UserConfig<G>::EntityChunkSize);
			}
			else {
				return DEFAULT_DENSE_CHUNK_SIZE;
			}
		}();
		static constexpr size_t DenseChunkSize = []() {
			if constexpr (HasUserConfigDenseChunkSize<T>) {
				return static_cast<std::size_t>(UserConfig<T>::DenseChunkSize);
			}
			else if constexpr (HasUserConfigDenseChunkSize<G>) {
				return static_cast<std::size_t>(UserConfig<G>::DenseChunkSize);
			}
			else {
				return DEFAULT_DENSE_CHUNK_SIZE;
			}
		}();
		static constexpr size_t SparseChunkSize = []() {
			if constexpr (HasUserConfigSparseChunkSize<T>) {
				return static_cast<std::size_t>(UserConfig<T>::SparseChunkSize);
			}
			else if constexpr (HasUserConfigSparseChunkSize<G>) {
				return static_cast<std::size_t>(UserConfig<G>::SparseChunkSize);
			}
			else {
				return DEFAULT_SPARSE_CHUNK_SIZE;
			}
		}();

		using SparseMapType = typename SparseMapTypeSelector<T, G, HasUserConfigSparseMapType<G>, HasUserConfigSparseMapType<T>>::type;
	};

	template <typename T>
	static constexpr auto EntityChunkSize = UserConfigResolver<T>::EntityChunkSize;

	template <typename T>
	static constexpr auto DenseChunkSize = UserConfigResolver<T>::DenseChunkSize;

	template <typename T>
	static constexpr auto SparseChunkSize = UserConfigResolver<T>::SparseChunkSize;

	template <typename T>
	using SparseMapType = typename UserConfigResolver<T>::SparseMapType;

	template <size_t CHUNK_SIZE>
	struct ChunkHelper
	{
		static_assert(std::has_single_bit(CHUNK_SIZE), "CHUNK_SIZE must be a power of two");
		static constexpr size_t LOG2 = std::countr_zero(CHUNK_SIZE);
		static constexpr size_t MASK = CHUNK_SIZE - 1;
		static constexpr size_t SIZE = CHUNK_SIZE;

		static constexpr size_t ChunkIndex(size_t globalIndex) noexcept
		{
			return globalIndex >> LOG2;
		}

		static constexpr size_t OffsetIndex(size_t globalIndex) noexcept
		{
			return globalIndex & MASK;
		}
	};

	template <typename T>
	concept HasChunkSize = requires {
		{ T::chunkSize } -> std::convertible_to<size_t>;
	};

	template <typename T>
	concept HasComponentCategory = requires {
		{ T::compCat } -> std::convertible_to<ComponentCategory>;
	};

	template <typename T>
	concept HasAllocationType = requires {
		{ T::allocType } -> std::convertible_to<AllocationType>;
	};

	template <typename T>
	struct CompCatTrait {
		static constexpr auto value = []() {
			if constexpr (HasComponentCategory<T>)
				return T::compCat;
			else
				return ComponentCategory::Dense;
		}();
	};

	template <typename T>
	static constexpr auto CompCatTraitV = CompCatTrait<T>::value;

	template <typename T>
	struct ChunkSizeTrait {
		static constexpr size_t value = []() {
			if constexpr (HasChunkSize<T>) {
				return T::chunkSize;
			}
			else if constexpr (CompCatTraitV<T> == ComponentCategory::Dense) {
				return DenseChunkSize<T>;
			}
			else {
				return SparseChunkSize<T>;
			}
		}();
	};

	template <typename T>
	struct AllocTypeTrait {
		static constexpr auto value = []() {
			if constexpr (HasAllocationType<T>)
				return T::allocType;
			else
				return AllocationType::Chunked;
		}();
	};

	template <typename T>
	struct ComponentTraits {
		static constexpr auto chunkSize = ChunkSizeTrait<T>::value;
		static constexpr auto compCat = CompCatTrait<T>::value;
		static constexpr auto allocType = AllocTypeTrait<T>::value;
		using StorageType = std::conditional_t<allocType == AllocationType::Contiguous, std::vector<T>, ChunkedArray<T, chunkSize>>;

		template<typename C>
		using StorageTypeT = std::conditional_t<allocType == AllocationType::Contiguous, std::vector<C>, ChunkedArray<C, chunkSize>>;
	};

	template<class T>
	constexpr const T* as_const_ptr(T* ptr) noexcept
	{
		return static_cast<const T*>(ptr);
	}
}


namespace recs {
	using namespace utils;

	template <typename... T>
	struct ExcludeComponentsListT : type_list_t<T...> {};

	template <typename T>
	struct OrderByT : type_list_t<T> {};

	template <typename... T>
	static constexpr ExcludeComponentsListT<T...> ExcludeComponentsList{};

	template <typename T>
	static constexpr OrderByT<T> OrderBy{};

	//TODO switch back to methods?
	struct Entity {
		using IdType = uint32_t;
		IdType id = std::numeric_limits<IdType>::max();
		constexpr bool operator==(const Entity& other) const noexcept { return id == other.id; }
		constexpr auto operator<=>(const Entity& other) const noexcept { return id <=> other.id; }
		explicit constexpr operator IdType() const noexcept { return id; }
	};

	namespace EntityTraits {
		static constexpr Entity::IdType INDEX_BITS = 20;
		static constexpr Entity::IdType VERSION_BITS = sizeof(Entity::IdType) * 8 - INDEX_BITS;

		static constexpr Entity::IdType INDEX_MASK = (1u << INDEX_BITS) - 1u;
		static constexpr Entity::IdType VERSION_MASK = (1u << VERSION_BITS) - 1u;

		static constexpr Entity::IdType INVALID_INDEX = INDEX_MASK;
	};

	static constexpr Entity NullEntity = Entity();

	constexpr bool IsNullEntity(Entity entity) noexcept {
		using namespace EntityTraits;
		return entity == NullEntity;
	}

	constexpr inline auto EntityToIntegral(Entity entity) noexcept {
		return entity.id;
	}

	constexpr inline auto IntegralToEntity(uint32_t u) noexcept {
		return Entity(u);
	}

	constexpr uint32_t EntityToIndex(Entity entity) noexcept {
		using namespace EntityTraits;
		return EntityToIntegral(entity) & INDEX_MASK;
	}

	constexpr uint32_t EntityToVersion(Entity entity) noexcept {
		using namespace EntityTraits;
		return (EntityToIntegral(entity) >> INDEX_BITS) & VERSION_MASK;
	}

	constexpr auto EntityToIndexAndVersion(Entity entity) noexcept {
		using namespace EntityTraits;
		const auto u = EntityToIntegral(entity);
		return std::make_pair(
			(u & INDEX_MASK),
			(u >> INDEX_BITS) & VERSION_MASK
		);
	}

	constexpr auto ComposeEntity(uint32_t index, uint32_t version) noexcept {
		using namespace EntityTraits;
		return Entity((version << INDEX_BITS) | (index & INDEX_MASK));
	}

	constexpr uint32_t NextEntityVersion(Entity entity) noexcept
	{
		using namespace EntityTraits;

		if (IsNullEntity(entity))
			return EntityToVersion(entity);

		return (EntityToVersion(entity) + 1u) & VERSION_MASK; // wrap around within VERSION_MASK
	}

	struct EntityHasher {
		size_t operator()(Entity entity) const {
			return static_cast<size_t>(EntityToIndex(entity));
		}
	};

	// match its methods with std::vector so they could use +- interchangeably
	template <class T, size_t CHUNK_SIZE>
	class ChunkedArray
	{
	public:
		using MyStoredType = T;
		using MyChunkHelper = ChunkHelper<CHUNK_SIZE>;

		template<bool IsConst>
		class Iterator {
		private:
			using ArrayType = std::conditional_t<IsConst, const ChunkedArray, ChunkedArray>;
			using ValueType = std::conditional_t<IsConst, const T, T>;
			using ReferenceType = std::conditional_t<IsConst, const T&, T&>;
			using PointerType = std::conditional_t<IsConst, const T*, T*>;
			ArrayType* array;
			size_t index;
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = T;
			using difference_type = std::ptrdiff_t;
			using pointer = PointerType;
			using reference = ReferenceType;

			Iterator(ArrayType* arr, size_t idx)
				: array(arr)
				, index(idx)
			{
			}

			// Allow conversion from non-const to const iterator
			Iterator(const Iterator<false>& other)
				: array(other.array)
				, index(other.index)
			{
			}

			ReferenceType operator*() const {
				return (*array)[index];
			}

			PointerType operator->() const {
				size_t c = MyChunkHelper::ChunkIndex(index);
				size_t o = MyChunkHelper::OffsetIndex(index);
				return &(array->chunks[c][o]);
			}

			Iterator& operator++() {
				++index;
				return *this;
			}

			Iterator operator++(int) {
				Iterator temp = *this;
				++index;
				return temp;
			}

			template<bool OtherIsConst>
			bool operator==(const Iterator<OtherIsConst>& other) const {
				return array == other.array && index == other.index;
			}

			template<bool OtherIsConst>
			bool operator!=(const Iterator<OtherIsConst>& other) const {
				return !(*this == other);
			}

			// Just in case, seems to be not required on MSVC and g++
			template<bool> friend class Iterator;
		};

		using iterator = Iterator<false>;
		using const_iterator = Iterator<true>;

		ChunkedArray() = default;

		void clear() {
			elemCount = 0;
			chunks.clear();
		}

		void reserve(size_t count)
		{
			const size_t numChunksNeeded = (count + CHUNK_SIZE - 1) >> MyChunkHelper::LOG2;
			chunks.reserve(numChunksNeeded);
		}

		void ensure_size(size_t count) {
			if (count == 0)
				return;

			if (count > elemCount) {
				elemCount = count;
				const size_t numChunksNeeded = MyChunkHelper::ChunkIndex(elemCount - 1) + 1;
				chunks.resize(numChunksNeeded, {});
			}
		}

		void trim() {
			if recs_unlikely(elemCount == 0) {
				clear();
				return;
			}

			const size_t numChunksNeeded = MyChunkHelper::ChunkIndex(elemCount - 1) + 1;
			chunks.resize(numChunksNeeded, {});
		}

		// direct access without bounds checking, assume SetSize has been called
		decltype(auto) operator[](size_t idx) {
			ensure_size(idx + 1);
			size_t c = MyChunkHelper::ChunkIndex(idx);
			size_t o = MyChunkHelper::OffsetIndex(idx);
			return chunks[c][o];
		}

		decltype(auto) operator[](size_t idx) const {
			size_t c = MyChunkHelper::ChunkIndex(idx);
			size_t o = MyChunkHelper::OffsetIndex(idx);
			return chunks[c][o];
		}

		template<typename... Args>
		decltype(auto) emplace_back(Args&& ... args) {
			size_t c = MyChunkHelper::ChunkIndex(elemCount);
			size_t o = MyChunkHelper::OffsetIndex(elemCount);
			ensure_size(elemCount + 1);

			return construct_and_assign_at_ref(chunks[c][o], std::forward<Args>(args)...);
		}

		decltype(auto) back() const {
			size_t c = MyChunkHelper::ChunkIndex(elemCount - 1);
			size_t o = MyChunkHelper::OffsetIndex(elemCount - 1);
			return chunks[c][o];
		}

		decltype(auto) back() {
			size_t c = MyChunkHelper::ChunkIndex(elemCount - 1);
			size_t o = MyChunkHelper::OffsetIndex(elemCount - 1);
			return chunks[c][o];
		}

		void pop_back() {
			construct_and_assign_at_ref(back());
			--elemCount;
			trim();
		}

		bool empty() const { return elemCount == 0; }

		size_t size() const noexcept { return elemCount; }

		iterator begin() {
			return iterator(this, 0);
		}

		iterator end() {
			return iterator(this, elemCount);
		}

		const_iterator begin() const {
			return const_iterator(this, 0);
		}

		const_iterator end() const {
			return const_iterator(this, elemCount);
		}

		const_iterator cbegin() const {
			return const_iterator(this, 0);
		}

		const_iterator cend() const {
			return const_iterator(this, elemCount);
		}
	private:
		std::vector<std::array<T, CHUNK_SIZE>> chunks{};
		size_t elemCount{};
	};

	template <typename... Cs>
	class Registry;

	template <typename TypedRegistry, typename T>
	class DenseComponentStorage {
	public:
		template <typename... Cs>
		friend class Registry;

		template<typename TR, typename... ICs>
		friend class ViewImpl;

		using MyStoredType = T;
	public:
		DenseComponentStorage(TypedRegistry& r)
			: regPtr(&r)
		{}

		/*
		[[nodiscard]] decltype(auto) GetRawStorage() const noexcept {
			return storage;
		}

		[[nodiscard]] decltype(auto) GetRawStorage() noexcept {
			return storage;
		}

		[[nodiscard]] decltype(auto) GetMappedEntities() const noexcept {
			return regPtr->entities;
		}
		*/
	private:
		void Init(Entity entity) noexcept {
			const auto index = EntityToIndex(entity);
			construct_and_assign_at_ref(storage[index]);
		}

		void Kill(Entity entity) noexcept {
			const auto index = EntityToIndex(entity);
			construct_and_assign_at_ref(storage[index]); // just in case
			// no reliable way to trim the storage yet
			// probably needs an invalid bit in entity
		}

		template<typename... Args>
		decltype(auto) Add(Entity entity, Args&&... args) noexcept {
			static_assert(always_false_v<T>, "Dense components don't need Add() call");
		}

		void Del(Entity entity) noexcept {
			static_assert(always_false_v<T>, "Dense components can't be deleted with Del()");
		}

		void SafeDel(Entity entity) noexcept {
			static_assert(always_false_v<T>, "Dense components can't be deleted with SafeDel()");
		}

		template<typename... Args>
		void Set(Entity entity, Args&&... args) noexcept {
			const auto index = EntityToIndex(entity);
			construct_and_assign_at_ref(storage[index], std::forward<Args>(args)...);
		}

		template<typename... Args>
		decltype(auto) SafeSet(Entity entity, Args&&... args) noexcept {
			const auto index = EntityToIndex(entity);
			return construct_and_assign_at_ref(storage[index], std::forward<Args>(args)...);
		}

		[[nodiscard]] decltype(auto) Get(Entity entity) const noexcept {
			const auto index = EntityToIndex(entity);
			return storage[index];
		}

		[[nodiscard]] decltype(auto) Get(Entity entity) noexcept {
			const auto index = EntityToIndex(entity);
			return storage[index];
		}

		[[nodiscard]] decltype(auto) TryGet(Entity entity) const noexcept {
			const auto index = EntityToIndex(entity);
			return &storage[index];
		}

		[[nodiscard]] decltype(auto) TryGet(Entity entity) noexcept {
			const auto index = EntityToIndex(entity);
			return &storage[index];
		}

		void Clear() noexcept {
			storage.clear();
		}

		auto Size() const noexcept {
			return storage.size();
		}
	private:
		ComponentTraits<MyStoredType>::StorageType storage;
		TypedRegistry* regPtr;
	};

	template <typename TypedRegistry, typename T>
	class SparseComponentStorage {
	public:
		template <typename... Cs>
		friend class Registry;

		template<typename TR, typename... ICs>
		friend class ViewImpl;

		using MyStoredType = T;
	public:
		SparseComponentStorage(TypedRegistry& r)
			: regPtr(&r)
		{}

		/*
		[[nodiscard]] decltype(auto) GetRawStorage() const noexcept {
			return storage;
		}

		[[nodiscard]] decltype(auto) GetRawStorage() noexcept {
			return storage;
		}

		[[nodiscard]] decltype(auto) GetMappedEntities() const noexcept {
			return mappedEntites;
		}
		*/
	private:
		void Init(Entity entity) {
			Add(entity);
		}

		void Kill(Entity entity) {
			if (!Has(entity))
				return;

			Del(entity);
		}

		template<typename... Args>
		decltype(auto) Add(Entity entity, Args&&... args) {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();

			const auto index = EntityToIndex(entity);
			auto& entityMembership = regPtr->sparseMembership[index];
			if (entityMembership.test(typeIdx))
				throw std::runtime_error("Sparse Component already added");

			entityMembership.set(typeIdx);

			auto& sparseMap = regPtr->sparseMaps[typeIdx];
			sparseMap.emplace(entity, storage.size());

			mappedEntites.emplace_back(entity);
			return storage.emplace_back(std::forward<Args>(args)...);
		}

		void Del(Entity entity) {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();
			const auto index = EntityToIndex(entity);
			auto& entityMembership = regPtr->sparseMembership[index];
			if (!entityMembership.test(typeIdx))
				throw std::runtime_error("Sparse Component already deleted");

			entityMembership.reset(typeIdx);

			auto& sparseMap = regPtr->sparseMaps[typeIdx];
			auto smIt = sparseMap.find(entity);
			assert(smIt != sparseMap.end() && "Logic error");

			const size_t posToReplace = smIt->second;
			const size_t lastIndex = storage.size() - 1;

			// must happen before sparseMap[movedEntity] = posToReplace;
			sparseMap.erase(smIt);

			if (posToReplace != lastIndex) {
				storage[posToReplace] = std::move(storage[lastIndex]);
				Entity movedEntity = mappedEntites[lastIndex];
				mappedEntites[posToReplace] = movedEntity;
				sparseMap[movedEntity] = posToReplace;
			}

			storage.pop_back();
			mappedEntites.pop_back();
		}

		void SafeDel(Entity entity) noexcept {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();
			const auto index = EntityToIndex(entity);
			auto& entityMembership = regPtr->sparseMembership[index];
			if (!entityMembership.test(typeIdx))
				return;

			entityMembership.reset(typeIdx);

			auto& sparseMap = regPtr->sparseMaps[typeIdx];
			auto smIt = sparseMap.find(entity);
			assert(smIt != sparseMap.end() && "Logic error");

			const size_t posToReplace = smIt->second;
			const size_t lastIndex = storage.size() - 1;

			// must happen before sparseMap[movedEntity] = posToReplace;
			sparseMap.erase(smIt);

			if (posToReplace != lastIndex) {
				storage[posToReplace] = std::move(storage[lastIndex]);
				Entity movedEntity = mappedEntites[lastIndex];
				mappedEntites[posToReplace] = movedEntity;
				sparseMap[movedEntity] = posToReplace;
			}

			storage.pop_back();
			mappedEntites.pop_back();
		}

		[[nodiscard]] bool Has(Entity entity) const noexcept {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();

			const auto index = EntityToIndex(entity);
			auto& entityMembership = regPtr->sparseMembership[index];
			return entityMembership.test(typeIdx);
		}

		template<typename... Args>
		void Set(Entity entity, Args&&... args) {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();

			const auto index = EntityToIndex(entity);

			if (!regPtr->sparseMembership[index].test(typeIdx))
				throw std::runtime_error("Sparse Component must be added first");

			auto& sparseMap = regPtr->sparseMaps[typeIdx];
			auto smIt = sparseMap.find(entity);
			assert(smIt != sparseMap.end() && "Logic error");

			const size_t pos = smIt->second;
			construct_and_assign_at_ref(storage[pos], std::forward<Args>(args)...);
		}

		template<typename... Args>
		decltype(auto) SafeSet(Entity entity, Args&&... args) noexcept {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();

			const auto index = EntityToIndex(entity);

			regPtr->sparseMembership[index].set(typeIdx);

			auto& sparseMap = regPtr->sparseMaps[typeIdx];
			auto smIt = sparseMap.find(entity);
			if (smIt == sparseMap.end()) {
				sparseMap.emplace(entity, storage.size());
				mappedEntites.emplace_back(entity);
				return storage.emplace_back(std::forward<Args>(args)...);
			}

			const size_t pos = smIt->second;
			return construct_and_assign_at_ref(storage[pos], std::forward<Args>(args)...);
		}

		[[nodiscard]] decltype(auto) TryGet(Entity entity) const {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();

			const auto index = EntityToIndex(entity);

			if (!regPtr->sparseMembership[index].test(typeIdx))
				return static_cast<const MyStoredType*>(nullptr);

			auto& sparseMap = regPtr->sparseMaps[typeIdx];
			auto smIt = sparseMap.find(entity);
			assert(smIt != sparseMap.end() && "Logic error");

			return &storage[smIt->second];
		}

		[[nodiscard]] decltype(auto) TryGet(Entity entity) {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();

			const auto index = EntityToIndex(entity);

			if (!regPtr->sparseMembership[index].test(typeIdx))
				return static_cast<MyStoredType*>(nullptr);

			auto& sparseMap = regPtr->sparseMaps[typeIdx];
			auto smIt = sparseMap.find(entity);
			assert(smIt != sparseMap.end() && "Logic error");

			return &storage[smIt->second];
		}

		[[nodiscard]] decltype(auto) Get(Entity entity) const {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();

			const auto index = EntityToIndex(entity);

			if (!regPtr->sparseMembership[index].test(typeIdx))
				throw std::runtime_error("Sparse Component must be added first");

			auto& sparseMap = regPtr->sparseMaps[typeIdx];
			auto smIt = sparseMap.find(entity);
			assert(smIt != sparseMap.end() && "Logic error");

			return storage[smIt->second];
		}

		[[nodiscard]] decltype(auto) Get(Entity entity) {
			static constexpr size_t typeIdx = TypedRegistry::template GetStorageIdx<MyStoredType>();

			const auto index = EntityToIndex(entity);

			if (!regPtr->sparseMembership[index].test(typeIdx))
				throw std::runtime_error("Sparse Component must be added first");

			auto& sparseMap = regPtr->sparseMaps[typeIdx];
			auto smIt = sparseMap.find(entity);
			assert(smIt != sparseMap.end() && "Logic error");

			return storage[smIt->second];
		}

		void Clear() noexcept {
			storage.clear();
			mappedEntites.clear();
			// regPtr->sparseMembership will be cleaned by Registry
		}

		auto Size() const noexcept {
			return storage.size();
		}
	private:
		ComponentTraits<MyStoredType>::StorageType storage;
		ChunkedArray<Entity, ComponentTraits<MyStoredType>::chunkSize> mappedEntites;
		// or keep its type aligned with the MyStoredType?
		//ComponentTraits<MyStoredType>::template StorageTypeT<Entity> mappedEntites;

		TypedRegistry* regPtr;
	};


	template<typename TypedRegistry, typename T>
	using StorageType = std::conditional_t<ComponentTraits<T>::compCat == ComponentCategory::Dense, DenseComponentStorage<TypedRegistry, T>, SparseComponentStorage<TypedRegistry, T>>;

	template<ComponentCategory compCat, typename T, typename C>
	using CondTupleOfOne = std::conditional_t<ComponentTraits<T>::compCat == compCat, std::tuple<C>, std::tuple<>>;

	template <ComponentCategory compCat, typename... Ts>
	using FilteredTupleTypeStorage = decltype(std::tuple_cat(std::declval<CondTupleOfOne<compCat, typename Ts::MyStoredType, Ts>>()...));

	template <ComponentCategory compCat, typename TypedRegistry, typename C>
	constexpr decltype(auto) MakeStorageTypeTuple(TypedRegistry& reg) {
		if constexpr (ComponentTraits<C>::compCat == compCat) {
			return std::make_tuple(StorageType<TypedRegistry, C>(reg));
		}
		else {
			return std::tuple<>();
		}
	};

	template <ComponentCategory compCat, typename TypedRegistry, typename... Cs>
	constexpr decltype(auto) MakeStorageTuple(TypedRegistry& reg) noexcept {
		return std::tuple_cat(MakeStorageTypeTuple<compCat, TypedRegistry, Cs>(reg)...);
	}

	template <typename... Cs>
	class Registry {
	public:
		using TypesList = type_list_t<Cs...>;
	public:
		template <typename R, typename T>
		friend class DenseComponentStorage;

		template <typename R, typename T>
		friend class SparseComponentStorage;

		template<typename TR, typename... ICs>
		friend class ViewImpl;
	public:
		Registry()
			: entities{}
			, fNext{ EntityTraits::INVALID_INDEX }
			, fSize{ 0 }
			, denseStorages{ MakeStorageTuple<ComponentCategory::Dense , Registry, Cs...>(*this) }
			, sparseStorages{ MakeStorageTuple<ComponentCategory::Sparse, Registry, Cs...>(*this) }
		{
			static_assert(NUM_COMPONENTS > 0, "Define at least one component at Registry type level");

			// Validate all components have CHUNK_SIZE that is a power of two
			(ValidateChunk<Cs>(), ...);

			// Check if the type can be default initialized
			(ValidateDefaultInitializable<Cs>(), ...);
		}
		~Registry() { Clear(); }
		Registry(const Registry&) = delete;
		Registry(Registry&&) noexcept = delete;
		Registry& operator=(const Registry&) = delete;
		Registry& operator=(Registry&&) noexcept = delete;
	public:
		Entity CreateEntity() {
			Entity entity;

			if (fSize) {
				const auto i = fNext;
				fNext = EntityToIndex(entities[i]);
				--fSize;

				const auto v = EntityToVersion(entities[i]);
				entities[i] = ComposeEntity(i, v);

				entity = entities[i];
			}
			else {
				if (entities.size() >= EntityTraits::INVALID_INDEX)
					throw std::runtime_error("Can't create Entity (too many entities)");

				entity = entities.emplace_back(ComposeEntity(
					static_cast<uint32_t>(entities.size()),
					0u
				));
			}

			const auto idx = EntityToIndex(entity);
			sparseMembership.ensure_size(idx + 1);

			for_each_tuple([this, &entity](auto&& s) {
				s.Init(entity);
			}, denseStorages);

			return entity;
		}

		void DestroyEntity(Entity entity) {
			CheckEntity(entity);

			const auto i = EntityToIndex(entity);
			const auto v = NextEntityVersion(entities[i]);
			entities[i] = ComposeEntity(fNext, v);
			fNext = i;
			++fSize;

			for_each_tuple([this, &entity](auto&& s) {
				s.Kill(entity);
			}, denseStorages);

			for_each_tuple([this, &entity](auto&& s) {
				s.Kill(entity);
			}, sparseStorages);
		}

		bool IsValidEntity(Entity entity) const {
			const auto [i, v] = EntityToIndexAndVersion(entity);
			if (i >= entities.size())
				return false;

			if (EntityToVersion(entities[i]) != v)
				return false;

			return true;
		}

		template<typename C, typename... Args>
		decltype(auto) Add(Entity entity, Args&&... args) {
			CheckEntity(entity);

			auto& storage = GetStorage<C>();
			return storage.Add(entity, std::forward<Args>(args)...);
		}

		template<typename... Cts>
		decltype(auto) Add(Entity entity) {
			CheckEntity(entity);

			return std::forward_as_tuple(GetStorage<Cts>().Add(entity)...);
		}

		template<typename C>
		void Del(Entity entity) {
			CheckEntity(entity);

			auto& storage = GetStorage<C>();
			storage.Del(entity);
		}

		template<typename C>
		void SafeDel(Entity entity) {
			CheckEntity(entity);

			auto& storage = GetStorage<C>();
			storage.SafeDel(entity);
		}

		template<typename C>
		[[nodiscard]] bool Has(Entity entity) const {
			CheckEntity(entity);

			return HasImpl<C>(EntityToIndex(entity));
		}

		template<typename... Cts>
		[[nodiscard]] bool HasAll(Entity entity) const {
			CheckEntity(entity);

			auto i = EntityToIndex(entity);
			return (true  && ... && HasImpl<Cts>(i));
		}

		template<typename... Cts>
		[[nodiscard]] bool HasAny(Entity entity) const {
			CheckEntity(entity);

			auto i = EntityToIndex(entity);
			return (false || ... || HasImpl<Cts>(i));
		}

		template<typename C, typename... Args>
		void Set(Entity entity, Args&&... args) {
			CheckEntity(entity);
			auto& storage = GetStorage<C>();
			storage.Set(entity, std::forward<Args>(args)...);
		}

		template<typename C, typename... Args>
		decltype(auto) SafeSet(Entity entity, Args&&... args) {
			CheckEntity(entity);

			auto& storage = GetStorage<C>();
			return storage.SafeSet(entity, std::forward<Args>(args)...);
		}

		template<typename C>
		[[nodiscard]] decltype(auto) Get(Entity entity) const {
			CheckEntity(entity);
			auto& storage = GetStorage<C>();
			return storage.Get(entity);
		}

		template<typename C>
		[[nodiscard]] decltype(auto) Get(Entity entity) {
			CheckEntity(entity);
			auto& storage = GetStorage<C>();
			return storage.Get(entity);
		}

		template<typename... Cts>
		[[nodiscard]] decltype(auto) Get(Entity entity) const requires MoreThanOneType<Cts...> {
			CheckEntity(entity);

			return std::forward_as_tuple(GetStorage<Cts>().Get(entity)...);
		}

		template<typename... Cts>
		[[nodiscard]] decltype(auto) Get(Entity entity) requires MoreThanOneType<Cts...> {
			CheckEntity(entity);

			return std::forward_as_tuple(GetStorage<Cts>().Get(entity)...);
		}

		template<typename... Cts>
		[[nodiscard]] decltype(auto) GetNonEmpty(Entity entity) const {
			CheckEntity(entity);

			return std::tuple_cat(ForwardNonEmpty<Cts>(entity)...);
		}

		template<typename... Cts>
		[[nodiscard]] decltype(auto) GetNonEmpty(Entity entity) {
			CheckEntity(entity);

			return std::tuple_cat(ForwardNonEmpty<Cts>(entity)...);
		}

		template<typename C>
		[[nodiscard]] decltype(auto) TryGet(Entity entity) const {
			CheckEntity(entity);
			auto& storage = GetStorage<C>();
			return storage.TryGet(entity);
		}

		template<typename C>
		[[nodiscard]] decltype(auto) TryGet(Entity entity) {
			CheckEntity(entity);
			auto& storage = GetStorage<C>();
			return storage.TryGet(entity);
		}

		template<typename... Cts>
		[[nodiscard]] decltype(auto) TryGet(Entity entity) const requires MoreThanOneType<Cts...> {
			CheckEntity(entity);

			return std::make_tuple(GetStorage<Cts>().TryGet(entity)...);
		}

		template<typename... Cts>
		[[nodiscard]] decltype(auto) TryGet(Entity entity) requires MoreThanOneType<Cts...> {
			CheckEntity(entity);

			return std::make_tuple(GetStorage<Cts>().TryGet(entity)...);
		}

		template<typename... Cts>
		void RemoveFromAll() {
			for (const auto& e : entities) {
				((Has<Cts>(e) && Del<Cts>(e)), ...);
			}
		}

		void Clear() noexcept {
			entities.clear();
			fNext = EntityTraits::INVALID_INDEX;
			fSize = 0;
			sparseMembership.clear();
			for_each_tuple([this](auto&& s) { s.Clear(); }, denseStorages);
			for_each_tuple([this](auto&& s) { s.Clear(); }, sparseStorages);

			for (auto& sm : sparseMaps)
				sm.clear();
		}

		auto RawSize() const noexcept {
			return entities.size();
		}

		auto Size() const noexcept {
			return entities.size() - fSize;
		}

		template<typename... ICs, typename... ECs>
		auto View(ExcludeComponentsListT<ECs...> excl = {}) noexcept {
			return ViewImpl<Registry, ICs...>(*this, excl);
		}
		template<typename... ICs, typename... ECs, typename OByT>
		auto View(OrderByT<OByT> obtl, ExcludeComponentsListT<ECs...> excl = {}) noexcept {
			return ViewImpl<Registry, ICs...>(*this, obtl, excl);
		}
	private:
		template<typename T>
		inline bool HasImpl(uint32_t index) const noexcept {
			using Td = std::decay_t<T>;
			if constexpr (ComponentTraits<Td>::compCat == ComponentCategory::Dense) {
				return true;
			}
			else {
				static constexpr size_t typeIdx = GetStorageIdx<Td>();
				return sparseMembership[index].test(typeIdx);
			}
		}

		template<typename T>
		inline decltype(auto) ForwardNonEmpty(Entity e) const noexcept {
			if constexpr (std::is_empty_v<T>) {
				return std::tuple<>{};
			}
			else {
				return std::forward_as_tuple(GetStorage<T>().Get(e));
			}
		}

		template<typename T>
		inline decltype(auto) ForwardNonEmpty(Entity e) noexcept {
			if constexpr (std::is_empty_v<T>) {
				return std::tuple<>{};
			}
			else {
				return std::forward_as_tuple(GetStorage<T>().Get(e));
			}
		}

		void CheckEntity(Entity entity) const {
			if (entity == NullEntity)
				throw std::runtime_error("Invalid Entity (Null Entity)");

			const auto [i, v] = EntityToIndexAndVersion(entity);
			if (i >= entities.size())
				throw std::runtime_error("Invalid Entity (out of bounds)");

			if (EntityToVersion(entities[i]) != v)
				throw std::runtime_error("Invalid Entity (already deleted)");
		}

		template <typename T>
		static void ValidateChunk() {
			static_assert((ComponentTraits<T>::allocType == AllocationType::Contiguous) || std::has_single_bit(ComponentTraits<T>::chunkSize), "Component CHUNK_SIZE must be a power of two");
		}

		template <typename T>
		static void ValidateDefaultInitializable() {
			static_assert(std::default_initializable<T>, "Component must be default initializable");
		}

		template <typename T>
		static constexpr size_t GetStorageIdx() noexcept {
			using Td = std::decay_t<T>;
			if constexpr (ComponentTraits<Td>::compCat == ComponentCategory::Dense) {
				using DenseStoragesT = decltype(denseStorages);
				using DenseStorageT = DenseComponentStorage<Registry, Td>;
				static_assert(tuple_contains_type_v<DenseStoragesT, DenseStorageT>, "DenseStorageT not found");
				return tuple_type_index_v<DenseStorageT, DenseStoragesT>;
			}
			else {
				using SparseStoragesT = decltype(sparseStorages);
				using SparseStorageT = SparseComponentStorage<Registry, Td>;
				static_assert(tuple_contains_type_v<SparseStoragesT, SparseStorageT>, "SparseStorageT not found");
				return tuple_type_index_v<SparseStorageT, SparseStoragesT>;
			}
		}

		template <typename T>
		inline decltype(auto) GetStorage() noexcept {
			if constexpr (ComponentTraits<T>::compCat == ComponentCategory::Dense) {
				return std::get<GetStorageIdx<T>()>(denseStorages);
			}
			else {
				return std::get<GetStorageIdx<T>()>(sparseStorages);
			}
		}

		template <typename T>
		decltype(auto) GetStorage() const noexcept {
			if constexpr (ComponentTraits<T>::compCat == ComponentCategory::Dense) {
				return std::get<GetStorageIdx<T>()>(denseStorages);
			}
			else {
				return std::get<GetStorageIdx<T>()>(sparseStorages);
			}
		}

	private:
		static constexpr size_t NUM_COMPONENTS = sizeof...(Cs);
		static constexpr size_t NUM_SPARSE_COMPONENTS = ((ComponentTraits<Cs>::compCat == ComponentCategory::Sparse ? 1 : 0) + ... + 0);
		static constexpr size_t NUM_DENSE_COMPONENTS = ((ComponentTraits<Cs>::compCat == ComponentCategory::Dense ? 1 : 0) + ... + 0);
		static_assert(NUM_SPARSE_COMPONENTS + NUM_DENSE_COMPONENTS == NUM_COMPONENTS);
	public:
		ChunkedArray<Entity, EntityChunkSize<Registry>> entities;
		uint32_t fNext;
		uint32_t fSize;
		ChunkedArray<std::bitset<NUM_SPARSE_COMPONENTS>, EntityChunkSize<Registry>> sparseMembership;
		FilteredTupleTypeStorage<ComponentCategory::Dense, DenseComponentStorage <Registry, Cs>...>  denseStorages;
		FilteredTupleTypeStorage<ComponentCategory::Sparse, SparseComponentStorage<Registry, Cs>...> sparseStorages;
		std::array<SparseMapType<Registry>, NUM_SPARSE_COMPONENTS> sparseMaps; // entity to dense pos
	};

	template<typename C>
	using IsDenseComponent = std::bool_constant<ComponentTraits<C>::compCat == ComponentCategory::Dense>;

	template<typename C>
	using IsSparseComponent = std::bool_constant<ComponentTraits<C>::compCat == ComponentCategory::Sparse>;

	template<typename Target>
	struct not_same_as {
		template<class X>
		using pred = std::bool_constant<!std::is_same_v<std::decay_t<X>, std::decay_t<Target>>>;
	};


	template<typename TypedRegistry, typename... ICs>
	class ViewImpl {
	public:
		template <typename... Cs>
		friend class Registry;
	private:
		template<typename... ECs>
		ViewImpl(TypedRegistry& r, ExcludeComponentsListT<ECs...> = {})
			: regPtr(&r)
		{
			InitCheck(ExcludeComponentsList<ECs...>);
			InitOrderByEntity(ExcludeComponentsList<ECs...>);
		}
		template<typename OByT, typename... ECs>
		ViewImpl(TypedRegistry& r, OrderByT<OByT>, ExcludeComponentsListT<ECs...>)
			: regPtr(&r)
		{
			InitCheck(ExcludeComponentsList<ECs...>);

			using SparseICs = type_list_filter_t<IsSparseComponent, type_list_t<ICs...>>;

			using OByTd = std::decay_t<OByT>;
			if constexpr (type_in_list_v<OByTd, SparseICs>) {
				InitOrderByType(ExcludeComponentsList<ECs...>, OrderBy<OByT>);
			}
			else {
				InitOrderByEntity(ExcludeComponentsList<ECs...>);
			}
		}
	private:
		template<typename... ECs>
		void InitCheck(ExcludeComponentsListT<ECs...> = {}) {
			constexpr bool ValidECTL = (true && ... && type_in_list_v<std::decay_t<ECs>, typename TypedRegistry::TypesList>);
			constexpr bool ValidICTL = (true && ... && type_in_list_v<std::decay_t<ICs>, typename TypedRegistry::TypesList>);
			static_assert(ValidECTL, "Exclude components list contains type not present in the associated registry");
			static_assert(ValidICTL, "Include components list contains type not present in the associated registry");

			using DecayedICTypesList = typename type_list_t<std::decay_t<ICs>...>::type;

			constexpr bool ECsInICs = (false || ... || type_in_list_v<std::decay_t<ECs>, DecayedICTypesList>);
			static_assert(!ECsInICs, "Exclude components list contains components from the include components list");

			using DenseECs = type_list_filter_t<IsDenseComponent, type_list_t<ECs...>>;
			static_assert(!ValidECTL || DenseECs::size == 0, "Exclude components list can't contain Dense components");
		}

		template<typename... ECs>
		void InitOrderByEntity(ExcludeComponentsListT<ECs...> = {}) {
			using SparseECs = type_list_t<ECs...>;
			using SparseICs = type_list_filter_t<IsSparseComponent, type_list_t<ICs...>>;

			constexpr static auto TypeListFunctorOr = []<typename F, typename... Ts>(type_list_t<Ts...>&&, F && f) {
				return (false || ... || f(type_list<Ts>));
			};
			constexpr static auto TypeListFunctorAnd = []<typename F, typename... Ts>(type_list_t<Ts...>&&, F && f) {
				return (true  && ... && f(type_list<Ts>));
			};

			entities.reserve(regPtr->entities.size()); //overkill, but one allocation only

			for (auto entity : regPtr->entities) {
				bool include = regPtr->IsValidEntity(entity) &&
					TypeListFunctorAnd(SparseICs{}, [this, entity]<typename T>(type_list_t<T>) {
					using Td = std::decay_t<T>;
					return regPtr->template Has<Td>(entity);
				}) &&
					!TypeListFunctorOr(SparseECs{}, [this, entity]<typename T>(type_list_t<T>) {
					using Td = std::decay_t<T>;
					return regPtr->template Has<Td>(entity);
				});

				if (include)
					entities.emplace_back(entity);
			}			
		}
		template<typename OByT, typename... ECs>
		void InitOrderByType(ExcludeComponentsListT<ECs...>, OrderByT<OByT>) {
			static_assert(type_in_list_v<std::decay_t<OByT>, typename TypedRegistry::TypesList>, "OrderBy component is not present in the associated registry");

			using SparseECs = type_list_t<ECs...>;
			using SparseICs = type_list_filter_t<IsSparseComponent, type_list_t<ICs...>>;

			if constexpr (SparseICs::size == 0) {
				InitOrderByEntity(ExcludeComponentsList<ECs...>);
			}
			else {
				using OByTd = std::decay_t<OByT>;
				const auto& oByTStorage = regPtr->template GetStorage<OByTd>();

				using RestOfSparseICs = type_list_filter_t<not_same_as<OByTd>::template pred, SparseICs>;

				constexpr static auto TypeListFunctorOr = []<typename F, typename... Ts>(type_list_t<Ts...>&&, F && f) {
					return (false || ... || f(type_list<Ts>));
				};
				constexpr static auto TypeListFunctorAnd = []<typename F, typename... Ts>(type_list_t<Ts...>&&, F && f) {
					return (true  && ... && f(type_list<Ts>));
				};

				entities.reserve(oByTStorage.Size());
				if constexpr (RestOfSparseICs::size == 0) {
					for (auto entity : oByTStorage.mappedEntites)
						entities.emplace_back(entity);
				}
				else {
					for (auto entity : oByTStorage.mappedEntites) {
						bool include = /*regPtr->IsValidEntity(entity) &&*/
							TypeListFunctorAnd(RestOfSparseICs{}, [this, entity]<typename T>(type_list_t<T>) {
							using Td = std::decay_t<T>;
							return regPtr->template Has<Td>(entity);
						}) &&
							!TypeListFunctorOr(SparseECs{}, [this, entity]<typename T>(type_list_t<T>) {
							using Td = std::decay_t<T>;
							return regPtr->template Has<Td>(entity);
						});

						if (include)
							entities.emplace_back(entity);
					}
				}
			}
		}
	public:
		template<typename C, typename... Args>
		void Set(Entity entity, Args&&... args) {
			regPtr->template Set<C>(entity, std::forward<Args>(args)...);
		}
		template<typename C>
		[[nodiscard]] decltype(auto) Get(Entity entity) const {
			return as_const_ptr(regPtr)->template Get<C>(entity);
		}
		template<typename C>
		[[nodiscard]] decltype(auto) Get(Entity entity) {
			return regPtr->template Get<C>(entity);
		}
		template<typename... Cts>
		[[nodiscard]] decltype(auto) Get(Entity entity) const requires MoreThanOneType<Cts...> {
			return as_const_ptr(regPtr)->template Get<Cts...>(entity);
		}
		template<typename... Cts>
		[[nodiscard]] decltype(auto) Get(Entity entity) requires MoreThanOneType<Cts...> {
			return regPtr->template Get<Cts...>(entity);
		}
		template<typename... Cts>
		[[nodiscard]] decltype(auto) GetNonEmpty(Entity entity) const {
			return as_const_ptr(regPtr)->template GetNonEmpty<Cts...>(entity);
		}
		template<typename... Cts>
		[[nodiscard]] decltype(auto) GetNonEmpty(Entity entity) {
			return regPtr->template GetNonEmpty<Cts...>(entity);
		}
		auto Size() const { return entities.size(); }

		decltype(auto) operator[](size_t i) const {
			return entities[i];
		}
	private:
		TypedRegistry* regPtr = nullptr;
		std::vector<Entity> entities;
	public:
		// Regular iterators: back-to-front
		auto begin() {
			return std::rbegin(entities);
		}
		auto end() {
			return std::rend(entities);
		}
		auto begin() const {
			return std::rbegin(entities);
		}
		auto end() const {
			return std::rend(entities);
		}
		auto cbegin() const {
			return std::rbegin(entities);
		}
		auto cend() const {
			return std::rend(entities);
		}

		// Reverse iterators: front-to-back
		auto rbegin() {
			return std::begin(entities);
		}
		auto rend() {
			return std::end(entities);
		}
		auto rbegin() const {
			return std::begin(entities);
		}
		auto rend() const {
			return std::end(entities);
		}
		auto crbegin() const {
			return std::begin(entities);
		}
		auto crend() const {
			return std::end(entities);
		}
	};
}

#undef recs_likely
#undef recs_unlikely