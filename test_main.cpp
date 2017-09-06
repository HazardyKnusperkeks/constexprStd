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
#include <functional>
#include <list>
#include <set>
#include <string>
#include <string_view>
#include <utility>

#include <constexprStd/algorithm>
#include <constexprStd/functional>
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

static constexpr int fib(const int i) noexcept {
	switch ( i ) {
		case 0 : 
		case 1 : return i;
	} //switch ( i )
	return fib(i-1) + fib(i-2);
}

static constexpr bool isLessThanEleven(const int i) noexcept { return i < 11; }
static constexpr bool isMultipleOfFive(const int i) noexcept { return i %  5 == 0; }
static constexpr bool isMultipleOfEleven(const int i) noexcept { return i % 11 == 0; }
static constexpr bool isOdd(const int i) noexcept { return i % 2 == 1; }

class TestConstexprStd : public QObject {
	Q_OBJECT
	private slots:
	//Algorithm lib
	//Non-modifying sequence operations
	void testCountIf(void) const noexcept;
	void testEqual(void) const noexcept;
	void testUnequal(void) const noexcept;
	void testFind(void) const noexcept;
	void testFindIf(void) const noexcept;
	void testFindIfNot(void) const noexcept;
	
	//Modifying sequence operations
	void testCopy(void) const noexcept;
	
	//Function objects
	//Polymorphic function wrappers
	void testInvoke(void) const noexcept;
	
	//Negators
	void testNotFn(void) const noexcept;
	
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
	
	std::forward_list<std::string> l{"foo", "bar", "moo"};
	QCOMPARE(*constexprStd::find(l.begin(), l.end(), "foo"), "foo");
	QCOMPARE(*         std::find(l.begin(), l.end(), "foo"), "foo");
	
	QCOMPARE(constexprStd::find(l.begin(), l.end(), "not"), l.end());
	QCOMPARE(         std::find(l.begin(), l.end(), "not"), l.end());
	
