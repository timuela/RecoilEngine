#pragma once

#include <memory>
#include <functional>
#include <tuple>
#include <variant>
#include <utility>
#include <type_traits>
#include <stdexcept>


#include "Matrix44f.h"
#include "Transform.hpp"

// TODO Move to namespace Recoil
namespace spring {
	template<bool...> struct bool_pack;
	template<bool... bs>
	using all_true = std::is_same<bool_pack<bs..., true>, bool_pack<true, bs...>>;

	// check if parameters pack is constructible
	template<class... Ts>
	using are_all_constructible = all_true<std::is_constructible_v<std::decay_t<Ts>>...>;

	// https://stackoverflow.com/questions/38067106/c-verify-callable-signature-of-template-type
	template<typename, typename, typename = void>
	struct is_signature : std::false_type {};

	// use static_assert(is_signature<SomeTemplateF, int(int)>::value);
	template<typename TFunc, typename Ret, typename... Args>
	struct is_signature<TFunc, Ret(Args...),
			typename std::enable_if_t<
				std::is_convertible<TFunc, std::function<Ret(Args...)>>::value
			>
		> : public std::true_type
	{};

	// use static_assert(is_signature<std::function<int(int)>, decltype(someFunction)>::value);
	template<typename TFunc1, typename TFunc2>
	struct is_signature<TFunc1, TFunc2,
			typename std::enable_if_t<
				std::is_convertible<TFunc1, TFunc2>::value
			>
		> : public std::true_type
	{};
	
	template<typename... Args>
	inline constexpr bool is_signature_v = is_signature<Args...>::value;

	// cpp20 compat
	template< class T >
	struct remove_cvref {
		typedef std::remove_cv_t<std::remove_reference_t<T>> type;
	};

