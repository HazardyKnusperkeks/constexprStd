/**
 ** This file is part of the constexprStd project.
 ** Copyright 2017 Björn Schäpers <bjoern@hazardy.de>.
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU Lesser General Public License as
 ** published by the Free Software Foundation, either version 3 of the
 ** License, or (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public License
 ** along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

/**
 * @file
 * @brief Contains details for the constexpr variants of <variant>
 */

#ifndef CONSTEXPRSTD_DETAILS_VARIANT_HPP
#define CONSTEXPRSTD_DETAILS_VARIANT_HPP

#include <array>
#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <variant>

#include "../utility"
#include "helper.hpp"
#include "uninitialized.hpp"

namespace constexprStd {
template<typename...>
class variant;

class BadVariantAssignment : public std::domain_error {
	public:
	BadVariantAssignment(void) :
			std::domain_error("Current type hold by right hand side is not applicable on the left hand side") {
		return;
	}
};

namespace details {
template<typename... Types>
union VarUnion;

template<typename T>
struct IsVariant : std::false_type { };

template<typename... Types>
struct IsVariant<constexprStd::variant<Types...>> : std::true_type { };

template<typename T>
inline constexpr bool IsVariantV = IsVariant<T>::value;
} //namespace details

template<typename... Types>
void swap(constexprStd::details::VarUnion<Types...>&, constexprStd::details::VarUnion<Types...>&) = delete;
} //namespace constexprStd

namespace std {
template<std::size_t I, typename Variant,
         std::enable_if_t<constexprStd::details::IsVariantV<std::decay_t<Variant>>>* = nullptr>
constexpr decltype(auto) get(Variant&& v);

template<typename T, typename Variant,
         std::enable_if_t<constexprStd::details::IsVariantV<std::decay_t<Variant>>>* = nullptr>
constexpr decltype(auto) get(Variant&& v);

template<typename... Types>
void swap(constexprStd::details::VarUnion<Types...>&, constexprStd::details::VarUnion<Types...>&) = delete;
} //namespace std

namespace constexprStd::details {
template<typename... Types>
struct OverloadSet {
	constexpr static void function(void) noexcept;
};

template<typename First, typename... Rest>
struct OverloadSet<First, Rest...> : OverloadSet<Rest...> {
	using OverloadSet<Rest...>::function;
	constexpr static std::integral_constant<std::size_t, sizeof...(Rest)> function(First) noexcept;
};

template<typename... Rest>
struct OverloadSet<void, Rest...> : OverloadSet<Rest...> {
	using OverloadSet<Rest...>::function;
};

template<typename Type, typename... Types>
using CompatibleTypeIndex = std::integral_constant<std::size_t, sizeof...(Types) - 1 -
	decltype(OverloadSet<Types...>::function(std::declval<Type>()))::value>;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"

template<typename... Types>
union VarUnion { };

template<typename First, typename... Rest>
union VarUnion<First, Rest...> {
	Uninitialized<First> Element;
	VarUnion<Rest...> Other;
	
	constexpr VarUnion(void) noexcept : Other{} {
		return;
	}
	
	template<typename... Args>
	constexpr VarUnion(const std::in_place_index_t<0>, Args&&... args)
			noexcept(std::is_nothrow_constructible_v<Uninitialized<First>, Args...>) :
			Element{std::forward<Args>(args)...} {
		if constexpr ( sizeof...(Args) == 0 ) {
			/* If args is empty we call the default ctor of Uninitialized, that for non trivially destructible types
			 * doesn't initialize, so we have to do it manually. */
			Element.defaultInit();
		} //if constexpr ( sizeof...(Args) == 0 )
		return;
	}
	
	template<std::size_t I, typename... Args>
	constexpr VarUnion(const std::in_place_index_t<I>, Args&&... args)
			noexcept(std::is_nothrow_constructible_v<VarUnion<Rest...>, std::in_place_index_t<I-1>, Args...>) :
			Other{std::in_place_index<I-1>, std::forward<Args>(args)...} {
		return;
	}
	
	template<std::size_t I>
	constexpr decltype(auto) get(void) & noexcept {
		if constexpr ( I == 0 ) {
			return Element.get();
		} //if constexpr ( I == 0 )
		else {
			return Other.template get<I-1>();
		} //else -> if constexpr ( I == 0 )
	}
	
