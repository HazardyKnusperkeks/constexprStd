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

#include "../iterator"
#include "algorithm_forward.hpp"
#include "helper.hpp"

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

struct Equal {
	template<typename T1, typename T2>
	constexpr bool operator()(const T1& t1, const T2& t2) const noexcept(noexcept(t1 == t2)) {
		return t1 == t2;
	}
};
} //namespace cmp

template<typename UnaryPredicate, typename T>
struct ReplaceCopyIf {
	UnaryPredicate Pred;
	const T& NewValue;
	
	constexpr const T& operator()(const T& value) const
			noexcept(noexcept(std::declval<const ReplaceCopyIf&>().Pred(value))) {
		return Pred(value) ? NewValue : value;
	}
	
	template<typename U>
	constexpr U operator()(const U& value) const
			noexcept(noexcept(std::declval<const ReplaceCopyIf&>().Pred(value)) &&
			         std::is_nothrow_constructible_v<U, const T&> && std::is_nothrow_copy_constructible_v<U>) {
		return Pred(value) ? NewValue : value;
	}
};

template<typename UnaryPredicate, typename T>
ReplaceCopyIf(UnaryPredicate&&, const T&) -> ReplaceCopyIf<UnaryPredicate, T>;

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

template<typename ForwardIter1, typename ForwardIter2, typename BinaryPredicate,
         std::enable_if_t<std::negation_v<IsRaIter<ForwardIter1>>>* = nullptr>
constexpr ForwardIter1 findEndImpl(const ForwardIter1 first, const ForwardIter1 last,
                                   const ForwardIter2 s_first, const ForwardIter2 s_last, BinaryPredicate pred)
		noexcept(noexcept(s_first == s_last) && std::is_nothrow_copy_constructible_v<ForwardIter1> &&
		         std::is_nothrow_copy_assignable_v<ForwardIter1> && std::is_nothrow_move_assignable_v<ForwardIter1> &&
		         noexcept(constexprStd::search(first, last, s_first, s_last, pred)) &&
		         noexcept(std::declval<ForwardIter1&>() != last)) {
	if ( s_first == s_last ) {
		return last;
	} //if ( s_first == s_last )
	
	auto ret = constexprStd::search(first, last, s_first, s_last, pred);
	if ( ret != last ) {
		auto match = ret;
		do { //while ( match != last )
			ret = match;
			match = constexprStd::search(constexprStd::next(match), last, s_first, s_last, pred);
		} while ( match != last );
	} //if ( ret != last )
	return ret;
}

template<typename RandomAccessIter1, typename RandomAccessIter2, typename BinaryPredicate,
         std::enable_if_t<IsRaIter<RandomAccessIter1>::value>* = nullptr>
constexpr RandomAccessIter1 findEndImpl(const RandomAccessIter1 first, const RandomAccessIter1 last,
                                        const RandomAccessIter2 s_first, const RandomAccessIter2 s_last,
                                        BinaryPredicate pred)
		noexcept(noexcept(s_first == s_last) && noexcept(constexprStd::distance(s_first, s_last)) &&
		         noexcept(constexprStd::distance(first, last)) &&
		         noexcept(std::next(first, std::declval<
		                                   typename std::iterator_traits<RandomAccessIter1>::difference_type>())) &&
		         std::is_nothrow_copy_constructible_v<RandomAccessIter1> &&
		         std::is_nothrow_copy_constructible_v<RandomAccessIter2> &&
		         noexcept(s_first != s_last) &&
		         noexcept(++std::declval<RandomAccessIter2&>(), ++std::declval<RandomAccessIter1&>()) &&
		         noexcept(!pred(*first, *s_first))) {
	if ( s_first == s_last ) {
		return last;
	} //if ( s_first == s_last )
	
	auto sDistance = constexprStd::distance(s_first, s_last);
	auto  distance = constexprStd::distance(first, last);
	
	if ( sDistance > distance ) {
		return last;
	} //if ( sDistance > distance )
	
	for ( auto dist = distance - sDistance; dist >= 0; --dist ) {
		auto iter = constexprStd::next(first, dist);
		auto ret  = iter;
		auto search = s_first;
		for ( ; search != s_last; ++search, ++iter ) {
			if ( !pred(*iter, *search) ) {
				break;
			} //if ( !pred(*iter, *search) )
		} //for ( ; search != s_last; ++search, ++iter )
		if ( search == s_last ) {
			return ret;
		} //if ( search == s_last )
	} //for ( auto dist = distance - sDistance; dist >= 0; --dist )
	return last;
}

template<typename T>
constexpr void threeSwap(T& one, T& two, T& three)
		noexcept(std::is_nothrow_move_constructible_v<T> && std::is_nothrow_move_assignable_v<T>) {
	auto temp  = std::move(one);
	     one   = std::move(two);
	     two   = std::move(three);
	     three = std::move(temp);
	return;
}
} //namespace constexprStd::details

#endif
