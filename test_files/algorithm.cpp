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
 * @brief Checks constexprStd/algorithm for self-containment and contains the tests.
 */

#include <constexprStd/algorithm>

#include "../test.hpp"
#include "test_constants.hpp"
#include "test_container.hpp"
#include "test_helper_functions.hpp"

#include <constexprStd/functional>

#include <algorithm>
#include <array>
#include <forward_list>
#include <iterator>
#include <sstream>
#include <tuple>
#include <utility>

#include <QTest>

template<typename T>
static constexpr void doubleValue(T& t) noexcept(noexcept(t += t)) {
	t += t;
	return;
}

void TestConstexprStd::testAllAnyNone(void) const noexcept {
	//We do not test constexprness, because we only use functions which are constexpr
	TestContainer allOdd{1, 3, 5, 7, 9, 11, 13, 15, 17, 19};
	TestContainer allEven{2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
	TestContainer mixed;
	
	QVERIFY( constexprStd::all_of( allOdd,                         isOdd));
	QVERIFY(          std::all_of( allOdd.begin(),  allOdd.end(),  isOdd));
	QVERIFY( constexprStd::any_of( allOdd,                         isOdd));
	QVERIFY(          std::any_of( allOdd.begin(),  allOdd.end(),  isOdd));
	QVERIFY(!constexprStd::none_of(allOdd,                         isOdd));
	QVERIFY(!         std::none_of(allOdd.begin(),  allOdd.end(),  isOdd));
	
	QVERIFY(!constexprStd::all_of( allEven,                        isOdd));
	QVERIFY(!         std::all_of( allEven.begin(), allEven.end(), isOdd));
	QVERIFY(!constexprStd::any_of( allEven,                        isOdd));
	QVERIFY(!         std::any_of( allEven.begin(), allEven.end(), isOdd));
	QVERIFY( constexprStd::none_of(allEven,                        isOdd));
	QVERIFY(          std::none_of(allEven.begin(), allEven.end(), isOdd));
	
	QVERIFY(!constexprStd::all_of( mixed,                          isOdd));
	QVERIFY(!         std::all_of( mixed.begin(),   mixed.end(),   isOdd));
	QVERIFY( constexprStd::any_of( mixed,                          isOdd));
	QVERIFY(          std::any_of( mixed.begin(),   mixed.end(),   isOdd));
	QVERIFY(!constexprStd::none_of(mixed,                          isOdd));
	QVERIFY(!         std::none_of(mixed.begin(),   mixed.end(),   isOdd));
	
	QVERIFY(!constexprStd::all_of( allOdd,                         isLessThanEleven));
	QVERIFY(!         std::all_of( allOdd.begin(),  allOdd.end(),  isLessThanEleven));
	QVERIFY( constexprStd::any_of( allOdd,                         isLessThanEleven));
	QVERIFY(          std::any_of( allOdd.begin(),  allOdd.end(),  isLessThanEleven));
	QVERIFY(!constexprStd::none_of(allOdd,                         isLessThanEleven));
	QVERIFY(!         std::none_of(allOdd.begin(),  allOdd.end(),  isLessThanEleven));
	
	QVERIFY(!constexprStd::all_of( allEven,                        isLessThanEleven));
	QVERIFY(!         std::all_of( allEven.begin(), allEven.end(), isLessThanEleven));
	QVERIFY( constexprStd::any_of( allEven,                        isLessThanEleven));
	QVERIFY(          std::any_of( allEven.begin(), allEven.end(), isLessThanEleven));
	QVERIFY(!constexprStd::none_of(allEven,                        isLessThanEleven));
	QVERIFY(!         std::none_of(allEven.begin(), allEven.end(), isLessThanEleven));
	
	QVERIFY( constexprStd::all_of( mixed,                          isLessThanEleven));
	QVERIFY(          std::all_of( mixed.begin(),   mixed.end(),   isLessThanEleven));
	QVERIFY( constexprStd::any_of( mixed,                          isLessThanEleven));
	QVERIFY(          std::any_of( mixed.begin(),   mixed.end(),   isLessThanEleven));
	QVERIFY(!constexprStd::none_of(mixed,                          isLessThanEleven));
	QVERIFY(!         std::none_of(mixed.begin(),   mixed.end(),   isLessThanEleven));
	return;
}

void TestConstexprStd::testForEach(void) const noexcept {
	auto l1 = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::for_each(c, *doubleValue<int>);
			return c;
		};
	
	static_assert(l1() == TestContainer{2, 4, 6, 8, 10, 12, 14, 16, 18, 20});
	
	auto l2 = [](void) constexpr noexcept {
			const TestContainer c;
			int sum = 0;
			auto sumUp = [&sum](const int i) constexpr noexcept { sum += i; return; };
			constexprStd::for_each(c, sumUp);
			return sum;
		};
	
	static_assert(l2() == 55);
	
	using ArrayType = std::array<std::string, 6>;
	
	ArrayType ca{fooString, barString, bazString, emptyString, "a", "b"};
	ArrayType sa{ca};
	const ArrayType cmp{fooStrings + fooString, barStrings + barString, bazStrings + bazString, "", "aa", "bb"};
	
	constexprStd::for_each(ca.begin(), ca.end(), *doubleValue<std::string>);
	         std::for_each(sa.begin(), sa.end(), *doubleValue<std::string>);
	
	QCOMPARE(ca, cmp);
	QCOMPARE(sa, cmp);
	
	int ccount = 0;
	int scount = 0;
	
	auto createCount = [](int& count) noexcept {
			return [&count](const std::string& s) noexcept {
					auto pos = s.find_first_of("aeiou");
					for ( ; pos != std::string::npos; pos = s.find_first_of("aeiou", pos + 1) ) {
						++count;
					} //for ( ; pos != std::string::npos; pos = s.find_first_of("aeiou", pos + 1) )
					return;
				};
		};
	
	constexprStd::for_each(ca.begin(), ca.end(), createCount(ccount));
	         std::for_each(sa.begin(), sa.end(), createCount(scount));
	
	QCOMPARE(ccount, 10);
	QCOMPARE(scount, 10);
	return;
}

void TestConstexprStd::testForEachN(void) const noexcept {
	auto l1 = [](void) constexpr noexcept {
		TestContainer c;
		constexprStd::for_each_n(c.begin(), 5, *doubleValue<int>);
		return c;
	};
	
	static_assert(l1() == TestContainer{2, 4, 6, 8, 10, 6, 7, 8, 9, 10});
	
	auto l2 = [](void) constexpr noexcept {
		const TestContainer c;
		int sum = 0;
		auto sumUp = [&sum](const int i) constexpr noexcept { sum += i; return; };
		constexprStd::for_each_n(c.begin(), 5, sumUp);
		return sum;
	};
	
	static_assert(l2() == 15);
	
	using ArrayType = std::array<std::string, 6>;
	
	//No std::for_each_n, at least on GCC 7.1
	
	ArrayType ca{fooString, barString, bazString, emptyString, "a", "b"};
	//ArrayType sa{ca};
	const ArrayType cmp{fooStrings + fooString, barStrings + barString, bazStrings + bazString, "", "a", "b"};
	
	auto cthree = constexprStd::for_each_n(ca.begin(), 3, *doubleValue<std::string>);
	//auto sthree =          std::for_each_n(sa.begin(), 3, *doubleValue<std::string>);
	
	QCOMPARE(ca, cmp);
	//QCOMPARE(sa, cmp);
	
	QCOMPARE(cthree, ca.begin() + 3);
	//QCOMPARE(sthree, sa.begin() + 3);
	
	int ccount = 0;
	//int scount = 0;
	
	auto createCount = [](int& count) noexcept {
		return [&count](const std::string& s) noexcept {
			auto pos = s.find_first_of("aeiou");
			for ( ; pos != std::string::npos; pos = s.find_first_of("aeiou", pos + 1) ) {
				++count;
			} //for ( ; pos != std::string::npos; pos = s.find_first_of("aeiou", pos + 1) )
			return;
		};
	};
	
	constexprStd::for_each_n(ca.begin(), 2, createCount(ccount));
	//         std::for_each_n(sa.begin(), 2, createCount(scount));
	
	QCOMPARE(ccount, 6);
	//QCOMPARE(scount, 6);
	return;
}

void TestConstexprStd::testCount(void) const noexcept {
	constexpr TestContainer c;
	static_assert(constexprStd::count(c.begin(), c.end(), 2) == 1);
	static_assert(constexprStd::count(c.begin(), c.end(), 17) == 0);
	static_assert(constexprStd::count(c, 5) == 1);
}

void TestConstexprStd::testCountIf(void) const noexcept {
	constexpr TestContainer c;
	static_assert(constexprStd::count_if(c.begin(), c.end(), isOdd) == 5);
	static_assert(constexprStd::count_if(c, isMultipleOfFive) == 2);
	static_assert(constexprStd::count_if(c, isMultipleOfEleven) == 0);
	return;
}

void TestConstexprStd::testMismatch(void) const noexcept {
	//We test only withour predicate, because it uses the one with predicate
	std::array a1{1, 2, 3, 4, 5};
	std::array a2{1, 2, 3, 4, 6};
	std::array a3{1, 2, 3, 4};
	std::array a4{1, 9, 3, 4, 7};
	
	std::pair ma1{a1.end(), a1.end()};
	QCOMPARE(constexprStd::mismatch(a1.begin(), a1.end(), a1.begin()),           ma1);
	QCOMPARE(         std::mismatch(a1.begin(), a1.end(), a1.begin()),           ma1);
	QCOMPARE(constexprStd::mismatch(a1.begin(), a1.end(), a1.begin(), a1.end()), ma1);
	QCOMPARE(         std::mismatch(a1.begin(), a1.end(), a1.begin(), a1.end()), ma1);
	QCOMPARE(constexprStd::mismatch(a1,                   a1.begin()),           ma1);
	QCOMPARE(constexprStd::mismatch(a1,                   a1),                   ma1);
	
	std::pair ma2{std::prev(a1.end()), std::prev(a2.end())};
	QCOMPARE(constexprStd::mismatch(a1.begin(), a1.end(), a2.begin()),           ma2);
	QCOMPARE(         std::mismatch(a1.begin(), a1.end(), a2.begin()),           ma2);
	QCOMPARE(constexprStd::mismatch(a1.begin(), a1.end(), a2.begin(), a2.end()), ma2);
	QCOMPARE(         std::mismatch(a1.begin(), a1.end(), a2.begin(), a2.end()), ma2);
	QCOMPARE(constexprStd::mismatch(a1,                   a2.begin()),           ma2);
	QCOMPARE(constexprStd::mismatch(a1,                   a2),                   ma2);
	
	std::pair ma3{std::prev(a1.end()), a3.end()};
	QCOMPARE(constexprStd::mismatch(a1.begin(), a1.end(), a3.begin()),           ma3);
	QCOMPARE(         std::mismatch(a1.begin(), a1.end(), a3.begin()),           ma3);
	QCOMPARE(constexprStd::mismatch(a1.begin(), a1.end(), a3.begin(), a3.end()), ma3);
	QCOMPARE(         std::mismatch(a1.begin(), a1.end(), a3.begin(), a3.end()), ma3);
	QCOMPARE(constexprStd::mismatch(a1,                   a3.begin()),           ma3);
	QCOMPARE(constexprStd::mismatch(a1,                   a3),                   ma3);
	
	std::pair ma4{std::next(a1.begin()), std::next(a4.begin())};
	QCOMPARE(constexprStd::mismatch(a1.begin(), a1.end(), a4.begin()),           ma4);
	QCOMPARE(         std::mismatch(a1.begin(), a1.end(), a4.begin()),           ma4);
	QCOMPARE(constexprStd::mismatch(a1.begin(), a1.end(), a4.begin(), a4.end()), ma4);
	QCOMPARE(         std::mismatch(a1.begin(), a1.end(), a4.begin(), a4.end()), ma4);
	QCOMPARE(constexprStd::mismatch(a1,                   a4.begin()),           ma4);
	QCOMPARE(constexprStd::mismatch(a1,                   a4),                   ma4);
	return;
}