	template<std::size_t I>
	constexpr decltype(auto) get(void) const & noexcept {
		if constexpr ( I == 0 ) {
			return Element.get();
		} //if constexpr ( I == 0 )
		else {
			return Other.template get<I-1>();
		} //else -> if constexpr ( I == 0 )
	}
	
	template<std::size_t I>
	constexpr decltype(auto) get(void) && noexcept {
		if constexpr ( I == 0 ) {
			return std::move(Element).get();
		} //if constexpr ( I == 0 )
		else {
			return std::move(Other).template get<I-1>();
		} //else -> if constexpr ( I == 0 )
	}
	
	template<std::size_t I>
	constexpr decltype(auto) get(void) const && noexcept {
		if constexpr ( I == 0 ) {
			return std::move(Element).get();
		} //if constexpr ( I == 0 )
		else {
			return std::move(Other).template get<I-1>();
		} //else -> if constexpr ( I == 0 )
	}
	
	template<std::size_t I, typename... Args>
	static constexpr bool isEmplaceNoexcept(void) {
		if constexpr ( I == 0 ) {
			return noexcept(Element.init(std::declval<Args&&>()...));
		} //if constexpr ( I == 0 )
		else {
			return noexcept(Other.template emplace<I-1>(std::declval<Args&&>()...));
		} //else -> if constexpr ( I == 0 )
	}
	
	template<std::size_t I, typename... Args>
	constexpr decltype(auto) emplace(Args&&... args) noexcept(isEmplaceNoexcept<I, Args&&...>()) {
		if constexpr ( I == 0 ) {
			return Element.init(std::forward<Args>(args)...);
		} //if constexpr ( I == 0 )
		else {
			return Other.template emplace<I-1>(std::forward<Args>(args)...);
		} //else -> if constexpr ( I == 0 )
	}
	
	template<std::size_t I>
	static constexpr bool isDeinitNoexcept(void) {
		if constexpr ( I == 0 ) {
			return noexcept(Element.deinit());
		} //if constexpr ( I == 0 )
		else {
			return noexcept(Other.template deinit<I-1>());
		} //else -> if constexpr ( I == 0 )
	}
	
	template<std::size_t I>
	constexpr decltype(auto) deinit(void) noexcept(isDeinitNoexcept<I>()) {
		if constexpr ( I == 0 ) {
			return Element.deinit();
		} //if constexpr ( I == 0 )
		else {
			return Other.template deinit<I-1>();
		} //else -> if constexpr ( I == 0 )
	}
	
	constexpr void swap(const std::size_t idx, VarUnion& that)
			noexcept(std::conjunction_v<std::is_nothrow_swappable<First>, std::is_nothrow_swappable<Rest>...>) {
		if ( idx == 0 ) {
			constexprStd::swap(Element.get(), that.Element.get());
		} //if ( idx == 0 )
		else {
			if constexpr ( sizeof...(Rest) > 0 ) {
				Other.swap(idx - 1, that.Other);
			} //if constexpr ( sizeof...(Rest) > 0 )o
		} //else -> if ( idx == 0 )
		return;
	}
	
	constexpr void assign(const std::size_t idx, const VarUnion& that)
			noexcept(std::conjunction_v<std::is_nothrow_copy_assignable<First>,
			                            std::is_nothrow_copy_assignable<Rest>...>) {
		if ( idx == 0 ) {
			Element.get() = that.Element.get();
		} //if ( idx == 0 )
		else {
			if constexpr ( sizeof...(Rest) > 0 ) {
				Other.assign(idx - 1, that.Other);
			} //if constexpr ( sizeof...(Rest) > 0 )
		} //else -> if ( idx == 0 )
		return;
	}
	
	constexpr void take(const std::size_t idx, VarUnion&& that)
			noexcept(std::conjunction_v<std::is_nothrow_move_assignable<First>,
			                            std::is_nothrow_move_assignable<Rest>...>) {
		if ( idx == 0 ) {
			Element.get() = std::move(that.Element).get();
		} //if ( idx == 0 )
		else {
			if constexpr ( sizeof...(Rest) > 0 ) {
				Other.take(idx - 1, std::move(that.Other));
			} //if constexpr ( sizeof...(Rest) > 0 )
		} //else -> if ( idx == 0 )
		return;
	}
	
