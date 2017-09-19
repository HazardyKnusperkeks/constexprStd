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
 * @brief Checks constexprStd/memory for self-containment and contains the tests.
 */

#include <constexprStd/memory>

#include "../test.hpp"

#include <memory>
#include <tuple>

#include <constexprStd/iterator>

void TestConstexprStd::testAllocator(void) const noexcept {
	struct DummyT {
		int X = 7;
		int Y = 8;
		
		constexpr DummyT(void) noexcept {}
		constexpr explicit DummyT(const int x) noexcept : X(x) {}
		constexpr explicit DummyT(const int x, const int y) noexcept : X(x), Y(y) {}
	};
	
	auto l = [](void) constexpr noexcept {
			constexprStd::allocator<DummyT, 25> a;
			DummyT *first  = a.allocate(1);
			DummyT *second = a.allocate(5);
			DummyT *third  = a.allocate(3);
			auto t1 = std::tuple{constexprStd::distance(first, second), constexprStd::distance(second, third)};
			
			a.deallocate(second, 5);
			second = a.allocate(7);
			DummyT *fourth = a.allocate(2);
			
			auto t2 = std::tuple{constexprStd::distance(third, second), constexprStd::distance(first, fourth)};
			
			a.construct(first);
			auto t3 = std::tuple{first->X, first->Y};
			
			a.construct(&second[0], 1);
			auto t4 = std::tuple{second[0].X, second[0].Y};
			
			auto secondOne = a.getPointer(second, 1);
			a.construct(secondOne, 1, 2);
			auto t5 = std::tuple{secondOne->X, secondOne->Y};
			
			auto secondTwo = a.getPointer(second, 2, static_cast<std::size_t>(constexprStd::distance(first, second)));
			a.construct(secondTwo, 3, 4);
			auto t6 = std::tuple{secondTwo->X, secondTwo->Y};
			
			auto secondThree = a.getPointer(second, 3,
			                                static_cast<std::size_t>(constexprStd::distance(first, second)) + 1);
			a.construct(secondThree, 5, 6);
			auto t7 = std::tuple{secondThree->X, secondThree->Y};
			return std::tuple_cat(t1, t2, t3, t4, t5, t6, t7);
		};
	
	static_assert(l() == std::tuple{1, 5, 3, 1, 7, 8, 1, 8, 1, 2, 3, 4, 5, 6});
	
	constexprStd::allocator<DummyT, 50> a;
	DummyT *first  = a.allocate(1);
	DummyT *second = a.allocate(5);
	DummyT *third  = a.allocate(3);
	QCOMPARE(constexprStd::distance(first,  second), 1);
	QCOMPARE(constexprStd::distance(second, third),  5);
	
	a.deallocate(second, 5);
	second = a.allocate(7);
	QCOMPARE(constexprStd::distance(third, second),  3);
	
	DummyT *fourth = a.allocate(2);
	QCOMPARE(constexprStd::distance(first, fourth),  1);
	
	a.construct(first);
	QCOMPARE(first->X, 7);
	QCOMPARE(first->Y, 8);
	
	a.construct(&second[0], 1);
	QCOMPARE(second[0].X, 1);
	QCOMPARE(second[0].Y, 8);
	
	a.construct(&second[1], 1, 2);
	QCOMPARE(second[1].X, 1);
	QCOMPARE(second[1].Y, 2);
	
	auto secondTwo = a.getPointer(second, 2, static_cast<std::size_t>(constexprStd::distance(first, second)));
	a.construct(secondTwo, 3, 4);
	QCOMPARE(secondTwo->X, 3);
	QCOMPARE(secondTwo->Y, 4);
	
	auto secondThree = a.getPointer(second, 3, static_cast<std::size_t>(constexprStd::distance(first, second)) + 1);
	a.construct(secondThree, 5, 6);
	QCOMPARE(secondThree->X, 5);
	QCOMPARE(secondThree->Y, 6);
	return;
}
