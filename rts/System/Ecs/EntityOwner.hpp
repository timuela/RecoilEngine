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
        decltype(auto) Add(Args&&... args) {
            assert(!registry.any_of<T>(entity));
            return registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename... T>
        decltype(auto) Add() {
            assert(!registry.any_of<T...>(entity));
            return std::forward_as_tuple(emplace_helper<T>()...);
        }

        template<typename... T>
        decltype(auto) Get() {
            assert(registry.all_of<T...>(entity));
            return registry.get<T...>(entity);
        }

        template<typename... T>
        decltype(auto) Get() const {
            assert(registry.all_of<T...>(entity));
            return std::as_const(registry).get<T...>(entity);
        }

        template<typename T, typename... Args>
        decltype(auto) GetOrAdd(Args&&... args) {
            return registry.get_or_emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename... T>
        decltype(auto) TryGet() {
            return registry.try_get<T...>(entity);
        }

        template<typename... T>
        decltype(auto) TryGet() const {
            return std::as_const(registry).try_get<T...>(entity);
        }

        template<typename T>
        bool Has() const {
            return std::as_const(registry).any_of<T>(entity);
        }

        template<typename... T>
        bool HasAll() const {
            return std::as_const(registry).all_of<T...>(entity);
        }

        template<typename... T>
        bool HasAny() const {
            return std::as_const(registry).any_of<T...>(entity);
        }

        template<typename T>
        decltype(auto) Set(T&& val) {
            assert(registry.all_of<T>(entity));
            return registry.replace<T>(entity, std::forward<T>(val));
        }

        template<typename... T>
        void Remove() {
            assert(registry.all_of<T...>(entity));
            registry.remove<T...>(entity);
        }

        auto EntityID() const { return entity; }

        template<typename... T>
        static void RemoveFromAll() {
            registry.clear<T...>();
        }

        template<typename... Components, typename... ExcludedComponents, typename Func>
        static void ForEachView(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
            auto entities = registry.view<Components...>(excl);
            for (auto entity : entities) {
                func(EntityReference<TypeTag>(entity), entities.template get<Components>(entity)...);
            }
        }

        template<typename... Components, typename... ExcludedComponents, typename Func>
        static auto ForEachViewAsync(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
            // copy excl and func on purpose
            return ThreadPool::Enqueue([excl, func]() {
                auto entities = registry.view<Components...>(excl);
                for (auto entity : entities) {
                    func(EntityReference<TypeTag>(entity), entities.template get<Components>(entity)...);
                }
            });
        }

        template<typename... Components, typename... ExcludedComponents, typename Func>
        static void ForEachViewParallel(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
            auto entities = registry.view<Components...>(excl);
            for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
                const auto entity = entities[i];
                func(EntityReference<TypeTag>(entity), entities.template get<Components>(entity)...);
            });
        }

        template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
        static void ForEachGroup(Func&& func, EntityIncludeType<ObservedComponents...> obsv = {}, EntityExcludeType<ExcludedComponents...> excl = {}) {
            auto entities = registry.group<OwnedComponents...>(obsv, excl);
            for (auto entity : entities) {
                func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
            }
        }
        template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
        static void ForEachGroup(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
            auto entities = registry.group<OwnedComponents...>(excl);
            for (auto entity : entities) {
                func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
            }
        }

        template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
        static auto ForEachGroupAsync(Func&& func, EntityIncludeType<ObservedComponents...> obsv = {}, EntityExcludeType<ExcludedComponents...> excl = {}) {
            // copy excl and func on purpose
            return ThreadPool::Enqueue([obsv, excl, func]() {
                auto entities = registry.group<OwnedComponents...>(obsv, excl);
                for (auto entity : entities) {
                    func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
                }
            });
        }
        template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
        static auto ForEachGroupAsync(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
            // copy excl and func on purpose
            return ThreadPool::Enqueue([excl, func]() {
                auto entities = registry.group<OwnedComponents...>(excl);
                for (auto entity : entities) {
                    func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
                }
            });
        }

        template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
        static void ForEachGroupParallel(Func&& func, EntityIncludeType<ObservedComponents...> obsv = {}, EntityExcludeType<ExcludedComponents...> excl = {}) {
            auto entities = registry.group<OwnedComponents...>(obsv, excl);
            for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
                const auto entity = entities[i];
                func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
            });
        }
        template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
        static void ForEachGroupParallel(Func&& func, EntityExcludeType<ExcludedComponents...> excl = {}) {
            auto entities = registry.group<OwnedComponents...>(excl);
            for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
                const auto entity = entities[i];
                func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
            });
        }

        static void Cleanup() {
            registry = {};
        }
    private:
        template<typename U>
        decltype(auto) emplace_helper() {
            if constexpr (std::is_void_v<decltype(registry.emplace<U>(entity))>) {
                registry.emplace<U>(entity);
                return std::monostate{}; // placeholder for void
            }
            else {
                return registry.emplace<U>(entity);
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