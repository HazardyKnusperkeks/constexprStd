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
} //namespace constexpr

#endif
