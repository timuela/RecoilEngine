/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#pragma once

#include <utility>
#include <compare>
#include <concepts>
#include <type_traits>

#include "System/Ecs/EcsMain.hpp"

template<class T>
struct BasicClassComponentType {
    using ValueType = T;
    ValueType value;
};

template<class T>
struct BasicComponentType {
    using ValueType = T;
    ValueType value = ValueType{};
};

template<>
struct BasicComponentType<entt::entity> {
    using ValueType = entt::entity;
    entt::entity value{entt::null};
};

template<typename T, typename U>
concept HasSameValueType = requires {
    typename T::ValueType;
    typename U::ValueType;
} && std::same_as<std::decay_t<typename T::ValueType>, std::decay_t<typename U::ValueType>>;

template<typename T>
concept IsComposedComponent =
    std::derived_from<T, BasicClassComponentType<typename T::ValueType>> ||
    std::derived_from<T, BasicComponentType<typename T::ValueType>>;


#define ALIAS_COMPONENT_DEF(Component, T, DefaultValue) \
struct Component : public BasicComponentType<T> { \
    Component() { value = T { DefaultValue }; } \
    ~Component() = default; \
    Component(const Component &) = default; \
    Component& operator=(const Component &) = default; \
    Component(Component &&) noexcept = default; \
    Component& operator=(Component &&) noexcept = default; \
    operator const T&() const { return value; } \
    operator       T&()       { return value; } \
    Component(const T& other) { value = other;  }  \
    Component& operator=(const T& other) { value = other; return *this; } \
    Component(T&& other) noexcept { value = std::move(other); } \
    Component& operator=(T&& other) noexcept { value = std::move(other); return *this; } \
}

#define ALIAS_COMPONENT(Component, T) \
struct Component : public BasicComponentType<T> { \
    Component() = default; \
    ~Component() = default; \
    Component(const Component &) = default; \
    Component& operator=(const Component &) = default; \
    Component(Component &&) noexcept = default; \
    Component& operator=(Component &&) noexcept = default; \
    operator const T&() const { return value; } \
    operator       T&()       { return value; } \
    Component(const T& other) { value = other;  }  \
    Component& operator=(const T& other) { value = other; return *this; } \
    Component(T&& other) noexcept { value = std::move(other); } \
    Component& operator=(T&& other) noexcept { value = std::move(other); return *this; } \
}

#define ALIAS_CLASS_COMPONENT(Component, T) \
struct Component : public BasicClassComponentType<T> { \
    Component() = default; \
    ~Component() = default; \
    Component(const Component &) = default; \
    Component& operator=(const Component &) = default; \
    Component(Component &&) noexcept = default; \
    Component& operator=(Component &&) noexcept = default; \
    operator const T&() const { return value; } \
    operator       T&()       { return value; } \
    Component(const T& other) { value = other;  }  \
    Component& operator=(const T& other) { value = other; return *this; } \
    Component(T&& other) noexcept { value = std::move(other); } \
    Component& operator=(T&& other) noexcept { value = std::move(other); return *this; } \
}
#define ALIAS_COMPONENT_LIST_RESERVE(Component, T, RESERVE) \
struct Component : public BasicClassComponentType<T> { \
    Component() { value.reserve(RESERVE); } \
    ~Component() = default; \
    Component(const Component &) = default; \
    Component& operator=(const Component &) = default; \
    Component(Component &&) noexcept = default; \
    Component& operator=(Component &&) noexcept = default; \
    operator const T&() const { return value; } \
    operator       T&()       { return value; } \
    Component(const T& other) { value = other;  }  \
    Component& operator=(const T& other) { value = other; return *this; } \
    Component(T&& other) noexcept { value = std::move(other); } \
    Component& operator=(T&& other) noexcept { value = std::move(other); return *this; } \
}
#define VOID_COMPONENT(Component) struct Component {}

// T = A<T> * B<T>
template<typename U, typename T> auto operator*(const T& t, const U& u)
    requires IsComposedComponent<T> && HasSameValueType<T, U>
{
    return t.value * u.value;
}