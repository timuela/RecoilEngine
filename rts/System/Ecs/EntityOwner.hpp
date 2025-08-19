#pragma once

#include <cassert>

#include "EcsMain.hpp"
#include "System/Threading/ThreadPool.h"
#include "System/TemplateUtils.hpp"

namespace ECS {
    using EntityType = entt::entity;

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
        template<typename T, typename... Args>
        T& Add(Args&&... args) {
            assert(!registry.any_of<T>(entity));
            return registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename... T>
        decltype(auto) Add() {
            assert(!registry.any_of<T...>(entity));
            return std::forward_as_tuple(registry.emplace<T>(entity)...);
        }

        template<typename... T>
        decltype(auto) Get() {
            assert(registry.all_of<T...>(entity));
            return registry.get<T...>(entity);
        }

        template<typename... T>
        decltype(auto) Get() const {
            assert(registry.all_of<T...>(entity));
            return registry.get<T...>(entity);
        }

        template<typename T, typename... Args>
        decltype(auto) GetOrAdd(Args&&... args) {
            return registry.get_or_emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        decltype(auto) GetOrAdd(Args&&... args) const {
            return registry.get_or_emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename... T>
        decltype(auto) TryGet() {
            return registry.try_get<T...>(entity);
        }

        template<typename... T>
        decltype(auto) TryGet() const {
            return registry.try_get<T...>(entity);
        }

        template<typename T>
        bool Has() const {
            return registry.any_of<T>(entity);
        }

        template<typename... T>
        bool HasAll() const {
            return registry.all_of<T...>(entity);
        }

        template<typename... T>
        bool HasAny() const {
            return registry.any_of<T...>(entity);
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

        auto Entity() const { return entity; }

        template<typename... T>
        static void RemoveFromAll() {
            registry.clear<T...>();
        }

        template<typename... Components, typename... ExcludedComponents, typename Func>
        static void ForEachView(Func&& func, entt::exclude_t<ExcludedComponents...> excl = {}) {
            auto entities = registry.view<Components...>(excl);
            for (auto entity : entities) {
                func(EntityReference<TypeTag>(entity), entities.template get<Components>(entity)...);
            }
        }

        template<typename... Components, typename... ExcludedComponents, typename Func>
        static auto ForEachViewAsync(Func&& func, entt::exclude_t<ExcludedComponents...> excl = {}) {
            // copy excl and func on purpose
            return ThreadPool::Enqueue([excl, func]() {
                auto entities = registry.view<Components...>(excl);
                for (auto entity : entities) {
                    func(EntityReference<TypeTag>(entity), entities.template get<Components>(entity)...);
                }
            });
        }

        template<typename... Components, typename... ExcludedComponents, typename Func>
        static void ForEachViewParallel(Func&& func, entt::exclude_t<ExcludedComponents...> excl = {}) {
            auto entities = registry.view<Components...>(excl);
            for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
                const auto entity = entities[i];
                func(EntityReference<TypeTag>(entity), entities.template get<Components>(entity)...);
            });
        }

        template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
        static void ForEachGroup(Func&& func, entt::get_t<ObservedComponents...> obsv = {}, entt::exclude_t<ExcludedComponents...> excl = {}) {
            auto entities = registry.group<OwnedComponents...>(obsv, excl);
            for (auto entity : entities) {
                func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
            }
        }
        template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
        static void ForEachGroup(Func&& func, entt::exclude_t<ExcludedComponents...> excl = {}) {
            auto entities = registry.group<OwnedComponents...>(excl);
            for (auto entity : entities) {
                func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
            }
        }

        template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
        static auto ForEachGroupAsync(Func&& func, entt::get_t<ObservedComponents...> obsv = {}, entt::exclude_t<ExcludedComponents...> excl = {}) {
            // copy excl and func on purpose
            return ThreadPool::Enqueue([obsv, excl, func]() {
                auto entities = registry.group<OwnedComponents...>(obsv, excl);
                for (auto entity : entities) {
                    func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
                }
            });
        }
        template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
        static auto ForEachGroupAsync(Func&& func, entt::exclude_t<ExcludedComponents...> excl = {}) {
            // copy excl and func on purpose
            return ThreadPool::Enqueue([excl, func]() {
                auto entities = registry.group<OwnedComponents...>(excl);
                for (auto entity : entities) {
                    func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
                }
            });
        }

        template<typename... OwnedComponents, typename... ObservedComponents, typename... ExcludedComponents, typename Func>
        static void ForEachGroupParallel(Func&& func, entt::get_t<ObservedComponents...> obsv = {}, entt::exclude_t<ExcludedComponents...> excl = {}) {
            auto entities = registry.group<OwnedComponents...>(obsv, excl);
            for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
                const auto entity = entities[i];
                func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
            });
        }
        template<typename... OwnedComponents, typename... ExcludedComponents, typename Func>
        static void ForEachGroupParallel(Func&& func, entt::exclude_t<ExcludedComponents...> excl = {}) {
            auto entities = registry.group<OwnedComponents...>(excl);
            for_mt_chunk(0, entities.size(), [&entities, &func](int i) {
                const auto entity = entities[i];
                func(EntityReference<TypeTag>(entity), entities.template get<OwnedComponents>(entity)...);
            });
        }

        static void Cleanup() {
            registry = {};
        }
    protected:
        EntityType entity{ entt::null };
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