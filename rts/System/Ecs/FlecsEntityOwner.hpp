#pragma once

#include <cassert>
#include <utility>
#include <tuple>

#include "EcsMain.hpp"
#include "System/Threading/ThreadPool.h"
#include "System/TemplateUtils.hpp"

namespace Flecs {
	using MyEntityType = flecs::entity;

	static inline auto NullEntity = MyEntityType{};

	// fwd declarations
	template <typename TypeTag>
	class EntityOwner;

	template <typename TypeTag>
	class EntityReference;

	template <typename... T>
	struct ExcludedComponentsListT : spring::type_list_t<T...> {};

	template <typename... T>
	static constexpr ExcludedComponentsListT<T...> ExcludedComponentsList{};

	template<typename... Ts>
	concept MoreThanOneType = (sizeof...(Ts) > 1);

	template <typename T>
	concept HasFlecsComponentTraits = requires {
		{ T::flecsTraits } -> std::convertible_to<flecs::entity_t>;
	};

	template <typename TypeTag>
	class EntityBase {
	public:
		class DeferToken {
		public:
			DeferToken(flecs::world& r)
				: regPtr(&r)
			{
				regPtr->defer_begin();
			}
			~DeferToken()
			{
				regPtr->defer_end();
			}
			DeferToken(DeferToken&&) noexcept = delete;
			DeferToken(const DeferToken&) = delete;
			DeferToken& operator=(DeferToken&&) noexcept = delete;
			DeferToken& operator=(const DeferToken&) = delete;
		private:
			flecs::world* regPtr;
		};
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
			assert(!has<T>());
			return emplace_helper<T>(std::forward<Args>(args)...);
		}

		template<typename... T>
		inline decltype(auto) Add() requires MoreThanOneType<T...> {
			assert(!has_any<T...>());
			return std::tuple_cat(add_helper<T>()...);
		}

		template<typename T>
		[[nodiscard]] inline decltype(auto) Get() {
			assert(has<T>());
			return single_get_helper<T>();
		}

		template<typename T>
		[[nodiscard]] inline decltype(auto) Get() const {
			assert(has<T>());
			return single_get_helper<T>();
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) Get() requires MoreThanOneType<T...> {
			assert(has_all<T...>());
			return std::tuple_cat(get_helper<T>()...);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) Get() const requires MoreThanOneType<T...> {
			assert(has_all<T...>());
			return std::tuple_cat(get_helper<spring::add_const_if_not<T>>()...);
		}

		template<typename T>
		[[nodiscard]] inline decltype(auto) GetMutable() const {
			assert(has_all<T>());
			return spring::as_mutable(*this).template single_get_helper<T>();
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) GetMutable() const requires MoreThanOneType<T...> {
			assert(has_all<T...>());
			return std::tuple_cat(spring::as_mutable(*this).template get_helper<T>()...);
		}

		template<typename T, typename... Args>
		[[nodiscard]] inline decltype(auto) GetOrAdd(Args&&... args) {
			if constexpr (std::is_empty_v<T>) {
				if (!has<T>())
					entity.add<T>();
			}
			else {
				if (auto* p = single_try_get_helper<T>(); p) {
					return *p;
				}

				return emplace_helper<T>(std::forward<Args>(args)...);
			}
		}

		template<typename T>
		[[nodiscard]] inline decltype(auto) TryGet() {
			return single_try_get_helper<T>();
		}

		template<typename T>
		[[nodiscard]] inline decltype(auto) TryGet() const {
			return single_try_get_helper<T>();
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) TryGet() requires MoreThanOneType<T...> {
			return std::tuple_cat(try_get_helper<T>()...);
		}

		template<typename... T>
		[[nodiscard]] inline decltype(auto) TryGet() const requires MoreThanOneType<T...> {
			return std::tuple_cat(try_get_helper<T>()...);
		}

		template<typename T>
		[[nodiscard]] inline bool Has() const {
			return has<T>();
		}

		template<typename... T>
		[[nodiscard]] inline bool HasAll() const {
			return has_all<T...>();
		}

		template<typename... T>
		[[nodiscard]] inline bool HasAny() const {
			return has_any<T...>();
		}

		template<typename T>
		inline decltype(auto) Set(T&& val) {
			assert(has<T>());
			return entity.set<T>(std::forward<T>(val));
		}

		template<typename... T>
		inline void Remove() {
			assert(has_all<T...>());
			entity.remove<T...>();
		}

		template<typename... T>
		inline void SafeRemove() {
			((has<T>() && entity.remove<T>()), ...);
		}

		template<typename T>
		inline void Enable() {
			static_assert(std::is_empty_v<T>);
			//assert(has<T>());
			if (!entity.enabled<T>())
				entity.enable<T>();
		}