void TestConstexprStd::testEqual(void) const noexcept {
	constexpr TestContainer cc1, cc2;
	constexpr std::array csa{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	constexpr int cba[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	constexpr std::array cla{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	
	//We only test the overloads without the predicates, because they call the ones with.
	
	//Test 3 argument overload
	static_assert(constexprStd::equal(cc1.begin(), cc1.end(), cc2.begin()));
	static_assert(constexprStd::equal(cc1.begin(), cc1.end(), csa.begin()));
	static_assert(constexprStd::equal(cc1.begin(), cc1.end(), std::begin(cba)));
	static_assert(constexprStd::equal(cc1.begin(), cc1.end(), cla.begin()));
	
	static_assert(constexprStd::equal(cc2.begin(),     cc2.end(),     cc1.begin()));
	static_assert(constexprStd::equal(csa.begin(),     csa.end(),     cc1.begin()));
	static_assert(constexprStd::equal(std::begin(cba), std::end(cba), cc1.begin()));
	//static_assert(!constexprStd::equal(cla.begin(),     cla.end(),     cc1.begin())); would access out of bounds on cc1
	
	//Test 1st 2 argument overload
	static_assert(constexprStd::equal(cc1, cc2.begin()));
	static_assert(constexprStd::equal(cc1, csa.begin()));
	static_assert(constexprStd::equal(cc1, std::begin(cba)));
	static_assert(constexprStd::equal(cc1, cla.begin()));
	
	static_assert(constexprStd::equal(cc2, cc1.begin()));
	static_assert(constexprStd::equal(csa, cc1.begin()));
	static_assert(constexprStd::equal(cba, cc1.begin()));
	//static_assert(!constexprStd::equal(cla, cc1.begin())); would access out of bounds on cc1
	
	//Test 4 argument overload
	static_assert( constexprStd::equal(cc1.begin(), cc1.end(), cc2.begin(),     cc2.end()));
	static_assert( constexprStd::equal(cc1.begin(), cc1.end(), csa.begin(),     csa.end()));
	static_assert( constexprStd::equal(cc1.begin(), cc1.end(), std::begin(cba), std::end(cba)));
	static_assert(!constexprStd::equal(cc1.begin(), cc1.end(), cla.begin(),     cla.end()));
	
	static_assert( constexprStd::equal(cc2.begin(),     cc2.end(),     cc1.begin(), cc1.end()));
	static_assert( constexprStd::equal(csa.begin(),     csa.end(),     cc1.begin(), cc1.end()));
	static_assert( constexprStd::equal(std::begin(cba), std::end(cba), cc1.begin(), cc1.end()));
	static_assert(!constexprStd::equal(cla.begin(),     cla.end(),     cc1.begin(), cc1.end()));
	
	//Test 2nd 2 argument overload
	static_assert( constexprStd::equal(cc1, cc2));
	static_assert( constexprStd::equal(cc1, csa));
	//Ambiguous because one overload works with const int[10], the next with const int*.
	//static_assert( constexprStd::equal(cc1, cba));
	static_assert(!constexprStd::equal(cc1, cla));
	
	static_assert( constexprStd::equal(cc2, cc1));
	static_assert( constexprStd::equal(csa, cc1));
	static_assert( constexprStd::equal(cba, cc1));
	static_assert(!constexprStd::equal(cla, cc1));
	
	
	//Test runtime and compare against std::equal
	std::list cnl{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	std::list cll{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	TestContainer sc1;
	
	//Test 3 argument overload
	QVERIFY((constexprStd::equal(cc1.begin(), cc1.end(), cc2.begin())));
	QVERIFY((         std::equal(sc1.begin(), sc1.end(), cc2.begin())));
	QVERIFY((constexprStd::equal(cc1.begin(), cc1.end(), csa.begin())));
	QVERIFY((         std::equal(sc1.begin(), sc1.end(), csa.begin())));
	QVERIFY((constexprStd::equal(cc1.begin(), cc1.end(), std::begin(cba))));
	QVERIFY((         std::equal(sc1.begin(), sc1.end(), std::begin(cba))));
	QVERIFY((constexprStd::equal(cc1.begin(), cc1.end(), cla.begin())));
	QVERIFY((         std::equal(sc1.begin(), sc1.end(), cla.begin())));
	QVERIFY((constexprStd::equal(cc1.begin(), cc1.end(), cnl.begin())));
	QVERIFY((         std::equal(sc1.begin(), sc1.end(), cnl.begin())));
	QVERIFY((constexprStd::equal(cc1.begin(), cc1.end(), cll.begin())));
	QVERIFY((         std::equal(sc1.begin(), sc1.end(), cll.begin())));
	
	//Test 1st 2 argument overload
	QVERIFY((constexprStd::equal(cc1, cc2.begin())));
	QVERIFY((constexprStd::equal(cc1, csa.begin())));
	QVERIFY((constexprStd::equal(cc1, std::begin(cba))));
	QVERIFY((constexprStd::equal(cc1, cla.begin())));
	QVERIFY((constexprStd::equal(cc1, cnl.begin())));
	QVERIFY((constexprStd::equal(cc1, cll.begin())));
	
	QVERIFY((constexprStd::equal(cc2, cc1.begin())));
	QVERIFY((constexprStd::equal(csa, cc1.begin())));
	QVERIFY((constexprStd::equal(cba, cc1.begin())));
	QVERIFY((constexprStd::equal(cnl, cc1.begin())));
	
	//Test 4 argument overload
	QVERIFY( (constexprStd::equal(cc1.begin(), cc1.end(), cc2.begin(),     cc2.end())));
	QVERIFY( (         std::equal(sc1.begin(), sc1.end(), cc2.begin(),     cc2.end())));
	QVERIFY( (constexprStd::equal(cc1.begin(), cc1.end(), csa.begin(),     csa.end())));
	QVERIFY( (         std::equal(sc1.begin(), sc1.end(), csa.begin(),     csa.end())));
	QVERIFY( (constexprStd::equal(cc1.begin(), cc1.end(), std::begin(cba), std::end(cba))));
	QVERIFY( (         std::equal(sc1.begin(), sc1.end(), std::begin(cba), std::end(cba))));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), cla.begin(),     cla.end())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), cla.begin(),     cla.end())));
	QVERIFY( (constexprStd::equal(cc1.begin(), cc1.end(), cnl.begin(),     cnl.end())));
	QVERIFY( (         std::equal(sc1.begin(), sc1.end(), cnl.begin(),     cnl.end())));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), cll.begin(),     cll.end())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), cll.begin(),     cll.end())));
	
	//Test 1st 2 argument overload
	QVERIFY( (constexprStd::equal(cc1, cc2)));
	QVERIFY( (constexprStd::equal(cc1, csa)));
	//QVERIFY( (constexprStd::equal(cc1, cba)));
	QVERIFY(!(constexprStd::equal(cc1, cla)));
	QVERIFY( (constexprStd::equal(cc1, cnl)));
	QVERIFY(!(constexprStd::equal(cc1, cll)));
	
	QVERIFY( (constexprStd::equal(cc2, cc1)));
	QVERIFY( (constexprStd::equal(csa, cc1)));
	QVERIFY( (constexprStd::equal(cba, cc1)));
	QVERIFY(!(constexprStd::equal(cla, cc1)));
	QVERIFY( (constexprStd::equal(cnl, cc1)));
	QVERIFY(!(constexprStd::equal(cll, cc1)));
	return;
}

void TestConstexprStd::testUnequal(void) const noexcept {
	constexpr TestContainer cc1;
	constexpr std::array csa{1, 2, 3, 4, 5, 6, 7, 8, 7, 10};
	constexpr int cba[10] = {1, 2, 3, 4, 5, 6, 7, 8, 7, 10};
	constexpr std::array cla{1, 2, 3, 4, 5, 6, 7, 8, 7, 10, 11, 12, 13};
	
	//We only test the overloads without the predicates, because they call the ones with.
	
	//Test 3 argument overload
	static_assert(!constexprStd::equal(cc1.begin(), cc1.end(), csa.begin()));
	static_assert(!constexprStd::equal(cc1.begin(), cc1.end(), std::begin(cba)));
	static_assert(!constexprStd::equal(cc1.begin(), cc1.end(), cla.begin()));
	
	static_assert(!constexprStd::equal(csa.begin(),     csa.end(),     cc1.begin()));
	static_assert(!constexprStd::equal(std::begin(cba), std::end(cba), cc1.begin()));
	static_assert(!constexprStd::equal(cla.begin(),     cla.end(),     cc1.begin()));
	
	//Test 1st 2 argument overload
	static_assert(!constexprStd::equal(cc1, csa.begin()));
	static_assert(!constexprStd::equal(cc1, std::begin(cba)));
	static_assert(!constexprStd::equal(cc1, cla.begin()));
	
	static_assert(!constexprStd::equal(csa, cc1.begin()));
	static_assert(!constexprStd::equal(cba, cc1.begin()));
	static_assert(!constexprStd::equal(cla, cc1.begin()));
	
	//Test 4 argument overload
	static_assert(!constexprStd::equal(cc1.begin(), cc1.end(), csa.begin(),     csa.end()));
	static_assert(!constexprStd::equal(cc1.begin(), cc1.end(), std::begin(cba), std::end(cba)));
	static_assert(!constexprStd::equal(cc1.begin(), cc1.end(), cla.begin(),     cla.end()));
	
	static_assert(!constexprStd::equal(csa.begin(),     csa.end(),     cc1.begin(), cc1.end()));
	static_assert(!constexprStd::equal(std::begin(cba), std::end(cba), cc1.begin(), cc1.end()));
	static_assert(!constexprStd::equal(cla.begin(),     cla.end(),     cc1.begin(), cc1.end()));
	
	//Test 2nd 2 argument overload
	static_assert(!constexprStd::equal(cc1, csa));
	//Ambiguous because one overload works with const int[10], the next with const int*.
	//static_assert( constexprStd::equal(cc1, cba));
	static_assert(!constexprStd::equal(cc1, cla));
	
	static_assert(!constexprStd::equal(csa, cc1));
	static_assert(!constexprStd::equal(cba, cc1));
	static_assert(!constexprStd::equal(cla, cc1));
	
	
	//Test runtime and compare against std::equal
	std::list cnl{1, 2, 3, 4, 5, 6, 7, 8, 7, 10};
	std::list cll{1, 2, 3, 4, 5, 6, 7, 8, 7, 10, 11, 12, 13};
	TestContainer sc1;
	
	//Test 3 argument overload
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), csa.begin())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), csa.begin())));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), std::begin(cba))));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), std::begin(cba))));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), cla.begin())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), cla.begin())));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), cnl.begin())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), cnl.begin())));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), cll.begin())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), cll.begin())));
	
	//Test 1st 2 argument overload
	QVERIFY(!(constexprStd::equal(cc1, csa.begin())));
	QVERIFY(!(constexprStd::equal(cc1, std::begin(cba))));
	QVERIFY(!(constexprStd::equal(cc1, cla.begin())));
	QVERIFY(!(constexprStd::equal(cc1, cnl.begin())));
	QVERIFY(!(constexprStd::equal(cc1, cll.begin())));
	
	QVERIFY(!(constexprStd::equal(csa, cc1.begin())));
	QVERIFY(!(constexprStd::equal(cba, cc1.begin())));
	QVERIFY(!(constexprStd::equal(cnl, cc1.begin())));
	
	//Test 4 argument overload
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), csa.begin(),     csa.end())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), csa.begin(),     csa.end())));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), std::begin(cba), std::end(cba))));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), std::begin(cba), std::end(cba))));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), cla.begin(),     cla.end())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), cla.begin(),     cla.end())));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), cnl.begin(),     cnl.end())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), cnl.begin(),     cnl.end())));
	QVERIFY(!(constexprStd::equal(cc1.begin(), cc1.end(), cll.begin(),     cll.end())));
	QVERIFY(!(         std::equal(sc1.begin(), sc1.end(), cll.begin(),     cll.end())));
	
	//Test 1st 2 argument overload
	QVERIFY(!(constexprStd::equal(cc1, csa)));
	//QVERIFY( (constexprStd::equal(cc1, cba)));
	QVERIFY(!(constexprStd::equal(cc1, cla)));
	QVERIFY(!(constexprStd::equal(cc1, cnl)));
	QVERIFY(!(constexprStd::equal(cc1, cll)));
	
	QVERIFY(!(constexprStd::equal(csa, cc1)));
	QVERIFY(!(constexprStd::equal(cba, cc1)));
	QVERIFY(!(constexprStd::equal(cla, cc1)));
	QVERIFY(!(constexprStd::equal(cnl, cc1)));
	QVERIFY(!(constexprStd::equal(cll, cc1)));
	return;
}

void TestConstexprStd::testFind(void) const noexcept {
	constexpr TestContainer c;
	static_assert(*constexprStd::find(c.begin(), c.end(), 5) == 5);
	static_assert(*constexprStd::find(c, 8) == 8);
	static_assert(constexprStd::find(c.begin(), c.end(), 11) == c.end());
	static_assert(constexprStd::find(c, 12) == c.end());
	
	std::forward_list<std::string> l{fooString, barString, bazString};
	
	auto cp = constexprStd::find(l.begin(), l.end(), fooString);
	auto sp =          std::find(l.begin(), l.end(), fooString);
	QVERIFY(cp != l.end());
	QVERIFY(sp != l.end());
	QCOMPARE(*cp, fooString);
	QCOMPARE(*sp, fooString);
	
	QCOMPARE(constexprStd::find(l.begin(), l.end(), "not"), l.end());
	QCOMPARE(         std::find(l.begin(), l.end(), "not"), l.end());
	
	cp = constexprStd::find(l, bazString);
	QVERIFY(cp != l.end());
	QCOMPARE(*cp, bazString);
	return;
}

void TestConstexprStd::testFindIf(void) const noexcept {
	//Test constexprness
	constexpr TestContainer cc;
	
	static_assert(*constexprStd::find_if(cc.begin(), cc.end(), isMultipleOfFive) == 5);
	static_assert(constexprStd::find_if(cc.begin(), cc.end(), isMultipleOfEleven) == cc.end());
	
	static_assert(*constexprStd::find_if(cc, isMultipleOfFive) == 5);
	static_assert(constexprStd::find_if(cc, isMultipleOfEleven) == cc.end());
	
	//I don't think a comparison to std::find_if is neccessary.
	return;
}

void TestConstexprStd::testFindIfNot(void) const noexcept {
	//Test constexprness
	constexpr TestContainer cc;
	
	static_assert(*constexprStd::find_if_not(cc.begin(), cc.end(), isOdd) == 2);
	static_assert(constexprStd::find_if_not(cc.begin(), cc.end(), isLessThanEleven) == cc.end());
	
	static_assert(*constexprStd::find_if_not(cc, isOdd) == 2);
	static_assert(constexprStd::find_if_not(cc, isLessThanEleven) == cc.end());
	
	//I don't think a comparison to std::find_if_not is neccessary.
	return;
}