	constexpr bool compareEqual(const std::size_t idx, const VarUnion& that) const
			noexcept(noexcept(std::declval<const First&>() == std::declval<const First&>()) &&
			         (noexcept(std::declval<const Rest&>() == std::declval<const Rest&>()) && ...)) {
		if ( idx == 0 ) {
			return Element.get() == that.Element.get();
		} //if ( idx == 0 )
		else {
			if constexpr ( sizeof...(Rest) > 0 ) {
				return Other.compareEqual(idx - 1, that.Other);
			} //if constexpr ( sizeof...(Rest) > 0 )
		} //else -> if ( idx == 0 )
		return false;
	}
	
	constexpr bool compareLess(const std::size_t idx, const VarUnion& that) const
			noexcept(noexcept(std::declval<const First&>() < std::declval<const First&>()) &&
			         (noexcept(std::declval<const Rest&>() < std::declval<const Rest&>()) && ...)) {
		if ( idx == 0 ) {
			return Element.get() < that.Element.get();
		} //if ( idx == 0 )
		else {
			if constexpr ( sizeof...(Rest) > 0 ) {
				return Other.compareLess(idx - 1, that.Other);
			} //if constexpr ( sizeof...(Rest) > 0 )
		} //else -> if ( idx == 0 )
		return false;
	}
};

template<bool TrivialyDestructible = true, typename... Types>
struct VariantStorage {
	VarUnion<Types...> Data;
	std::size_t Index;
	
	constexpr VariantStorage(void)
			noexcept(std::is_nothrow_constructible_v<VarUnion<Types...>, std::in_place_index_t<0>>) :
			Data{std::in_place_index<0>}, Index{0} {
		return;
	}
	
	constexpr VariantStorage(const VariantStorage& that) noexcept(noexcept(Data.assign(Index, that.Data))) :
			Index{that.Index} {
		Data.assign(Index, that.Data);
		return;
	}
	
	constexpr VariantStorage(VariantStorage&& that) noexcept(noexcept(Data.take(Index, std::move(that.Data)))) :
			Index{that.Index} {
		Data.take(Index, std::move(that.Data));
		return;
	}
	
	template<std::size_t I, typename... Args>
	constexpr VariantStorage(const std::in_place_index_t<I>, Args&&... args)
			noexcept(std::is_nothrow_constructible_v<VarUnion<Types...>, std::in_place_index_t<I>, Args...>) :
			Data{std::in_place_index<I>, std::forward<Args>(args)...}, Index{I} {
		return;
	}
	
	constexpr void reset(void) noexcept {
		Index = std::variant_npos;
		return;
	}
	
	constexpr bool valid(void) const noexcept {
		return Index != std::variant_npos;
	}
	
	template<std::size_t I, typename T, typename... Args>
	constexpr T& emplace(Args&&... args)
			noexcept(std::conjunction_v<std::is_nothrow_constructible<T, Args...>,
			                            std::is_nothrow_move_assignable<T>>) {
		reset();
		T& ret = Data.template emplace<I>(std::forward<Args>(args)...);
		Index = I;
		return ret;
	}
	
	template<std::size_t I, typename T, typename U, typename... Args>
	constexpr T& emplace(std::initializer_list<U> il, Args&&... args)
			noexcept(std::conjunction_v<std::is_nothrow_constructible<T, std::initializer_list<U>, Args...>,
			                            std::is_nothrow_move_assignable<T>>) {
		reset();
		T& ret = Data.template emplace<I>(il, std::forward<Args>(args)...);
		Index = I;
		return ret;
	}
};

#pragma GCC diagnostic pop

template<typename... Types>
struct VariantStorage<false, Types...> {
	using NothrowCopyCtor   = std::bool_constant<std::conjunction_v<std::is_nothrow_copy_constructible<Types>...>>;
	using NothrowMoveCtor   = std::bool_constant<std::conjunction_v<std::is_nothrow_move_constructible<Types>...>>;
	using NothrowMoveAssign = std::bool_constant<std::conjunction_v<std::is_nothrow_move_assignable<Types>...>>;
	using NothrowDtor       = std::bool_constant<std::conjunction_v<std::is_nothrow_destructible<Types>...>>;
	
	using AllIndices = std::index_sequence_for<Types...>;
	
	VarUnion<Types...> Data;
	std::size_t Index;
	
