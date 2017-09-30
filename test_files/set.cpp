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

using namespace std::string_literals;

void TestConstexprStd::testSet(void) const noexcept {
	auto l = [](void) constexpr noexcept {
			constexprStd::set<int, 15> set;
			std::array a{6, 7, 8};
			set.clear();
			
			auto ins1 = set.insert(1);
			auto t1   = std::tuple{ins1.second};
			auto ins2 = set.insert(1);
			auto t2   = std::tuple{ins2.second, *ins2.first};
			for ( int i : {2, 3, 4, 5} ) {
				set.insert(i);
			} //for ( int i : {2, 3, 4, 5} )
			auto t3   = std::tuple{set.size()};
			set.insert(a.begin(), a.end());
			auto t4   = std::tuple{set.size()};
			set.insert(a.end(), a.end());
			auto t5   = std::tuple{set.size()};
			set.insert({9, 10, 11, 12});
			auto t6   = std::tuple{set.size()};
			
			constexprStd::set<int, 15> iset(a.begin(), a.end());
			auto t7   = std::tuple{iset.size()};
			
			constexprStd::set<int, 15> cset(iset);
			auto t8   = std::tuple{cset.size()};
			auto t9   = std::tuple{set.count(2), set.count(55), set.find(33) == set.end(), set.find(3) == set.end()};
			
			set.clear();
			iset.clear();
			cset.clear();
			auto tX   = std::tuple{set.size()};
			return std::tuple_cat(t1, t2, t3, t4, t5, t6, t7, t8, t9, tX);
		};
	
	static_assert(l() == std::tuple{true, false, 1, 5, 8, 8, 12, 3, 3, 1, 0, true, false, 0});
	
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
	
	std::array a{"hello", "world", "42"};
	cset.insert(a.begin(), a.end());
	sset.insert(a.begin(), a.end());
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	
	cset.insert(a.end(), a.end());
	sset.insert(a.end(), a.end());
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	
	cset.insert({"this", "is", "a", "test"});
	sset.insert({"this", "is", "a", "test"});
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	
	constexprStd::set<CountInstances<std::string>, 15> icset(cset.begin(), cset.end());
	         std::set<CountInstances<std::string>>     isset(sset.begin(), sset.end());
	QVERIFY(std::equal(icset.begin(), icset.end(), isset.begin(), isset.end()));
	
	constexprStd::set<CountInstances<std::string>, 15> ccset(cset);
	         std::set<CountInstances<std::string>>     csset(sset);
	QVERIFY(std::equal(ccset.begin(), ccset.end(), csset.begin(), csset.end()));
	
	QCOMPARE(cset.count("a"),    static_cast<decltype(cset.count(""))>(1));
	QCOMPARE(sset.count("a"),    static_cast<decltype(sset.count(""))>(1));
	QCOMPARE(cset.count("damn"), static_cast<decltype(cset.count(""))>(0));
	QCOMPARE(sset.count("damn"), static_cast<decltype(sset.count(""))>(0));
	
	QVERIFY(cset.find("damn") == cset.end());
	QVERIFY(sset.find("damn") == sset.end());
	QVERIFY(std::as_const(cset).find("damn") == cset.cend());
	QVERIFY(std::as_const(sset).find("damn") == sset.cend());
	QCOMPARE(*cset.find(fooString), fooString);
	QCOMPARE(*sset.find(fooString), fooString);
	QCOMPARE(*std::as_const(cset).find(fooString), fooString);
	QCOMPARE(*std::as_const(sset).find(fooString), fooString);
	
	QVERIFY(cset.checkBinarySearchTree());
	QVERIFY(cset.checkBlackDepth());
	QCOMPARE(*cset.erase(cset.find("n")), "o");
	QCOMPARE(*sset.erase(sset.find("n")), "o");
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	QVERIFY(cset.checkBinarySearchTree());
	QVERIFY(cset.checkBlackDepth());
	
	QCOMPARE(*cset.erase(cset.find("a"), cset.find("f")), "f");
	QCOMPARE(*sset.erase(sset.find("a"), sset.find("f")), "f");
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	QVERIFY(cset.checkBinarySearchTree());
	QVERIFY(cset.checkBlackDepth());
	
	QCOMPARE(cset.erase(fooString), 1u);
	QCOMPARE(sset.erase(fooString), 1u);
	QCOMPARE(cset.erase(fooString), 0u);
	QCOMPARE(sset.erase(fooString), 0u);
	
	QCOMPARE(cset.count(fooString), 0u);
	QCOMPARE(sset.count(fooString), 0u);
	cset.emplace(fooString);
	sset.emplace(fooString);
	QCOMPARE(cset.count(fooString), 1u);
	QCOMPARE(sset.count(fooString), 1u);
	
	cset.clear();
	sset.clear();
	icset.clear();
	isset.clear();
	ccset.clear();
	csset.clear();
	QCOMPARE(instances, 2); //cstr and sstr on the stack
	{
		constexprStd::setDestroy<CountInstances<std::string>, 15> dset;
		auto hint = dset.end();
		for ( const auto& s : {"a", "b", "c", "d"} ) {
			hint = dset.emplace_hint(hint, s);
		} //for ( const auto& s : {"a", "b", "c", "d"} )
		QCOMPARE(instances, 6);
	}
	QCOMPARE(instances, 2);
	
	std::string generatedString(30, ' ');
	auto generator = [s = generatedString, idx = 0u, c = 'a'](void) mutable noexcept {
			s[idx] = c;
			if ( c++ == 'z' ) {
				c = 'a';
				++idx;
			} //if ( c++ == 'z' )
			return s;
		};
	std::generate_n(std::inserter(cset, cset.end()), 26 * 30, generator);
	QCOMPARE(instances, 2 + 26 * 30);
	constexprStd::set_base copy(cset);
	QCOMPARE(instances, 2 + 26 * 30 * 2);
	QVERIFY(std::equal(cset.begin(), cset.end(), copy.begin(), copy.end()));
	
	constexprStd::set moveSet(std::move(cset));
	QCOMPARE(instances, 2 + 26 * 30 * 3);
	QVERIFY(std::equal(moveSet.begin(), moveSet.end(), copy.begin(), copy.end()));
	
	constexprStd::set_base moveBase(std::move(copy));
	QCOMPARE(instances, 2 + 26 * 30 * 3);
	QVERIFY(copy.empty());
	QVERIFY(std::equal(moveSet.begin(), moveSet.end(), moveBase.begin(), moveBase.end()));
	
	cset.clear();
	copy.clear();
	moveSet.clear();
	moveBase.clear();
	QCOMPARE(instances, 2);
	return;
}