void TestConstexprStd::testFindEnd(void) const noexcept {
	auto lambda = [](void) constexpr noexcept {
			std::array a{1, 2, 3, 4, 5, 1, 2, 5, 4};
			std::array s1{1, 2};
			std::array s2{5};
			std::array s3{7};
			auto d1 = constexprStd::distance(a.begin(), constexprStd::find_end(a, s1.begin(), s1.end()));
			auto d2 = constexprStd::distance(a.begin(), constexprStd::find_end(a, s2));
			auto d3 = constexprStd::distance(a.begin(), constexprStd::find_end(a, s3));
			return std::tuple{d1, d2, d3};
		};
	
	static_assert(lambda() == std::tuple{5, 7, 9});
	
	std::string s = "123 hallo 123";
	std::string searchS1 = "123";
	std::string searchS2 = "l";
	std::string searchS3 = fooString;
	std::string searchS4 = "123 ";
	std::string searchS5 = "123 hallo 123 bar";
	
	std::forward_list<char> l;
	std::forward_list<char> searchL1;
	std::forward_list<char> searchL2;
	std::forward_list<char> searchL3;
	std::forward_list<char> searchL4;
	std::forward_list<char> searchL5;
	
	std::copy(       s.rbegin(),        s.rend(), std::front_inserter(l));
	std::copy(searchS1.rbegin(), searchS1.rend(), std::front_inserter(searchL1));
	std::copy(searchS2.rbegin(), searchS2.rend(), std::front_inserter(searchL2));
	std::copy(searchS3.rbegin(), searchS3.rend(), std::front_inserter(searchL3));
	std::copy(searchS4.rbegin(), searchS4.rend(), std::front_inserter(searchL4));
	std::copy(searchS5.rbegin(), searchS5.rend(), std::front_inserter(searchL5));
	
	constexpr std::iterator_traits<std::string::iterator>::difference_type d1 = 10;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d2 =  7;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d3 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d4 =  0;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d5 = 13;
	
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchS1.begin(), searchS1.end())), d1);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchS2.begin(), searchS2.end())), d2);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchS3.begin(), searchS3.end())), d3);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchS4.begin(), searchS4.end())), d4);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchS5.begin(), searchS5.end())), d5);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchL1.begin(), searchL1.end())), d1);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchL2.begin(), searchL2.end())), d2);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchL3.begin(), searchL3.end())), d3);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchL4.begin(), searchL4.end())), d4);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchL5.begin(), searchL5.end())), d5);
	QCOMPARE(std::distance(s.begin(),          std::find_end(s.begin(), s.end(), searchL3.end(),   searchL3.end())), d3);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchS1.begin(), searchS1.end())), d1);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchS2.begin(), searchS2.end())), d2);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchS3.begin(), searchS3.end())), d3);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchS4.begin(), searchS4.end())), d4);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchS5.begin(), searchS5.end())), d5);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchL1.begin(), searchL1.end())), d1);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchL2.begin(), searchL2.end())), d2);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchL3.begin(), searchL3.end())), d3);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchL4.begin(), searchL4.end())), d4);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchL5.begin(), searchL5.end())), d5);
	QCOMPARE(std::distance(l.begin(),          std::find_end(l.begin(), l.end(), searchL3.end(),   searchL3.end())), d3);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchS1.begin(), searchS1.end())), d1);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchS2.begin(), searchS2.end())), d2);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchS3.begin(), searchS3.end())), d3);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchS4.begin(), searchS4.end())), d4);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchS5.begin(), searchS5.end())), d5);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchL1.begin(), searchL1.end())), d1);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchL2.begin(), searchL2.end())), d2);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchL3.begin(), searchL3.end())), d3);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchL4.begin(), searchL4.end())), d4);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchL5.begin(), searchL5.end())), d5);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_end(s.begin(), s.end(), searchL3.end(),   searchL3.end())), d3);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchS1.begin(), searchS1.end())), d1);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchS2.begin(), searchS2.end())), d2);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchS3.begin(), searchS3.end())), d3);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchS4.begin(), searchS4.end())), d4);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchS5.begin(), searchS5.end())), d5);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchL1.begin(), searchL1.end())), d1);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchL2.begin(), searchL2.end())), d2);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchL3.begin(), searchL3.end())), d3);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchL4.begin(), searchL4.end())), d4);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchL5.begin(), searchL5.end())), d5);
	QCOMPARE(std::distance(l.begin(), constexprStd::find_end(l.begin(), l.end(), searchL3.end(),   searchL3.end())), d3);
	return;
}

void TestConstexprStd::testFindFirstOf(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			std::array s1{8, 9, 2};
			std::array s2{17, 11};
			std::array<int, 0> s3{};
			auto d1 = constexprStd::distance(c.begin(), constexprStd::find_first_of(c, s1));
			auto d2 = constexprStd::distance(c.begin(), constexprStd::find_first_of(c, s2));
			auto d3 = constexprStd::distance(c.begin(), constexprStd::find_first_of(c, s3));
			auto d4 = constexprStd::distance(c.begin(), constexprStd::find_first_of(c, c.rbegin(), c.rend()));
			return std::tuple{d1, d2, d3, d4};
		};
	
	static_assert(l() == std::tuple{1, 10, 10, 0});
	
	std::string s = "123 hallo 123";
	std::string searchS1 = "4568 ";
	std::string searchS2 = emptyString;
	std::string searchS3 = fooString;
	std::string searchS4 = "kljdsahfkljdsahfjnsafkjhsadkjfh1";
	std::string searchS5 = "qwertzuipü+sdfgjköä#<yxcvbnm,.-";
	
	constexpr std::iterator_traits<std::string::iterator>::difference_type d1 =  3;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d2 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d3 =  8;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d4 =  0;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d5 = 13;
	
	QCOMPARE(std::distance(s.begin(),          std::find_first_of(s.begin(), s.end(), searchS1.begin(), searchS1.end())), d1);
	QCOMPARE(std::distance(s.begin(),          std::find_first_of(s.begin(), s.end(), searchS2.begin(), searchS2.end())), d2);
	QCOMPARE(std::distance(s.begin(),          std::find_first_of(s.begin(), s.end(), searchS3.begin(), searchS3.end())), d3);
	QCOMPARE(std::distance(s.begin(),          std::find_first_of(s.begin(), s.end(), searchS4.begin(), searchS4.end())), d4);
	QCOMPARE(std::distance(s.begin(),          std::find_first_of(s.begin(), s.end(), searchS5.begin(), searchS5.end())), d5);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_first_of(s.begin(), s.end(), searchS1.begin(), searchS1.end())), d1);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_first_of(s.begin(), s.end(), searchS2.begin(), searchS2.end())), d2);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_first_of(s.begin(), s.end(), searchS3.begin(), searchS3.end())), d3);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_first_of(s.begin(), s.end(), searchS4.begin(), searchS4.end())), d4);
	QCOMPARE(std::distance(s.begin(), constexprStd::find_first_of(s.begin(), s.end(), searchS5.begin(), searchS5.end())), d5);
	return;
}

void TestConstexprStd::testAdjacentFind(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			std::array a{6, 5, 9, 3, 3, 2, 0};
			TestContainer c;
			
			auto d1 = constexprStd::distance(a.begin(), constexprStd::adjacent_find(a));
			auto d2 = constexprStd::distance(a.begin(), constexprStd::adjacent_find(a, bothOdd));
			auto d3 = constexprStd::distance(c.begin(), constexprStd::adjacent_find(c));
			auto d4 = constexprStd::distance(c.begin(), constexprStd::adjacent_find(c, bothOdd));
			return std::tuple{d1, d2, d3, d4};
		};
	
	static_assert(l() == std::tuple{3, 1, 10, 10});
	
	constexpr std::iterator_traits<std::string::iterator>::difference_type d1 =  1;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d2 =  1;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d3 = 30;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d4 = 47;
	
	std::string s1(fooString);
	std::string s2(longString);
	
	QCOMPARE(std::distance(s1.begin(),          std::adjacent_find(s1.begin(), s1.end())           ), d1);
	QCOMPARE(std::distance(s1.begin(),          std::adjacent_find(s1.begin(), s1.end(), bothVocal)), d2);
	QCOMPARE(std::distance(s2.begin(),          std::adjacent_find(s2.begin(), s2.end())           ), d3);
	QCOMPARE(std::distance(s2.begin(),          std::adjacent_find(s2.begin(), s2.end(), bothVocal)), d4);
	QCOMPARE(std::distance(s1.begin(), constexprStd::adjacent_find(s1.begin(), s1.end())           ), d1);
	QCOMPARE(std::distance(s1.begin(), constexprStd::adjacent_find(s1.begin(), s1.end(), bothVocal)), d2);
	QCOMPARE(std::distance(s2.begin(), constexprStd::adjacent_find(s2.begin(), s2.end())           ), d3);
	QCOMPARE(std::distance(s2.begin(), constexprStd::adjacent_find(s2.begin(), s2.end(), bothVocal)), d4);
	return;
}

void TestConstexprStd::testSearch(void) const noexcept {
	auto lambda = [](void) constexpr noexcept {
			std::array a{1, 2, 3, 4, 5, 1, 2, 5, 4};
			std::array s1{1, 2};
			std::array s2{5};
			std::array s3{7};
			auto d1 = constexprStd::distance(a.begin(), constexprStd::search(a, s1.begin(), s1.end()));
			auto d2 = constexprStd::distance(a.begin(), constexprStd::search(a, s2));
			auto d3 = constexprStd::distance(a.begin(), constexprStd::search(a, s3));
			return std::tuple{d1, d2, d3};
		};
	
	static_assert(lambda() == std::tuple{0, 4, 9});
	
	std::string s = "123 hallo 123";
	std::string s1 = "123";
	std::string s2 = "l";
	std::string s3 = fooString;
	std::string s4 = " 123";
	std::string s5 = "123 hallo 123 bar";
	std::string s6 = "x";
	std::string s7 = "";
	
	constexpr std::iterator_traits<std::string::iterator>::difference_type d1 =  0;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d2 =  6;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d3 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d4 =  9;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d5 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d6 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d7 =  0;
	
	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), s1.begin(), s1.end())), d1);
	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), s2.begin(), s2.end())), d2);
	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), s3.begin(), s3.end())), d3);
	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), s4.begin(), s4.end())), d4);
	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), s5.begin(), s5.end())), d5);
	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), s6.begin(), s6.end())), d6);
	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), s7.begin(), s7.end())), d7);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), s1.begin(), s1.end())), d1);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), s2.begin(), s2.end())), d2);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), s3.begin(), s3.end())), d3);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), s4.begin(), s4.end())), d4);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), s5.begin(), s5.end())), d5);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), s6.begin(), s6.end())), d6);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), s7.begin(), s7.end())), d7);
	return;
}

void TestConstexprStd::testSearchSearcher(void) const noexcept {
	std::string s = "123 hallo 123";
	std::string s1 = "123";
	std::string s2 = "l";
	std::string s3 = fooString;
	std::string s4 = " 123";
	std::string s5 = "123 hallo 123 bar";
	std::string s6 = "x";
	std::string s7 = "";
	
	constexpr std::iterator_traits<std::string::iterator>::difference_type d1 =  0;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d2 =  6;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d3 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d4 =  9;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d5 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d6 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d7 =  0;
	
	constexprStd::default_searcher searcher1{s1};
	constexprStd::default_searcher searcher2{s2};
	constexprStd::default_searcher searcher3{s3};
	constexprStd::default_searcher searcher4{s4};
	constexprStd::default_searcher searcher5{s5};
	constexprStd::default_searcher searcher6{s6};
	constexprStd::default_searcher searcher7{s7};
	
	//Apprently missing in GCCs STL
//	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), searcher1)), d1);
//	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), searcher2)), d2);
//	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), searcher3)), d3);
//	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), searcher4)), d4);
//	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), searcher5)), d5);
//	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), searcher6)), d6);
//	QCOMPARE(std::distance(s.begin(),          std::search(s.begin(), s.end(), searcher7)), d7);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), searcher1)), d1);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), searcher2)), d2);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), searcher3)), d3);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), searcher4)), d4);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), searcher5)), d5);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), searcher6)), d6);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s.begin(), s.end(), searcher7)), d7);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s,                  searcher1)), d1);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s,                  searcher2)), d2);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s,                  searcher3)), d3);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s,                  searcher4)), d4);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s,                  searcher5)), d5);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s,                  searcher6)), d6);
	QCOMPARE(std::distance(s.begin(), constexprStd::search(s,                  searcher7)), d7);
	return;
}