	template<std::size_t I>
	void elementCopyCtor(const VariantStorage& that) noexcept(NothrowCopyCtor::value) {
		using Type = std::decay_t<std::variant_alternative_t<I, std::variant<Types...>>>;
		static_assert(std::is_copy_constructible_v<Type>);
		Data.template emplace<I>(that.Data.template get<I>());
		return;
	}
	
	template<std::size_t I>
	void elementMoveCtor(VariantStorage&& that) noexcept(NothrowMoveCtor::value) {
		using Type = std::decay_t<std::variant_alternative_t<I, std::variant<Types...>>>;
		static_assert(std::is_move_constructible_v<Type>);
		Data.template emplace<I>(std::move(that.Data.template get<I>()));
		return;
	}
	
	template<std::size_t I>
	void elementDtor(void) noexcept(NothrowDtor::value) {
		Data.template deinit<I>();
		return;
	}
	
	using CopyCtorType = std::conditional_t<NothrowCopyCtor::value,
	                                        void (VariantStorage::*)(const VariantStorage&) noexcept,
	                                        void (VariantStorage::*)(const VariantStorage&)>;
	using MoveCtorType = std::conditional_t<NothrowCopyCtor::value,
	                                        void (VariantStorage::*)(VariantStorage&&) noexcept,
	                                        void (VariantStorage::*)(VariantStorage&&)>;
	using DtorType = std::conditional_t<NothrowCopyCtor::value,
	                                    void (VariantStorage::*)(void) noexcept,
	                                    void (VariantStorage::*)(void)>;
	
	template<std::size_t... Idx>
	static inline const std::array<CopyCtorType, sizeof...(Idx)>
		ElementCopyCtors{&VariantStorage::elementCopyCtor<Idx>...};
	
	template<std::size_t... Idx>
	static inline const std::array<MoveCtorType, sizeof...(Idx)>
		ElementMoveCtors{&VariantStorage::elementMoveCtor<Idx>...};
	
	template<std::size_t... Idx>
	static inline const std::array<DtorType, sizeof...(Idx)> ElementDtors{&VariantStorage::elementDtor<Idx>...};
	
	VariantStorage(void) noexcept(std::is_nothrow_constructible_v<VarUnion<Types...>, std::in_place_index_t<0>>) :
			Data{std::in_place_index<0>}, Index{0} {
		return;
	}
	
	template<std::size_t I, typename... Args>
	VariantStorage(const std::in_place_index_t<I>, Args&&... args)
			noexcept(std::is_nothrow_constructible_v<VarUnion<Types...>, std::in_place_index_t<I>, Args...>) :
			Data{std::in_place_index<I>, std::forward<Args>(args)...}, Index{I} {
		return;
	}
	
	VariantStorage(const VariantStorage& that) noexcept(NothrowCopyCtor::value) : Index{that.Index} {
		copy(that);
		return;
	}
	
	VariantStorage(VariantStorage&& that) noexcept(NothrowMoveCtor::value) : Index{that.Index} {
		move(std::move(that));
		return;
	}
	
	~VariantStorage(void) noexcept(NothrowDtor::value) {
		reset();
		return;
	}
	
	template<std::size_t... Idx>
	void copyImpl(const std::index_sequence<Idx...>, const VariantStorage& that) noexcept(NothrowCopyCtor::value) {
		(this->*ElementCopyCtors<Idx...>[Index])(that);
		return;
	}
	
	void copy(const VariantStorage& that) noexcept(NothrowCopyCtor::value) {
		if ( valid() ) {
			try {
				copyImpl(AllIndices{}, that);
			} //try
			catch ( ... ) {
				invalidate();
				if constexpr ( !NothrowCopyCtor::value ) {
					throw;
				} //if constexpr ( !NothrowCopyCtor::value )
			} //catch ( ... )
		} //if ( valid() )
		return;
	}
	
	template<std::size_t... Idx>
	void moveImpl(const std::index_sequence<Idx...>, VariantStorage&& that) noexcept(NothrowMoveCtor::value) {
		(this->*ElementMoveCtors<Idx...>[Index])(std::move(that));
		return;
	}
	
	void move(VariantStorage&& that) noexcept(NothrowMoveCtor::value) {
		if ( valid() ) {
			try {
				moveImpl(AllIndices{}, std::move(that));
			} //try
			catch ( ... ) {
				invalidate();
				if constexpr ( !NothrowMoveCtor::value ) {
					throw;
				} //if constexpr ( !NothrowMoveCtor::value )
			} //catch ( ... )
		} //if ( valid() )
		return;
	}
	
