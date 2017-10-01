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
 * @brief Contains helper traits or functions.
 */

#ifndef CONSTEXPRSTD_DETAILS_HELPER_HPP
#define CONSTEXPRSTD_DETAILS_HELPER_HPP

#include <iterator>
#include <type_traits>

#include <experimental/type_traits>

namespace std {
template<typename T1, typename T2>
struct pair;
} //namespace std

namespace constexprStd {
template<typename T1, typename T2>
struct pair;
} //namespace constexprStd

namespace constexprStd::details {
template<typename Iter>
using IsIterType = typename std::iterator_traits<Iter>::iterator_category;

template<typename T>
using IsIter = typename std::experimental::is_detected<IsIterType, T>;

template<typename T>
constexpr inline bool IsIterV = IsIter<T>::value;

template<typename Iter>
struct IsRaIter : std::conditional_t<IsIter<Iter>::value,
                                     std::is_same<std::experimental::detected_or_t<std::void_t<>, IsIterType, Iter>,
                                                  std::random_access_iterator_tag>,
                                     std::false_type> { };

template<typename Type, typename... Types>
struct Count : std::integral_constant<std::size_t, 0> { };

template<typename Type, typename First, typename... Rest>
struct Count<Type, First, Rest...> : std::integral_constant<std::size_t, Count<Type, Rest...>::value +
	std::conditional_t<std::is_same_v<Type, First>, std::integral_constant<std::size_t, 1>,
	                   std::integral_constant<std::size_t, 0>>::value> { };

template<typename Type, typename... Types>
constexpr inline auto CountV = Count<Type, Types...>::value;

template<std::size_t I, typename Type, typename... Types>
struct TypeIndexImpl : std::integral_constant<std::size_t, static_cast<std::size_t>(-1)> { };

template<std::size_t I, typename Type, typename First, typename... Types>
struct TypeIndexImpl<I, Type, First, Types...> : std::conditional_t<std::is_same_v<Type, First>,
                                                                    std::integral_constant<std::size_t, I>,
                                                                    TypeIndexImpl<I + 1, Type, Types...>> { };

template<typename Type, typename... Types>
struct TypeIndex : TypeIndexImpl<0, Type, Types...> { };

template<typename T>
struct IsPair : std::false_type { };

template<typename T1, typename T2>
struct IsPair<std::pair<T1, T2>> : std::true_type { };

template<typename T1, typename T2>
struct IsPair<constexprStd::pair<T1, T2>> : std::true_type { };

template<typename T, typename Iter>
using IsSearcherType = decltype(std::declval<const T&>()(std::declval<Iter>(), std::declval<Iter>()));

template<typename T, typename Iter>
using IsSearcher = typename std::experimental::is_detected_exact<std::pair<Iter, Iter>, IsSearcherType, T, Iter>;

template<typename T, typename Iter>
constexpr inline auto IsSearcherV = IsSearcher<T, Iter>::value;

struct EnableDefaultCtorTag {
	explicit constexpr EnableDefaultCtorTag(void) = default;
};

template<bool Condition, typename For>
struct EnableDefaultCtor {
	constexpr EnableDefaultCtor(void) = default;
	explicit constexpr EnableDefaultCtor(const EnableDefaultCtorTag) noexcept { return; }
};

template<typename For>
struct EnableDefaultCtor<false, For> {
	EnableDefaultCtor(void) = delete;
	explicit constexpr EnableDefaultCtor(const EnableDefaultCtorTag) noexcept { return; }
};

template<bool Condition, typename For>
struct EnableCopyCtor {
	constexpr EnableCopyCtor(void) = default;
	constexpr EnableCopyCtor(const EnableCopyCtor&) = default;
	constexpr EnableCopyCtor(EnableCopyCtor&&) = default;
	constexpr EnableCopyCtor& operator=(const EnableCopyCtor&) = default;
	constexpr EnableCopyCtor& operator=(EnableCopyCtor&&) = default;
};

template<typename For>
struct EnableCopyCtor<false, For> {
	constexpr EnableCopyCtor(void) = default;
	constexpr EnableCopyCtor(const EnableCopyCtor&) = delete;
	constexpr EnableCopyCtor(EnableCopyCtor&&) = default;
	constexpr EnableCopyCtor& operator=(const EnableCopyCtor&) = default;
	constexpr EnableCopyCtor& operator=(EnableCopyCtor&&) = default;
};

template<bool Condition, typename For>
struct EnableMoveCtor {
	constexpr EnableMoveCtor(void) = default;
	constexpr EnableMoveCtor(const EnableMoveCtor&) = default;
	constexpr EnableMoveCtor(EnableMoveCtor&&) = default;
	constexpr EnableMoveCtor& operator=(const EnableMoveCtor&) = default;
	constexpr EnableMoveCtor& operator=(EnableMoveCtor&&) = default;
};

template<typename For>
struct EnableMoveCtor<false, For> {
	constexpr EnableMoveCtor(void) = default;
	constexpr EnableMoveCtor(const EnableMoveCtor&) = default;
	constexpr EnableMoveCtor(EnableMoveCtor&&) = delete;
	constexpr EnableMoveCtor& operator=(const EnableMoveCtor&) = default;
	constexpr EnableMoveCtor& operator=(EnableMoveCtor&&) = default;
};

template<bool Condition, typename For>
struct EnableCopyAssign {
	constexpr EnableCopyAssign(void) = default;
	constexpr EnableCopyAssign(const EnableCopyAssign&) = default;
	constexpr EnableCopyAssign(EnableCopyAssign&&) = default;
	constexpr EnableCopyAssign& operator=(const EnableCopyAssign&) = default;
	constexpr EnableCopyAssign& operator=(EnableCopyAssign&&) = default;
};

template<typename For>
struct EnableCopyAssign<false, For> {
	constexpr EnableCopyAssign(void) = default;
	constexpr EnableCopyAssign(const EnableCopyAssign&) = default;
	constexpr EnableCopyAssign(EnableCopyAssign&&) = default;
	constexpr EnableCopyAssign& operator=(const EnableCopyAssign&) noexcept = delete;
	constexpr EnableCopyAssign& operator=(EnableCopyAssign&&) = default;
};

template<bool Condition, typename For>
struct EnableMoveAssign {
	constexpr EnableMoveAssign(void) = default;
	constexpr EnableMoveAssign(const EnableMoveAssign&) = default;
	constexpr EnableMoveAssign(EnableMoveAssign&&) = default;
	constexpr EnableMoveAssign& operator=(const EnableMoveAssign&) = default;
	constexpr EnableMoveAssign& operator=(EnableMoveAssign&&) = default;
};

template<typename For>
struct EnableMoveAssign<false, For> {
	constexpr EnableMoveAssign(void) = default;
	constexpr EnableMoveAssign(const EnableMoveAssign&) = default;
	constexpr EnableMoveAssign(EnableMoveAssign&&) = default;
	constexpr EnableMoveAssign& operator=(const EnableMoveAssign&) = default;
	constexpr EnableMoveAssign& operator=(EnableMoveAssign&&) noexcept = delete;
};
} //namespace constexprStd::details

#endif
