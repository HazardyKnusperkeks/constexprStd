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
 * @brief Checks constexprStd/iterator for self-containment and contains the tests.
 */

#include <constexprStd/iterator>

#include "../test.hpp"

#include <forward_list>
#include <iterator>

#include "test_container.hpp"

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

void TestConstexprStd::testNext(void) const noexcept {
	auto l = []{
			TestContainer ret;
			auto iter = constexprStd::next(ret.begin(), 7);
			return *iter;
		};
	static_assert(l() == 8);
	
	TestContainer c;
	auto citer = constexprStd::next(c.begin(), 5);
	auto siter =          std::next(c.begin(), 5);
	
	QCOMPARE(*citer, 6);
	QCOMPARE(*siter, 6);
	return;
}

void TestConstexprStd::testPrev(void) const noexcept {
	auto l = []{
			TestContainer ret;
			auto iter = constexprStd::prev(ret.end(), 3);
			return *iter;
		};
	static_assert(l() == 8);
	
	TestContainer c;
	auto citer = constexprStd::prev(c.end(), 5);
	auto siter =          std::prev(c.end(), 5);
	
	QCOMPARE(*citer, 6);
	QCOMPARE(*siter, 6);
	return;
}
