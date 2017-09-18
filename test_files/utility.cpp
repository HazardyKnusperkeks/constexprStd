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
 * @brief Checks constexprStd/utility for self-containment and contains the tests.
 */

#include <constexprStd/utility>

#include "../test.hpp"

#include <string>
#include <utility>

#include "test_constants.hpp"

void TestConstexprStd::testExchange(void) const noexcept {
	//Test the acutal constexprness
	auto l = [](int i) constexpr noexcept {
			return constexprStd::exchange(i, 5);
		};
	static_assert(l(8) == 8);
	
	//Test runtime behavior and compare against std::exchange.
	std::string cstr1 = fooString;
	std::string sstr1 = fooString;
	QCOMPARE(cstr1, fooString);
	QCOMPARE(sstr1, fooString);
	
	auto cstr2 = constexprStd::exchange(cstr1, std::string{barString});
	auto sstr2 =          std::exchange(sstr1, std::string{barString});
	QCOMPARE(cstr1, barString);
	QCOMPARE(sstr1, barString);
	QCOMPARE(cstr2, fooString);
	QCOMPARE(sstr2, fooString);
	
	cstr2 = constexprStd::exchange(cstr1, bazString);
	sstr2 =          std::exchange(sstr1, bazString);
	QCOMPARE(cstr1, bazString);
	QCOMPARE(sstr1, bazString);
	QCOMPARE(cstr2, barString);
	QCOMPARE(sstr2, barString);
	return;
}

void TestConstexprStd::testSwap(void) const noexcept {
	//Test the acutal constexprness
	auto l = [](void) constexpr noexcept {
		int ret = 5, foo = 8;
		constexprStd::swap(ret, foo);
		return ret;
	};
	static_assert(l() == 8);
	
	//Test runtime behavior
	std::string str1 = fooString, str2 = barString;
	
	constexprStd::swap(str1, str2);
	
	QCOMPARE(str1, barString);
	QCOMPARE(str2, fooString);
	return;
}
