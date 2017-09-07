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
 * @brief Contains details for the constexpr variants of <algorithm>
 */

#ifndef CONSTEXPRSTD_DETAILS_ALGORITHM_HPP
#define CONSTEXPRSTD_DETAILS_ALGORITHM_HPP

#include <type_traits>

#include <constexprStd/iterator>

#include "helper.hpp"

//Declarations
namespace constexprStd {
template<typename IterT1, typename IterT2, typename BinaryPredicate>
constexpr bool equal(IterT1 first1, const IterT1 last1, IterT2 first2, const BinaryPredicate pred)
		noexcept(noexcept(first1 != last1) && noexcept(++first1, ++first2) && noexcept(pred(*first1, *first2)));
} //namespace constexpr

namespace constexprStd::details {
namespace cmp {
template<typename T>
struct EqualToValue {
	const T& Object;
	
	template<typename U>
	constexpr bool operator()(const U& compare) const noexcept(noexcept(Object == compare)) {
		return Object == compare;
	}
};

template<typename T>
EqualToValue(T) -> EqualToValue<T>;

template<typename T1, typename T2>
constexpr bool equal(const T1& t1, const T2& t2) noexcept(noexcept(t1 == t2)) {
	return t1 == t2;
}
} //namespace cmp

template<typename IterT1, typename IterT2, typename BinaryPredicate,
         std::enable_if_t<std::conjunction_v<IsRaIter<IterT1>, IsRaIter<IterT2>>>* = nullptr>
constexpr bool equalImpl(const IterT1 first1, const IterT1 last1, const IterT2 first2, const IterT2 last2,
                         const BinaryPredicate pred)
		noexcept(noexcept(constexprStd::distance(first1, last1)) && noexcept(constexprStd::distance(first2, last2)) &&
		         noexcept(constexprStd::equal(first1, last1, first2, pred))) {
	if ( constexprStd::distance(first1, last1) != constexprStd::distance(first2, last2) ) {
		return false;
	} //if ( constexprStd::distance(first1, last1) != constexprStd::distance(first2, last2) )
	return constexprStd::equal(first1, last1, first2, pred);
}

template<typename IterT1, typename IterT2, typename BinaryPredicate,
         std::enable_if_t<std::negation_v<std::conjunction<IsRaIter<IterT1>, IsRaIter<IterT2>>>>* = nullptr>
constexpr bool equalImpl(IterT1 first1, const IterT1 last1, IterT2 first2, const IterT2 last2,
                         const BinaryPredicate pred)
		noexcept(noexcept(first1 != last1 && first2 != last2) && noexcept(++first1, ++first2) &&
		         noexcept(pred(*first1, *first2)) && noexcept(first1 == last1 && first2 == last2)) {
	for ( ; first1 != last1 && first2 != last2; ++first1, ++first2 ) {
		if ( !pred(*first1, *first2) ) {
			return false;
		} //if ( !pred(*first1, *first2) )
	} //for ( ; first1 != last1 && first2 != last2; ++first1, ++first2 )
	return first1 == last1 && first2 == last2;
}
} //namespace constexprStd::details

#endif
