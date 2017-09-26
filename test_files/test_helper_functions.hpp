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

#ifndef TEST_HELPER_FUNCTIONS_HPP
#define TEST_HELPER_FUNCTIONS_HPP

constexpr bool isLessThanEleven(const int i) noexcept { return i < 11; }
constexpr bool isMultipleOfFive(const int i) noexcept { return i %  5 == 0; }
constexpr bool isMultipleOfEleven(const int i) noexcept { return i % 11 == 0; }
constexpr bool isOdd(const int i) noexcept { return i % 2 == 1; }

constexpr int fib(const int i) noexcept {
	switch ( i ) {
		case 0 :
		case 1 : return i;
	} //switch ( i )
	return fib(i-1) + fib(i-2);
}

#endif
