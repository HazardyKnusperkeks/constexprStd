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
			std::tuple t1{p1.to_std()};
			
			MoveInt mv{4};
			
			pair p2{1, 2};
			std::tuple t2{p2.to_std()};
			pair p3(3, mv);
			std::tuple t3{std::tuple_cat(p3.to_std(), std::tuple{mv.Moved})};
			mv = 6;
			pair p4(5, std::move(mv));
			std::tuple t4{std::tuple_cat(p4.to_std(), std::tuple{mv.Moved})};
			pair p5{p2};
			std::tuple t5{std::tuple_cat(p5.to_std(), std::tuple{p2.second.Moved})};
			pair p6{std::move(p2)};
			std::tuple t6{std::tuple_cat(p6.to_std(), std::tuple{p2.second.Moved}, p2.to_std())};
			
			constexprStd::pair<MoveInt, int> df{42, 66};
			pair p7{df};
			std::tuple t7{std::tuple_cat(p7.to_std(), std::tuple{df.first.Moved})};
			
			pair p8{std::move(df)};
			std::tuple t8{std::tuple_cat(p8.to_std(), std::tuple{df.first.Moved}, df.to_std())};
			
			std::pair<MoveInt, int> sdp{17, 20};
			pair p9{sdp};
			std::tuple t9{std::tuple_cat(p9.to_std(), std::tuple{sdp.first.Moved})};
			
			pair p10{std::move(sdp)};
			std::tuple t10{std::tuple_cat(p10.to_std(), std::tuple{sdp.first.Moved}, sdp)};
			
			constexprStd::pair<std::tuple<int, int>, double> pp{std::piecewise_construct, std::tuple{5, 4.3},
			                                                    std::tuple{3.}};
			
			p1 = p3;
			std::tuple t11{p1.to_std()};
			
			p2 = std::move(p3);
			std::tuple t12{std::tuple_cat(p2.to_std(), std::tuple{p3.second.Moved}, p3.to_std())};
			
			df.first = 9;
			p1 = df;
			std::tuple t13{p1.to_std()};
			
			p2 = std::move(df);
			std::tuple t14{std::tuple_cat(p2.to_std(), std::tuple{df.first.Moved}, df.to_std())};
			
			sdp.first = 17;
			p1 = sdp;
			std::tuple t15{p1.to_std()};
			
			p2 = std::move(sdp);
			std::tuple t16{std::tuple_cat(p2.to_std(), std::tuple{sdp.first.Moved}, sdp)};
			
			std::pair<int, MoveInt> sp{45, 23};
			p3.swap(sp);
			std::tuple t17{std::tuple_cat(p3.to_std(), sp)};
			
			constexprStd::swap(p3, p2);
			std::tuple t18{std::tuple_cat(p3.to_std(), p2.to_std())};
			
			std::swap(p2, p3);
			std::tuple t19{std::tuple_cat(p3.to_std(), p2.to_std())};
			
			using constexprStd::swap;
			swap(p2, p3);
			std::tuple t20{std::tuple_cat(p3.to_std(), p2.to_std())};
			
			return std::tuple_cat(t1, t2, t3, t4, t5, t6, t7, t8, t9, t10, pp.to_std(), t11, t12, t13, t14, t15, t16,
			                      t17, t18, t19, t20);
		};
	
	//                              t1--  t2--  t3---------  t4--------  t5---------  t6--------------
	static_assert(l() == std::tuple{0, 0, 1, 2, 3, 4, false, 5, 6, true, 1, 2, false, 1, 2, true, 1, 0,
	//                              t7-----------  t8-----------------  t9-----------  t10----------------
	                                42, 66, false, 42, 66, true, 0, 66, 17, 20, false, 17, 20, true, 0, 20,
	//                              pp------------------  t11-  t12-------------  t13--  t14---------------  t15---
	                                std::tuple{5, 4}, 3., 3, 4, 3, 4, true, 3, 0, 9, 66, 9, 66, true, 0, 66, 17, 20,
	//                              t16----------------  t17---------  t18-----------  t19-----------  t20-----------
	                                17, 20, true, 0, 20, 45, 23, 3, 0, 17, 20, 45, 23, 45, 23, 17, 20, 17, 20, 45, 23});
	
	using cpair = constexprStd::pair<int, std::string>;
	using spair =          std::pair<int, std::string>;
	
	//It seems that GCC didn't implement the const rvalue ref overload
	static_assert(std::is_same_v<decltype(std::get<0          >(std::declval<      cpair  >())), decltype(std::get<0          >(std::declval<      spair  >()))>);
