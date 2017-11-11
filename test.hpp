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

#ifndef TEST_HPP
#define TEST_HPP

#include <QTest>

#include <string>

class TestConstexprStd : public QObject {
	Q_OBJECT
	private slots:
	//Internals
	void testUninitialized(void) const noexcept;
	
	//Algorithm lib
	//Non-modifying sequence operations
	void testAllAnyNone(void) const noexcept;
	void testForEach(void) const noexcept;
	void testForEachN(void) const noexcept;
	void testCount(void) const noexcept;
	void testCountIf(void) const noexcept;
	void testMismatch(void) const noexcept;
	void testEqual(void) const noexcept;
	void testUnequal(void) const noexcept;
	void testFind(void) const noexcept;
	void testFindIf(void) const noexcept;
	void testFindIfNot(void) const noexcept;
	void testFindEnd(void) const noexcept;
	void testFindFirstOf(void) const noexcept;
	void testAdjacentFind(void) const noexcept;
	void testSearch(void) const noexcept;
	void testSearchSearcher(void) const noexcept;
	void testSearchN(void) const noexcept;
	
	//Modifying sequence operations
	void testCopy(void) const noexcept;
	void testCopyIf(void) const noexcept;
	void testCopyN(void) const noexcept;
	void testCopyBackward(void) const noexcept;
	void testMove(void) const noexcept;
	void testMoveBackward(void) const noexcept;
	void testFill(void) const noexcept;
	void testFillN(void) const noexcept;
	void testTransform(void) const noexcept;
	void testGenerate(void) const noexcept;
	void testGenerateN(void) const noexcept;
	void testRemoveIf(void) const noexcept;
	void testRemoveCopyIf(void) const noexcept;
	void testReplaceIf(void) const noexcept;
	void testReplaceCopyIf(void) const noexcept;
	void testSwapRanges(void) const noexcept;
	void testReverse(void) const noexcept;
	void testReverseCopy(void) const noexcept;
	void testRotate(void) const noexcept;
	void testRotateCopy(void) const noexcept;
	void testUnique(void) const noexcept;
	void testUniqueCopy(void) const noexcept;
	
	//Partitioning operations
	void testIsPartioned(void) const noexcept;
	void testPartition(void) const noexcept;
	void testPartitionCopy(void) const noexcept;
	void testStablePartition(void) const noexcept;
	void testPartitionPoint(void) const noexcept;
	
	//Sorting operations
	void testIsSortedUntil(void) const noexcept;
	void testIsSorted(void) const noexcept;
	
	//Heap operations
	void testIsHeapUntil(void) const noexcept;
	void testIsHeap(void) const noexcept;
	void testPushHeap(void) const noexcept;
	void testMakeHeap(void) const noexcept;
	void testPopHeap(void) const noexcept;
	
	//Minimum/maximum operations
	void testLexicographicalCompare(void) const noexcept;
	
	//Permutations
	void testIsPermutation(void) const noexcept;
	
	//Function objects
	//Polymorphic function wrappers
	void testInvoke(void) const noexcept;
	
	//Negators
	void testNotFn(void) const noexcept;
	
	//Searcher
	void testDefaultSeracher(void) const noexcept;
	
	//Iterator lib
	//Iterator adaptors
	void testInsert_iterator(void) const noexcept;
	
	//Iterator operations
	void testAdvance(void) const noexcept;
	void testDistance(void) const noexcept;
	void testNext(void) const noexcept;
	void testPrev(void) const noexcept;
	
	//Dynamic memory management
	//Allocators
	void testAllocator(void) const noexcept;
	
	//Utility lib
	void testBitset(void) const noexcept;
	
	//Variant
	void testVariantStaticAsserts(void) const noexcept;
	void testVariantConstructor(void) const noexcept;
	void testVariantDestructor(void) const noexcept;
	void testVariantInteraction(void) const noexcept;
	void testVariantEmplace(void) const noexcept;
	void testVariantCopyAssign(void) const noexcept;
	void testVariantMoveAssign(void) const noexcept;
	void testVariantConvertAssign(void) const noexcept;
	void testVariantCompare(void) const noexcept;
	
	//Variant extension
	void testDifferentVariantCompare(void) const noexcept;
	void testDifferentVariantAssign(void) const noexcept;
	
	//Pairs and tuples
	void testPair(void) const noexcept;
	void testPairComparison(void) const noexcept;
	void testPairComparison_data(void) const noexcept;
	
	//Swap, forward and move
	void testExchange(void) const noexcept;
	void testSwap(void) const noexcept;
	
	//Container
	void testSet(void) const noexcept;
	void testSetTransparentCompare(void) const noexcept;
	void testSetErase(void) const noexcept;
	void testSetComparisonOperators(void) const noexcept;
	void testSetFailCaseOne(void) const noexcept;
	void testSetRandom(void) const noexcept;
	
	public:
	explicit TestConstexprStd(QObject *parent = nullptr);
};

namespace QTest {
inline bool qCompare(const std::string& t1, const char *t2, const char *actual, const char *expected, const char *file,
                     const int line) {
	return compare_string_helper(t1.c_str(), t2, actual, expected, file, line);
}
} //namespace QTest

#endif
