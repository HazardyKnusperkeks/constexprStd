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
 * @brief Checks constexprStd/set for self-containment and contains the tests.
 */

#include <constexprStd/set>

#include "../test.hpp"

#include <iostream>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "test_constants.hpp"
#include "test_count_instances.hpp"

void TestConstexprStd::testSet(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			constexprStd::set<int, 15> set;
			set.clear();
			
			auto ins1 = set.insert(1);
			auto t1   = std::tuple{ins1.second};
			auto ins2 = set.insert(1);
			auto t2   = std::tuple{ins2.second, *ins2.first};
			for ( int i : {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} ) {
				set.insert(i);
			} //for ( int i : {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15} )
			auto t3   = std::tuple{set.size()};
			return std::tuple_cat(t1, t2, t3);
		};
	
	static_assert(l() == std::tuple{true, false, 1, 15});
	
	int& instances = CountInstances<std::string>::Instances;
	if ( instances != 0 ) {
		QWARN("Had to reset CountInstances<std::string>::Instances");
		instances = 0;
	} //if ( instances != 0 )
	
	constexprStd::set<CountInstances<std::string>, 15> cset;
	         std::set<CountInstances<std::string>> sset;
	QCOMPARE(instances, 0);
	
	//Test empty clear
	cset.clear();
	sset.clear();
	QCOMPARE(instances, 0);
	
	auto cins1 = cset.insert(fooString);
	auto sins1 = sset.insert(fooString);
	QVERIFY(cins1.second);
	QVERIFY(sins1.second);
	QCOMPARE(instances, 2);
	
	auto cins2 = cset.insert(fooString);
	auto sins2 = sset.insert(fooString);
	QVERIFY(!cins2.second);
	QVERIFY(!sins2.second);
	QCOMPARE(*cins2.first, fooString);
	QCOMPARE(*sins2.first, fooString);
	QCOMPARE(instances, 2);
	
	cset.insert(barString);
	sset.insert(barString);
	
	cset.insert(bazString);
	sset.insert(bazString);
	QCOMPARE(instances, 6);
	
	CountInstances<std::string> cstr(emptyString);
	CountInstances<std::string> sstr(emptyString);
	
	cset.insert(cstr);
	sset.insert(sstr);
	QCOMPARE(cset.size(), static_cast<decltype(cset.size())>(4));
	QCOMPARE(sset.size(), static_cast<decltype(sset.size())>(4));
	QCOMPARE(instances, 10);
	
	for ( const auto& s : {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o"} ) {
		cset.insert(s);
		sset.insert(s);
	} //for ( const auto& s : {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o"} )
	
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	
	auto citer = cset.end();
	auto siter = sset.end();
	
	for ( const auto& s : {"z", "y", "x", "u"} ) {
		citer = cset.insert(citer, s);
		siter = sset.insert(siter, s);
	} //for ( const auto& s : {"z", "y", "x", "u"} )
	
	cset.insert(citer, cstr);
	sset.insert(siter, sstr);
	QVERIFY(std::equal(cset.rbegin(), cset.rend(), sset.rbegin(), sset.rend()));
	
	cset.clear();
	sset.clear();
	QCOMPARE(instances, 2); //cstr and sstr on the stack
	{
		constexprStd::setDestroy<CountInstances<std::string>, 15> dset;
	}
	QCOMPARE(instances, 2);
	return;
}

void TestConstexprStd::testSetFailCaseOne(void) const noexcept {
	constexpr std::array a{5, 4, 3, 2, 3, 2, 3};
	constexprStd::setDestroy<int, 10> cset;
	std::set<int> sset;
	
	auto citer = cset.end();
	auto siter = sset.end();
	
	for ( auto i = 0u; i < a.size(); ++i ) {
		citer = cset.insert(citer, a[i]);
		siter = sset.insert(siter, a[i]);
		
//		if ( !std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()) ) {
//			QCOMPARE(i, 0u);
//		} //if ( !std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()) )
	} //for ( auto i = 0u; i < a.size(); ++i )
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	return;
}

void TestConstexprStd::testSetRandom(void) const noexcept {
	constexprStd::setDestroy<int, 10> cset;
	std::set<int> sset;
	
	struct PrintSetOnEarlyExit {
		std::vector<int> Vec;
		bool Print = true;
		
		~PrintSetOnEarlyExit(void) noexcept {
			if ( Print ) {
				std::cout<<"Insertion order on error: ";
				for ( const auto& i : Vec ) {
					std::cout<<i<<", ";
				} //for ( const auto& i : Vec )
				std::cout<<std::endl;
			} //if ( Print )
			return;
		}
	};
	
	PrintSetOnEarlyExit p;
	p.Vec.reserve(30000);
	std::mt19937 gen{std::random_device{}()};
	std::uniform_int_distribution<int> distribution{0, 100000};
	
	auto citer = cset.end();
	auto siter = sset.end();
	
	for ( int i = 0; i < 30000; ++i ) {
		int rand = distribution(gen);
		p.Vec.emplace_back(rand);
		citer = cset.insert(citer, rand);
		siter = sset.insert(siter, rand);
		
//		if ( i % 1000 == 0 ) {
//			QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
//		} //if ( i % 1000 == 0 )
	} //for ( int i = 0; i < 30000; ++i )
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	p.Print = false;
	return;
}
