#pragma once

#include <cassert>
#include <utility>
#include <tuple>

#include "System/Ecs/RecsMain.hpp"
#include "System/Threading/ThreadPool.h"

namespace RECS {
	using namespace recs;

	// fwd declarations
	template <typename ... Cs>
	class EntityOwner;

	template <typename ... Cs>
	class EntityReference;

	template<typename... Ts>
	concept MoreThanOneType = (sizeof...(Ts) > 1);


	template <typename ... Cs>
	class EntityBase {
	protected:
		EntityBase() {}
		EntityBase(const EntityBase&) = default;
		EntityBase(EntityBase&& other) noexcept = default;

		EntityBase& operator=(const EntityBase&) = default;
		EntityBase& operator=(EntityBase&& other) noexcept = default;
	public:
		bool operator==(const EntityBase& other) const {
			return entity == other.entity;
		}

		template<typename T, typename... Args>
		inline decltype(auto) Add(Args&&... args) {
			return registry.template Add<T>(entity, std::forward<Args>(args)...);
		}

		template<typename... T>
		inline decltype(auto) Add() requires MoreThanOneType<T...> {
			return registry.template Add<T...>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) Get() {
			return registry.template Get<T...>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) Get() const {
			return std::as_const(registry).template Get<T...>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) GetMutable() const {
			return registry.template Get<T...>(entity);
		}

		template<typename T, typename... Args>
		inline decltype(auto) GetOrAdd(Args&&... args) {
			return registry.template SafeSet<T>(entity, std::forward<Args>(args)...);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) TryGet() {
			return registry.template TryGet<T...>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) TryGet() const {
			return std::as_const(registry).template TryGet<T...>(entity);
		}

		template<typename T>
		[[nodiscard]] inline bool Has() const {
			return std::as_const(registry).template Has<T>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline bool HasAll() const {
			return std::as_const(registry).template HasAll<T...>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline bool HasAny() const {
			return std::as_const(registry).template HasAny<T...>(entity);
		}

		template<typename T>
		inline decltype(auto) Set(T&& val) {
			return registry.template Set<T>(entity, std::forward<T>(val));
		}

		template<typename... T>
		inline void Remove() {
			registry.template Del<T...>(entity);
		}

		template<typename... T>
		inline void SafeRemove() {
			registry.template SafeDel<T...>(entity);
		}

		[[nodiscard]] auto EntityID() const { return entity; }

		template<typename... T>
		inline static void RemoveFromAll() {
			registry.template RemoveFromAll<T...>();
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		inline static void ForEachView(Func&& func, ExcludeComponentsListT<ExcludedComponents...> excl = {}) {
			auto entities = registry.template View<Components...>(excl);
			for (auto entity : entities) {
				std::apply(func, make_arg_tuple(entities, entity, type_list<Components...>));
			}
		}

		template<typename... Components, typename... ExcludedComponents, typename OByT, typename Func>
		inline static void ForEachView(Func&& func, OrderByT<OByT> obtl, ExcludeComponentsListT<ExcludedComponents...> excl = {}) {
			auto entities = registry.template View<Components...>(obtl, excl);
			for (auto entity : entities) {
				std::apply(func, make_arg_tuple(entities, entity, type_list<Components...>));
			}
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		[[nodiscard]] inline static auto ForEachViewAsync(Func&& func, ExcludeComponentsListT<ExcludedComponents...> excl = {}) {
			// copy excl and func on purpose
			return ThreadPool::Enqueue([excl, func]() {
				auto entities = registry.template View<Components...>(excl);
				for (auto entity : entities) {
					std::apply(func, make_arg_tuple(entities, entity, type_list<Components...>));
				}
			});
		}

		template<typename... Components, typename... ExcludedComponents, typename OByT, typename Func>
		[[nodiscard]] inline static auto ForEachViewAsync(Func&& func, OrderByT<OByT> obtl, ExcludeComponentsListT<ExcludedComponents...> excl = {}) {
			// copy excl and func on purpose
			return ThreadPool::Enqueue([excl, obtl, func]() {
				auto entities = registry.template View<Components...>(obtl, excl);
				for (auto entity : entities) {
					std::apply(func, make_arg_tuple(entities, entity, type_list<Components...>));
				}
			});
		}

		template<typename... Components, typename... ExcludedComponents, typename OByT, typename Func>
		inline static void ForEachViewParallel(Func&& func, OrderByT<OByT> obtl, ExcludeComponentsListT<ExcludedComponents...> excl = {}) {
			auto entities = registry.template View<Components...>(obtl, excl);

			for_mt_chunk(0, entities.Size(), [&entities, &func](int i) {
				std::apply(func, make_arg_tuple(entities, std::as_const(entities)[i], type_list<Components...>));
			}, ParallelMinChunk, ParallelMaxChunk);
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		inline static void ForEachViewParallel(Func&& func, ExcludeComponentsListT<ExcludedComponents...> excl = {}) {
			auto entities = registry.template View<Components...>(excl);

			for_mt_chunk(0, entities.Size(), [&entities, &func](int i) {
				std::apply(func, make_arg_tuple(entities, std::as_const(entities)[i], type_list<Components...>));
			}, ParallelMinChunk, ParallelMaxChunk);
		}

		inline static void SetParallelNumberOfChunks(int minChunks = ENTT_PACKED_PAGE, int maxChunks = std::numeric_limits<int>::max()) {
			ParallelMinChunk = minChunks;
			ParallelMaxChunk = maxChunks;
		}

		inline static void Cleanup() {
			registry = {};
		}
	private:
		template<typename... Ts, typename V>
		inline static decltype(auto) make_arg_tuple(V&& v, Entity e, type_list_t<Ts...>) {
			return std::tuple_cat(
				std::make_tuple(EntityReference<Cs...>(e)),
				v.template GetNonEmpty<Ts...>(e)
			);
		}
	protected:
		Entity entity{};
		static inline Registry<Cs...> registry{};
		static inline int ParallelMinChunk = DEFAULT_DENSE_CHUNK_SIZE;
		static inline int ParallelMaxChunk = std::numeric_limits<int>::max();
	};

	// non-owning class for an entity
	template <typename... Cs>
	class EntityReference : public EntityBase<Cs...> {
	public:
		using EB = EntityBase<Cs...>;
		explicit EntityReference(const EntityBase<Cs...>& entityOwner)
			: EB()
		{
			this->entity = entityOwner.Entity();
		}
		explicit EntityReference(const Entity ownedEntity = NullEntity)
			: EB()
		{
			this->entity = ownedEntity;
		}
	};


	// owning class for an entity that automatically creates and destroys it
	template <typename... Cs>
	class EntityOwner : public EntityBase<Cs...> {
	public:
		using EB = EntityBase<Cs...>;
		explicit EntityOwner()
			: EB()
		{
			this->entity = EB::registry.CreateEntity();
		}

		~EntityOwner() {
			EB::registry.DestroyEntity(this->entity);
		}

		EntityOwner(const EntityOwner&) = delete;
		EntityOwner(EntityOwner&& other) noexcept { *this = std::move(other); }

		EntityOwner& operator=(const EntityOwner&) = delete;
		EntityOwner& operator=(EntityOwner&& other) noexcept {
			std::swap(this->entity, other.entity);
			return *this;
		}

		EntityReference<Cs...> CreateReference() const {
			return EntityReference<Cs...>(*this);
		}
	};
}