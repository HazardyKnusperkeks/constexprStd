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

#include <algorithm>
#include <array>
#include <forward_list>
#include <iterator>
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
	QVERIFY(std::all_of(std::begin(ba1), std::begin(ba1), isEmpty));
	QVERIFY(std::all_of(std::begin(ba2), std::begin(ba2), isEmpty));
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
