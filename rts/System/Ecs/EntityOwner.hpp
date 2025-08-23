#pragma once

#include <cassert>
#include <utility>
#include <tuple>

#include "EcsMain.hpp"
#include "System/Threading/ThreadPool.h"
#include "System/TemplateUtils.hpp"

namespace ECS {
	// fwd declarations
	template <typename TypeTag>
	class EntityOwner;

	template <typename TypeTag>
	class EntityReference;

	template <typename... T>
	struct OwnedComponentsListT : spring::type_list_t<T...> {};
	template <typename... T>
	struct ObservedComponentsListT : spring::type_list_t<T...> {};
	template <typename... T>
	struct ExcludedComponentsListT : spring::type_list_t<T...> {};

	template <typename... T>
	static constexpr OwnedComponentsListT OwnedComponentsList{};
	template <typename... T>
	static constexpr ObservedComponentsListT ObservedComponentsList{};
	template <typename... T>
	static constexpr ExcludedComponentsListT ExcludedComponentsList{};


	template <typename TypeTag>
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
			assert(!registry.any_of<T>(entity));
			return registry.emplace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename... T>
		inline decltype(auto) Add() {
			assert(!registry.any_of<T...>(entity));
			// alternatively can skip void types completely with forward_nonvoid_as_tuple(T...)?
			return std::forward_as_tuple(emplace_helper<T>()...);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) Get() {
			assert(registry.all_of<T...>(entity));
			return registry.get<T...>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) Get() const {
			assert(registry.all_of<T...>(entity));
			return std::as_const(registry).get<T...>(entity);
		}

		template<typename T, typename... Args>
		[[nodiscard]] inline decltype(auto) GetOrAdd(Args&&... args) {
			return registry.get_or_emplace<T>(entity, std::forward<Args>(args)...);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) TryGet() {
			return registry.try_get<T...>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) TryGet() const {
			return std::as_const(registry).try_get<T...>(entity);
		}

		template<typename T>
		[[nodiscard]] inline bool Has() const {
			return std::as_const(registry).any_of<T>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline bool HasAll() const {
			return std::as_const(registry).all_of<T...>(entity);
		}

		template<typename... T>
		[[nodiscard]] inline bool HasAny() const {
			return std::as_const(registry).any_of<T...>(entity);
		}

		template<typename T>
		inline decltype(auto) Set(T&& val) {
			assert(registry.all_of<T>(entity));
			return registry.replace<T>(entity, std::forward<T>(val));
		}

		template<typename... T>
		inline void Remove() {
			assert(registry.all_of<T...>(entity));
			registry.remove<T...>(entity);
		}

		[[nodiscard]] auto EntityID() const { return entity; }

		template<typename... T>
		inline static void RemoveFromAll() {
			registry.clear<T...>();
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		inline static void ForEachView(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.view<Components...>(excl);
			for (auto entity : entities) {
				std::apply(func, make_arg_tuple(entities, entity, spring::type_list<Components...>));
			}
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		inline static auto ForEachViewAsync(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			// copy excl and func on purpose
			return ThreadPool::Enqueue([excl, func]() {
				auto entities = registry.view<Components...>(excl);
				for (auto entity : entities) {
					std::apply(func, make_arg_tuple(entities, entity, spring::type_list<Components...>));
				}
			});
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		inline static void ForEachViewParallel(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.view<Components...>(excl);

			// entities doesn't allow for random access, so we need to copy them to a vector first
			static std::vector<EntityType> entitiesVec;
			entitiesVec.clear();
			entitiesVec.reserve(entities.size_hint());
			for (auto entity : entities) { entitiesVec.emplace_back(entity); }

			for_mt_chunk(0, entitiesVec.size(), [&entities, &func](int i) {
				const auto entity = entitiesVec[i];
				std::apply(func, make_arg_tuple(entities, entity, spring::type_list<Components...>));
			});
		}

		template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
		inline static void ForEachGroup(Func&& func, EntityIncludeType<ObservedComponents...> obsv, EntityExcludeType<ExcludedComponents...> excl = {}) {
			spring::type_list_t<OwnedComponents...> owned;
			auto entities = registry.group<OwnedComponents...>(obsv, excl);
			for (auto entity : entities) {
				std::apply(func, make_arg_tuple(entities, entity, spring::type_list<OwnedComponents..., ObservedComponents...>));
			}
		}
		template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
		inline static void ForEachGroup(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.group<OwnedComponents...>(excl);
			for (auto entity : entities) {
				std::apply(func, make_arg_tuple(entities, entity, spring::type_list<OwnedComponents...>));
			}
		}

		template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
		inline static auto ForEachGroupAsync(Func&& func, EntityIncludeType<ObservedComponents...> obsv, EntityExcludeType<ExcludedComponents...> excl = {}) {
			// copy excl and func on purpose
			return ThreadPool::Enqueue([obsv, excl, func]() {
				auto entities = registry.group<OwnedComponents...>(obsv, excl);
				for (auto entity : entities) {
					std::apply(func, make_arg_tuple(entities, entity, spring::type_list<OwnedComponents..., ObservedComponents...>));
				}
			});
		}
		template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
		inline static auto ForEachGroupAsync(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			// copy excl and func on purpose
			return ThreadPool::Enqueue([excl, func]() {
				auto entities = registry.group<OwnedComponents...>(excl);
				for (auto entity : entities) {
					std::apply(func, make_arg_tuple(entities, entity, spring::type_list<OwnedComponents...>));
				}
			});
		}

		template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
		inline static void ForEachGroupParallel(Func&& func, EntityIncludeType<ObservedComponents...> obsv, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.group<OwnedComponents...>(obsv, excl);
			for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
				const auto entity = entities[i];
				std::apply(func, make_arg_tuple(entities, entity, spring::type_list<OwnedComponents..., ObservedComponents...>));
			});
		}
		template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
		inline static void ForEachGroupParallel(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.group<OwnedComponents...>(excl);
			for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
				const auto entity = entities[i];
				std::apply(func, make_arg_tuple(entities, entity, spring::type_list<OwnedComponents...>));
			});
		}

