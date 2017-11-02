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
 * @brief Forward declares some of the algorihmns defined in <iterator>
 */

#ifndef CONSTEXPRSTD_DETAILS_ITERATOR_FORWARD_HPP
#define CONSTEXPRSTD_DETAILS_ITERATOR_FORWARD_HPP

#include "iterator.hpp"

namespace constexprStd {
template<typename InputIterator, typename Distance>
constexpr void advance(InputIterator& i, const Distance n)
		noexcept(noexcept(details::advanceImpl(i, n, details::iteratorCategory<InputIterator>)));

template<typename InputIterator>
constexpr InputIterator next(InputIterator iter,
                             const typename std::iterator_traits<InputIterator>::difference_type n = 1)
                             noexcept(noexcept(constexprStd::advance(iter, n)));
} //namespace constexpr

#endif