	template<std::size_t... Idx>
	void resetImpl(const std::index_sequence<Idx...>) noexcept(NothrowDtor::value) {
		(this->*ElementDtors<Idx...>[Index])();
		return;
	}
	
	void reset(void) noexcept(NothrowDtor::value) {
		if ( valid() ) {
			try {
				resetImpl(AllIndices{});
			} //try
			catch ( ... ) {
				invalidate();
				if constexpr ( !NothrowDtor::value ) {
					throw;
				} //if constexpr ( !NothrowDtor::value )
			} //catch ( ... )
			invalidate();
		} //if ( valid() )
		return;
	}
	
	constexpr bool valid(void) const noexcept {
		return Index != std::variant_npos;
	}
	
	constexpr void invalidate(void) noexcept {
		Index = std::variant_npos;
		return;
	}
	
	template<std::size_t I, typename T, typename... Args>
	T& emplace(Args&&... args) noexcept(NothrowDtor::value && std::is_nothrow_constructible_v<T, Args...>) {
		reset();
		T& ret = Data.template emplace<I>(std::forward<Args>(args)...);
		Index = I;
		return ret;
	}
	
	template<std::size_t I, typename T, typename U, typename... Args>
	T& emplace(std::initializer_list<U> il, Args&&... args)
			noexcept(NothrowDtor::value && std::is_nothrow_constructible_v<T, std::initializer_list<U>, Args...>) {
		reset();
		T& ret = Data.template emplace<I>(il, std::forward<Args>(args)...);
		Index = I;
		return ret;
	}
};

template<bool TrivialDestructible, typename... Types>
struct VariantBase : public VariantStorage<TrivialDestructible, Types...> {
	using Storage    = VariantStorage<TrivialDestructible, Types...>;
	using Union      = decltype(Storage::Data);
	using AllIndices = std::index_sequence_for<Types...>;
	
	template<std::size_t I>
	using Type = std::variant_alternative_t<I, std::variant<Types...>>;
	
	template<typename T>
	using CheckNothrowCopyOrThrowMove = std::bool_constant<std::is_nothrow_copy_constructible_v<T> ||
	                                                       !std::is_nothrow_move_assignable_v<T>>;
	
	static constexpr std::array<bool, sizeof...(Types)> IsNothrowCopyOrThrowMove{
		CheckNothrowCopyOrThrowMove<Types>::value...};
	
	using AllNothrowCopyOrThrowMove = std::bool_constant<((std::is_nothrow_copy_constructible_v<Types> ||
		!std::is_nothrow_move_assignable_v<Types>) && ...)>;
	
	template<std::size_t I, typename T = Type<I>>
	static constexpr void callEmplace(VariantBase& thiz, const VariantBase& that)
			noexcept(noexcept(thiz.template emplace<I, T>(that.Data.template get<I>()))) {
		thiz.template emplace<I, T>(that.Data.template get<I>());
		return;
	}
	
	template<std::size_t I, typename T = Type<I>>
	static constexpr void callMoveEmplace(VariantBase& thiz, VariantBase&& that)
			noexcept(noexcept(thiz.template emplace<I, T>(std::move(that.Data).template get<I>()))) {
		thiz.template emplace<I, T>(std::move(that.Data).template get<I>());
		return;
	}
	
	template<std::size_t I, std::size_t... Idx>
	static constexpr bool emplaceIsNoexcept(const std::index_sequence<I, Idx...>) noexcept {
		/* This two fold check is neccessary because if we would evaluate the noexcept query for a type that is not copy
		 * constructible, the traits would try to call said non existing copy constrcutor in a decltype call. Of course
		 * this is not possible and so we have to prevent the compiler from instantiating this. */
		if constexpr ( std::is_copy_constructible_v<Type<I>> ) {
			if constexpr ( !noexcept(std::declval<Storage&>().
			                         template emplace<I, Type<I>>(std::declval<const Type<I>&>())) ) {
				return false;
			} //if constexpr ( !noexcept(std::declval<Storage&>().emplace<I, Type<I>>(std::declval<const Type<I>&>())) )
		} //if constexpr ( std::is_copy_constructible_v<Type<I>> )
		if constexpr ( sizeof...(Idx) > 0 ) {
			return emplaceIsNoexcept(std::index_sequence<Idx...>{});
		} //if constexpr ( sizeof...(Idx) > 0 )
		return true;
	}
	
