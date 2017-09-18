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
 * @brief Checks constexprStd/details/uninitialized.hpp for self-containment and contains the tests.
 */

#include <constexprStd/details/uninitialized.hpp>

#include "../test.hpp"

#include <string>
#include <type_traits>

#include "test_container.hpp"
#include "test_constants.hpp"
#include "test_count_instances.hpp"

void TestConstexprStd::testUninitialized(void) const noexcept {
	using constexprStd::details::Uninitialized;
	
	//Test the literal variant
	static_assert(std::is_trivially_destructible_v<TestContainer>);
	static_assert(std::is_literal_type_v<TestContainer>);
	
	constexpr Uninitialized<TestContainer> cc1, cc2{7, 8, 9};
	static_assert(cc1.get().at(4) == 5);
	static_assert(cc2.get().at(1) == 8);
	static_assert(Uninitialized<TestContainer>{}.get().at(1) == 2);
	static_assert([]{Uninitialized<TestContainer> c{}; return c.get().at(1); }() == 2);
	
	Uninitialized<TestContainer> rc1, rc2{9, 8, 7};
	QCOMPARE(cc1.get().at(4), 5);
	QCOMPARE(cc2.get().at(1), 8);
	QCOMPARE(Uninitialized<TestContainer>{}.get().at(1), 2);
	QCOMPARE([]{Uninitialized<TestContainer> c{}; return c.get().at(1); }(), 2);
	
	//Test the non literal variant
	static_assert(!std::is_trivially_destructible_v<CountInstances<std::string>>);
	static_assert(!std::is_literal_type_v<CountInstances<std::string>>);
	
	using UninitString = Uninitialized<CountInstances<std::string>>;
	
	auto count = [](void) -> int& {
			return CountInstances<std::string>::Instances;
		};
	
	count() = 0;
	
	/* Basicly we leak the content of the strings, because their destructor never get called. But the strings will most
	 * likely be on the stack, because of the short string optimization. But even if they are not, the leak is only in
	 * this test. The correct usage is to manually call the destructor, like in our last temp. */
	UninitString s1;
	QCOMPARE(count(), 0);
	
	s1.init();
	QCOMPARE(count(), 1);
	QCOMPARE(s1.get(), emptyString);
	
	const UninitString s2{barString};
	QCOMPARE(s2.get(), barString);
	QCOMPARE(count(), 2);
	
	{
		UninitString temp{fooString};
		QCOMPARE(temp.get(), fooString);
		QCOMPARE(count(), 3);
		
		QCOMPARE(UninitString{barString}.get(), barString);
		QCOMPARE(count(), 4);
	}
	
	//The objects didn't get destroied!
	QCOMPARE(count(), 4);
	
	{
		UninitString temp{longString};
		QCOMPARE(count(), 5);
		temp.get().~CountInstances<std::string>();
		QCOMPARE(count(), 4);
	}
	QCOMPARE(count(), 4);
	
	s1.deinit();
	QCOMPARE(count(), 3);
	
	//Reset for future tests
	count() = 0;
	return;
}
