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

#include <QtTest>

#include <algorithm>
#include <array>
#include <forward_list>
#include <list>
#include <set>
#include <string>
#include <string_view>
#include <utility>

#include <constexprStd/algorithm>
#include <constexprStd/iterator>
#include <constexprStd/utility>

namespace QTest {
static bool qCompare(const std::string& t1, const char *t2, const char *actual, const char *expected, const char *file,
                     const int line) {
	return compare_string_helper(t1.c_str(), t2, actual, expected, file, line);
}
} //namespace QTest

class TestContainer : public std::array<int, 10> {
	public:
	constexpr TestContainer(void) noexcept : std::array<int, 10>{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}} {
		return;
	}
	
	constexpr TestContainer(std::array<int, 10> a) noexcept : std::array<int, 10>{a} {
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


class TestConstexprStd : public QObject {
	Q_OBJECT
	private slots:
	//Algorithm lib
	//Non-modifying sequence operations
	void testEqual(void) const noexcept;
	void testUnequal(void) const noexcept;
	
	//Modifying sequence operations
	void testCopy(void) const noexcept;
	
	//Iterator lib
	//Iterator adaptors
	void testInsert_iterator(void) const noexcept;
	
	//Iterator operations
	void testAdvance(void) const noexcept;
	void testDistance(void) const noexcept;
	
	//Utility lib
	//Swap, forward and move
	void testExchange(void) const noexcept;
	
	public:
	explicit TestConstexprStd(QObject *parent = nullptr) : QObject(parent) { return; }
};

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

void TestConstexprStd::testInsert_iterator(void) const noexcept {
	//Test the acutal constexprness
	auto l = [](void) constexpr {
			TestContainer c;
			auto iter = c.begin() + 5;
			auto inserter = constexprStd::inserter(c, iter);
			for ( int i = 0; i < 3; ++i ) {
				*inserter = i;
			} //for ( int i = 0; i < 3; ++i )
			return c;
		};
	static_assert(l() == TestContainer{std::array{1, 2, 3, 4, 5, 0, 1, 2, 9, 10}});
	
	//Test runtime behavior and compare against std::insert_iterator.
	std::set<int> cs{1, 4, 5}, ss{cs};
	auto cinserter = constexprStd::inserter(cs, cs.end());
	auto sinserter =          std::inserter(ss, ss.end());
	
	for ( int i = 0; i < 3; ++i ) {
		*cinserter = i;
		*sinserter = i;
	} //for ( int i = 0; i < 3; ++i )
	QCOMPARE(cs, (std::set{0, 1, 2, 4, 5}));
	QCOMPARE(ss, (std::set{0, 1, 2, 4, 5}));
	return;
}

void TestConstexprStd::testAdvance(void) const noexcept {
	//Test the acutal constexprness
	auto ran1 = [](void) constexpr {
			TestContainer c;
			auto iter = c.begin();
			constexprStd::advance(iter, 4);
			return *iter;
		};
	static_assert(ran1() == 5);
	auto ran2 = [](void) constexpr {
			TestContainer c;
			auto iter = c.end();
			constexprStd::advance(iter, -6);
			return *iter;
		};
	static_assert(ran2() == 5);
	auto bidi1 = [](void) constexpr {
			TestContainer c;
			auto iter = c.begin();
			//A little cheating
			constexprStd::details::advanceImpl(iter, 4, std::bidirectional_iterator_tag{});
			return *iter;
		};
	static_assert(bidi1() == 5);
	auto bidi2 = [](void) constexpr {
			TestContainer c;
			auto iter = c.end();
			//A little cheating
			constexprStd::details::advanceImpl(iter, -6, std::bidirectional_iterator_tag{});
			return *iter;
		};
	static_assert(bidi2() == 5);
	auto fwd = [](void) constexpr {
		TestContainer c;
		auto iter = c.begin();
		//A little cheating
		constexprStd::details::advanceImpl(iter, 4, std::input_iterator_tag{});
		return *iter;
	};
	static_assert(fwd() == 5);
	
	//Test runtime behavior and compare against std::advance.
	
	//Random Access
	TestContainer c;
	auto cciter = c.begin();
	auto sciter = c.begin();
	constexprStd::advance(cciter, 8);
	         std::advance(sciter, 8);
	QCOMPARE(*cciter, 9);
	QCOMPARE(*sciter, 9);
	
	cciter = c.end();
	sciter = c.end();
	constexprStd::advance(cciter, -2);
	         std::advance(sciter, -2);
	QCOMPARE(*cciter, 9);
	QCOMPARE(*sciter, 9);
	
	//Bidi
	std::list<int> l{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto cliter = l.begin();
	auto sliter = l.begin();
	constexprStd::advance(cliter, 8);
	         std::advance(sliter, 8);
	QCOMPARE(*cliter, 9);
	QCOMPARE(*sliter, 9);
	
	cliter = l.end();
	sliter = l.end();
	constexprStd::advance(cliter, -2);
	         std::advance(sliter, -2);
	QCOMPARE(*cliter, 9);
	QCOMPARE(*sliter, 9);
	
	//Fwd
	std::forward_list<int> f{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	auto cfiter = f.begin();
	auto sfiter = f.begin();
	constexprStd::advance(cfiter, 8);
	         std::advance(sfiter, 8);
	QCOMPARE(*cfiter, 9);
	QCOMPARE(*sfiter, 9);
	return;
}

void TestConstexprStd::testDistance(void) const noexcept {
	//Test the acutal constexprness
	auto ran1 = [](void) constexpr {
			TestContainer c;
			return constexprStd::distance(c.begin(), c.end());
		};
	static_assert(ran1() == 10);
	auto ran2 = [](void) constexpr {
			TestContainer c;
			return constexprStd::distance(c.end(), c.begin());
		};
	static_assert(ran2() == -10);
	auto fwd = [](void) constexpr {
		TestContainer c;
		//A little cheating
		return constexprStd::details::distanceImpl(c.begin(), c.end(), std::input_iterator_tag{});
	};
	static_assert(fwd() == 10);
	
	//Test runtime behavior and compare against std::distance.
	
	//Random Access
	TestContainer c;
	auto ccdist = constexprStd::distance(c.begin(), c.end());
	auto scdist =          std::distance(c.begin(), c.end());
	QCOMPARE(ccdist, 10);
	QCOMPARE(scdist, 10);
	
	ccdist = constexprStd::distance(c.end(), c.begin());
	scdist =          std::distance(c.end(), c.begin());
	QCOMPARE(ccdist, -10);
	QCOMPARE(scdist, -10);
	
	//Fwd
	std::forward_list<int> f{1, 2, 3, 4, 5, 6};
	auto cfdist = constexprStd::distance(f.begin(), f.end());
	auto sfdist =          std::distance(f.begin(), f.end());
	QCOMPARE(cfdist, 6);
	QCOMPARE(sfdist, 6);
	return;
}

void TestConstexprStd::testExchange(void) const noexcept {
	//Test the acutal constexprness
	auto l = [](int i) constexpr noexcept {
			return constexprStd::exchange(i, 5);
		};
	static_assert(l(8) == 8);
	
	//Test runtime behavior and compare against std::exchange.
	std::string cstr1 = "foo";
	std::string sstr1 = "foo";
	QCOMPARE(cstr1, "foo");
	QCOMPARE(sstr1, "foo");
	
	auto cstr2 = constexprStd::exchange(cstr1, std::string{"bar"});
	auto sstr2 =          std::exchange(sstr1, std::string{"bar"});
	QCOMPARE(cstr1, "bar");
	QCOMPARE(sstr1, "bar");
	QCOMPARE(cstr2, "foo");
	QCOMPARE(sstr2, "foo");
	
	cstr2 = constexprStd::exchange(cstr1, "baz");
	sstr2 =          std::exchange(sstr1, "baz");
	QCOMPARE(cstr1, "baz");
	QCOMPARE(sstr1, "baz");
	QCOMPARE(cstr2, "bar");
	QCOMPARE(sstr2, "bar");
	return;
}

QTEST_APPLESS_MAIN(TestConstexprStd)

#include "test_main.moc"