struct String {
	std::string S;
	
	String(std::string s) noexcept(std::is_nothrow_move_constructible_v<std::string>) : S(std::move(s)) {
		return;
	}
	
	String(const char*) {
		throw std::exception{};
	}
	
	operator std::string(void) const {
		return S;
	}
	
	auto toS(void) const {
		return S;
	}
	
	bool operator<(const char *c) const noexcept(noexcept(S < c)) {
		return S < c;
	}
	
	bool operator<(const std::string& c) const noexcept(noexcept(S < c)) {
		return S < c;
	}
	
	bool operator<(const String& c) const noexcept(noexcept(S < S)) {
		return S < c.S;
	}
	
	friend bool operator<(const char *c, const String& s) noexcept(noexcept(c < std::declval<const std::string&>())) {
		return c < s.S;
	}
	
	friend bool operator<(const std::string& c, const String& s) noexcept(noexcept(c < c)) {
		return c < s.S;
	}
};

void TestConstexprStd::testSetTransparentCompare(void) const noexcept {
	constexprStd::set<String, 10>              cnset;
	constexprStd::set<String, 10, std::less<>> ctset;
	         std::set<String>                  snset;
	         std::set<String,     std::less<>> stset;
	
	std::generate_n(std::inserter(cnset, cnset.end()), 39, [c = 'a', lower = true](void) mutable {
			std::string s(1, c);
			++c;
			if ( lower ) {
				if ( c++ == 'z' ) {
					c = 'A';
					lower = false;
				} //if ( c++ == 'z' )
			} //if ( lower )
			return s;
		});
	
	std::copy(cnset.begin(), cnset.end(), std::inserter(ctset, ctset.end()));
	std::copy(cnset.begin(), cnset.end(), std::inserter(snset, snset.end()));
	std::copy(cnset.begin(), cnset.end(), std::inserter(stset, stset.end()));
	
	constexpr decltype(cnset.count("")) zero = 0;
	constexpr decltype(cnset.count("")) one  = 1;
	
	try { QCOMPARE(cnset.count("a"), one); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QCOMPARE(snset.count("a"), one); QVERIFY(false); } catch ( const std::exception& ) { }
	QCOMPARE(ctset.count("a"),  one);
	QCOMPARE(stset.count("a"),  one);
	QCOMPARE(cnset.count("a"s), one);
	QCOMPARE(ctset.count("a"s), one);
	QCOMPARE(snset.count("a"s), one);
	QCOMPARE(stset.count("a"s), one);
	
	try { QCOMPARE(cnset.count("b"), zero); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QCOMPARE(snset.count("b"), zero); QVERIFY(false); } catch ( const std::exception& ) { }
	QCOMPARE(ctset.count("b"),  zero);
	QCOMPARE(stset.count("b"),  zero);
	QCOMPARE(cnset.count("b"s), zero);
	QCOMPARE(ctset.count("b"s), zero);
	QCOMPARE(snset.count("b"s), zero);
	QCOMPARE(stset.count("b"s), zero);
	
	try { QCOMPARE(cnset.find("a")->toS(), "a"); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QCOMPARE(snset.find("a")->toS(), "a"); QVERIFY(false); } catch ( const std::exception& ) { }
	QCOMPARE(ctset.find("a")->toS(),  "a");
	QCOMPARE(stset.find("a")->toS(),  "a");
	QCOMPARE(cnset.find("a"s)->toS(), "a");
	QCOMPARE(ctset.find("a"s)->toS(), "a");
	QCOMPARE(snset.find("a"s)->toS(), "a");
	QCOMPARE(stset.find("a"s)->toS(), "a");
	
	try { QVERIFY(cnset.find("b") == cnset.end()); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QVERIFY(snset.find("b") == snset.end()); QVERIFY(false); } catch ( const std::exception& ) { }
	QVERIFY(ctset.find("b")  == ctset.end());
	QVERIFY(stset.find("b")  == stset.end());
	QVERIFY(cnset.find("b"s) == cnset.end());
	QVERIFY(ctset.find("b"s) == ctset.end());
	QVERIFY(snset.find("b"s) == snset.end());
	QVERIFY(stset.find("b"s) == stset.end());
	
	std::string toGreatElements = "y"s < "Z"s ? "Z" : "y";
	++toGreatElements[0];
	const char *toGreatElement = toGreatElements.data();
	
	try { QCOMPARE(cnset.lower_bound("a")->toS(), "a"); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QCOMPARE(snset.lower_bound("a")->toS(), "a"); QVERIFY(false); } catch ( const std::exception& ) { }
	QCOMPARE(ctset.lower_bound("a")->toS(),  "a");
	QCOMPARE(stset.lower_bound("a")->toS(),  "a");
	QCOMPARE(cnset.lower_bound("a"s)->toS(), "a");
	QCOMPARE(ctset.lower_bound("a"s)->toS(), "a");
	QCOMPARE(snset.lower_bound("a"s)->toS(), "a");
	QCOMPARE(stset.lower_bound("a"s)->toS(), "a");
	
	try { QCOMPARE(cnset.lower_bound("b")->toS(), "c"); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QCOMPARE(snset.lower_bound("b")->toS(), "c"); QVERIFY(false); } catch ( const std::exception& ) { }
	QCOMPARE(ctset.lower_bound("b")->toS(),  "c");
	QCOMPARE(stset.lower_bound("b")->toS(),  "c");
	QCOMPARE(cnset.lower_bound("b"s)->toS(), "c");
	QCOMPARE(ctset.lower_bound("b"s)->toS(), "c");
	QCOMPARE(snset.lower_bound("b"s)->toS(), "c");
	QCOMPARE(stset.lower_bound("b"s)->toS(), "c");
	
	try { QCOMPARE(cnset.lower_bound("x")->toS(), "y"); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QCOMPARE(snset.lower_bound("x")->toS(), "y"); QVERIFY(false); } catch ( const std::exception& ) { }
	QCOMPARE(ctset.lower_bound("x")->toS(),  "y");
	QCOMPARE(stset.lower_bound("x")->toS(),  "y");
	QCOMPARE(cnset.lower_bound("x"s)->toS(), "y");
	QCOMPARE(ctset.lower_bound("x"s)->toS(), "y");
	QCOMPARE(snset.lower_bound("x"s)->toS(), "y");
	QCOMPARE(stset.lower_bound("x"s)->toS(), "y");
	
	try { QVERIFY(cnset.lower_bound(toGreatElement) == cnset.end()); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QVERIFY(snset.lower_bound(toGreatElement) == snset.end()); QVERIFY(false); } catch ( const std::exception& ) { }
	QVERIFY(ctset.lower_bound(toGreatElement)  == ctset.end());
	QVERIFY(stset.lower_bound(toGreatElement)  == stset.end());
	QVERIFY(cnset.lower_bound(toGreatElements) == cnset.end());
	QVERIFY(ctset.lower_bound(toGreatElements) == ctset.end());
	QVERIFY(snset.lower_bound(toGreatElements) == snset.end());
	QVERIFY(stset.lower_bound(toGreatElements) == stset.end());
	
	try { QCOMPARE(cnset.upper_bound("a")->toS(), "c"); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QCOMPARE(snset.upper_bound("a")->toS(), "c"); QVERIFY(false); } catch ( const std::exception& ) { }
	QCOMPARE(ctset.upper_bound("a")->toS(),  "c");
	QCOMPARE(stset.upper_bound("a")->toS(),  "c");
	QCOMPARE(cnset.upper_bound("a"s)->toS(), "c");
	QCOMPARE(ctset.upper_bound("a"s)->toS(), "c");
	QCOMPARE(snset.upper_bound("a"s)->toS(), "c");
	QCOMPARE(stset.upper_bound("a"s)->toS(), "c");
	
	try { QCOMPARE(cnset.upper_bound("b")->toS(), "c"); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QCOMPARE(snset.upper_bound("b")->toS(), "c"); QVERIFY(false); } catch ( const std::exception& ) { }
	QCOMPARE(ctset.upper_bound("b")->toS(),  "c");
	QCOMPARE(stset.upper_bound("b")->toS(),  "c");
	QCOMPARE(cnset.upper_bound("b"s)->toS(), "c");
	QCOMPARE(ctset.upper_bound("b"s)->toS(), "c");
	QCOMPARE(snset.upper_bound("b"s)->toS(), "c");
	QCOMPARE(stset.upper_bound("b"s)->toS(), "c");
	
	try { QCOMPARE(cnset.upper_bound("x")->toS(), "y"); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QCOMPARE(snset.upper_bound("x")->toS(), "y"); QVERIFY(false); } catch ( const std::exception& ) { }
	QCOMPARE(ctset.upper_bound("x")->toS(),  "y");
	QCOMPARE(stset.upper_bound("x")->toS(),  "y");
	QCOMPARE(cnset.upper_bound("x"s)->toS(), "y");
	QCOMPARE(ctset.upper_bound("x"s)->toS(), "y");
	QCOMPARE(snset.upper_bound("x"s)->toS(), "y");
	QCOMPARE(stset.upper_bound("x"s)->toS(), "y");
	
	try { QVERIFY(cnset.upper_bound(toGreatElement) == cnset.end()); QVERIFY(false); } catch ( const std::exception& ) { }
	try { QVERIFY(snset.upper_bound(toGreatElement) == snset.end()); QVERIFY(false); } catch ( const std::exception& ) { }
	QVERIFY(ctset.upper_bound(toGreatElement)  == ctset.end());
	QVERIFY(stset.upper_bound(toGreatElement)  == stset.end());
	QVERIFY(cnset.upper_bound(toGreatElements) == cnset.end());
	QVERIFY(ctset.upper_bound(toGreatElements) == ctset.end());
	QVERIFY(snset.upper_bound(toGreatElements) == snset.end());
	QVERIFY(stset.upper_bound(toGreatElements) == stset.end());
	
	auto cniters = std::pair{cnset.end(), cnset.end()};
	auto ctiters = std::pair{ctset.end(), ctset.end()};
	auto sniters = std::pair{snset.end(), snset.end()};
	auto stiters = std::pair{stset.end(), stset.end()};
	
	try { cniters = cnset.equal_range("a"); QVERIFY(false); } catch ( const std::exception& ) { }
	try { sniters = snset.equal_range("a"); QVERIFY(false); } catch ( const std::exception& ) { }
	ctiters = ctset.equal_range("a");  QCOMPARE(ctiters.first->toS(), "a"); QCOMPARE(ctiters.second->toS(), "c");
	stiters = stset.equal_range("a");  QCOMPARE(stiters.first->toS(), "a"); QCOMPARE(stiters.second->toS(), "c");
	cniters = cnset.equal_range("a"s); QCOMPARE(cniters.first->toS(), "a"); QCOMPARE(cniters.second->toS(), "c");
	sniters = snset.equal_range("a"s); QCOMPARE(sniters.first->toS(), "a"); QCOMPARE(sniters.second->toS(), "c");
	ctiters = ctset.equal_range("a"s); QCOMPARE(ctiters.first->toS(), "a"); QCOMPARE(ctiters.second->toS(), "c");
	stiters = stset.equal_range("a"s); QCOMPARE(stiters.first->toS(), "a"); QCOMPARE(stiters.second->toS(), "c");
	
	try { cniters = cnset.equal_range("b"); QVERIFY(false); } catch ( const std::exception& ) { }
	try { sniters = snset.equal_range("b"); QVERIFY(false); } catch ( const std::exception& ) { }
	ctiters = ctset.equal_range("b");  QCOMPARE(ctiters.first->toS(), "c"); QCOMPARE(ctiters.second->toS(), "c");
	stiters = stset.equal_range("b");  QCOMPARE(stiters.first->toS(), "c"); QCOMPARE(stiters.second->toS(), "c");
	cniters = cnset.equal_range("b"s); QCOMPARE(cniters.first->toS(), "c"); QCOMPARE(cniters.second->toS(), "c");
	sniters = snset.equal_range("b"s); QCOMPARE(sniters.first->toS(), "c"); QCOMPARE(sniters.second->toS(), "c");
	ctiters = ctset.equal_range("b"s); QCOMPARE(ctiters.first->toS(), "c"); QCOMPARE(ctiters.second->toS(), "c");
	stiters = stset.equal_range("b"s); QCOMPARE(stiters.first->toS(), "c"); QCOMPARE(stiters.second->toS(), "c");
	
	try { cniters = cnset.equal_range(toGreatElement); QVERIFY(false); } catch ( const std::exception& ) { }
	try { sniters = snset.equal_range(toGreatElement); QVERIFY(false); } catch ( const std::exception& ) { }
	ctiters = ctset.equal_range(toGreatElement);  QVERIFY(ctiters.first == ctset.end()); QVERIFY(ctiters.second == ctset.end());
	stiters = stset.equal_range(toGreatElement);  QVERIFY(stiters.first == stset.end()); QVERIFY(stiters.second == stset.end());
	cniters = cnset.equal_range(toGreatElements); QVERIFY(cniters.first == cnset.end()); QVERIFY(cniters.second == cnset.end());
	sniters = snset.equal_range(toGreatElements); QVERIFY(sniters.first == snset.end()); QVERIFY(sniters.second == snset.end());
	ctiters = ctset.equal_range(toGreatElements); QVERIFY(ctiters.first == ctset.end()); QVERIFY(ctiters.second == ctset.end());
	stiters = stset.equal_range(toGreatElements); QVERIFY(stiters.first == stset.end()); QVERIFY(stiters.second == stset.end());
	
	cnset.clear();
	ctset.clear();
	return;
}