	static constexpr bool emplaceAreNoexcept(void) noexcept {
		return emplaceIsNoexcept(AllIndices{});
	}
	
	using AllEmplaceNoexcept = std::bool_constant<emplaceAreNoexcept()>;
	
	template<std::size_t I, std::size_t... Idx>
	static constexpr bool moveEmplaceIsNoexcept(const std::index_sequence<I, Idx...>) noexcept {
		if constexpr ( std::is_move_constructible_v<Type<I>> ) {
			if constexpr ( !noexcept(std::declval<Storage&>().
			                         template emplace<I, Type<I>>(std::declval<Type<I>&&>())) ) {
				return false;
			} //if constexpr ( !noexcept(std::declval<Storage&>().emplace<I, Type<I>>(std::declval<Type<I>&&>())) )
		} //if constexpr ( std::is_move_constructible_v<Type<I>> )
		if constexpr ( sizeof...(Idx) > 0 ) {
			return moveEmplaceIsNoexcept(std::index_sequence<Idx...>{});
		} //if constexpr ( sizeof...(Idx) > 0 )
		return true;
	}
	
	static constexpr bool moveEmplaceAreNoexcept(void) noexcept {
		return moveEmplaceIsNoexcept(AllIndices{});
	}
	
	using AllMoveEmplaceNoexcept = std::bool_constant<moveEmplaceAreNoexcept()>;
	
	using EmplaceType = std::conditional_t<AllEmplaceNoexcept::value,
	                                       void (*)(VariantBase&, const VariantBase&) noexcept,
	                                       void (*)(VariantBase&, const VariantBase&)>;
	
	template<std::size_t... Idx>
	static constexpr std::array<EmplaceType, sizeof...(Idx)> EmplaceCalls{&VariantBase::callEmplace<Idx>...};
	
	template<std::size_t... Idx>
	constexpr void callEmplace(const std::index_sequence<Idx...>, const VariantBase& that)
			noexcept(AllEmplaceNoexcept::value) {
		EmplaceCalls<Idx...>[that.Index](*this, that);
		return;
	}
	
	using MoveEmplaceType = std::conditional_t<AllMoveEmplaceNoexcept::value,
	                                           void (*)(VariantBase&, VariantBase&&) noexcept,
	                                           void (*)(VariantBase&, VariantBase&&)>;
	
	template<std::size_t... Idx>
	static constexpr std::array<MoveEmplaceType, sizeof...(Idx)> MoveEmplaceCalls{
		&VariantBase::callMoveEmplace<Idx>...};
	
	template<std::size_t... Idx>
	constexpr void callMoveEmplace(const std::index_sequence<Idx...>, VariantBase&& that)
			noexcept(AllMoveEmplaceNoexcept::value) {
		MoveEmplaceCalls<Idx...>[that.Index](*this, std::move(that));
		return;
	}
	
	constexpr VariantBase(void) = default;
	constexpr VariantBase(const VariantBase&) = default;
	constexpr VariantBase(VariantBase&&) = default;
	
	template<std::size_t I, typename... Args>
	constexpr VariantBase(const std::in_place_index_t<I>, Args&&... args)
			noexcept(std::is_nothrow_constructible_v<Storage, std::in_place_index_t<I>, Args...>) :
			Storage{std::in_place_index<I>, std::forward<Args>(args)...} {
		return;
	}
	
	static constexpr bool calcNoexceptMoveAssign(void) noexcept {
		if constexpr ( std::conjunction_v<std::is_move_assignable<Types>...> ) {
			return noexcept(std::declval<Storage&>().reset()) &&
			       noexcept(Storage::Data.take(Storage::Index, std::declval<Union&&>()));
		} //if constexpr ( std::conjunction_v<std::is_move_assignable<Types>...> )
		return false;
	}
	
	using NoexceptMoveAssign = std::bool_constant<calcNoexceptMoveAssign()>;
	