void TestConstexprStd::testSearchN(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			std::array a{3, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2};
			auto d1 = constexprStd::distance(a.begin(), constexprStd::search_n(a, 1, 1));
			auto d2 = constexprStd::distance(a.begin(), constexprStd::search_n(a, 2, 1));
			auto d3 = constexprStd::distance(a.begin(), constexprStd::search_n(a, 3, 1));
			auto d4 = constexprStd::distance(a.begin(), constexprStd::search_n(a, 4, 1));
			auto d5 = constexprStd::distance(a.begin(), constexprStd::search_n(a, 5, 1));
			auto d6 = constexprStd::distance(a.begin(), constexprStd::search_n(a, 6, 1));
			auto d7 = constexprStd::distance(a.begin(), constexprStd::search_n(a, 7, 1));
			return std::tuple{d1, d2, d3, d4, d5, d6, d7};
		};
	
	static_assert(l() == std::tuple{1, 1, 1, 5, 5, 5, 12});
	
	std::string s = "foobooooboobooooooo";
	
	constexpr std::iterator_traits<std::string::iterator>::difference_type d0 =  0;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d1 =  1;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d2 =  1;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d3 =  4;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d4 =  4;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d5 = 12;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d6 = 12;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d7 = 12;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d8 = 19;
	
	QCOMPARE(std::distance(s.begin(),          std::search_n(s.begin(), s.end(), 0, 'o')), d0);
	QCOMPARE(std::distance(s.begin(),          std::search_n(s.begin(), s.end(), 1, 'o')), d1);
	QCOMPARE(std::distance(s.begin(),          std::search_n(s.begin(), s.end(), 2, 'o')), d2);
	QCOMPARE(std::distance(s.begin(),          std::search_n(s.begin(), s.end(), 3, 'o')), d3);
	QCOMPARE(std::distance(s.begin(),          std::search_n(s.begin(), s.end(), 4, 'o')), d4);
	QCOMPARE(std::distance(s.begin(),          std::search_n(s.begin(), s.end(), 5, 'o')), d5);
	QCOMPARE(std::distance(s.begin(),          std::search_n(s.begin(), s.end(), 6, 'o')), d6);
	QCOMPARE(std::distance(s.begin(),          std::search_n(s.begin(), s.end(), 7, 'o')), d7);
	QCOMPARE(std::distance(s.begin(),          std::search_n(s.begin(), s.end(), 8, 'o')), d8);
	QCOMPARE(std::distance(s.begin(), constexprStd::search_n(s.begin(), s.end(), 0, 'o')), d0);
	QCOMPARE(std::distance(s.begin(), constexprStd::search_n(s.begin(), s.end(), 1, 'o')), d1);
	QCOMPARE(std::distance(s.begin(), constexprStd::search_n(s.begin(), s.end(), 2, 'o')), d2);
	QCOMPARE(std::distance(s.begin(), constexprStd::search_n(s.begin(), s.end(), 3, 'o')), d3);
	QCOMPARE(std::distance(s.begin(), constexprStd::search_n(s.begin(), s.end(), 4, 'o')), d4);
	QCOMPARE(std::distance(s.begin(), constexprStd::search_n(s.begin(), s.end(), 5, 'o')), d5);
	QCOMPARE(std::distance(s.begin(), constexprStd::search_n(s.begin(), s.end(), 6, 'o')), d6);
	QCOMPARE(std::distance(s.begin(), constexprStd::search_n(s.begin(), s.end(), 7, 'o')), d7);
	QCOMPARE(std::distance(s.begin(), constexprStd::search_n(s.begin(), s.end(), 8, 'o')), d8);
	return;
}

void TestConstexprStd::testCopy(void) const noexcept {
	//Test the acutal constexprness
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			std::array a{42, 66, 0x185, 1337};
			constexprStd::copy(a.begin(), a.end(), c.begin());
			return c;
		};
	static_assert(l() == TestContainer{std::array{42, 66, 0x185, 1337, 5, 6, 7, 8, 9, 10}});
	
	//Test runtime behavior and compare against std::copy.
	TestContainer cc;
	TestContainer sc;
	const std::array sa{19, 97};
	int ba[2] = {18, 85};
	const TestContainer expected1;
	const TestContainer expected2{std::array{19, 97,  3,  4,  5,  6,  7,  8,  9, 10}};
	const TestContainer expected3{std::array{19, 97, 18, 85,  5,  6,  7,  8,  9, 10}};
	const TestContainer expected4{std::array{19, 97, 18, 85, 19, 97,  7,  8,  9, 10}};
	const TestContainer expected5{std::array{19, 97, 18, 85, 19, 97, 18, 85,  9, 10}};
	auto citer = cc.begin();
	auto siter = sc.begin();
	
	QCOMPARE(cc, expected1);
	QCOMPARE(sc, expected1);
	
	citer = constexprStd::copy(sa.begin(), sa.end(), citer);
	siter =          std::copy(sa.begin(), sa.end(), siter);
	QCOMPARE(cc, expected2);
	QCOMPARE(sc, expected2);
	
	citer = constexprStd::copy(&ba[0], &ba[2], citer);
	siter =          std::copy(&ba[0], &ba[2], siter);
	QCOMPARE(cc, expected3);
	QCOMPARE(sc, expected3);
	
	//Test convenience overoads
	citer = constexprStd::copy(sa, citer);
	QCOMPARE(cc, expected4);
	
	citer = constexprStd::copy(ba, citer);
	QCOMPARE(cc, expected5);
	return;
}

void TestConstexprStd::testCopyIf(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c{};
			std::array<int, 5> a{};
			constexprStd::copy_if(c, a.begin(), isOdd);
			return a;
		};
	static_assert(TestContainer(l()) == TestContainer{1, 3, 5, 7, 9});
	
	std::array<int, 40> a;
	std::generate(a.begin(), a.end(), [i = -2](void) mutable noexcept  { return ++i; });
	
	std::vector<int> cv;
	std::vector<int> sv;
	
	constexprStd::copy_if(a.begin(), a.end(), std::back_inserter(cv), isFib);
	         std::copy_if(a.begin(), a.end(), std::back_inserter(sv), isFib);
	QVERIFY(cv == sv);
	return;
}

void TestConstexprStd::testCopyN(void) const noexcept {
	//Test the acutal constexprness
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			std::array a{42, 66, 0x185, 1337};
			constexprStd::copy_n(a.begin(), 3, c.begin());
			return c;
		};
	static_assert(l() == TestContainer{std::array{42, 66, 0x185, 4, 5, 6, 7, 8, 9, 10}});
	
	//Test runtime behavior and compare against std::copy.
	TestContainer cc;
	TestContainer sc;
	const std::array sa{19, 97};
	int ba[2] = {18, 85};
	const TestContainer expected1;
	const TestContainer expected2{std::array{19,  2,  3,  4,  5,  6,  7,  8,  9, 10}};
	const TestContainer expected3{std::array{19, 18, 85,  4,  5,  6,  7,  8,  9, 10}};
	auto citer = cc.begin();
	auto siter = sc.begin();
	
	QCOMPARE(cc, expected1);
	QCOMPARE(sc, expected1);
	
	citer = constexprStd::copy_n(sa.begin(), 1, citer);
	siter =          std::copy_n(sa.begin(), 1, siter);
	QCOMPARE(cc, expected2);
	QCOMPARE(sc, expected2);
	
	citer = constexprStd::copy_n(&ba[0], 2, citer);
	siter =          std::copy_n(&ba[0], 2, siter);
	QCOMPARE(cc, expected3);
	QCOMPARE(sc, expected3);
	return;
}

void TestConstexprStd::testCopyBackward(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			auto middle = constexprStd::next(c.begin(), 5);
			constexprStd::copy_backward(c.begin(), middle, c.end());
			return c;
		};
	
	static_assert(l() == TestContainer{1, 2, 3, 4, 5, 1, 2, 3, 4, 5});
	
	TestContainer c, s;
	auto citer = c.begin();
	auto siter = s.begin();
	const auto cend = std::prev(c.end());
	const auto send = std::prev(s.end());
	
	for ( ; citer != cend && siter != send; ++citer, ++siter ) {
		constexprStd::copy_backward(citer, cend, c.end());
		         std::copy_backward(siter, send, s.end());
		QVERIFY(c == s);
	} //for ( ; citer != cend && siter != send; ++citer, ++siter )
	QVERIFY(std::all_of(c.begin(), c.end(), constexprStd::details::cmp::EqualToValue{1}));
	return;
}

void TestConstexprStd::testMove(void) const noexcept {
	//Test the acutal constexprness
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			std::array a{42, 66, 0x185, 1337};
			constexprStd::move(a, c.begin());
			return c;
		};
	static_assert(l() == TestContainer{std::array{42, 66, 0x185, 1337, 5, 6, 7, 8, 9, 10}});
	
	//Test runtime behavior and compare against std::copy.
	std::array<std::string, 5> ca;
	std::array<std::string, 5> sa;
	std::array<std::string, 2> a1{longString, "We have to use long strings"};
	std::array<std::string, 2> a2{longString, "We have to use long strings"};
	std::string ba1[2] = {"to actually see the move", "this is a test"};
	std::string ba2[2] = {"to actually see the move", "this is a test"};
	const std::array<std::string, 5> expected1;
	const std::array<std::string, 5> expected2{longString, "We have to use long strings"};
	const std::array<std::string, 5> expected3{longString, "We have to use long strings", "to actually see the move", "this is a test"};
	auto citer = ca.begin();
	auto siter = sa.begin();
	
	auto isEmpty = [](const std::string& s) noexcept { return s.empty(); };
	
	QCOMPARE(ca, expected1);
	QCOMPARE(sa, expected1);
	
	citer = constexprStd::move(a1.begin(), a1.end(), citer);
	siter =          std::move(a2.begin(), a2.end(), siter);
	QCOMPARE(ca, expected2);
	QCOMPARE(sa, expected2);
	QVERIFY(std::all_of(a1.begin(), a1.end(), isEmpty));
	QVERIFY(std::all_of(a2.begin(), a2.end(), isEmpty));
	
	citer = constexprStd::move(&ba1[0], &ba1[2], citer);
	siter =          std::move(&ba2[0], &ba2[2], siter);
	QCOMPARE(ca, expected3);
	QCOMPARE(sa, expected3);
	QVERIFY(std::all_of(std::begin(ba1), std::end(ba1), isEmpty));
	QVERIFY(std::all_of(std::begin(ba2), std::end(ba2), isEmpty));
	
	std::set<std::string> cfs(ca.begin(), ca.end()), cts;
	std::set<std::string> sfs(ca.begin(), ca.end()), sts;
	
	constexprStd::move(cfs.begin(), cfs.end(), std::inserter(cts, cts.end()));
	         std::move(sfs.begin(), sfs.end(), std::inserter(sts, sts.end()));
	QVERIFY(!std::all_of(std::begin(cfs), std::end(cfs), isEmpty));
	QVERIFY(!std::all_of(std::begin(sfs), std::end(sfs), isEmpty));
	QVERIFY(std::is_permutation(cts.begin(), cts.end(), ca.begin(), ca.end()));
	QVERIFY(std::is_permutation(sts.begin(), sts.end(), sa.begin(), sa.end()));
	return;
}

void TestConstexprStd::testMoveBackward(void) const noexcept {
	std::array<std::string, 5> c{fooString, barString, longString};
	std::array<std::string, 5> s{c};
	const std::array<std::string, 5> expected{emptyString, emptyString, fooString, barString, longString};
	
	constexprStd::move_backward(c.begin(), std::next(c.begin(), 3), c.end());
	         std::move_backward(s.begin(), std::next(s.begin(), 3), s.end());
	
	QCOMPARE(c, expected);
	QCOMPARE(s, expected);
	return;
}

void TestConstexprStd::testFill(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::fill(c, 17);
			return c;
		};
	static_assert(l() == TestContainer{17, 17, 17, 17, 17, 17, 17, 17, 17, 17});
	
	int c[50];
	int s[50];
	
	constexprStd::fill(std::begin(c), std::end(c), 22);
	         std::fill(std::begin(s), std::end(s), 22);
	
	constexprStd::details::cmp::EqualToValue is22{22};
	QVERIFY(std::all_of(std::begin(c), std::end(c), is22));
	QVERIFY(std::all_of(std::begin(s), std::end(s), is22));
	return;
}

void TestConstexprStd::testFillN(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::fill_n(c.begin(), 4, 17);
			return c;
		};
	static_assert(l() == TestContainer{17, 17, 17, 17, 5, 6, 7, 8, 9, 10});
	
	int c[50];
	int s[50];
	
	constexprStd::fill(std::begin(c), std::end(c), 22);
	         std::fill(std::begin(s), std::end(s), 22);
	
	constexprStd::details::cmp::EqualToValue is22{22};
	QVERIFY(std::all_of(std::begin(c), std::end(c), is22));
	QVERIFY(std::all_of(std::begin(s), std::end(s), is22));
	
	constexprStd::fill_n(std::begin(c), 23, 88);
	         std::fill_n(std::begin(s), 23, 88);
	
	QCOMPARE(std::count(std::begin(c), std::end(c), 88), 23);
	QCOMPARE(std::count(std::begin(s), std::end(s), 88), 23);
	return;
}

void TestConstexprStd::testTransform(void) const noexcept {
	auto unary = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::transform(c, c.begin(), [](const int i) constexpr noexcept { return i * 5; });
			return c;
		};
	static_assert(unary() == TestContainer{5, 10, 15, 20, 25, 30, 35, 40, 45, 50});
	
	auto binary = [unary](void) constexpr noexcept {
			TestContainer c1{unary()};
			TestContainer c2{17, 29, 30, 23, 3, 99, 35, 65, 98, 100};
			TestContainer ret;
			auto l = [](const int i, const int j) {
					if ( isOdd(i) && isOdd(j) ) {
						return i + j;
					} //if ( isOdd(i) && isOdd(j) )
					return 0;
				};
			constexprStd::transform(c1, c2.begin(), ret.begin(), l);
			return ret;
		};
	static_assert(binary() == TestContainer{22, 0, 0, 0, 28, 0, 70, 0, 0, 0});
	
	auto countVocals = [count = 0](const std::string& s) mutable noexcept {
			return count += static_cast<int>(std::count_if(s.begin(), s.end(), isVocal));
		};
	
	std::array<std::string, 5> strings{fooString, barString, longString, emptyString, "abcdefg"};
	std::vector<int> c, s;
	
	         std::transform(strings.begin(), strings.end(), std::back_inserter(s), countVocals);
	constexprStd::transform(strings.begin(), strings.end(), std::back_inserter(c), countVocals);
	
	QCOMPARE(s.size(), 5u);
	QCOMPARE(s, (std::vector{2, 3, 18, 18, 20}));
	QVERIFY(c == s);
	
	const char chars[] = "abcde";
	std::array<std::string, 5> cs, ss;
	
	auto gen = [](const int count, const char ch) noexcept {
			return std::string(static_cast<std::string::size_type>(count), ch);
		};
	
	         std::transform(c.begin(), c.end(), std::begin(chars), ss.begin(), gen);
	constexprStd::transform(c.begin(), c.end(), std::begin(chars), cs.begin(), gen);
	
	QCOMPARE(ss, (std::array<std::string, 5>{"aa", "bbb", "cccccccccccccccccc", "dddddddddddddddddd",
	                                         "eeeeeeeeeeeeeeeeeeee"}));
	QVERIFY(cs == ss);
	return;
}

