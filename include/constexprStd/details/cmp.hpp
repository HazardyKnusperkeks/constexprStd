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
 * @brief Contains some compare helper.
 */

#ifndef CONSTEXPRSTD_DETAILS_CMP_HPP
#define CONSTEXPRSTD_DETAILS_CMP_HPP

namespace constexprStd::details::cmp {
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
} //namespace constexprStd::details::cmp

#endif