	constexpr VariantBase& operator=(const VariantBase& that)
			noexcept(noexcept(Storage::Data.assign(Storage::Index, that.Data)) && AllEmplaceNoexcept::value &&
			         noexcept(std::declval<Storage&>().reset()) && NoexceptMoveAssign::value) {
		if ( this == &that ) {
			return *this;
		} //if ( this == &that )
		
		if ( that.valid() ) {
			if ( Storage::Index == that.Index ) {
				Storage::Data.assign(Storage::Index, that.Data);
			} //if ( Storage::Index == that.Index )
			else {
				if constexpr ( AllNothrowCopyOrThrowMove::value ) {
					callEmplace(AllIndices{}, that);
				} //if constexpr ( AllNothrowCopyOrThrowMove::value )
				else {
					if ( IsNothrowCopyOrThrowMove[that.Index] ) {
						callEmplace(AllIndices{}, that);
					} //if ( IsNothrowCopyOrThrowMove[that.Index] )
					else {
						//Create temporary and thus call the move assignment
						*this = VariantBase{that};
					} //else -> if ( IsNothrowCopyOrThrowMove[that.Index] )
				} //else -> if constexpr ( AllNothrowCopyOrThrowMove::value )
			} //else -> if ( Storage::Index == that.Index )
		} //if ( that.valid() )
		else {
			Storage::reset();
		} //else -> if ( that.valid() )
		return *this;
	}
	
	constexpr VariantBase& operator=(VariantBase&& that) noexcept(NoexceptMoveAssign::value) {
		if ( that.valid() ) {
			if ( Storage::Index == that.Index ) {
				Storage::Data.take(Storage::Index, std::move(that.Data));
			} //if ( Storage::Index == that.Index )
			else {
				callMoveEmplace(AllIndices{}, std::move(that));
			} //else -> if ( Storage::Index == that.Index )
		} //if ( that.valid() )
		else {
			Storage::reset();
		} //else -> if ( that.valid() )
		return *this;
	}
	
	template<typename AssignT, typename InputT>
	constexpr void convertAssign(InputT&& t)
			noexcept(std::is_nothrow_assignable_v<AssignT&, InputT&&> &&
			         (!(std::is_nothrow_constructible_v<AssignT, InputT> ||
			            !std::is_nothrow_move_constructible_v<AssignT>) ||
			          noexcept(std::declval<Storage&>().template emplace<TypeIndex<AssignT, Types...>::value, AssignT>(
			                   std::forward<InputT>(t)))) &&
			         ((std::is_nothrow_constructible_v<AssignT, InputT> ||
			           !std::is_nothrow_move_constructible_v<AssignT>) ||
			          (NoexceptMoveAssign::value && std::is_nothrow_constructible_v<AssignT, InputT&&>))) {
		using AssignIndex = TypeIndex<AssignT, Types...>;
		if ( AssignIndex::value == Storage::Index ) {
			Storage::Data.template get<AssignIndex::value>() = std::forward<InputT>(t);
		} //if ( AssignIndex::value == Storage::Index )
		else {
			if constexpr ( std::is_nothrow_constructible_v<AssignT, InputT> ||
			               !std::is_nothrow_move_constructible_v<AssignT> ) {
				Storage::template emplace<AssignIndex::value, AssignT>(std::forward<InputT>(t));
			} //if constexpr ( std::is_nothrow_const_v<AssignT, InputT> || !std::is_nothrow_move_const_v<AssignT> )
			else {
				operator=(VariantBase(std::in_place_index<AssignIndex::value>, std::forward<InputT>(t)));
			} //else -> if constexpr ( std::is_nothrow_const_v<AT, IT> || !std::is_nothrow_move_const_v<AT> )
		} //else -> if ( AssignIndex::value == Storage::Index )
		return;
	}
	
	constexpr bool compareEqual(const VariantBase& that) const
			noexcept(noexcept(Storage::Data.compareEqual(Storage::Index, that.Data))) {
		return Storage::Data.compareEqual(Storage::Index, that.Data);
	}
	
	constexpr bool compareLess(const VariantBase& that) const
			noexcept(noexcept(Storage::Data.compareLess(Storage::Index, that.Data))) {
		return Storage::Data.compareLess(Storage::Index, that.Data);
	}
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"

template<typename... Types1>
struct CompareVariant {
	const variant<Types1...>& V1;
	
	//We only need to check Types1, because we will never compare against a type containend in Types2, but not in Types1
	using AllEqualCompareNoexcept =
		std::bool_constant<(noexcept(std::declval<const Types1&>() == std::declval<const Types1&>()) && ...)>;
	