void TestConstexprStd::testGenerate(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::generate(c, [v = 10](void) mutable noexcept { return v--; });
			return c;
		};
	static_assert(l() == TestContainer{10, 9, 8, 7, 6, 5, 4, 3, 2, 1});
	
	std::array<int, 20> c{}, s{};
	
	auto makeFib = [i = 0](void) mutable { return fib(i++); };
	
	         std::generate(s.begin(), s.end(), makeFib);
	constexprStd::generate(c.begin(), c.end(), makeFib);
	
	QVERIFY(std::all_of(s.begin(), s.end(), isFib));
	QVERIFY(s == c);
	return;
}

void TestConstexprStd::testGenerateN(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			std::array<int, 10> a{};
			auto g = [i = 0](void) mutable constexpr noexcept {
					return ++i;
				};
			constexprStd::generate_n(a.begin(), 10, g);
			return a;
		};
	static_assert(TestContainer{l()} == TestContainer{});
	
	std::vector<int> cv;
	std::vector<int> sv;
	
	auto citer = std::back_inserter(cv);
	auto siter = std::back_inserter(sv);
	for ( int i = 0; i < 10; ++i ) {
		auto il = [i](void) noexcept { return i; };
		citer = constexprStd::generate_n(citer, 1, il);
		siter =          std::generate_n(siter, 1, il);
	} //for ( int i = 0; i < 10; ++i )
	
	auto cl = [i = 0](void) mutable constexpr noexcept { return fib(i); };
	auto sl = [i = 0](void) mutable constexpr noexcept { return fib(i); };
	
	constexprStd::generate_n(citer, 1, cl);
	         std::generate_n(siter, 1, sl);
	QVERIFY(sv == cv);
	return;
}

void TestConstexprStd::testRemoveIf(void) const noexcept {
	constexpr TestContainer intExpected{4, 6, 8, 10, 6, 7, 8, 9, 10, 10};
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::remove_if(c, isOdd);
			constexprStd::remove(c, 2);
			return c;
		};
	static_assert(l() == intExpected);
	
	TestContainer s;
	std::remove_if(s.begin(), s.end(), isOdd);
	std::remove(s.begin(), s.end(), 2);
	QVERIFY(s == intExpected);
	
	std::array<std::string, 6> ca{bazString, emptyString, fooString, barString, longString};
	std::array<std::string, 6> sa{bazString, emptyString, fooString, barString, longString};
	const std::array<std::string, 6> stringExpected{emptyString, fooString, longString, emptyString, emptyString};
	
	auto check = [](const std::string& str) noexcept { return !str.empty() && str[0] == 'b'; };
	
	const auto siter =          std::remove_if(sa.begin(), sa.end(), check);
	const auto citer = constexprStd::remove_if(ca.begin(), ca.end(), check);
	
	QVERIFY(sa == stringExpected);
	QVERIFY(ca == stringExpected);
	QVERIFY(*siter == *citer);
	QVERIFY(std::distance(sa.begin(), siter) == std::distance(ca.begin(), citer));
	return;
}

void TestConstexprStd::testRemoveCopyIf(void) const noexcept {
	constexpr TestContainer intExpected{2, 4, 6, 8, 10};
	constexpr TestContainer c;
	
	constexpr auto l = [c](void) constexpr noexcept {
			std::array<int, 5> a{};
			constexprStd::remove_copy_if(c, a.begin(), isOdd);
			return TestContainer{a};
		};
	static_assert(l() == intExpected);
	
	std::array<int, 5> s;
	std::remove_copy_if(c.begin(), c.end(), s.begin(), isOdd);
	QVERIFY(TestContainer{s} == intExpected);
	
	const std::array<std::string, 10> ca{fooString, fooString, bazString, emptyString, fooString, barString, fooString, fooString, longString, fooString};
	const std::array<std::string, 10> sa{fooString, fooString, bazString, emptyString, fooString, barString, fooString, fooString, longString, fooString};
	const std::vector<std::string> stringExpected1{bazString, emptyString, barString, longString};
	const std::vector<std::string> stringExpected2{emptyString, longString};
	
	std::vector<std::string> c1, c2, s1, s2;
	
	auto check = [](const std::string& str) noexcept { return !str.empty() && str[0] == 'b'; };
	
	         std::remove_copy(sa.begin(), sa.end(), std::back_inserter(s1), fooString);
	constexprStd::remove_copy(ca.begin(), ca.end(), std::back_inserter(c1), fooString);
	
	QVERIFY(c1 == stringExpected1);
	QVERIFY(s1 == stringExpected1);
	
	         std::remove_copy_if(s1.begin(), s1.end(), std::back_inserter(s2), check);
	constexprStd::remove_copy_if(c1.begin(), c1.end(), std::back_inserter(c2), check);
	
	QVERIFY(s2 == stringExpected2);
	QVERIFY(c2 == stringExpected2);
	return;
}

void TestConstexprStd::testReplaceIf(void) const noexcept {
	constexpr TestContainer intExpected{5, 9, 5, 4, 5, 6, 5, 8, 5, 10};
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::replace_if(c, isOdd, 5);
			constexprStd::replace(c, 2, 9);
			return c;
		};
	static_assert(l() == intExpected);
	
	TestContainer s;
	std::replace_if(s.begin(), s.end(), isOdd, 5);
	std::replace(s.begin(), s.end(), 2, 9);
	QVERIFY(s == intExpected);
	
	std::array<std::string, 5> ca{bazString, emptyString, fooString, barString, longString};
	std::array<std::string, 5> sa{bazString, emptyString, fooString, barString, longString};
	const std::array<std::string, 5> stringExpected{fooString, barString, fooString, fooString, longString};
	
	auto check = [](const std::string& str) noexcept { return !str.empty() && str[0] == 'b'; };
	
	         std::replace_if(sa.begin(), sa.end(), check, fooString);
	constexprStd::replace_if(ca.begin(), ca.end(), check, fooString);
	
	         std::replace(sa.begin(), sa.end(), emptyString, barString);
	constexprStd::replace(ca.begin(), ca.end(), emptyString, barString);
	
	QVERIFY(sa == stringExpected);
	QVERIFY(ca == stringExpected);
	return;
}

void TestConstexprStd::testReplaceCopyIf(void) const noexcept {
	constexpr TestContainer intExpected1{5, 2, 5, 4, 5, 6, 5, 8, 5, 10};
	constexpr TestContainer intExpected2{1, 9, 3, 4, 5, 6, 7, 8, 9, 10};
	constexpr TestContainer c;
	
	constexpr auto l1 = [c](void) constexpr noexcept {
			std::array<int, 10> a{};
			constexprStd::replace_copy_if(c, a.begin(), isOdd, 5);
			return TestContainer{a};
		};
	static_assert(l1() == intExpected1);
	
	constexpr auto l2 = [c](void) constexpr noexcept {
			std::array<int, 10> a{};
			constexprStd::replace_copy(c, a.begin(), 2, 9);
			return TestContainer{a};
		};
	static_assert(l2() == intExpected2);
	
	std::array<int, 10> s;
	std::replace_copy_if(c.begin(), c.end(), s.begin(), isOdd, 5);
	QVERIFY(TestContainer{s} == intExpected1);
	
	std::replace_copy(c.begin(), c.end(), s.begin(), 2, 9);
	QVERIFY(TestContainer{s} == intExpected2);
	
	std::array<std::string, 5> src{bazString, emptyString, fooString, barString, longString};
	std::array<std::string, 5> ca{};
	std::array<std::string, 5> sa{};
	const std::array<std::string, 5> stringExpected1{bazString, fooString, fooString, barString, longString};
	const std::array<std::string, 5> stringExpected2{fooString, emptyString, fooString, fooString, longString};
	
	auto check = [](const std::string& str) noexcept { return !str.empty() && str[0] == 'b'; };
	
	         std::replace_copy(src.begin(), src.end(), sa.begin(), emptyString, fooString);
	constexprStd::replace_copy(src.begin(), src.end(), ca.begin(), emptyString, fooString);
	
	QVERIFY(sa == stringExpected1);
	QVERIFY(ca == stringExpected1);
	
	         std::replace_copy_if(src.begin(), src.end(), sa.begin(), check, fooString);
	constexprStd::replace_copy_if(src.begin(), src.end(), ca.begin(), check, fooString);
	
	QVERIFY(sa == stringExpected2);
	QVERIFY(ca == stringExpected2);
	return;
}

void TestConstexprStd::testSwapRanges(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			std::array<int, 10> a{};
			constexprStd::swap_ranges(c, a.begin());
			return c;
		};
	static_assert(l() == TestContainer{0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
	return;
}

void TestConstexprStd::testReverse(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::reverse(c);
			return c;
		};
	static_assert(l() == TestContainer{10, 9, 8, 7, 6, 5, 4, 3, 2, 1});
	
	std::array<int, 3> s{1, 2, 3}, c{1, 2, 3};
	
	         std::reverse(s.end(), s.end());
	constexprStd::reverse(c.end(), c.end());
	
	QCOMPARE(s, (std::array{1, 2, 3}));
	QVERIFY(c == s);
	
	         std::reverse(s.begin(), s.end());
	constexprStd::reverse(c.begin(), c.end());
	
	QCOMPARE(s, (std::array{3, 2, 1}));
	QVERIFY(c == s);
	return;
}

void TestConstexprStd::testReverseCopy(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c, ret{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
			constexprStd::reverse_copy(c, ret.begin());
			return ret;
		};
	static_assert(l() == TestContainer{10, 9, 8, 7, 6, 5, 4, 3, 2, 1});
	
	const std::array<int, 3> from{1, 2, 3};
	std::array<int, 3> s{0, 0, 0}, c{0, 0, 0};
	
	         std::reverse_copy(from.begin(), from.end(), s.begin());
	constexprStd::reverse_copy(from.begin(), from.end(), c.begin());
	
	QCOMPARE(s, (std::array{3, 2, 1}));
	QVERIFY(c == s);
	return;
}

void TestConstexprStd::testRotate(void) const noexcept {
	constexpr std::array<int, 10> expected1{2, 3, 4, 5, 6, 7, 8, 9, 10, 1};
	constexpr std::array<int, 10> expected2{5, 6, 7, 8, 9, 10, 1, 2, 3, 4};
	
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::rotate(c.begin(), constexprStd::next(c.begin()), c.end());
			return c;
		};
	
	static_assert(l() == TestContainer{expected1});
	
	TestContainer s;
	TestContainer c{l()};
	
	std::rotate(s.begin(), std::next(s.begin()), s.end());
	QVERIFY(s == expected1);
	
	auto siter =          std::rotate(s.begin(), std::next(s.begin(), 3), s.end());
	auto citer = constexprStd::rotate(c.begin(), std::next(c.begin(), 3), c.end());
	QVERIFY(s == expected2);
	QVERIFY(c == expected2);
	
	QVERIFY(std::distance(s.begin(), siter) == std::distance(c.begin(), citer));
	QVERIFY(*siter == *citer);
	
	for ( int i = 0; i <= 10; ++i ) {
		         std::rotate(s.begin(), std::next(s.begin(), i), s.end());
		constexprStd::rotate(c.begin(), std::next(c.begin(), i), c.end());
		
		if ( c != s ) {
			QCOMPARE(i, -1);
		} //if ( c != s )
	} //for ( int i = 0; i <= 10; ++i )
	
	std::forward_list<int> sl{1, 2, 3, 4, 5, 6};
	std::forward_list<int> cl{1, 2, 3, 4, 5, 6};
	
	         std::rotate(sl.begin(), std::next(sl.begin(), 1), sl.end());
	constexprStd::rotate(cl.begin(), std::next(cl.begin(), 1), cl.end());
	
	         std::rotate(sl.begin(), std::next(sl.begin(), 2), sl.end());
	constexprStd::rotate(cl.begin(), std::next(cl.begin(), 2), cl.end());
	
	QVERIFY((sl == std::forward_list{4, 5, 6, 1, 2, 3}));
	QVERIFY(sl == cl);
	return;
}

void TestConstexprStd::testRotateCopy(void) const noexcept {
	constexpr std::array<int, 10> expected{5, 6, 7, 8, 9, 10, 1, 2, 3, 4};
	
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			std::array<int, 10> a{};
			constexprStd::rotate_copy(c.begin(), constexprStd::next(c.begin(), 4), c.end(), a.begin());
			return a;
		};
	
	static_assert(TestContainer{l()} == TestContainer{expected});
	
	TestContainer s;
	std::array<int, 10> a{};
	
	std::rotate_copy(s.begin(), std::next(s.begin(), 4), s.end(), a.begin());
	QVERIFY(a == expected);
	return;
}

void TestConstexprStd::testUnique(void) const noexcept {
	constexpr std::array<int, 16> from     {1, 3, 1, 1, 4, 3, 4, 4, 5, 7, 7, 5, 2, 2, 9, 4};
	constexpr std::array<int, 12> expected1{1, 3, 1, 4, 3, 4, 5, 7, 5, 2, 9, 4};
	constexpr std::array<int, 10> expected2{1, 4, 3, 4, 4, 5, 2, 2, 9, 4};
	
	constexpr auto l1 = [from,expected1](void) constexpr noexcept {
			auto copy{from};
			auto iter = constexprStd::unique(copy);
			return constexprStd::equal(copy.begin(), iter, expected1.begin(), expected1.end());
		};
	
	constexpr auto l2 = [from,expected2](void) constexpr noexcept {
			auto copy{from};
			auto iter = constexprStd::unique(copy, bothOdd);
			return constexprStd::equal(copy.begin(), iter, expected2.begin(), expected2.end());
		};
	
	static_assert(l1());
	static_assert(l2());
	
	auto copy1{from};
	auto iter1 = std::unique(copy1.begin(), copy1.end());
	QVERIFY(std::equal(copy1.begin(), iter1, expected1.begin(), expected1.end()));
	
	auto copy2{from};
	auto iter2 = std::unique(copy2.begin(), copy2.end(), bothOdd);
	QVERIFY(std::equal(copy2.begin(), iter2, expected2.begin(), expected2.end()));
	return;
}

