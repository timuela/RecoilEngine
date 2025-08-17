#pragma once

#include <utility>

#include "EcsMain.hpp"
#include "System/Threading/ThreadPool.h"
#include "System/TemplateUtils.hpp"

namespace ECS {
    template <typename TypeTag>
    class EntityOwner {
    public:
        explicit EntityOwner()
            : entity(registry.create())
        {}

        ~EntityOwner() {
            if (registry.valid(entity)) {
                registry.destroy(entity);
            }
        }

        EntityOwner(const EntityOwner&) = delete;
        EntityOwner(EntityOwner&& other) noexcept { *this = std::move(other); }

        EntityOwner& operator=(const EntityOwner&) = delete;
        EntityOwner& operator=(EntityOwner&& other) noexcept {
            std::swap(entity, other.entity);
            return *this;
        }

        template<typename T, typename... Args>
        T& Add(Args&&... args) {
            return registry.emplace<T>(entity, std::forward<Args>(args)...);
        }

        template<typename... T>
        decltype(auto) Add() {
            return std::forward_as_tuple(registry.emplace<T>(entity)...);
        }

        template<typename... T>
        decltype(auto) Get() {
            return registry.get<T...>(entity);
        }

        template<typename... T>
        decltype(auto) Get() const {
            return registry.get<T...>(entity);
        }

        template<typename... T>
        bool Has() const {
            return registry.all_of<T...>(entity);
        }

        template<typename T>
        decltype(auto) Set(T&& val) {
            auto& curr = registry.get<T>(entity);
            curr = std::forward<T>(val);
            return curr;
        }

        template<typename... T>
        void Remove() {
            registry.remove<T...>(entity);
        }

        auto Entity() const { return entity; }

        template<typename... Components, typename... ExcludeComponents, typename Func>
        static void ForEachView(Func&& func, entt::exclude_t<ExcludeComponents...> excl = {}) {
            auto items = registry.view<Components...>(excl);
            for (auto entity : items) {
                func(entity, items.template get<Components>(entity)...);
            }
        }

        template<typename... Components, typename... ExcludeComponents, typename Func>
        static auto ForEachViewAsync(Func&& func, entt::exclude_t<ExcludeComponents...> excl = {}) {
            // copy excl and func on purpose
            return ThreadPool::Enqueue([excl, func]() {
                auto items = registry.view<Components...>(excl);
                for (auto entity : items) {
                    func(entity, items.template get<Components>(entity)...);
                }
            });
        }

        template<typename... Components, typename... ExcludeComponents, typename Func>
        static void ForEachViewParallel(Func&& func, entt::exclude_t<ExcludeComponents...> excl = {}) {
            auto items = registry.view<Components...>(excl);
            for_mt_chunk(0, items.size(), [&items, &func](int i) {
                const auto entity = items[i];
                func(entity, items.template get<Components>(entity)...);
            });
        }

        template<typename... Components, typename... ExcludeComponents, typename Func>
        static void ForEachGroup(Func&& func, entt::exclude_t<ExcludeComponents...> excl = {}) {
            auto items = registry.group<Components...>(excl);
            for (auto entity : items) {
                func(entity, items.template get<Components>(entity)...);
            }
        }

        template<typename... Components, typename... ExcludeComponents, typename Func>
        static auto ForEachGroupAsync(Func&& func, entt::exclude_t<ExcludeComponents...> excl = {}) {
            // copy excl and func on purpose
            return ThreadPool::Enqueue([excl, func]() {
                auto items = registry.group<Components...>(excl);
                for (auto entity : items) {
                    func(entity, items.template get<Components>(entity)...);
                }
            });
        }

        template<typename... Components, typename... ExcludeComponents, typename Func>
        static void ForEachGroupParallel(Func&& func, entt::exclude_t<ExcludeComponents...> excl = {}) {
            auto items = registry.group<Components...>(excl);
            for_mt_chunk(0, items.size(), [&items, &func](int i) {
                const auto entity = items[i];
                func(entity, items.template get<Components>(entity)...);
            });
        }

    private:
        entt::entity entity{ entt::null };
        static inline entt::registry registry{};
    };
}