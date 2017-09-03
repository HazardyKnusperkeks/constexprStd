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

#include <array>
#include <forward_list>
#include <list>
#include <set>

#include <constexprStd/iterator>

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
	//Iterator lib
	//Iterator adaptors
	void testInsert_iterator(void) const noexcept;
	
	//Iterator operations
	void testAdvance(void) const noexcept;
	
	public:
	explicit TestConstexprStd(QObject *parent = nullptr) : QObject(parent) { return; }
};

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

QTEST_APPLESS_MAIN(TestConstexprStd)

#include "test_main.moc"