void TestConstexprStd::testUniqueCopy(void) const noexcept {
	//Case 1, from is forward iterator
	constexpr std::array<int, 16> from     {1, 3, 1, 1, 4, 3, 4, 4, 5, 7, 7, 5, 2, 2, 9, 4};
	constexpr std::array<int, 16> expected1{1, 3, 1, 4, 3, 4, 5, 7, 5, 2, 9, 4, 0, 0, 0, 0};
	constexpr std::array<int, 16> expected2{1, 4, 3, 4, 4, 5, 2, 2, 9, 4, 0, 0, 0, 0, 0, 0};
	
	constexpr auto l1 = [from](void) constexpr noexcept {
			std::decay_t<decltype(from)> copy{};
			auto iter = constexprStd::unique_copy(from, copy.begin());
			if ( *iter != 0 ) {
				*iter = -1;
			} //if ( *iter != 0 )
			return copy;
		};
	
	constexpr auto l2 = [from](void) constexpr noexcept {
			std::decay_t<decltype(from)> copy{};
			auto iter = constexprStd::unique_copy(from, copy.begin(), bothOdd);
			if ( *iter != 0 ) {
				*iter = -1;
			} //if ( *iter != 0 )
			return copy;
		};
	
	std::decay_t<decltype(from)> s1{}, s2{};
	constexpr auto c1{l1()};
	constexpr auto c2{l2()};
	
	std::unique_copy(from.begin(), from.end(), s1.begin());
	std::unique_copy(from.begin(), from.end(), s2.begin(), bothOdd);
	
	QVERIFY(s1 == expected1);
	QVERIFY(s2 == expected2);
	QVERIFY(c1 == expected1);
	QVERIFY(c2 == expected2);
	
	//Case 2, from is input, but to is forward
	constexpr auto string = "Foo,  Bar,    Baaazzz0rr";
	std::istringstream stream;
	constexpr std::array<char, 24> streamExpected1{"Fo, Bar, Baz0r"};
	constexpr std::array<char, 24> streamExpected2{"Fo,  Bar,    Bazzz0rr"};
	
	std::array<char, 24> ss1{}, ss2{}, cs1{}, cs2{};
	
	using streamIter = std::istreambuf_iterator<char>;
	
	stream.str(string);
	         std::unique_copy(streamIter{stream}, streamIter{}, ss1.begin());
	stream.str(string);
	constexprStd::unique_copy(streamIter{stream}, streamIter{}, cs1.begin());
	stream.str(string);
	         std::unique_copy(streamIter{stream}, streamIter{}, ss2.begin(), bothVocal);
	stream.str(string);
	constexprStd::unique_copy(streamIter{stream}, streamIter{}, cs2.begin(), bothVocal);
	
	QVERIFY(ss1 == streamExpected1);
	QVERIFY(ss2 == streamExpected2);
	QVERIFY(cs1 == streamExpected1);
	QVERIFY(cs2 == streamExpected2);
	
	//Case 3, from is input, and to is output
	std::vector<char> sv1, sv2, cv1, cv2;
	
	stream.str(string);
	         std::unique_copy(streamIter{stream}, streamIter{}, std::back_inserter(sv1));
	stream.str(string);
	constexprStd::unique_copy(streamIter{stream}, streamIter{}, std::back_inserter(cv1));
	stream.str(string);
	         std::unique_copy(streamIter{stream}, streamIter{}, std::back_inserter(sv2), bothVocal);
	stream.str(string);
	constexprStd::unique_copy(streamIter{stream}, streamIter{}, std::back_inserter(cv2), bothVocal);
	
	QVERIFY(std::equal(sv1.begin(), sv1.end(), streamExpected1.begin()));
	QVERIFY(std::equal(sv2.begin(), sv2.end(), streamExpected2.begin()));
	QVERIFY(sv1 == cv1);
	QVERIFY(sv2 == cv2);
	return;
}

void TestConstexprStd::testIsPartioned(void) const noexcept {
	constexpr TestContainer c1, c2{1, 3, 5, 7, 9, 2, 4, 6, 8, 10}, c3{2, 4, 6, 8, 10, 1, 3, 5, 7, 9};
	static_assert(!constexprStd::is_partitioned(c1, isOdd));
	static_assert( constexprStd::is_partitioned(c2, isOdd));
	static_assert(!constexprStd::is_partitioned(c3, isOdd));
	
	QVERIFY(!std::is_partitioned(c1.begin(), c1.end(), isOdd));
	QVERIFY( std::is_partitioned(c2.begin(), c2.end(), isOdd));
	QVERIFY(!std::is_partitioned(c3.begin(), c3.end(), isOdd));
	return;
}

