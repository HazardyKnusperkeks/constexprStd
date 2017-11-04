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
 * @brief Contains the declaration and definition of find, find_if, and find_if_not.
 */

#ifndef CONSTEXPRSTD_DETAILS_FIND_HPP
#define CONSTEXPRSTD_DETAILS_FIND_HPP

#include "../functional"
#include "cmp.hpp"

namespace constexprStd {
template<typename InputIter, typename UnaryPredicate>
constexpr InputIter find_if(InputIter first, const InputIter last, const UnaryPredicate pred)
		noexcept(noexcept(first != last && !pred(*first)) && noexcept(++first)) {
	for ( ; first != last && !pred(*first); ++first ) {
		
	} //for ( ; first != last && !pred(*first); ++first )
	return first;
}

template<typename Container, typename UnaryPredicate>
constexpr auto find_if(Container&& c, const UnaryPredicate pred)
		noexcept(noexcept(constexprStd::find_if(std::begin(std::forward<Container>(c)),
		                                        std::end(std::forward<Container>(c)), pred))) {
	return constexprStd::find_if(std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c)), pred);
}

template<typename InputIter, typename T>
constexpr InputIter find(const InputIter first, const InputIter last, const T& t)
		noexcept(noexcept(constexprStd::find_if(first, last, details::cmp::EqualToValue{t}))) {
	return constexprStd::find_if(first, last, details::cmp::EqualToValue{t});
}

template<typename Container, typename T>
constexpr auto find(Container&& c, const T& t)
		noexcept(noexcept(constexprStd::find(std::begin(std::forward<Container>(c)),
		                                     std::end(std::forward<Container>(c)), t))) {
	return constexprStd::find(std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c)), t);
}

template<typename InputIter, typename UnaryPredicate>
constexpr InputIter find_if_not(const InputIter first, const InputIter last, UnaryPredicate pred)
		noexcept(noexcept(constexprStd::find_if(first, last, constexprStd::not_fn(std::move(pred))))) {
	return constexprStd::find_if(first, last, constexprStd::not_fn(std::move(pred)));
}

template<typename Container, typename UnaryPredicate>
constexpr auto find_if_not(Container&& c, const UnaryPredicate pred)
		noexcept(noexcept(constexprStd::find_if_not(std::begin(std::forward<Container>(c)),
		                                            std::end(std::forward<Container>(c)), pred))) {
	return constexprStd::find_if_not(std::begin(std::forward<Container>(c)), std::end(std::forward<Container>(c)),
	                                 pred);
}
} //namespace constexprStd

#endif