	QCOMPARE(*constexprStd::find(l, "moo"), "moo");
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

void TestConstexprStd::testInvoke(void) const noexcept {
	struct Ackermann {
		int N;
		constexpr int calc(const int m) const {
			return calcImpl(m);
		} //constexpr int calc(const int m) const
		
		constexpr int calcImpl(const int m) const & noexcept {
			if ( N == 0 ) {
				return m + 1;
			} //if ( N == 0 )
			if ( m == 0 ) {
				return Ackermann{N-1}.calcImpl(1);
			} //if ( m == 0 )
			return Ackermann{N-1}.calcImpl(Ackermann{N}.calcImpl(m-1));
		}
		
		constexpr int calcImpl(int m) && noexcept {
			for ( ; ; ) {
				if ( N == 0 ) {
					return m + 1;
				} //if ( N == 0 )
				if ( m == 0 ) {
					--N;
					m = 1;
				} //if ( m == 0 )
				else {
					int oldN = N;
					m = std::move(*this).calcImpl(m-1);
					N = oldN - 1;
				} //else -> if ( m == 0 )
			} //for ( ; ; )
		}
	};
	
	struct Base {
		int X;
		constexpr int calc(const int y) const noexcept {
			return X * y;
		}
	};
	
	struct Derived : Base {
		int Y;
		constexpr int calc(const int x) const noexcept {
			return x + Y;
		}
	};
	
	struct Callable {
		constexpr long long operator()(const int a, int b, const int c) const noexcept {
			long long ret = 1;
			for ( ; b; --b ) {
				ret *= a;
			} //for ( ; b; --b )
			return ret + c;
		}
	};
	
	auto lambda = [](void) constexpr noexcept {
			return 42;
		};
	
	constexpr Ackermann a2{2};
	constexpr Callable c;
	
	//Test a free function
	static_assert(constexprStd::invoke(fib, 9) == 34);
	QCOMPARE(std::invoke(fib, 9), 34);
	
	//Test a lambda
	static_assert(constexprStd::invoke(lambda) == 42);
	QCOMPARE(std::invoke(lambda), 42);
	
	//Test a member function
	static_assert(constexprStd::invoke(&Ackermann::calc, a2, 17) == 37);
	static_assert(constexprStd::invoke(&Ackermann::calc, Ackermann{3}, 5) == 253);
	QCOMPARE(std::invoke(&Ackermann::calc, a2, 17), 37);
	QCOMPARE(std::invoke(&Ackermann::calc, Ackermann{3}, 5), 253);
	
	//Test a reference wrapper to a member function
	//Create this variable so the correct constructor of reference_wrapper is called, not the deleted rvalue reference
	auto pointerToMemberFunction = &Ackermann::calc;
	std::reference_wrapper calcWrapper{pointerToMemberFunction};
	QCOMPARE(         std::invoke(calcWrapper, a2, 19), 41);
	QCOMPARE(constexprStd::invoke(calcWrapper, a2, 19), 41);
	QCOMPARE(         std::invoke(calcWrapper, Ackermann{1}, 19), 21);
	QCOMPARE(constexprStd::invoke(calcWrapper, Ackermann{1}, 19), 21);
	
	//Test (access) a data member
	static_assert(constexprStd::invoke(&Ackermann::N, a2) == 2);
	static_assert(constexprStd::invoke(&Ackermann::N, Ackermann{3}) == 3);
	QCOMPARE(std::invoke(&Ackermann::N, a2), 2);
	QCOMPARE(std::invoke(&Ackermann::N, Ackermann{3}), 3);
	
	//Test a reference wrapper to a data member
	//See above
	auto pointerToMember = &Ackermann::N;
	std::reference_wrapper nWrapper{pointerToMember};
	QCOMPARE(         std::invoke(nWrapper, a2), 2);
	QCOMPARE(constexprStd::invoke(nWrapper, a2), 2);
	QCOMPARE(         std::invoke(nWrapper, Ackermann{337}), 337);
	QCOMPARE(constexprStd::invoke(nWrapper, Ackermann{337}), 337);
	
	//Test a function object
	static_assert(constexprStd::invoke(c, 4, 18, 7) == 68719476743ll);
	static_assert(constexprStd::invoke(Callable{}, 4, 18, 7) == 68719476743ll);
	QCOMPARE(std::invoke(c, 4, 18, 7), 68719476743ll);
	QCOMPARE(std::invoke(Callable{}, 4, 18, 7), 68719476743ll);
	
	//Test returned reference
	constexpr int x1 = 9;
	static_assert(&constexprStd::invoke([&x1](void) -> const int& { return x1; }) == &x1);
	
	int x2 = 8;
	auto xLambda = [&x2](void) -> int& { return x2; };
	std::invoke(xLambda) = 12;
	QCOMPARE(x2, 12);
	constexprStd::invoke(xLambda) = 99;
	QCOMPARE(x2, 99);
	
	//Test derived member function
	constexpr Base b{8};
	constexpr Derived d{{3}, 5};
	
	static_assert(constexprStd::invoke(&Base::calc,    b, 2) == 16);
	static_assert(constexprStd::invoke(&Base::calc,    d, 2) ==  6);
	static_assert(constexprStd::invoke(&Derived::calc, d, 2) ==  7);
	
	QCOMPARE(std::invoke(&Base::calc,    b, 2), 16);
	QCOMPARE(std::invoke(&Base::calc,    d, 2),  6);
	QCOMPARE(std::invoke(&Derived::calc, d, 2),  7);
	return;
}
	
void TestConstexprStd::testNotFn(void) const noexcept {
	static_assert(constexprStd::not_fn(isMultipleOfFive)(7));
	QVERIFY(std::not_fn(isMultipleOfFive)(7));
	
	constexpr auto isEvenC = constexprStd::not_fn(isOdd);
	auto isEven = std::not_fn(isOdd);
	
	static_assert(isEvenC(2));
	QVERIFY(isEven(2));
	
	static_assert(constexprStd::not_fn([](){ return false; })());
	QVERIFY(std::not_fn([](){ return false; })());
	
	constexpr auto multiArgument = [](const int a, const int b, const int c, const int d) {
			return a == 1 && b == 2 && c == 3 && d == 4;
		};
	static_assert(!constexprStd::not_fn(multiArgument)(1, 2, 3, 4));
	QVERIFY(!std::not_fn(multiArgument)(1, 2, 3, 4));
	
	struct Foo {
		constexpr bool func(void) const noexcept {
			return false;
		}
	};
	
	constexpr Foo f;
	static_assert(constexprStd::not_fn(&Foo::func)(f));
	QVERIFY(std::not_fn(&Foo::func)(f));
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