void TestConstexprStd::testSetErase(void) const noexcept {
	constexpr int elements = 10000;
	constexpr int stepSize = 1000;
	
	//Try to produce all cases of erase
	std::set<int> sbegin;
	std::generate_n(std::inserter(sbegin, sbegin.end()), elements, [i = 0](void) mutable noexcept { return ++i; });
	std::set<int> send(sbegin), squarter(sbegin), smiddle(sbegin);
	constexprStd::set<int, 10> cbegin(sbegin.begin(), sbegin.end());
	constexprStd::set<int, 10> cend(cbegin), cquarter(cbegin), cmiddle(cbegin);
	
	//Remove always the first element
	auto siter = sbegin.begin();
	auto citer = cbegin.begin();
	for ( int i = 1; i <= elements; ++i ) {
		siter = sbegin.erase(siter);
		citer = cbegin.erase(citer);
		if ( i % stepSize == 0 ) {
			auto equal = std::equal(siter, sbegin.end(), citer, cbegin.end());
			auto depth = cbegin.checkBlackDepth();
			if ( !equal || !depth ) {
				QCOMPARE(i, 0);
			} //if ( !equal || !depth )
		} //if ( i % stepSize == 0 )
	} //for ( int i = 1; i <= elements; ++i )
	
	//Remove always the last element
	siter = send.end();
	citer = cend.end();
	for ( int i = 1; i <= elements; ++i ) {
		siter = send.erase(std::prev(siter));
		citer = cend.erase(std::prev(citer));
		if ( i % stepSize == 0 ) {
			auto equal = std::equal(send.begin(), siter, cend.begin(), citer);
			auto depth = cend.checkBlackDepth();
			if ( !equal || !depth ) {
				QCOMPARE(i, 0);
			} //if ( !equal || !depth )
		} //if ( i % stepSize == 0 )
	} //for ( int i = 1; i <= elements; ++i )
	
	//Remove always the middle element
	for ( int i = 1; i <= elements; ++i ) {
		smiddle.erase(std::next(smiddle.begin(), (elements - i) / 2));
		cmiddle.erase(std::next(cmiddle.begin(), (elements - i) / 2));
		if ( i % stepSize == 0 ) {
			auto equal = std::equal(smiddle.begin(), smiddle.end(), cmiddle.begin(), cmiddle.end());
			auto depth = cmiddle.checkBlackDepth();
			if ( !equal || !depth ) {
				QCOMPARE(i, 0);
			} //if ( !equal || !depth )
		} //if ( i % stepSize == 0 )
	} //for ( int i = 1; i <= elements; ++i )
	
	//Remove always the element on 1/4th of the range
	for ( int i = 1; i <= elements; ++i ) {
		squarter.erase(std::next(squarter.begin(), (elements - i) / 4));
		cquarter.erase(std::next(cquarter.begin(), (elements - i) / 4));
		if ( i % stepSize == 0 ) {
			auto equal = std::equal(squarter.begin(), squarter.end(), cquarter.begin(), cquarter.end());
			auto depth = cquarter.checkBlackDepth();
			if ( !equal || !depth ) {
				QCOMPARE(i, 0);
			} //if ( !equal || !depth )
		} //if ( i % stepSize == 0 )
	} //for ( int i = 1; i <= elements; ++i )
	return;
}

