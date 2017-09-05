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
} //namespace constexprStd::details

#endif