	// Generic dispatcher using lambda array, w/o recursion
	template<typename Tuple, typename Func, std::size_t... Is>
	inline void tuple_exec_at_impl(size_t index, Tuple& t, Func&& f, std::index_sequence<Is...>)
	{
		using FnType = void(*)(Tuple&, Func&&);
		static constexpr FnType table[] = {
			[](Tuple& tup, Func&& func) { func(std::get<Is>(tup)); }...
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

	// https://blog.tartanllama.xyz/exploding-tuples-fold-expressions/
	template <std::size_t... Idx>
	auto make_index_dispatcher(std::index_sequence<Idx...>) {
		return [](auto&& f) { (f(std::integral_constant<std::size_t, Idx>{}), ...); };
	}

	template <std::size_t N>
	auto make_index_dispatcher() {
		return make_index_dispatcher(std::make_index_sequence<N>{});
	}

	template<typename T>
	struct return_type { using type = T; };

	template<typename R, typename... Ts>
	struct return_type<std::function<R(Ts...)>> { using type = R; };

	template<typename R, typename... Ts>
	struct return_type<std::function<R(Ts...)> const> { using type = R; };

	template<typename R, typename T, typename... Ts>
	struct return_type<std::function<R(Ts...)> T::*> { using type = R; };

	template<typename R, typename T, typename... Ts>
	struct return_type<std::function<R(Ts...)> const T::*> { using type = R; };

	template<typename R, typename T, typename... Ts>
	struct return_type<std::function<R(Ts...)> T::* const&> { using type = R; };

	template<typename R, typename T, typename... Ts>
	struct return_type<std::function<R(Ts...)> const T::* const> { using type = R; };

	template<typename R, typename... Ts>
	struct return_type<R(*)(Ts...)> { using type = R; };

	template<typename R, typename... Ts>
	struct return_type<R& (*)(Ts...)> { using type = R; };

	template<typename R, typename T>
	struct return_type<R(T::*)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<R& (T::*)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<std::shared_ptr<R>(T::*)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<std::shared_ptr<R>& (T::*)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<R(T::* const)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<R& (T::* const)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<std::shared_ptr<R>(T::* const)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<std::shared_ptr<R>& (T::* const)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<R(T::* const&)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<R& (T::* const&)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<std::shared_ptr<R>(T::* const&)() const> { using type = R; };

	template<typename R, typename T>
	struct return_type<std::shared_ptr<R>& (T::* const&)() const> { using type = R; };

	template<typename T>
	using return_type_t = typename return_type<T>::type;
	

	template<typename TupleType, typename Type>
	struct tuple_contains_type;

	template<typename Type, typename... TupleElementTypes>
	struct tuple_contains_type<std::tuple<TupleElementTypes...>, Type> : std::disjunction<std::is_same<Type, TupleElementTypes>...> {};

	template<typename TupleType, typename Type>
	constexpr inline bool tuple_contains_type_v = tuple_contains_type<TupleType, Type>::value;


	template <typename T, typename Tuple>
	struct tuple_type_index;

	template <typename T, typename... Types>
	struct tuple_type_index<T, std::tuple<T, Types...>> {
		static const std::size_t value = 0;
	};

	template <typename T, typename U, typename... Types>
	struct tuple_type_index<T, std::tuple<U, Types...>> {
		static const std::size_t value = 1 + tuple_type_index<T, std::tuple<Types...>>::value;
	};
	template <typename T, typename Tuple>
	constexpr size_t tuple_type_index_v = tuple_type_index<T, Tuple>::value;


	template<typename FuncType>
	struct func_signature;

	template<typename ReturnType, typename... ArgTypes>
	struct func_signature<ReturnType(ArgTypes...)> {
		using type = std::tuple<ArgTypes...>;
		using ret = ReturnType;
	};

	template<typename FuncType>
	using func_signature_t = typename func_signature<FuncType>::type;

	template<
		typename F,
		std::enable_if_t<std::is_function<F>::value, bool> = true
	>
	auto arg_types_tuple_t(const F&) -> typename func_signature<F>::type;
	template<
		typename F,
		std::enable_if_t<std::is_function<F>::value, bool> = true
	>
	auto arg_types_tuple_t(const F*) -> typename func_signature<F>::type;


	// This particular helper accepts a nullptr, in which case it falls back to a specified default signature, or just an empty tuple
	template<auto FuncPtr, typename... FallbackSignature>
	struct func_ptr_signature {
		using type = func_signature_t<std::remove_pointer_t<std::remove_pointer_t<decltype(FuncPtr)>>>;
	};
	template<typename... FallbackSignature>
	struct func_ptr_signature<nullptr, FallbackSignature...> {
		using type = std::tuple<FallbackSignature...>;
	};

	template<auto FuncPtr, typename... FallbackSignature>
	using func_ptr_signature_t = typename func_ptr_signature<FuncPtr, FallbackSignature...>::type;
};

namespace Recoil {
	// taken from https://github.com/spnda/fastgltf/blob/main/include/fastgltf/util.hpp
	// The MIT License
	// Copyright (C) 2022 - 2025 Sean Apeler
	template <typename Visitor, typename Variant, std::size_t... i>
	constexpr bool is_exhaustive_visitor(std::integer_sequence<std::size_t, i...>) noexcept {
		return std::conjunction_v<std::is_invocable<Visitor, std::variant_alternative_t<i, std::remove_cvref_t<Variant>>>...>;
	}

	/**
	 * Simple wrapper around std::visit for a single variant that checks at compile-time if the given visitor contains
	 * overloads for *all* required alternatives.
	 */
	template<typename Visitor, typename Variant>
		constexpr decltype(auto) visit_exhaustive(Visitor&& visitor, Variant&& variant) {
		static_assert(is_exhaustive_visitor<Visitor, Variant>(std::make_index_sequence<std::variant_size_v<std::remove_cvref_t<Variant>>>()),
			"The visitor does not include all necessary overloads for the given variant");
		return std::visit(std::forward<Visitor>(visitor), std::forward<Variant>(variant));
	}

	// useful in the context of static_assert(always_false_v<T>, ...)
	// https://stackoverflow.com/a/76675119
	template <typename T>
	constexpr bool always_false_v = false;
}

namespace Concepts {
	template <typename T>
	concept HasSizeAndData = requires(T t) {
		{ t.size() } -> std::same_as<std::size_t>;
		{ t.data() } -> std::convertible_to<const typename T::value_type*>;
	};
	template<typename T>
	concept HasMemberBeginEnd = requires(T t) {
		{ t.begin() } -> std::input_or_output_iterator;
		{ t.end() } -> std::sentinel_for<decltype(t.begin())>;
	};
	template <typename T>
	concept CanTransform = std::same_as<T, CMatrix44f> || std::same_as<T, Transform>;
}