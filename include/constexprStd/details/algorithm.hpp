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
#include "find.hpp"
#include "helper.hpp"

namespace constexprStd::details {
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

template<typename ForwardIter, typename OutputIter, typename BinaryPredicate>
constexpr OutputIter uniqueCopyImpl(ForwardIter first, const ForwardIter last, OutputIter d_first, BinaryPredicate pred,
                                    const std::forward_iterator_tag)
		noexcept(std::is_nothrow_move_constructible_v<ForwardIter> &&
		         noexcept(constexprStd::adjacent_find(first, last, pred)) && noexcept(first != last) &&
		         std::is_nothrow_move_assignable_v<ForwardIter> && std::is_nothrow_move_assignable_v<OutputIter> &&
		         noexcept(constexprStd::copy(first, ++first, d_first)) &&
		         std::is_nothrow_copy_assignable_v<ForwardIter> && noexcept(first != last && pred(*first, *first)) &&
		         noexcept(++first) && noexcept(constexprStd::copy(first, last, d_first))) {
	auto iter = constexprStd::adjacent_find(first, last, pred);
	for ( ; iter != last; iter = constexprStd::adjacent_find(first, last, pred) ) {
		d_first = constexprStd::copy(first, ++iter, d_first);
		for ( first = iter; first != last && pred(*iter, *first); ++first ) {
			
		} //for ( first = iter; first != last && pred(*iter, *first); ++first )
	} //for ( ; iter != last; iter = constexprStd::adjacent_find(first, last, pred) )
	return constexprStd::copy(first, last, d_first);
}

template<typename Iter>
using typeOf = typename std::iterator_traits<Iter>::value_type;

template<typename InputIter, typename OutputIter, typename BinaryPredicate>
constexpr OutputIter uniqueCopyInputImpl(InputIter first, const InputIter last, OutputIter d_first,
                                         BinaryPredicate pred, const std::output_iterator_tag)
		noexcept(std::is_nothrow_move_constructible_v<typeOf<InputIter>> && noexcept(*first) &&
		         std::is_nothrow_copy_assignable_v<InputIter> && noexcept(++first) && noexcept(first != last) &&
		         noexcept(!pred(std::declval<typeOf<InputIter>&>(), std::declval<typeOf<InputIter>&>())) &&
		         std::is_nothrow_move_assignable_v<typeOf<InputIter>> && noexcept(++d_first) &&
		         std::is_nothrow_assignable_v<decltype(*d_first), typeOf<InputIter>&>) {
	auto lastCopied = *first;
	*d_first = lastCopied;
	for ( ++first; first != last; ++first ) {
		auto current = *first;
		if ( !pred(lastCopied, current) ) {
			*++d_first = lastCopied = std::move(current);
		} //if ( !pred(lastCopied, current) )
	} //for ( ++first; first != last; ++first )
	return ++d_first;
}

template<typename Iter1, typename Iter2>
constexpr bool sameType = std::is_same_v<typename std::iterator_traits<Iter1>::value_type,
                                         typename std::iterator_traits<Iter2>::value_type>;

template<typename InputIter, typename OutputIter, typename BinaryPredicate>
constexpr OutputIter uniqueCopyInputImpl(InputIter first, const InputIter last, OutputIter d_first,
                                         BinaryPredicate pred, const std::forward_iterator_tag)
		noexcept((!sameType<InputIter, OutputIter> || (noexcept(*d_first = *first) && noexcept(++first) &&
		                                               noexcept(first != last) && noexcept(++d_first) &&
		                                               noexcept(!pred(*d_first, *first)))) &&
		         (sameType<InputIter, OutputIter> ||
		          noexcept(uniqueCopyInputImpl(first, last, d_first, std::move(pred), std::output_iterator_tag{})))) {
	if constexpr ( sameType<InputIter, OutputIter> ) {
		*d_first = *first;
		for ( ++first; first != last; ++first ) {
			if ( !pred(*d_first, *first) ) {
				*++d_first = *first;
			} //if ( !pred(*d_first, *first) )
		} //for ( ++first; first != last; ++first )
	} //if constexpr ( sameType<InputIter, OutputIter> )
	else {
		return uniqueCopyInputImpl(first, last, d_first, std::move(pred), std::output_iterator_tag{});
	} //else -> if constexpr ( sameType<InputIter, OutputIter> )
	return ++d_first;
}

template<typename InputIter, typename OutputIter, typename BinaryPredicate>
constexpr OutputIter uniqueCopyImpl(const InputIter first, const InputIter last, const OutputIter d_first,
                                    BinaryPredicate pred, const std::input_iterator_tag)
		noexcept(noexcept(uniqueCopyInputImpl(first, last, d_first, std::move(pred), iteratorCategory<OutputIter>))) {
	return uniqueCopyInputImpl(first, last, d_first, std::move(pred), iteratorCategory<OutputIter>);
}

template<typename BidirIter, typename UnaryPredicate>
constexpr BidirIter partitionImpl(BidirIter first, BidirIter last, UnaryPredicate pred,
                                  const std::bidirectional_iterator_tag)
		noexcept(noexcept(first != last) && noexcept(first != last && pred(*first)) && noexcept(++first) &&
		         noexcept(first == last) && noexcept(first != --last && !pred(*last)) &&
		         noexcept(constexprStd::iter_swap(first, last))) {
	while ( first != last ) {
		for ( ; first != last && pred(*first); ++first ) {
			
		} //for ( ; first != last && pred(*first); ++first )
		
		if ( first == last ) {
			break;
		} //if ( first == last )
		
		for ( ; first != --last && !pred(*last); ) {
			
		} //for ( ; first != --last && !pred(*last); )
		
		if ( first != last ) {
			constexprStd::iter_swap(first, last);
			++first;
		} //if ( first != last )
	} //while ( first != last )
	return first;
}

template<typename ForwardIter, typename UnaryPredicate>
constexpr ForwardIter partitionImpl(ForwardIter first, const ForwardIter last, UnaryPredicate pred,
                                    const std::forward_iterator_tag)
noexcept(std::is_nothrow_move_constructible_v<ForwardIter> &&
         noexcept(constexprStd::find_if_not(first, last, pred)) && std::is_nothrow_copy_assignable_v<ForwardIter>) {
	auto ret = constexprStd::find_if_not(first, last, pred);
	if ( ret != last ) {
		first = ret;
		++first;
		while ( first != last ) {
			first = constexprStd::find_if(first, last, pred);
			if ( first != last ) {
				constexprStd::iter_swap(ret, first);
				++ret;
				++first;
			} //if ( first != last )
		} //while ( first != last )
	} //if ( ret != last )
	return ret;
}

template<typename BidirIter>
constexpr void rotateOneRight(const BidirIter first, BidirIter last)
noexcept {
	auto beforeLast = constexprStd::prev(last);
	auto temp = std::move(*beforeLast);
	constexprStd::move_backward(first, beforeLast, last);
	*first = std::move(temp);
	return;
}

template<typename RandomAccessIter, typename UnaryPredicate>
constexpr RandomAccessIter partitionPointImpl(RandomAccessIter first, RandomAccessIter last, UnaryPredicate pred,
                                              const std::random_access_iterator_tag)
		noexcept(noexcept(first != last) && noexcept(constexprStd::distance(first, last)) &&
		         std::is_nothrow_move_constructible_v<typename
		                                              std::iterator_traits<RandomAccessIter>::difference_type> &&
		         noexcept(std::declval<typename std::iterator_traits<RandomAccessIter>::difference_type&>() / 2) &&
		         noexcept(constexprStd::next(first)) && std::is_nothrow_move_constructible_v<RandomAccessIter> &&
		         noexcept(pred(*first)) && noexcept(++first) && std::is_nothrow_copy_assignable_v<RandomAccessIter>) {
	while ( first != last ) {
		const auto dist = constexprStd::distance(first, last);
		const auto incr = dist / 2;
		auto middle = constexprStd::next(first, incr);
		
		if ( pred(*middle) ) {
			first = ++middle;
		} //if ( pred(*middle) )
		else {
			last = middle;
		} //else -> if ( pred(*middle) )
	} //while ( first != last )
	return last;
}

template<typename ForwardIter, typename UnaryPredicate>
constexpr ForwardIter partitionPointImpl(ForwardIter first, ForwardIter last, UnaryPredicate pred,
                                         const std::forward_iterator_tag)
		noexcept(noexcept(constexprStd::find_if_not(first, last, std::move(pred)))) {
	return constexprStd::find_if_not(first, last, std::move(pred));
}
} //namespace constexprStd::details

#endif