void TestConstexprStd::testPartition(void) const noexcept {
	constexpr TestContainer f1, f2{1, 3, 5, 7, 9, 2, 4, 6, 8, 10}, f3{2, 4, 6, 8, 10, 1, 3, 5, 7, 9};
	
	TestContainer s1{f1}, s2{f2}, s3{f3};
	TestContainer c1{f1}, c2{f2}, c3{f3};
	
	constexpr TestContainer cc{[f1](void) constexpr noexcept {
			TestContainer copy{f1};
			constexprStd::partition(copy, isOdd);
			return copy;
		}()};
	
	QVERIFY(std::is_partitioned(cc.begin(), cc.end(), isOdd));
	
	int count = 0;
	auto pred = [&count](const int x) noexcept { ++count; return isOdd(x); };
	
	auto siter1 = std::partition(s1.begin(), s1.end(), pred);
	QVERIFY(std::is_partitioned(s1.begin(), s1.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	auto siter2 = std::partition(s2.begin(), s2.end(), pred);
	QVERIFY(std::is_partitioned(s2.begin(), s2.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	auto siter3 = std::partition(s3.begin(), s3.end(), pred);
	QVERIFY(std::is_partitioned(s3.begin(), s3.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	auto citer1 = constexprStd::partition(c1.begin(), c1.end(), pred);
	QVERIFY(std::is_partitioned(c1.begin(), c1.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	auto citer2 = constexprStd::partition(c2.begin(), c2.end(), pred);
	QVERIFY(std::is_partitioned(c2.begin(), c2.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	auto citer3 = constexprStd::partition(c3.begin(), c3.end(), pred);
	QVERIFY(std::is_partitioned(c3.begin(), c3.end(), isOdd));
	QCOMPARE(count, 10);
	
	QVERIFY(*siter1 == *citer1);
	QVERIFY(*siter2 == *citer2);
	QVERIFY(*siter3 == *citer3);
	
	std::forward_list<int> sl1, sl2, sl3, cl1, cl2, cl3;
	std::copy(f1.rbegin(), f1.rend(), std::front_inserter(sl1));
	std::copy(f2.rbegin(), f2.rend(), std::front_inserter(sl2));
	std::copy(f3.rbegin(), f3.rend(), std::front_inserter(sl3));
	std::copy(f1.rbegin(), f1.rend(), std::front_inserter(cl1));
	std::copy(f2.rbegin(), f2.rend(), std::front_inserter(cl2));
	std::copy(f3.rbegin(), f3.rend(), std::front_inserter(cl3));
	
	count = 0;
	std::partition(sl1.begin(), sl1.end(), pred);
	QVERIFY(std::is_partitioned(sl1.begin(), sl1.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	std::partition(sl2.begin(), sl2.end(), pred);
	QVERIFY(std::is_partitioned(sl2.begin(), sl2.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	std::partition(sl3.begin(), sl3.end(), pred);
	QVERIFY(std::is_partitioned(sl3.begin(), sl3.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	constexprStd::partition(cl1.begin(), cl1.end(), pred);
	QVERIFY(std::is_partitioned(cl1.begin(), cl1.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	constexprStd::partition(cl2.begin(), cl2.end(), pred);
	QVERIFY(std::is_partitioned(cl2.begin(), cl2.end(), isOdd));
	QCOMPARE(count, 10);
	
	count = 0;
	constexprStd::partition(cl3.begin(), cl3.end(), pred);
	QVERIFY(std::is_partitioned(cl3.begin(), cl3.end(), isOdd));
	QCOMPARE(count, 10);
	return;
}

void TestConstexprStd::testPartitionCopy(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			std::array<int, 5> odd{}, even{};
			constexprStd::partition_copy(c, odd.begin(), even.begin(), isOdd);
			return std::pair{odd, even};
		};
	constexpr std::array expectedOdd{1, 3, 5, 7, 9}, expectedEven{2, 4, 6, 8, 10};
	constexpr TestContainer tc{};
	constexpr auto c{l()};
	QVERIFY(c.first  == expectedOdd);
	QVERIFY(c.second == expectedEven);
	
	std::array<int, 5> even{}, odd{};
	std::partition_copy(tc.begin(), tc.end(), odd.begin(), even.begin(), isOdd);
	QVERIFY(odd  == expectedOdd);
	QVERIFY(even == expectedEven);
	return;
}

void TestConstexprStd::testStablePartition(void) const noexcept {
	constexpr TestContainer f1, f2{1, 3, 5, 7, 9, 2, 4, 6, 8, 10}, f3{2, 4, 6, 8, 10, 1, 3, 5, 7, 9};
	constexpr TestContainer expected{f2};
	
	TestContainer s1{f1}, s2{f2}, s3{f3};
	TestContainer c1{f1}, c2{f2}, c3{f3};
	
	constexpr TestContainer cc{[f1](void) constexpr noexcept {
			TestContainer copy{f1};
			constexprStd::stable_partition(copy, isOdd);
			return copy;
		}()};
	
	QVERIFY(cc == expected);
	
	int count = 0;
	auto pred = [&count](const int x) noexcept { ++count; return isOdd(x); };
	
	auto siter1 = std::stable_partition(s1.begin(), s1.end(), pred);
	QVERIFY(s1 == expected);
	QCOMPARE(count, 10);
	
	count = 0;
	auto siter2 = std::stable_partition(s2.begin(), s2.end(), pred);
	QVERIFY(s2 == expected);
	QCOMPARE(count, 10);
	
	count = 0;
	auto siter3 = std::stable_partition(s3.begin(), s3.end(), pred);
	QVERIFY(s3 == expected);
	QCOMPARE(count, 10);
	
	count = 0;
	auto citer1 = constexprStd::stable_partition(c1.begin(), c1.end(), pred);
	QVERIFY(c1 == expected);
	QCOMPARE(count, 10);
	
	count = 0;
	auto citer2 = constexprStd::stable_partition(c2.begin(), c2.end(), pred);
	QVERIFY(c2 == expected);
	QCOMPARE(count, 10);
	
	count = 0;
	auto citer3 = constexprStd::stable_partition(c3.begin(), c3.end(), pred);
	QVERIFY(c3 == expected);
	QCOMPARE(count, 10);
	
	QVERIFY(*siter1 == *citer1);
	QVERIFY(*siter2 == *citer2);
	QVERIFY(*siter3 == *citer3);
	return;
}

void TestConstexprStd::testPartitionPoint(void) const noexcept {
	constexpr TestContainer c{1, 3, 5, 7, 9, 2, 4, 6, 8, 10};
	static_assert(*constexprStd::partition_point(c, isOdd) == 2);
	QCOMPARE(*std::partition_point(c.begin(), c.end(), isOdd), 2);
	
	std::forward_list<int> l{1, 3, 5, 7, 9, 2, 4, 6, 8, 10};
	QCOMPARE(*constexprStd::partition_point(l.begin(), l.end(), isOdd), 2);
	QCOMPARE(*         std::partition_point(l.begin(), l.end(), isOdd), 2);
	return;
}

void TestConstexprStd::testIsSortedUntil(void) const noexcept {
	constexpr TestContainer c1, c2{1, 2, 3, 4, 6, 5, 7, 8, 9, 10}, c3{2, 1, 4, 5}, c4{1, 1, 1, 2, 4, 3};
	
	static_assert( constexprStd::is_sorted_until(c1) == c1.end());
	static_assert(*constexprStd::is_sorted_until(c2) == 5);
	static_assert(*constexprStd::is_sorted_until(c3) == 1);
	static_assert(*constexprStd::is_sorted_until(c4) == 3);
	
	QCOMPARE( std::is_sorted_until(c1.begin(), c1.end()), c1.end());
	QCOMPARE(*std::is_sorted_until(c2.begin(), c2.end()), 5);
	QCOMPARE(*std::is_sorted_until(c3.begin(), c3.end()), 1);
	QCOMPARE(*std::is_sorted_until(c4.begin(), c4.end()), 3);
	return;
}

void TestConstexprStd::testIsSorted(void) const noexcept {
	constexpr TestContainer c1, c2{1, 2, 3, 4, 6, 5, 7, 8, 9, 10}, c3{2, 1, 4, 5}, c4{1, 1, 1, 2, 4, 3};
	
	static_assert( constexprStd::is_sorted(c1));
	static_assert(!constexprStd::is_sorted(c2));
	static_assert(!constexprStd::is_sorted(c3));
	static_assert(!constexprStd::is_sorted(c4));
	
	QVERIFY( std::is_sorted(c1.begin(), c1.end()));
	QVERIFY(!std::is_sorted(c2.begin(), c2.end()));
	QVERIFY(!std::is_sorted(c3.begin(), c3.end()));
	QVERIFY(!std::is_sorted(c4.begin(), c4.end()));
	return;
}

void TestConstexprStd::testIsHeapUntil(void) const noexcept {
	constexpr TestContainer cc;
	
	static_assert(constexprStd::is_heap_until(cc) == constexprStd::next(cc.begin()));
	static_assert(constexprStd::is_heap_until(cc, std::greater<>{}) == cc.end());
	
	TestContainer c{1, 10, 9, 8, 7, 6, 5, 4, 3, 2};
	
	auto first = c.begin(), last = c.end();
	
	do { //while ( std::next_permutation(first, last) )
		auto siter =          std::is_heap_until(first, last);
		auto citer = constexprStd::is_heap_until(first, last);
		
		auto sdist = std::distance(first, siter);
		auto cdist = std::distance(first, citer);
		
		QCOMPARE(cdist, sdist);
	} while ( std::next_permutation(first, last) );
	std::prev_permutation(first, last);
	
	for ( auto iter = first; iter != last; ++iter ) {
		auto siter =          std::is_heap_until(first, iter);
		auto citer = constexprStd::is_heap_until(first, iter);
		
		auto sdist = std::distance(first, siter);
		auto cdist = std::distance(first, citer);
		
		QCOMPARE(cdist, sdist);
	} //for ( auto iter = first; iter != last; ++iter )
	return;
}

void TestConstexprStd::testIsHeap(void) const noexcept {
	constexpr TestContainer c;
	static_assert(!constexprStd::is_heap(c));
	static_assert( constexprStd::is_heap(c, std::greater<>{}));
	return;
}

void TestConstexprStd::testPushHeap(void) const noexcept {
	constexpr std::array a{5, 4, 5, 2, 1, 9};
	static_assert(!constexprStd::is_heap(a));
	constexpr auto l = [a](void) constexpr noexcept {
			auto copy{a};
			constexprStd::push_heap(copy.begin(), copy.end());
			return copy;
		};
	constexpr auto b{l()};
	static_assert(constexprStd::is_heap(b));
	return;
}

void TestConstexprStd::testMakeHeap(void) const noexcept {
	constexpr TestContainer a;
	static_assert(!constexprStd::is_heap(a));
	constexpr auto l = [a](void) constexpr noexcept {
			auto copy{a};
			constexprStd::make_heap(copy);
			return copy;
		};
	constexpr auto b{l()};
	static_assert(constexprStd::is_heap(b));
	return;
}

void TestConstexprStd::testPopHeap(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			TestContainer c;
			constexprStd::pop_heap(c.begin(), c.end(), std::greater<>{});
			return *constexprStd::is_heap_until(c, std::greater<>{});
		};
	static_assert(l() == 1);
	
	std::array s{2, 9, 17, 34, 9, 90, 34, 5, 2, 0, 7, 23};
	std::make_heap(s.begin(), s.end());
	decltype(s) c{s};
	
	for ( auto sIter = s.end(), cIter = c.end(); sIter != s.begin(); --sIter, --cIter ) {
		         std::pop_heap(s.begin(), sIter);
		constexprStd::pop_heap(c.begin(), cIter);
		
		const auto sDist = std::distance(s.begin(), std::is_heap_until(s.begin(), s.end()));
		const auto cDist = std::distance(c.begin(), std::is_heap_until(c.begin(), c.end()));
		QCOMPARE(cDist, sDist);
	} //for ( auto sIter = s.end(), cIter = c.end(); sIter != s.begin(); --sIter, --cIter )
	QVERIFY(c == s);
	return;
}

void TestConstexprStd::testLexicographicalCompare(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			std::array<int,       3> a1{1, 2, 3};
			std::array<long long, 3> a2{1, 2, 3};
			std::array<long long, 4> a3{1, 2, 3, 4};
			std::array<int,       3> a4{3, 2, 3};
			std::array<int,       0> a5{};
			
			bool b1 = constexprStd::lexicographical_compare(a1, a1);
			bool b2 = constexprStd::lexicographical_compare(a1, a2);
			bool b3 = constexprStd::lexicographical_compare(a1, a3);
			bool b4 = constexprStd::lexicographical_compare(a1, a4);
			bool b5 = constexprStd::lexicographical_compare(a1, a5);
			auto t1 = std::tuple{b1, b2, b3, b4, b5};
			
			b1 = constexprStd::lexicographical_compare(a2, a1);
			b2 = constexprStd::lexicographical_compare(a2, a2);
			b3 = constexprStd::lexicographical_compare(a2, a3);
			b4 = constexprStd::lexicographical_compare(a2, a4);
			b5 = constexprStd::lexicographical_compare(a2, a5);
			auto t2 = std::tuple{b1, b2, b3, b4, b5};
			
			b1 = constexprStd::lexicographical_compare(a3, a1);
			b2 = constexprStd::lexicographical_compare(a3, a2);
			b3 = constexprStd::lexicographical_compare(a3, a3);
			b4 = constexprStd::lexicographical_compare(a3, a4);
			b5 = constexprStd::lexicographical_compare(a3, a5);
			auto t3 = std::tuple{b1, b2, b3, b4, b5};
			
			b1 = constexprStd::lexicographical_compare(a4, a1);
			b2 = constexprStd::lexicographical_compare(a4, a2);
			b3 = constexprStd::lexicographical_compare(a4, a3);
			b4 = constexprStd::lexicographical_compare(a4, a4);
			b5 = constexprStd::lexicographical_compare(a4, a5);
			auto t4 = std::tuple{b1, b2, b3, b4, b5};
			
			b1 = constexprStd::lexicographical_compare(a5, a1);
			b2 = constexprStd::lexicographical_compare(a5, a2);
			b3 = constexprStd::lexicographical_compare(a5, a3);
			b4 = constexprStd::lexicographical_compare(a5, a4);
			b5 = constexprStd::lexicographical_compare(a5, a5);
			auto t5 = std::tuple{b1, b2, b3, b4, b5};
			
			return std::tuple_cat(t1, t2, t3, t4, t5);
		};
	
	static_assert(l() == std::tuple{false, false, true,  true,  false,
	                                false, false, true,  true,  false,
	                                false, false, false, true,  false,
	                                false, false, false, false, false,
	                                true,  true,  true,  true,  false});
	
	std::string s1{fooString};
	std::string s2{barString};
	std::string s3{bazString};
	std::string s4{"foos"};
	std::string s5{emptyString};
	
	constexpr bool b11 = false;
	constexpr bool b12 = false;
	constexpr bool b13 = false;
	constexpr bool b14 = true;
	constexpr bool b15 = false;
	
	constexpr bool b21 = true;
	constexpr bool b22 = false;
	constexpr bool b23 = true;
	constexpr bool b24 = true;
	constexpr bool b25 = false;
	
	constexpr bool b31 = true;
	constexpr bool b32 = false;
	constexpr bool b33 = false;
	constexpr bool b34 = true;
	constexpr bool b35 = false;
	
	constexpr bool b41 = false;
	constexpr bool b42 = false;
	constexpr bool b43 = false;
	constexpr bool b44 = false;
	constexpr bool b45 = false;
	
	constexpr bool b51 = true;
	constexpr bool b52 = true;
	constexpr bool b53 = true;
	constexpr bool b54 = true;
	constexpr bool b55 = false;
	
	QCOMPARE(         std::lexicographical_compare(s1.begin(), s1.end(), s1.begin(), s1.end()), b11);
	QCOMPARE(         std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end()), b12);
	QCOMPARE(         std::lexicographical_compare(s1.begin(), s1.end(), s3.begin(), s3.end()), b13);
	QCOMPARE(         std::lexicographical_compare(s1.begin(), s1.end(), s4.begin(), s4.end()), b14);
	QCOMPARE(         std::lexicographical_compare(s1.begin(), s1.end(), s5.begin(), s5.end()), b15);
	QCOMPARE(constexprStd::lexicographical_compare(s1.begin(), s1.end(), s1.begin(), s1.end()), b11);
	QCOMPARE(constexprStd::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end()), b12);
	QCOMPARE(constexprStd::lexicographical_compare(s1.begin(), s1.end(), s3.begin(), s3.end()), b13);
	QCOMPARE(constexprStd::lexicographical_compare(s1.begin(), s1.end(), s4.begin(), s4.end()), b14);
	QCOMPARE(constexprStd::lexicographical_compare(s1.begin(), s1.end(), s5.begin(), s5.end()), b15);
	
	QCOMPARE(         std::lexicographical_compare(s2.begin(), s2.end(), s1.begin(), s1.end()), b21);
	QCOMPARE(         std::lexicographical_compare(s2.begin(), s2.end(), s2.begin(), s2.end()), b22);
	QCOMPARE(         std::lexicographical_compare(s2.begin(), s2.end(), s3.begin(), s3.end()), b23);
	QCOMPARE(         std::lexicographical_compare(s2.begin(), s2.end(), s4.begin(), s4.end()), b24);
	QCOMPARE(         std::lexicographical_compare(s2.begin(), s2.end(), s5.begin(), s5.end()), b25);
	QCOMPARE(constexprStd::lexicographical_compare(s2.begin(), s2.end(), s1.begin(), s1.end()), b21);
	QCOMPARE(constexprStd::lexicographical_compare(s2.begin(), s2.end(), s2.begin(), s2.end()), b22);
	QCOMPARE(constexprStd::lexicographical_compare(s2.begin(), s2.end(), s3.begin(), s3.end()), b23);
	QCOMPARE(constexprStd::lexicographical_compare(s2.begin(), s2.end(), s4.begin(), s4.end()), b24);
	QCOMPARE(constexprStd::lexicographical_compare(s2.begin(), s2.end(), s5.begin(), s5.end()), b25);
	
	QCOMPARE(         std::lexicographical_compare(s3.begin(), s3.end(), s1.begin(), s1.end()), b31);
	QCOMPARE(         std::lexicographical_compare(s3.begin(), s3.end(), s2.begin(), s2.end()), b32);
	QCOMPARE(         std::lexicographical_compare(s3.begin(), s3.end(), s3.begin(), s3.end()), b33);
	QCOMPARE(         std::lexicographical_compare(s3.begin(), s3.end(), s4.begin(), s4.end()), b34);
	QCOMPARE(         std::lexicographical_compare(s3.begin(), s3.end(), s5.begin(), s5.end()), b35);
	QCOMPARE(constexprStd::lexicographical_compare(s3.begin(), s3.end(), s1.begin(), s1.end()), b31);
	QCOMPARE(constexprStd::lexicographical_compare(s3.begin(), s3.end(), s2.begin(), s2.end()), b32);
	QCOMPARE(constexprStd::lexicographical_compare(s3.begin(), s3.end(), s3.begin(), s3.end()), b33);
	QCOMPARE(constexprStd::lexicographical_compare(s3.begin(), s3.end(), s4.begin(), s4.end()), b34);
	QCOMPARE(constexprStd::lexicographical_compare(s3.begin(), s3.end(), s5.begin(), s5.end()), b35);
	
	QCOMPARE(         std::lexicographical_compare(s4.begin(), s4.end(), s1.begin(), s1.end()), b41);
	QCOMPARE(         std::lexicographical_compare(s4.begin(), s4.end(), s2.begin(), s2.end()), b42);
	QCOMPARE(         std::lexicographical_compare(s4.begin(), s4.end(), s3.begin(), s3.end()), b43);
	QCOMPARE(         std::lexicographical_compare(s4.begin(), s4.end(), s4.begin(), s4.end()), b44);
	QCOMPARE(         std::lexicographical_compare(s4.begin(), s4.end(), s5.begin(), s5.end()), b45);
	QCOMPARE(constexprStd::lexicographical_compare(s4.begin(), s4.end(), s1.begin(), s1.end()), b41);
	QCOMPARE(constexprStd::lexicographical_compare(s4.begin(), s4.end(), s2.begin(), s2.end()), b42);
	QCOMPARE(constexprStd::lexicographical_compare(s4.begin(), s4.end(), s3.begin(), s3.end()), b43);
	QCOMPARE(constexprStd::lexicographical_compare(s4.begin(), s4.end(), s4.begin(), s4.end()), b44);
	QCOMPARE(constexprStd::lexicographical_compare(s4.begin(), s4.end(), s5.begin(), s5.end()), b45);
	
	QCOMPARE(         std::lexicographical_compare(s5.begin(), s5.end(), s1.begin(), s1.end()), b51);
	QCOMPARE(         std::lexicographical_compare(s5.begin(), s5.end(), s2.begin(), s2.end()), b52);
	QCOMPARE(         std::lexicographical_compare(s5.begin(), s5.end(), s3.begin(), s3.end()), b53);
	QCOMPARE(         std::lexicographical_compare(s5.begin(), s5.end(), s4.begin(), s4.end()), b54);
	QCOMPARE(         std::lexicographical_compare(s5.begin(), s5.end(), s5.begin(), s5.end()), b55);
	QCOMPARE(constexprStd::lexicographical_compare(s5.begin(), s5.end(), s1.begin(), s1.end()), b51);
	QCOMPARE(constexprStd::lexicographical_compare(s5.begin(), s5.end(), s2.begin(), s2.end()), b52);
	QCOMPARE(constexprStd::lexicographical_compare(s5.begin(), s5.end(), s3.begin(), s3.end()), b53);
	QCOMPARE(constexprStd::lexicographical_compare(s5.begin(), s5.end(), s4.begin(), s4.end()), b54);
	QCOMPARE(constexprStd::lexicographical_compare(s5.begin(), s5.end(), s5.begin(), s5.end()), b55);
	return;
}

void TestConstexprStd::testIsPermutation(void) const noexcept {
	constexpr std::array sa1{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10};
	constexpr std::array sa2{ 1,  2,  3,  4,  5,  6,  7,  9,  8, 10};
	constexpr std::array sa3{10,  2,  3,  4,  5,  6,  7,  8,  9,  1};
	constexpr std::array sa4{ 5,  6,  1,  2,  3,  4,  7,  8,  9, 10};
	constexpr std::array sa5{10,  9,  8,  7,  6,  5,  4,  3,  2,  1};
	constexpr std::array sa6{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 12};
	constexpr std::array sa7{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12};
	constexpr std::array sa8{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 12, 11};
	
	auto sameParity = [](const int x, const int y) noexcept {
			return x % 2 == y % 2;
		};
	
	//Test normal variant
	//Compare 1 to X
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa1.begin(), sa1.end()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa2.begin(), sa2.end()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa3.begin(), sa3.end()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa4.begin(), sa4.end()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa5.begin(), sa5.end()));
	static_assert(!constexprStd::is_permutation(sa1.begin(), sa1.end(), sa6.begin(), sa6.end()));
	static_assert(!constexprStd::is_permutation(sa1.begin(), sa1.end(), sa7.begin(), sa7.end()));
	static_assert(!constexprStd::is_permutation(sa1.begin(), sa1.end(), sa8.begin(), sa8.end()));
	
	//Compare 2 to X as container overload
	static_assert( constexprStd::is_permutation(sa2, sa1));
	static_assert( constexprStd::is_permutation(sa2, sa2));
	static_assert( constexprStd::is_permutation(sa2, sa3));
	static_assert( constexprStd::is_permutation(sa2, sa4));
	static_assert( constexprStd::is_permutation(sa2, sa5));
	static_assert(!constexprStd::is_permutation(sa2, sa6));
	static_assert(!constexprStd::is_permutation(sa2, sa7));
	static_assert(!constexprStd::is_permutation(sa2, sa8));
	
	//Compare 6 to X
	static_assert(!constexprStd::is_permutation(sa6.begin(), sa6.end(), sa1.begin(), sa1.end()));
	static_assert(!constexprStd::is_permutation(sa6.begin(), sa6.end(), sa2.begin(), sa2.end()));
	static_assert(!constexprStd::is_permutation(sa6.begin(), sa6.end(), sa3.begin(), sa3.end()));
	static_assert(!constexprStd::is_permutation(sa6.begin(), sa6.end(), sa4.begin(), sa4.end()));
	static_assert(!constexprStd::is_permutation(sa6.begin(), sa6.end(), sa5.begin(), sa5.end()));
	static_assert( constexprStd::is_permutation(sa6.begin(), sa6.end(), sa6.begin(), sa6.end()));
	static_assert(!constexprStd::is_permutation(sa6.begin(), sa6.end(), sa7.begin(), sa7.end()));
	static_assert(!constexprStd::is_permutation(sa6.begin(), sa6.end(), sa8.begin(), sa8.end()));
	
	//Compare 7 to X
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa1.begin(), sa1.end()));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa2.begin(), sa2.end()));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa3.begin(), sa3.end()));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa4.begin(), sa4.end()));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa5.begin(), sa5.end()));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa6.begin(), sa6.end()));
	static_assert( constexprStd::is_permutation(sa7.begin(), sa7.end(), sa7.begin(), sa7.end()));
	static_assert( constexprStd::is_permutation(sa7.begin(), sa7.end(), sa8.begin(), sa8.end()));
	
	//Test predicate variant
	//Compare 1 to X
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa1.begin(), sa1.end(), sameParity));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa2.begin(), sa2.end(), sameParity));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa3.begin(), sa3.end(), sameParity));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa4.begin(), sa4.end(), sameParity));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa5.begin(), sa5.end(), sameParity));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa6.begin(), sa6.end(), sameParity));
	static_assert(!constexprStd::is_permutation(sa1.begin(), sa1.end(), sa7.begin(), sa7.end(), sameParity));
	static_assert(!constexprStd::is_permutation(sa1.begin(), sa1.end(), sa8.begin(), sa8.end(), sameParity));
	
	//Compare 2 to X as container overload
	static_assert( constexprStd::is_permutation(sa2, sa1, sameParity));
	static_assert( constexprStd::is_permutation(sa2, sa2, sameParity));
	static_assert( constexprStd::is_permutation(sa2, sa3, sameParity));
	static_assert( constexprStd::is_permutation(sa2, sa4, sameParity));
	static_assert( constexprStd::is_permutation(sa2, sa5, sameParity));
	static_assert( constexprStd::is_permutation(sa2, sa6, sameParity));
	static_assert(!constexprStd::is_permutation(sa2, sa7, sameParity));
	static_assert(!constexprStd::is_permutation(sa2, sa8, sameParity));
	
	//Compare 7 to X
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa1.begin(), sa1.end(), sameParity));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa2.begin(), sa2.end(), sameParity));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa3.begin(), sa3.end(), sameParity));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa4.begin(), sa4.end(), sameParity));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa5.begin(), sa5.end(), sameParity));
	static_assert(!constexprStd::is_permutation(sa7.begin(), sa7.end(), sa6.begin(), sa6.end(), sameParity));
	static_assert( constexprStd::is_permutation(sa7.begin(), sa7.end(), sa7.begin(), sa7.end(), sameParity));
	static_assert( constexprStd::is_permutation(sa7.begin(), sa7.end(), sa8.begin(), sa8.end(), sameParity));
	
	//Test 3 iterator variant
	//Compare 1 to X
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa1.begin()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa2.begin()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa3.begin()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa4.begin()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa5.begin()));
	static_assert(!constexprStd::is_permutation(sa1.begin(), sa1.end(), sa6.begin()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa7.begin()));
	static_assert( constexprStd::is_permutation(sa1.begin(), sa1.end(), sa8.begin()));
	
	//Compare 2 to X with Container overload
	static_assert( constexprStd::is_permutation(sa2, sa1.begin()));
	static_assert( constexprStd::is_permutation(sa2, sa2.begin()));
	static_assert( constexprStd::is_permutation(sa2, sa3.begin()));
	static_assert( constexprStd::is_permutation(sa2, sa4.begin()));
	static_assert( constexprStd::is_permutation(sa2, sa5.begin()));
	static_assert(!constexprStd::is_permutation(sa2, sa6.begin()));
	static_assert( constexprStd::is_permutation(sa2, sa7.begin()));
	static_assert( constexprStd::is_permutation(sa2, sa8.begin()));
	
	//Compare 3 to X with predicate
	static_assert( constexprStd::is_permutation(sa3.begin(), sa3.end(), sa1.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa3.begin(), sa3.end(), sa2.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa3.begin(), sa3.end(), sa3.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa3.begin(), sa3.end(), sa4.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa3.begin(), sa3.end(), sa5.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa3.begin(), sa3.end(), sa6.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa3.begin(), sa3.end(), sa7.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa3.begin(), sa3.end(), sa8.begin(), sameParity));
	
	//Compare 4 to X with container overload and predicate
	static_assert( constexprStd::is_permutation(sa4, sa1.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa4, sa2.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa4, sa3.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa4, sa4.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa4, sa5.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa4, sa6.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa4, sa7.begin(), sameParity));
	static_assert( constexprStd::is_permutation(sa4, sa8.begin(), sameParity));
	
	//And now with not random access iterators
	std::forward_list l1{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10};
	std::forward_list l2{ 1,  2,  3,  4,  5,  6,  7,  9,  8, 10};
	std::forward_list l3{10,  2,  3,  4,  5,  6,  7,  8,  9,  1};
	std::forward_list l4{ 5,  6,  1,  2,  3,  4,  7,  8,  9, 10};
	std::forward_list l5{10,  9,  8,  7,  6,  5,  4,  3,  2,  1};
	std::forward_list l6{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 12};
	std::forward_list l7{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12};
	std::forward_list l8{ 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 12, 11};
	
	//Test normal variant
	//Compare 1 to X
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l1.begin(), l1.end()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l2.begin(), l2.end()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l3.begin(), l3.end()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l4.begin(), l4.end()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l5.begin(), l5.end()));
	QVERIFY(!constexprStd::is_permutation(l1.begin(), l1.end(), l6.begin(), l6.end()));
	QVERIFY(!constexprStd::is_permutation(l1.begin(), l1.end(), l7.begin(), l7.end()));
	QVERIFY(!constexprStd::is_permutation(l1.begin(), l1.end(), l8.begin(), l8.end()));
	
	//Compare 2 to X as container overload
	QVERIFY( constexprStd::is_permutation(l2, l1));
	QVERIFY( constexprStd::is_permutation(l2, l2));
	QVERIFY( constexprStd::is_permutation(l2, l3));
	QVERIFY( constexprStd::is_permutation(l2, l4));
	QVERIFY( constexprStd::is_permutation(l2, l5));
	QVERIFY(!constexprStd::is_permutation(l2, l6));
	QVERIFY(!constexprStd::is_permutation(l2, l7));
	QVERIFY(!constexprStd::is_permutation(l2, l8));
	
	//Compare 6 to X
	QVERIFY(!constexprStd::is_permutation(l6.begin(), l6.end(), l1.begin(), l1.end()));
	QVERIFY(!constexprStd::is_permutation(l6.begin(), l6.end(), l2.begin(), l2.end()));
	QVERIFY(!constexprStd::is_permutation(l6.begin(), l6.end(), l3.begin(), l3.end()));
	QVERIFY(!constexprStd::is_permutation(l6.begin(), l6.end(), l4.begin(), l4.end()));
	QVERIFY(!constexprStd::is_permutation(l6.begin(), l6.end(), l5.begin(), l5.end()));
	QVERIFY( constexprStd::is_permutation(l6.begin(), l6.end(), l6.begin(), l6.end()));
	QVERIFY(!constexprStd::is_permutation(l6.begin(), l6.end(), l7.begin(), l7.end()));
	QVERIFY(!constexprStd::is_permutation(l6.begin(), l6.end(), l8.begin(), l8.end()));
	
	//Compare 7 to X
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l1.begin(), l1.end()));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l2.begin(), l2.end()));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l3.begin(), l3.end()));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l4.begin(), l4.end()));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l5.begin(), l5.end()));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l6.begin(), l6.end()));
	QVERIFY( constexprStd::is_permutation(l7.begin(), l7.end(), l7.begin(), l7.end()));
	QVERIFY( constexprStd::is_permutation(l7.begin(), l7.end(), l8.begin(), l8.end()));
	
	//Test predicate variant
	//Compare 1 to X
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l1.begin(), l1.end(), sameParity));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l2.begin(), l2.end(), sameParity));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l3.begin(), l3.end(), sameParity));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l4.begin(), l4.end(), sameParity));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l5.begin(), l5.end(), sameParity));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l6.begin(), l6.end(), sameParity));
	QVERIFY(!constexprStd::is_permutation(l1.begin(), l1.end(), l7.begin(), l7.end(), sameParity));
	QVERIFY(!constexprStd::is_permutation(l1.begin(), l1.end(), l8.begin(), l8.end(), sameParity));
	
	//Compare 2 to X as container overload
	QVERIFY( constexprStd::is_permutation(l2, l1, sameParity));
	QVERIFY( constexprStd::is_permutation(l2, l2, sameParity));
	QVERIFY( constexprStd::is_permutation(l2, l3, sameParity));
	QVERIFY( constexprStd::is_permutation(l2, l4, sameParity));
	QVERIFY( constexprStd::is_permutation(l2, l5, sameParity));
	QVERIFY( constexprStd::is_permutation(l2, l6, sameParity));
	QVERIFY(!constexprStd::is_permutation(l2, l7, sameParity));
	QVERIFY(!constexprStd::is_permutation(l2, l8, sameParity));
	
	//Compare 7 to X
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l1.begin(), l1.end(), sameParity));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l2.begin(), l2.end(), sameParity));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l3.begin(), l3.end(), sameParity));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l4.begin(), l4.end(), sameParity));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l5.begin(), l5.end(), sameParity));
	QVERIFY(!constexprStd::is_permutation(l7.begin(), l7.end(), l6.begin(), l6.end(), sameParity));
	QVERIFY( constexprStd::is_permutation(l7.begin(), l7.end(), l7.begin(), l7.end(), sameParity));
	QVERIFY( constexprStd::is_permutation(l7.begin(), l7.end(), l8.begin(), l8.end(), sameParity));
	
	//Test 3 iterator variant
	//Compare 1 to X
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l1.begin()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l2.begin()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l3.begin()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l4.begin()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l5.begin()));
	QVERIFY(!constexprStd::is_permutation(l1.begin(), l1.end(), l6.begin()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l7.begin()));
	QVERIFY( constexprStd::is_permutation(l1.begin(), l1.end(), l8.begin()));
	
	//Compare 2 to X with Container overload
	QVERIFY( constexprStd::is_permutation(l2, l1.begin()));
	QVERIFY( constexprStd::is_permutation(l2, l2.begin()));
	QVERIFY( constexprStd::is_permutation(l2, l3.begin()));
	QVERIFY( constexprStd::is_permutation(l2, l4.begin()));
	QVERIFY( constexprStd::is_permutation(l2, l5.begin()));
	QVERIFY(!constexprStd::is_permutation(l2, l6.begin()));
	QVERIFY( constexprStd::is_permutation(l2, l7.begin()));
	QVERIFY( constexprStd::is_permutation(l2, l8.begin()));
	
	//Compare 3 to X with predicate
	QVERIFY( constexprStd::is_permutation(l3.begin(), l3.end(), l1.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l3.begin(), l3.end(), l2.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l3.begin(), l3.end(), l3.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l3.begin(), l3.end(), l4.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l3.begin(), l3.end(), l5.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l3.begin(), l3.end(), l6.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l3.begin(), l3.end(), l7.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l3.begin(), l3.end(), l8.begin(), sameParity));
	
	//Compare 4 to X with container overload and predicate
	QVERIFY( constexprStd::is_permutation(l4, l1.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l4, l2.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l4, l3.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l4, l4.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l4, l5.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l4, l6.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l4, l7.begin(), sameParity));
	QVERIFY( constexprStd::is_permutation(l4, l8.begin(), sameParity));
	return;
}