	template<std::size_t I1, std::size_t I2, typename... Types2>
	static constexpr bool compare(const variant<Types1...>& v1, const variant<Types2...>& v2)
			noexcept(AllEqualCompareNoexcept::value) {
		using T1 = std::decay_t<std::variant_alternative_t<I1, decltype(v1)>>;
		using T2 = std::decay_t<std::variant_alternative_t<I2, decltype(v2)>>;
		if constexpr ( std::is_same_v<T1, T2> ) {
			return std::get<I1>(v1) == std::get<I2>(v2);
		} //if constexpr ( std::is_same_v<T1, T2> )
		return false;
	}
	
	template<typename... Types2>
	using CompareType = std::conditional_t<AllEqualCompareNoexcept::value,
	                                       bool(*)(const variant<Types1...>&, const variant<Types2...>&) noexcept,
	                                       bool(*)(const variant<Types1...>&, const variant<Types2...>&)>;
	
	template<std::size_t I1, typename... Types2>
	static constexpr std::array<CompareType<Types2...>, sizeof...(Types2)>
		CompareT2{&CompareVariant::compare<I1, TypeIndex<Types2, Types2...>::value>...};
	
	template<typename... Types2, std::size_t... I1s>
	static constexpr auto createCompare(const std::index_sequence<I1s...>) noexcept {
		std::array ret{CompareT2<I1s, Types2...>...};
		return ret;
	}
	
	template<typename... Types2>
	constexpr bool operator()(const variant<Types2...>& v2) const noexcept(AllEqualCompareNoexcept::value) {
		auto cmp = createCompare<Types2...>(std::index_sequence_for<Types1...>{});
		return cmp[V1.index()][v2.index()](V1, v2);
	}
};

template<typename... Types1>
CompareVariant(const variant<Types1...>&) -> CompareVariant<Types1...>;

#pragma GCC diagnostic pop

[[noreturn]] inline void throwBadVariantAssignment(void) {
	throw BadVariantAssignment{};
}

template<typename... Types1>
struct AssignVariant {
	variant<Types1...>& V1;
	
	template<typename T>
	static constexpr bool checkType(void) noexcept {
		if constexpr ( CountV<T, Types1...> == 1 ) {
			return noexcept(V1 = std::declval<const T&>());
		} //if constexpr ( CountV<T, Types1...> == 1 )
		return false;
	}
	
	template<typename... Types2>
	using CanNoexceptCopyAllTypes = std::bool_constant<(checkType<Types2>() && ...)>;
	
	template<std::size_t I, typename... Types2>
	static constexpr void assign(variant<Types1...>& v1, const variant<Types2...>& v2)
			noexcept(CanNoexceptCopyAllTypes<Types2...>::value) {
		using T = std::decay_t<std::variant_alternative_t<I, decltype(v2)>>;
		if constexpr ( CountV<T, Types1...> == 1 ) {
			v1 = std::get<I>(v2);
		} //if constexpr ( CountV<T, Types1...> == 1 )
		//The else case never gets called.
		return;
	}
	
	template<typename... Types2>
	using AssignType = std::conditional_t<CanNoexceptCopyAllTypes<Types2...>::value,
	                                      void (*)(variant<Types1...>&, const variant<Types2...>&) noexcept,
	                                      void (*)(variant<Types1...>&, const variant<Types2...>&)>;
	
	template<typename... Types2>
	static constexpr std::array<AssignType<Types2...>, sizeof...(Types2)>
		Assign{&AssignVariant::assign<TypeIndex<Types2, Types2...>::value, Types2...>...};
	
	template<typename... Types2>
	constexpr void operator()(const variant<Types2...>& v2) noexcept(CanNoexceptCopyAllTypes<Types2...>::value) {
		if ( v2.valueless_by_exception() ) {
			V1.reset();
			return;
		} //if ( v2.valueless_by_exception() )
		
		constexpr std::array canHoldType{(CountV<Types2, Types1...> == 1)...};
		const auto v2Index = v2.index();
		if ( !canHoldType[v2Index] ) {
			throwBadVariantAssignment();
		} //if ( !canHoldType[v2Index] )
		
		Assign<Types2...>[v2Index](V1, v2);
		return;
	}
};

template<typename... Types1>
AssignVariant(const variant<Types1...>&) -> AssignVariant<Types1...>;

[[noreturn]] inline void throwBadVariantAccess(void) {
	throw std::bad_variant_access{};
}
} //namespace constexprStd::details

#endif