		template<typename T>
		inline void Disable() {
			static_assert(std::is_empty_v<T>);
			//assert(has<T>());
			if (entity.enabled<T>())
				entity.disable<T>();
		}

		template<typename T>
		inline bool Enabled() const {
			static_assert(std::is_empty_v<T>);
			//assert(has<T>());
			return entity.enabled<T>();
		}

		[[nodiscard]] auto EntityID() const { return entity; }

		template<typename... T>
		inline static void RemoveFromAll() {
			registry.remove_all<T...>();
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		inline static void ForEachView(Func&& func, ExcludedComponentsListT<ExcludedComponents...> = {}) {
			auto query = make_query<Components...>(ExcludedComponentsList<ExcludedComponents...>);

			query.each([&func]<typename... Args>(flecs::entity e, Args&& ... args) {
				std::invoke(std::forward<Func>(func), EntityReference<TypeTag>(e), std::forward<Args>(args)...);
			});
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		[[nodiscard]] inline static auto ForEachViewAsync(Func&& func, ExcludedComponentsListT<ExcludedComponents...> = {}) {
			// copy func on purpose
			return ThreadPool::Enqueue([func]() {
				auto query = make_query<Components...>(ExcludedComponentsList<ExcludedComponents...>);

				query.each([&func]<typename... Args>(flecs::entity e, Args&& ... args) {
					func(EntityReference<TypeTag>(e), std::forward<Args>(args)...);
				});
			});
		}

		template<typename... Components, typename... ExcludedComponents, typename Func>
		inline static void ForEachViewParallel(Func&& func, ExcludedComponentsListT<ExcludedComponents...> = {}) {
			auto query = make_query<Components...>(ExcludedComponentsList<ExcludedComponents...>);

			const int numElems = query.count();
			if (numElems == 0)
				return;

			static const int maxThreads = ThreadPool::GetNumThreads();

			const int chunkSize = std::clamp(numElems / maxThreads + (numElems % maxThreads != 0), ParallelMinChunk, ParallelMaxChunk);
			const int numThreads = numElems / chunkSize + (numElems % chunkSize != 0);

			if (numThreads == 1) {
				query.each([&func]<typename... Args>(flecs::entity e, Args&& ... args) {
					func(EntityReference<TypeTag>(e), std::forward<Args>(args)...);
				});
				return;
			}

			for_mt(0, numThreads, 1, [func, &query, numThreads](const int numThread) {
				query.iter().worker(numThread, numThreads).each([func]<typename... Args>(flecs::entity e, Args&& ... args) {
					func(EntityReference<TypeTag>(e), std::forward<Args>(args)...);
				});
			});

		}

		inline static void SetParallelNumberOfChunks(int minChunks = ENTT_PACKED_PAGE, int maxChunks = std::numeric_limits<int>::max()) {
			ParallelMinChunk = minChunks;
			ParallelMaxChunk = maxChunks;
		}

		inline static void Cleanup() {
			registry = std::move(flecs::world());
		}

		inline static auto DumpRegistry() {
			return registry.to_json();
		}

		inline static auto ScopedDefer() {
			return DeferToken(registry);
		}
	private:
		template<typename T>
		bool has() const noexcept {
			return entity.has<T>();
		}

		template<typename... T>
		bool has_any() const noexcept {
			return (false || ... || entity.has<T>());
		}

		template<typename... T>
		bool has_all() const noexcept {
			return (true  && ... && entity.has<T>());
		}

		template<typename U, typename... Args>
		inline decltype(auto) emplace_helper(Args&&... args) {
			if constexpr (HasFlecsComponentTraits<U>) {
				registry.component<U>().add(U::flecsTraits);
			}

			if constexpr (std::is_empty_v<U>) {
				entity.add<U>(std::forward<Args>(args)...);
			}
			else {
				entity.emplace<U>(std::forward<Args>(args)...);
				return entity.get_mut<U>();
			}
		}

		template<typename U>
		inline decltype(auto) single_get_helper() {
			if constexpr (std::is_empty_v<U>) {
				return; // don't return anything for single type
			}
			else if constexpr (std::is_const_v<U>) {
				return entity.get<U>();
			}
			else {
				return entity.get_mut<U>();
			}
		}

		template<typename U>
		inline decltype(auto) single_get_helper() const {
			if constexpr (std::is_empty_v<U>) {
				return;  // don't return anything for single type
			}
			else {
				return entity.get<U>();
			}
		}

		template<typename U>
		inline decltype(auto) single_try_get_helper() {
			if constexpr (std::is_empty_v<U>) {
				return; // don't return anything for empty type
			}
			else if constexpr (std::is_const_v<U>) {
				return entity.try_get<U>();
			}
			else {
				return entity.try_get_mut<U>();
			}
		}

		template<typename U>
		inline decltype(auto) single_try_get_helper() const {
			if constexpr (std::is_empty_v<U>) {
				return; // don't return anything for empty type
			}
			else {
				return entity.try_get<U>();
			}
		}

		template<typename U>
		inline decltype(auto) add_helper() {
			if constexpr (HasFlecsComponentTraits<U>) {
				registry.component<U>().add(U::flecsTraits);
			}

			entity.emplace<U>();
			if constexpr (std::is_empty_v<U>) {
				return std::tuple<>(); // placeholder for void
			}
			else {
				return std::forward_as_tuple(entity.get_mut<U>());
			}
		}

		template<typename U>
		inline decltype(auto) get_helper() {
			if constexpr (std::is_empty_v<U>) {
				return std::tuple<>(); // placeholder for void
			}
			else if constexpr (std::is_const_v<U>) {
				return std::forward_as_tuple(entity.get<U>());
			}
			else {
				return std::forward_as_tuple(entity.get_mut<U>());
			}
		}

		template<typename U>
		inline decltype(auto) get_helper() const {
			if constexpr (std::is_empty_v<U>) {
				return std::tuple<>(); // placeholder for void
			}
			else {
				return std::forward_as_tuple(entity.get<U>());
			}
		}


		template<typename U>
		inline decltype(auto) try_get_helper() {
			if constexpr (std::is_empty_v<U>) {
				return std::tuple<>(); // placeholder for void
			}
			else if constexpr (std::is_const_v<U>) {
				return std::make_tuple(entity.try_get<U>());
			}
			else {
				return std::make_tuple(entity.try_get_mut<U>());
			}
		}

		template<typename U>
		inline decltype(auto) try_get_helper() const {
			if constexpr (std::is_empty_v<U>) {
				return std::tuple<>(); // placeholder for void
			}
			else {
				return std::make_tuple(entity.try_get<U>());
			}
		}

		template<class C>
		using IsEmptyComponent = std::bool_constant<std::is_empty_v<C>>;

		template<class C>
		using IsFullComponent = std::bool_constant<!std::is_empty_v<C>>;

		template<typename... Components, typename... ExcludedComponents>
		inline decltype(auto) static make_query(ExcludedComponentsListT<ExcludedComponents...>) {
			using namespace spring;

			using Cs = type_list_filter_t<IsFullComponent , type_list_t<Components...>>;
			using Ts = type_list_filter_t<IsEmptyComponent, type_list_t<Components...>>;
			static_assert(Cs::size + Ts::size == type_list_t<Components...>::size);

			using DecayedComponentsTL = typename type_list_t<std::decay_t<Components>...>::type;
			constexpr bool ECsInCts = (false || ... || type_in_list_v<std::decay_t<ExcludedComponents>, DecayedComponentsTL>);
			static_assert(!ECsInCts, "Exclude components list contains components from the include components list");

			return []<typename... C, typename... T, typename... X>(type_list_t<C...>, type_list_t<T...>, type_list_t<X...>) {
				if constexpr (sizeof ...(T) == 0 && sizeof ...(X) == 0) {
					return registry
						.query_builder<C...>()
						.build();
				}
				else if constexpr (sizeof ...(T) > 0 && sizeof ...(X) == 0) {
					return registry
						.query_builder<C...>()
						.template with<T...>()
						.build();
				}
				else if constexpr (sizeof ...(T) == 0 && sizeof ...(X) > 0) {
					return registry
						.query_builder<C...>()
						.template without<X...>()
						.build();
				}
				else {
					return registry
						.query_builder<C...>()
						.template with<T...>()
						.template without<X...>()
						.build();
				}
			}(Cs{}, Ts{}, type_list<ExcludedComponents...>);
		}
	protected:
		MyEntityType entity;
		static inline flecs::world registry;
		static inline int ParallelMinChunk = ENTT_PACKED_PAGE;
		static inline int ParallelMaxChunk = std::numeric_limits<int>::max();
	};

	// non-owning class for an entity
	template <typename TypeTag>
	class EntityReference : public EntityBase<TypeTag> {
	public:
		using EB = EntityBase<TypeTag>;
		explicit EntityReference(const EntityBase<TypeTag>& EntityOwner)
			: EB()
		{
			this->entity = EntityOwner.Entity();
		}
		explicit EntityReference(const MyEntityType ownedEntity = {})
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
		explicit EntityOwner(const char* name = nullptr)
			: EB()
		{
			this->entity = EB::registry.entity(name);
		}

		~EntityOwner() {
			if (this->entity.is_valid()) {
				this->entity.destruct();
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