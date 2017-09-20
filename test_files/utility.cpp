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
#include <tuple>
#include <utility>

#include "test_constants.hpp"
#include "test_move_int.hpp"

void TestConstexprStd::testPair(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			using pair = constexprStd::pair<int, MoveInt>;
			pair p1{};
			
			MoveInt mv{4};
			
			pair p2{1, 2};
			std::tuple t1{p2.to_std()};
			pair p3(3, mv);
			std::tuple t2{mv.Moved};
			mv = 6;
			pair p4(5, std::move(mv));
			std::tuple t3{mv.Moved};
			pair p5{p2};
			std::tuple t4{p2.second.Moved};
			pair p6{std::move(p2)};
			std::tuple t5{p2.second.Moved};
			
			constexprStd::pair<MoveInt, int> df{42, 66};
			pair p7{df};
			std::tuple t6{df.first.Moved};
			
			pair p8{std::move(df)};
			std::tuple t7{df.first.Moved};
			
			std::pair<MoveInt, int> sp{17, 20};
			pair p9{sp};
			std::tuple t8{sp.first.Moved};
			
			pair p10{std::move(sp)};
			std::tuple t9{sp.first.Moved};
			
			constexprStd::pair<std::tuple<int, int>, double> pp{std::piecewise_construct, std::tuple{5, 4.3},
			                                                    std::tuple{3.}};
			
			return std::tuple_cat(p1.to_std(), t1, p3.to_std(), t2, p4.to_std(), t3, p5.to_std(), t4, p6.to_std(), t5,
			                      p2.to_std(), p7.to_std(), t6, p8.to_std(), t7, df.to_std(), p9.to_std(), t8,
			                      p10.to_std(), t9, pp.to_std());
		};
	
	static_assert(l() == std::tuple{0, 0, 1, 2, 3, 4, false, 5, 6, true, 1, 2, false, 1, 2, true, 1, 0, 42, 66, false,
	                                42, 66, true, 0, 66, 17, 20, false, 17, 20, true, std::tuple{5, 4}, 3.});
//	l();
	
	using cpair = constexprStd::pair<int, std::string>;
	using spair =          std::pair<int, std::string>;
	
	#define CMP(c, s) QVERIFY(c.first == s.first); QVERIFY(c.second == s.second)
	
	//Construct
	//Default
	cpair cp1;
	spair sp1;
	CMP(cp1, sp1);
	
	//Aggregate
	cpair cp2{4, fooStrings};
	spair sp2{4, fooStrings};
	CMP(cp2, sp2);
	
	//"Normal"
	cpair cp3(4, fooStrings);
	spair sp3(4, fooStrings);
	CMP(cp3, sp3);
	
	auto cstr = barStrings;
	auto sstr = barStrings;
	
	//Convert/Move
	cpair cp4{1, std::move(cstr)};
	spair sp4{1, std::move(sstr)};
	CMP(cp4, sp4);
	QCOMPARE(cstr, emptyString);
	QCOMPARE(sstr, emptyString);
	
	//Copy
	cpair cp5{cp2};
	spair sp5{sp2};
	CMP(cp5, sp5);
	
	//Move
	cpair cp6{std::move(cp2)};
	spair sp6{std::move(sp2)};
	CMP(cp2, sp2);
	CMP(cp6, sp6);
	QCOMPARE(cp2.second, emptyString);
	
	//Copy from different type
	constexprStd::pair<MoveInt, const char*> cdf{99, fooString};
	         std::pair<MoveInt, const char*> sdf{99, fooString};
	
	cpair cp7{cdf};
	spair sp7{sdf};
	CMP(cp7, sp7);
	QVERIFY(!cdf.first.Moved);
	QVERIFY(!sdf.first.Moved);
	
	//Move from different type
	cpair cp8{std::move(cdf)};
	spair sp8{std::move(sdf)};
	CMP(cdf, sdf);
	CMP(cp8, sp8);
	QVERIFY(cdf.first.Moved);
	QVERIFY(sdf.first.Moved);
	
	//Copy from std pair
	sdf.first = {99};
	cpair cp9{sdf};
	CMP(cp9, sdf);
	QVERIFY(!sdf.first.Moved);
	
	//Move from std pair
	cpair cp10{std::move(sdf)};
	CMP(cp10, sp8);
	QVERIFY(sdf.first.Moved);
	
	//Piecewise
	constexprStd::pair<std::tuple<int, bool, char>, std::tuple<bool, bool, double>> cpp{std::piecewise_construct,
	                                                                                    std::tuple{4, false, 'h'},
	                                                                                    std::tuple{true, false, 2.3}};
	         std::pair<std::tuple<int, bool, char>, std::tuple<bool, bool, double>> spp{std::piecewise_construct,
	                                                                                    std::tuple{4, false, 'h'},
	                                                                                    std::tuple{true, false, 2.3}};
	CMP(cpp, spp);
	
	#undef CMP
	return;
}

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