		inline static void Cleanup() {
			registry = {};
		}
	private:
		template<typename U>
		inline decltype(auto) emplace_helper() {
			if constexpr (std::is_void_v<decltype(registry.emplace<U>(entity))>) {
				registry.emplace<U>(entity);
				return std::monostate{}; // placeholder for void
			}
			else {
				return registry.emplace<U>(entity);
			}
		}

		template<typename... Ts, typename R>
		inline decltype(auto) static make_arg_tuple(R& r, EntityType entity, spring::type_list_t<Ts...> tl) {
			auto TypeTuple = [&]<typename Type>(spring::type_list_t<Type> tt) {
				if constexpr (entt::is_ebco_eligible_v<Type>) {
					return std::tuple<>{};
				}
				else {
					return std::make_tuple(r.template get<Type>(entity));
				}
			};

			return std::tuple_cat(
				std::make_tuple(EntityReference<TypeTag>(entity)),
				TypeTuple(spring::type_list_t<Ts>{})...
			);
		}

		template<typename... Types>
		struct no_void_types {
			using type = spring::type_list_skip_void_t<std::conditional_t<!entt::is_ebco_eligible_v<Types>, Types, void>...>;
		};
		template<typename... Types>
		using no_void_types_t = typename no_void_types<Types...>::type;
	protected:
		EntityType entity{ NullEntity };
		static inline entt::registry registry{};
	};

	// non-owning class for an entity
	template <typename TypeTag>
	class EntityReference : public EntityBase<TypeTag> {
	public:
		using EB = EntityBase<TypeTag>;
		explicit EntityReference(const EntityBase<TypeTag>& entityOwner)
			: EB()
		{
			this->entity = entityOwner.Entity();
		}
		explicit EntityReference(const EntityType ownedEntity)
			: EB()
		{
			this->entity = ownedEntity;
		}
	};


	// owning class for an entity that automatically creates and destroys it
	template <typename TypeTag>
	class EntityOwner : public EntityBase<TypeTag> {
	public:
		using EB = EntityBase<TypeTag>;
		explicit EntityOwner()
			: EB()
		{
			this->entity = EB::registry.create();
		}

		~EntityOwner() {
			if (EB::registry.valid(this->entity)) {
				EB::registry.destroy(this->entity);
			}
		}

		EntityOwner(const EntityOwner&) = delete;
		EntityOwner(EntityOwner&& other) noexcept { *this = std::move(other); }

		EntityOwner& operator=(const EntityOwner&) = delete;
		EntityOwner& operator=(EntityOwner&& other) noexcept {
			std::swap(this->entity, other.entity);
			return *this;
		}

		EntityReference<TypeTag> CreateReference() const {
			return EntityReference<TypeTag>(*this);
		}
	};
}