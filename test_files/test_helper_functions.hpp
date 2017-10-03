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
constexpr bool isVocal(const char c) noexcept {
	switch ( c ) {
		case 'a' :
		case 'e' :
		case 'i' :
		case 'o' :
		case 'u' : return true;
	} //switch ( c )
	return false;
}

constexpr bool bothOdd(const int i, const int j) noexcept { return isOdd(i) && isOdd(j); }
constexpr bool bothVocal(const char c, const char d) noexcept { return isVocal(c) && isVocal(d); }

constexpr int fib(const int i) noexcept {
	switch ( i ) {
		case 0 :
		case 1 : return i;
	} //switch ( i )
	return fib(i-1) + fib(i-2);
}

constexpr bool isFib(const int i) noexcept {
	if ( i < 0 ) {
		return false;
	} //if ( i < 0 )
	
	int c = 0;
	int f = fib(0);
	while ( f < i ) {
		f = fib(++c);
	} //while ( f < i )
	return f == i;
}

#endif