//	static_assert(std::is_same_v<decltype(std::get<0          >(std::declval<const cpair  >())), decltype(std::get<0          >(std::declval<const spair  >()))>);
	static_assert(std::is_same_v<decltype(std::get<0          >(std::declval<      cpair& >())), decltype(std::get<0          >(std::declval<      spair& >()))>);
	static_assert(std::is_same_v<decltype(std::get<0          >(std::declval<const cpair& >())), decltype(std::get<0          >(std::declval<const spair& >()))>);
	static_assert(std::is_same_v<decltype(std::get<0          >(std::declval<      cpair&&>())), decltype(std::get<0          >(std::declval<      spair&&>()))>);
//	static_assert(std::is_same_v<decltype(std::get<0          >(std::declval<const cpair&&>())), decltype(std::get<0          >(std::declval<const spair&&>()))>);
	static_assert(std::is_same_v<decltype(std::get<1          >(std::declval<      cpair  >())), decltype(std::get<1          >(std::declval<      spair  >()))>);
//	static_assert(std::is_same_v<decltype(std::get<1          >(std::declval<const cpair  >())), decltype(std::get<1          >(std::declval<const spair  >()))>);
	static_assert(std::is_same_v<decltype(std::get<1          >(std::declval<      cpair& >())), decltype(std::get<1          >(std::declval<      spair& >()))>);
	static_assert(std::is_same_v<decltype(std::get<1          >(std::declval<const cpair& >())), decltype(std::get<1          >(std::declval<const spair& >()))>);
	static_assert(std::is_same_v<decltype(std::get<1          >(std::declval<      cpair&&>())), decltype(std::get<1          >(std::declval<      spair&&>()))>);
//	static_assert(std::is_same_v<decltype(std::get<1          >(std::declval<const cpair&&>())), decltype(std::get<1          >(std::declval<const spair&&>()))>);
	static_assert(std::is_same_v<decltype(std::get<int        >(std::declval<      cpair  >())), decltype(std::get<int        >(std::declval<      spair  >()))>);
//	static_assert(std::is_same_v<decltype(std::get<int        >(std::declval<const cpair  >())), decltype(std::get<int        >(std::declval<const spair  >()))>);
	static_assert(std::is_same_v<decltype(std::get<int        >(std::declval<      cpair& >())), decltype(std::get<int        >(std::declval<      spair& >()))>);
	static_assert(std::is_same_v<decltype(std::get<int        >(std::declval<const cpair& >())), decltype(std::get<int        >(std::declval<const spair& >()))>);
	static_assert(std::is_same_v<decltype(std::get<int        >(std::declval<      cpair&&>())), decltype(std::get<int        >(std::declval<      spair&&>()))>);
//	static_assert(std::is_same_v<decltype(std::get<int        >(std::declval<const cpair&&>())), decltype(std::get<int        >(std::declval<const spair&&>()))>);
	static_assert(std::is_same_v<decltype(std::get<std::string>(std::declval<      cpair  >())), decltype(std::get<std::string>(std::declval<      spair  >()))>);
//	static_assert(std::is_same_v<decltype(std::get<std::string>(std::declval<const cpair  >())), decltype(std::get<std::string>(std::declval<const spair  >()))>);
	static_assert(std::is_same_v<decltype(std::get<std::string>(std::declval<      cpair& >())), decltype(std::get<std::string>(std::declval<      spair& >()))>);
	static_assert(std::is_same_v<decltype(std::get<std::string>(std::declval<const cpair& >())), decltype(std::get<std::string>(std::declval<const spair& >()))>);
	static_assert(std::is_same_v<decltype(std::get<std::string>(std::declval<      cpair&&>())), decltype(std::get<std::string>(std::declval<      spair&&>()))>);
//	static_assert(std::is_same_v<decltype(std::get<std::string>(std::declval<const cpair&&>())), decltype(std::get<std::string>(std::declval<const spair&&>()))>);
	
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
	
	//Assignment
	//Normal copy
	cp1 = cp3;
	sp1 = sp3;
	CMP(cp1, sp1);
	
	//Move
	cp2 = std::move(cp3);
	sp2 = std::move(sp3);
	CMP(cp2, sp2);
	QCOMPARE(cp3.second, emptyString);
	QCOMPARE(sp3.second, emptyString);
	
	//Copy from different type
	cdf.first = 13;
	sdf.first = 13;
	cp1 = cdf;
	sp1 = sdf;
	CMP(cp1, sp1);
	CMP(cdf, sdf);
	
	//Move from different type
	cp2 = std::move(cdf);
	sp2 = std::move(sdf);
	CMP(cp2, sp2);
	CMP(cdf, sdf);
	QVERIFY(cdf.first.Moved);
	QVERIFY(sdf.first.Moved);
	
	//Copy from std
	sdf.first = 97;
	cp1 = sdf;
	CMP(cp1, sdf);
	
	//Move from std
	cp2 = std::move(sdf);
	CMP(cp1, cp2);
	QVERIFY(sdf.first.Moved);
	
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
