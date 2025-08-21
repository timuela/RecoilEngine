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
		inline static void ForEachView(const Func& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.view<Components...>(excl);
			for (auto entity : entities) {
				func(EntityReference<TypeTag>(entity), entities.template get<Components>(entity)...);
			}
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		inline static auto ForEachViewAsync(const Func& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			// copy excl and func on purpose
			return ThreadPool::Enqueue([excl, func]() {
				auto entities = registry.view<Components...>(excl);
				for (auto entity : entities) {
					func(EntityReference<TypeTag>(entity), entities.template get<Components>(entity)...);
				}
			});
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		inline static void ForEachViewParallel(const Func& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.view<Components...>(excl);
			for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
				const auto entity = entities[i];
				func(EntityReference<TypeTag>(entity), entities.template get<Components>(entity)...);
			});
		}

		template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
		inline static void ForEachGroup(const Func& func, EntityIncludeType<ObservedComponents...> obsv, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.group<OwnedComponents...>(obsv, excl);
			try_invoke_with_arguments<OwnedComponents..., ObservedComponents...>(entities, func);
		}
		template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
		inline static void ForEachGroup(const Func& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.group<OwnedComponents...>(excl);
			for (auto entity : entities) {
				func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
			}
		}

		template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
		inline static auto ForEachGroupAsync(const Func& func, EntityIncludeType<ObservedComponents...> obsv, EntityExcludeType<ExcludedComponents...> excl = {}) {
			// copy excl and func on purpose
			return ThreadPool::Enqueue([obsv, excl, func]() {
				auto entities = registry.group<OwnedComponents...>(obsv, excl);
				for (auto entity : entities) {
					func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
				}
			});
		}
		template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
		inline static auto ForEachGroupAsync(const Func& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			// copy excl and func on purpose
			return ThreadPool::Enqueue([excl, func]() {
				auto entities = registry.group<OwnedComponents...>(excl);
				for (auto entity : entities) {
					func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
				}
			});
		}

		template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
		inline static void ForEachGroupParallel(const Func& func, EntityIncludeType<ObservedComponents...> obsv, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.group<OwnedComponents...>(obsv, excl);
			for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
				const auto entity = entities[i];
				func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
			});
		}
		template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
		inline static void ForEachGroupParallel(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
			auto entities = registry.group<OwnedComponents...>(excl);
			for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
				const auto entity = entities[i];
				func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
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

		template<typename R, typename... Types>
		inline static decltype(auto) get_tuple_from_type_list(R& r, entt::entity entity, spring::type_list_t<Types...> t) {
			if constexpr (sizeof...(Types) == 0) {
				return std::tuple<>{};
			}
			else {
				return std::make_tuple(r.template get<Types>(entity)...);
			}
		}

		template<typename R, typename... Types>
		inline static decltype(auto) get_type_list(R& r, entt::entity entity, spring::type_list_t<Types...> t) {
			using NoVoidList = spring::type_list_skip_void_t<std::conditional_t<!entt::is_ebco_eligible_v<Types>, Types, void>...>;
			return get_tuple_from_type_list(r, entity, NoVoidList{});
		}

		template<typename... Types, typename R, typename Func>
		inline static void try_invoke_with_tuple_args(R& r, const Func& func, std::tuple<Types...>&& allArgsTuple) {
			using AllArgsTuple = std::decay_t<decltype(allArgsTuple)>;

			if constexpr (spring::is_tuple_list_args_invocable_v<Func, AllArgsTuple>) {
				// Success: invoke
				std::apply(func, std::move(allArgsTuple));
			}
			else {
				// Fail: try popping an optional arg (if any remain)
				constexpr std::size_t N = std::tuple_size_v<AllArgsTuple>;
				if constexpr (N > 0) {
					try_invoke_with_tuple_args(r, func, spring::tuple_pop_back_type(allArgsTuple));
				}
				else {
					static_assert(Recoil::always_false_v<Func>, "No valid overload found for given function and args.");
				}
			}
		}

		template<typename... Types, typename R, typename Func>
		inline static void try_invoke_with_arguments(R& r, const Func& func) {
			for (auto entity : r) {
				auto allArgsTuple = std::tuple_cat(
					std::make_tuple(EntityReference<TypeTag>(entity)),
					get_type_list(r, entity, spring::type_list_t<Types...>{})
				);

				try_invoke_with_tuple_args(r, func, std::move(allArgsTuple));
			}
		}

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