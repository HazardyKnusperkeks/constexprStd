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

#include <QTest>

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
