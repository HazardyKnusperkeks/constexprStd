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

#ifndef TEST_CONTAINER_HPP
#define TEST_CONTAINER_HPP

#include <array>

struct Tag { };

class TestContainer : public std::array<int, 10> {
	public:
	constexpr TestContainer(std::initializer_list<int> il) noexcept : std::array<int, 10>{} {
		auto listBegin = il.begin();
		const auto listEnd = il.end();
		auto arrayBegin = begin();
		const auto arrayEnd = end();
		
		for ( ; listBegin != listEnd && arrayBegin != arrayEnd; ++listBegin, ++arrayBegin ) {
			*arrayBegin = *listBegin;
		} //for ( ; listBegin != listEnd && arrayBegin != arrayEnd; ++listBegin, ++arrayBegin )
		return;
	}
	
	template<std::size_t N>
	constexpr TestContainer(std::array<int, N> a) noexcept : std::array<int, 10>{} {
		auto externalBegin = a.begin();
		const auto externalEnd = a.end();
		auto internalBegin = begin();
		const auto internalEnd = end();
		
		for ( ; externalBegin != externalEnd && internalBegin != internalEnd; ++externalBegin, ++internalBegin ) {
			*internalBegin = *externalBegin;
		} //for ( ; externalBegin != externalEnd && internalBegin != internalEnd; ++externalBegin, ++internalBegin )
		return;
	}
	
	constexpr TestContainer(void) noexcept : std::array<int, 10>{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}} {
		return;
	}
	
	constexpr TestContainer(std::array<int, 10> a) noexcept : std::array<int, 10>{a} {
		return;
	}
	
	constexpr TestContainer(const Tag, int a, const int b, const int c = 1) noexcept : std::array<int, 10>{} {
		for ( std::size_t i = 0; i < 10u && a <= b; ++i, a +=c ) {
			operator[](i) = a;
		} //for ( std::size_t i = 0; i < 10u && a <= b; ++i, a += c )
		return;
	}
	
	constexpr iterator insert(iterator iter, const int value) noexcept {
		*iter = value;
		return iter;
	}
	
	constexpr bool operator==(const TestContainer& c) const noexcept {
		for ( std::size_t i = 0; i < 10u; ++i ) {
			if ( operator[](i) != c[i] ) {
				return false;
			} //if ( operator[](i) != c[i] )
		} //for ( std::size_t i = 0; i < 10u; ++i )
		return true;
	}
};

constexpr bool operator==(const TestContainer& c, const std::array<int, 10>& a) noexcept {
	return c.operator==(a);
}

#endif
