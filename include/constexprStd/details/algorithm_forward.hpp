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
 * @brief Forward declares some of the algorihmns defined in <algorithm>
 */

#ifndef CONSTEXPRSTD_DETAILS_ALGORITHM_FORWARD_HPP
#define CONSTEXPRSTD_DETAILS_ALGORITHM_FORWARD_HPP

#include <type_traits>

#include "iterator_forward.hpp"

namespace constexprStd {
template<typename IterT1, typename IterT2, typename BinaryPredicate>
constexpr bool equal(IterT1 first1, const IterT1 last1, IterT2 first2, const BinaryPredicate pred)
		noexcept(noexcept(first1 != last1) && noexcept(++first1, ++first2) && noexcept(pred(*first1, *first2)));

template<typename ForwardIter1, typename ForwardIter2, typename BinaryPredicate>
constexpr ForwardIter1 search(ForwardIter1 first, const ForwardIter1 last,
                              const ForwardIter2 s_first, const ForwardIter2 s_last, BinaryPredicate pred)
		noexcept(noexcept(first != last) && noexcept(++first) && std::is_nothrow_copy_constructible_v<ForwardIter1> &&
		         std::is_nothrow_copy_constructible_v<ForwardIter2> &&
		         noexcept(first != last && std::declval<ForwardIter2&>() != s_last &&
		                  pred(*first, *std::declval<ForwardIter2&>())) &&
		         noexcept(++first, ++std::declval<ForwardIter2&>()) &&
		         noexcept(std::declval<ForwardIter2&>() == s_last) && noexcept(first == last));

template<typename ForwardIter, typename BinaryPredicate>
constexpr ForwardIter adjacent_find(ForwardIter first, const ForwardIter last, BinaryPredicate pred)
		noexcept(noexcept(first == last) && noexcept(constexprStd::next(first)) && noexcept(first != last) &&
		         noexcept(++first, ++first) && noexcept(pred(*first, *first)));

template<typename InputIterator, typename OutputIterator>
constexpr OutputIterator copy(InputIterator sourceFirst, const InputIterator sourceLast, OutputIterator destination)
		noexcept(noexcept(sourceFirst != sourceLast) && noexcept(++sourceFirst) && noexcept(++destination) &&
		         noexcept(*destination = *sourceFirst));

template<typename ForwardIter1, typename ForwardIter2>
constexpr void iter_swap(const ForwardIter1 iter1, const ForwardIter2 iter2)
		noexcept(std::is_nothrow_swappable_with_v<typename std::iterator_traits<ForwardIter1>::value_type,
		                                          typename std::iterator_traits<ForwardIter2>::value_type>);

template<typename BidirIter1, typename BidirIter2>
constexpr BidirIter2 move_backward(const BidirIter1 first, BidirIter1 last, BidirIter2 d_last)
		noexcept(noexcept(first != last) && noexcept(*--d_last = std::move(*--last)));

template<typename RandIter, typename Compare = std::less<>,
         typename Dist = typename std::iterator_traits<RandIter>::difference_type>
constexpr RandIter is_heap_until(const RandIter first, const RandIter last, Compare cmp = {})
		noexcept(noexcept(constexprStd::distance(first, last)) && std::is_nothrow_move_constructible_v<Dist> &&
		         noexcept(std::declval<const Dist&>() / 2) && std::is_nothrow_constructible_v<Dist, int> &&
		         std::is_nothrow_copy_constructible_v<RandIter> &&
		         noexcept(std::declval<Dist&>() < std::declval<const Dist&>()) &&
		         noexcept(++std::declval<Dist&>(), ++std::declval<RandIter&>()) &&
		         noexcept(std::declval<Dist&>() * 2 + 1) &&
		         noexcept(constexprStd::next(first, std::declval<const Dist&>())) &&
		         std::is_nothrow_move_constructible_v<RandIter> &&
		         noexcept(cmp(*std::declval<RandIter&>(), *std::declval<RandIter&>())) &&
		         noexcept(++std::declval<RandIter&>() != last));

template<typename RandIter, typename Compare = std::less<>,
         typename Dist = typename std::iterator_traits<RandIter>::difference_type>
constexpr void push_heap(const RandIter first, RandIter last, Compare cmp = {})
		noexcept(noexcept(first == last) && noexcept(constexprStd::distance(first, --last)) &&
		         std::is_nothrow_move_constructible_v<Dist> && std::is_nothrow_constructible_v<bool, Dist&> &&
		         noexcept((std::declval<Dist&>() - 1) / 2) &&
		         noexcept(constexprStd::next(first, std::declval<Dist&>())) &&
		         noexcept(constexprStd::next(first, std::declval<const Dist&>())) &&
		         noexcept(cmp(*first, *first)) && noexcept(constexprStd::iter_swap(first, first)) &&
		         std::is_nothrow_copy_assignable_v<Dist>);

template<typename RandIter, typename Compare = std::less<>>
constexpr void make_heap(const RandIter first, const RandIter last, Compare cmp = {})
		noexcept(noexcept(constexprStd::is_heap_until(first, last, cmp)) &&
		         std::is_nothrow_move_constructible_v<RandIter> && noexcept(std::declval<RandIter&>()++ != last) &&
		         noexcept(constexprStd::push_heap(first, std::declval<RandIter&>(), cmp)));

template<typename RandIter, typename Compare = std::less<>,
         typename Dist = typename std::iterator_traits<RandIter>::difference_type>
constexpr void pop_heap(const RandIter first, RandIter last, Compare cmp = {})
		noexcept(noexcept(first == last) && noexcept(constexprStd::iter_swap(first, --last)) &&
		         noexcept(constexprStd::distance(first, last)) && std::is_nothrow_move_constructible_v<Dist> &&
		         noexcept(std::declval<const Dist&>() / 2) && std::is_nothrow_constructible_v<Dist, int> &&
		         noexcept(std::declval<Dist&>() < std::declval<const Dist&>()) &&
		         noexcept(std::declval<Dist&>() * 2 + 1) && noexcept(std::declval<const Dist&>() + 1) &&
		         noexcept(constexprStd::next(first, std::declval<const Dist&>())) &&
		         noexcept(cmp(*first, *first)) && noexcept(constexprStd::iter_swap(first, first)) &&
		         std::is_nothrow_copy_assignable_v<Dist>);

template<typename RandIter, typename Compare = std::less<>>
constexpr void sort_heap(const RandIter first, RandIter last, Compare cmp = {})
		noexcept(noexcept(first != last) && noexcept(--last) && noexcept(constexprStd::pop_heap(first, last, cmp)));
} //namespace constexpr

#endif