void TestConstexprStd::testSetComparisonOperators(void) const noexcept {
	constexprStd::set<int, 10>         cset1{1, 2, 3};
	constexprStd::setDestroy<int, 10>  cset2{1, 1, 2};
	constexprStd::set_base             cset3{cset1}; cset3.insert(4);
	
	std::set<int> sset1{1, 2, 3};
	std::set<int> sset2{1, 1, 2};
	std::set<int> sset3{1, 2, 3, 4};
	
	QVERIFY( (sset1 == sset1)); QVERIFY(!(sset2 == sset1)); QVERIFY(!(sset3 == sset1));
	QVERIFY(!(sset1 <  sset1)); QVERIFY( (sset2 <  sset1)); QVERIFY(!(sset3 <  sset1));
	QVERIFY( (sset1 >= sset1)); QVERIFY(!(sset2 >= sset1)); QVERIFY( (sset3 >= sset1));
	QVERIFY(!(sset1 == sset2)); QVERIFY( (sset2 == sset2)); QVERIFY(!(sset3 == sset2));
	QVERIFY(!(sset1 <  sset2)); QVERIFY(!(sset2 <  sset2)); QVERIFY(!(sset3 <  sset2));
	QVERIFY( (sset1 >= sset2)); QVERIFY( (sset2 >= sset2)); QVERIFY( (sset3 >= sset2));
	QVERIFY(!(sset1 == sset3)); QVERIFY(!(sset2 == sset3)); QVERIFY( (sset3 == sset3));
	QVERIFY( (sset1 <  sset3)); QVERIFY( (sset2 <  sset3)); QVERIFY(!(sset3 <  sset3));
	QVERIFY(!(sset1 >= sset3)); QVERIFY(!(sset2 >= sset3)); QVERIFY( (sset3 >= sset3));
	
	QVERIFY( (cset1 == cset1)); QVERIFY(!(cset2 == cset1)); QVERIFY(!(cset3 == cset1));
	QVERIFY(!(cset1 <  cset1)); QVERIFY( (cset2 <  cset1)); QVERIFY(!(cset3 <  cset1));
	QVERIFY( (cset1 >= cset1)); QVERIFY(!(cset2 >= cset1)); QVERIFY( (cset3 >= cset1));
	QVERIFY(!(cset1 == cset2)); QVERIFY( (cset2 == cset2)); QVERIFY(!(cset3 == cset2));
	QVERIFY(!(cset1 <  cset2)); QVERIFY(!(cset2 <  cset2)); QVERIFY(!(cset3 <  cset2));
	QVERIFY( (cset1 >= cset2)); QVERIFY( (cset2 >= cset2)); QVERIFY( (cset3 >= cset2));
	QVERIFY(!(cset1 == cset3)); QVERIFY(!(cset2 == cset3)); QVERIFY( (cset3 == cset3));
	QVERIFY( (cset1 <  cset3)); QVERIFY( (cset2 <  cset3)); QVERIFY(!(cset3 <  cset3));
	QVERIFY(!(cset1 >= cset3)); QVERIFY(!(cset2 >= cset3)); QVERIFY( (cset3 >= cset3));
	
	QVERIFY( (cset1 == sset1)); QVERIFY(!(cset2 == sset1)); QVERIFY(!(cset3 == sset1));
	QVERIFY(!(cset1 <  sset1)); QVERIFY( (cset2 <  sset1)); QVERIFY(!(cset3 <  sset1));
	QVERIFY( (cset1 >= sset1)); QVERIFY(!(cset2 >= sset1)); QVERIFY( (cset3 >= sset1));
	QVERIFY(!(cset1 == sset2)); QVERIFY( (cset2 == sset2)); QVERIFY(!(cset3 == sset2));
	QVERIFY(!(cset1 <  sset2)); QVERIFY(!(cset2 <  sset2)); QVERIFY(!(cset3 <  sset2));
	QVERIFY( (cset1 >= sset2)); QVERIFY( (cset2 >= sset2)); QVERIFY( (cset3 >= sset2));
	QVERIFY(!(cset1 == sset3)); QVERIFY(!(cset2 == sset3)); QVERIFY( (cset3 == sset3));
	QVERIFY( (cset1 <  sset3)); QVERIFY( (cset2 <  sset3)); QVERIFY(!(cset3 <  sset3));
	QVERIFY(!(cset1 >= sset3)); QVERIFY(!(cset2 >= sset3)); QVERIFY( (cset3 >= sset3));
	
	QVERIFY( (sset1 == cset1)); QVERIFY(!(sset2 == cset1)); QVERIFY(!(sset3 == cset1));
	QVERIFY(!(sset1 <  cset1)); QVERIFY( (sset2 <  cset1)); QVERIFY(!(sset3 <  cset1));
	QVERIFY( (sset1 >= cset1)); QVERIFY(!(sset2 >= cset1)); QVERIFY( (sset3 >= cset1));
	QVERIFY(!(sset1 == cset2)); QVERIFY( (sset2 == cset2)); QVERIFY(!(sset3 == cset2));
	QVERIFY(!(sset1 <  cset2)); QVERIFY(!(sset2 <  cset2)); QVERIFY(!(sset3 <  cset2));
	QVERIFY( (sset1 >= cset2)); QVERIFY( (sset2 >= cset2)); QVERIFY( (sset3 >= cset2));
	QVERIFY(!(sset1 == cset3)); QVERIFY(!(sset2 == cset3)); QVERIFY( (sset3 == cset3));
	QVERIFY( (sset1 <  cset3)); QVERIFY( (sset2 <  cset3)); QVERIFY(!(sset3 <  cset3));
	QVERIFY(!(sset1 >= cset3)); QVERIFY(!(sset2 >= cset3)); QVERIFY( (sset3 >= cset3));
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
		std::vector<int> Insert;
		std::vector<int> Erase;
		bool Print = true;
		
		~PrintSetOnEarlyExit(void) noexcept {
			if ( Print ) {
				std::cout<<"Insertion order on error: ";
				for ( const auto& i : Insert ) {
					std::cout<<i<<", ";
				} //for ( const auto& i : Insert )
				std::cout<<"\nErase indicies on error: ";
				for ( const auto& i : Erase ) {
					std::cout<<i<<", ";
				} //for ( const auto& i : Erase )
				std::cout<<std::endl;
			} //if ( Print )
			return;
		}
	};
	
	constexpr int elements = 10000;
	
	PrintSetOnEarlyExit p;
	p.Insert.reserve(elements);
	p.Erase.reserve(elements);
	std::mt19937 gen{std::random_device{}()};
	std::uniform_int_distribution<int> distribution{0, 100000};
	
	auto citer = cset.end();
	auto siter = sset.end();
	
	for ( int i = 0; i < elements; ++i ) {
		int rand = distribution(gen);
		p.Insert.emplace_back(rand);
		citer = cset.insert(citer, rand);
		siter = sset.insert(siter, rand);
		
//		if ( i % 1000 == 0 ) {
//			QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
//		} //if ( i % 1000 == 0 )
	} //for ( int i = 0; i < elements; ++i )
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	QVERIFY(cset.checkBlackDepth());
	
	int elementsInSet = static_cast<int>(sset.size());
	citer = cset.begin();
	siter = sset.begin();
	int oldIndex = 0;
	constexpr int elementsDeletedAtOnce = 5;
	for ( int i = 0; i < elementsInSet; ++i ) {
		int index = std::uniform_int_distribution<int>{0, std::max(elementsInSet - 1, 0)}(gen);
		p.Erase.emplace_back(index);
		auto diff = index - oldIndex;
		std::advance(citer, diff);
		std::advance(siter, diff);
		diff = std::min<int>(elementsDeletedAtOnce, elementsInSet - index);
		citer = cset.erase(citer, std::next(citer, diff));
		siter = sset.erase(siter, std::next(siter, diff));
		oldIndex = index;
		elementsInSet -= elementsDeletedAtOnce;
		
//		if ( i % 1000 == 0 ) {
//			QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
//			QVERIFY(cset.checkBlackDepth());
//		} //if ( i % 1000 == 0 )
	} //for ( int i = 0; i < elementsInSet; ++i )
	QVERIFY(std::equal(cset.begin(), cset.end(), sset.begin(), sset.end()));
	QVERIFY(cset.checkBlackDepth());
	p.Print = false;
	return;
}
