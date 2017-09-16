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
#include <type_traits>
#include <utility>
#include <variant>

#include <constexprStd/algorithm>
#include <constexprStd/functional>
#include <constexprStd/iterator>
#include <constexprStd/utility>
#include <constexprStd/variant>

using namespace std::string_literals;

namespace {
struct Tag { };

struct LiteralThrows {
	int I = 0;
	
	static inline bool MoveThrows = false;
	
	constexpr LiteralThrows(void) noexcept = default;
	constexpr explicit LiteralThrows(const int i) noexcept : I(i) {}
	explicit LiteralThrows(double) {
		throw std::exception{};
	}
	
	constexpr LiteralThrows(const LiteralThrows& that) noexcept(false) : I(that.I) {
		return;
	}
	
	constexpr LiteralThrows(LiteralThrows&& that) : I(that.I) {
		if ( MoveThrows ) {
			throw std::bad_cast{};
		} //if ( MoveThrows )
		that.I = -1;
		return;
	}
	
	constexpr LiteralThrows& operator=(const LiteralThrows& that) {
		if ( that.I == 42 ) {
			throw std::bad_cast{};
		} //if ( that.I == 42 )
		I = that.I;
		return *this;
	}
	
	LiteralThrows& operator=(LiteralThrows&& that) {
		if ( MoveThrows ) {
			throw std::bad_cast{};
		} //if ( MoveThrows )
		I = constexprStd::exchange(that.I, -1);
		return *this;
	}
	
	constexpr bool operator==(const LiteralThrows& l) const noexcept {
		return I == l.I;
	}
	
	constexpr bool operator<(const LiteralThrows& i) const noexcept {
		return I < i.I;
	}
};

constexpr bool operator!=(const LiteralThrows& lhs, const LiteralThrows& rhs) noexcept {
	return !(lhs == rhs);
}

constexpr bool operator> (const LiteralThrows& lhs, const LiteralThrows& rhs) noexcept {
	return rhs < lhs;
}

constexpr bool operator<=(const LiteralThrows& lhs, const LiteralThrows& rhs) noexcept {
	return !(lhs > rhs);
}

constexpr bool operator>=(const LiteralThrows& lhs, const LiteralThrows& rhs) noexcept {
	return !(lhs < rhs);
}

static_assert(std::is_trivially_destructible_v<LiteralThrows>);

class LiteralThrowsMoveThrower {
	public:
	LiteralThrowsMoveThrower(void) {
		activate();
		return;
	}
	
	~LiteralThrowsMoveThrower(void) {
		deactivate();
		return;
	}
	
	LiteralThrowsMoveThrower(const LiteralThrowsMoveThrower&) = delete;
	
	void activate(void) const {
		LiteralThrows::MoveThrows = true;
		return;
	}
	
	void deactivate(void) const {
		LiteralThrows::MoveThrows = false;
		return;
	}
};

class TestContainer : public std::array<int, 10> {
	public:
	constexpr TestContainer(std::initializer_list<int> il) noexcept : std::array<int, 10>{} {
		auto listBegin = il.begin();
		const auto listEnd = il.end();
		auto arrayBegin = begin();
		const auto arrayEnd = end();
		
		for ( ; listBegin != listEnd && arrayBegin != arrayEnd; ++listBegin, ++arrayBegin ) {
			*arrayBegin = *listBegin;
		} //for ( ; listBegin != listEnd && arrayBegin != arrayEnd; ++listBegin, ++arrayBegin )
		return;
	}
	
	constexpr TestContainer(void) noexcept : std::array<int, 10>{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}} {
		return;
	}
	
	constexpr TestContainer(std::array<int, 10> a) noexcept : std::array<int, 10>{a} {
		return;
	}
	
	constexpr TestContainer(const Tag, int a, const int b, const int c = 1) noexcept : std::array<int, 10>{} {
		for ( std::size_t i = 0; i < 10u && a <= b; ++i, a +=c ) {
			operator[](i) = a;
		} //for ( std::size_t i = 0; i < 10u && a <= b; ++i, a += c )
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

constexpr bool operator==(const TestContainer& c, const std::array<int, 10>& a) noexcept {
	return c.operator==(a);
}

struct NoDefault {
	explicit constexpr NoDefault(int) noexcept {
		return;
	}
	
	constexpr bool operator==(const NoDefault&) const noexcept { return true; }
};

struct NotMovable {
	constexpr NotMovable(void) = default;
	constexpr NotMovable(const NotMovable&) = delete;
	constexpr NotMovable(NotMovable&&) = delete;
};

struct NotCopyable {
	constexpr NotCopyable(void) = default;
	NotCopyable(const NotCopyable&) = delete;
	constexpr NotCopyable(NotCopyable&&) = default;
	constexpr NotCopyable& operator=(NotCopyable&&) = default;
};

struct Bool {
	bool B;
	Bool() noexcept : B(false) {}
	explicit Bool(bool b) noexcept : B(b) {}
};

template<typename T>
struct CountInstances : public T {
	static inline int Instances = 0;
	
	template<typename... Args>
	CountInstances(Args&&... args) : T{std::forward<Args>(args)...} {
		++Instances;
		return;
	}
	
	CountInstances(const CountInstances&) = default;
	CountInstances(CountInstances&&) = default;
	CountInstances& operator=(const CountInstances&) = default;
	CountInstances& operator=(CountInstances&&) = default;
	
	~CountInstances(void) noexcept {
		--Instances;
		return;
	}
};

using CV1 = constexprStd::variant<int, bool, char, std::pair<double, double>>;
using CV2 = constexprStd::variant<TestContainer, int, bool(*)(const int) noexcept, NotCopyable>;
using CV3 = constexprStd::variant<NoDefault, int, NotCopyable, bool>;
using CV4 = constexprStd::variant<std::monostate, NoDefault, int, bool>;
using CV5 = constexprStd::variant<NotMovable, NoDefault, int, bool>;
using CV6 = constexprStd::variant<NoDefault, int, bool>;
using RV1 = constexprStd::variant<std::string, bool, bool(*)(const int)>;
using RV2 = constexprStd::variant<std::string, bool, NotCopyable>;
using RV3 = constexprStd::variant<NoDefault, NotCopyable, std::forward_list<int>, std::string, CountInstances<Bool>>;
using RV4 = constexprStd::variant<std::monostate, NoDefault, std::forward_list<int>, std::string>;
using RV5 = constexprStd::variant<NotMovable, std::forward_list<int>, std::string>;
using RV6 = constexprStd::variant<NoDefault, std::string, bool>;
using SV1 =          std::variant<std::string, bool, bool(*)(const int)>;
using SV2 =          std::variant<std::string, bool, NotCopyable>;
using SV3 =          std::variant<NoDefault, NotCopyable, std::forward_list<int>, std::string, CountInstances<Bool>>;
using SV4 =          std::variant<std::monostate, NoDefault, std::forward_list<int>, std::string>;
using SV5 =          std::variant<NotMovable, std::forward_list<int>, std::string>;
using SV6 =          std::variant<NoDefault, std::string, bool>;

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

static constexpr inline const char *fooString    = "foo";
static constexpr inline const char *barString    = "bar";
static constexpr inline const char *bazString    = "baz";
static constexpr inline const char *emptyString  = "";
static constexpr inline const char *longString   = "This is a long string, to be allocated on the heap!";
static const     inline std::string fooStrings   = fooString;
static const     inline std::string barStrings   = barString;
static const     inline std::string bazStrings   = bazString;
static const     inline std::string emptyStrings = emptyString;
static const     inline std::string longStrings  = longString;

} //namespace

namespace QTest {
static bool qCompare(const std::string& t1, const char *t2, const char *actual, const char *expected, const char *file,
                     const int line) {
	return compare_string_helper(t1.c_str(), t2, actual, expected, file, line);
}
static bool qCompare(const Bool& t1, const bool& t2, const char *actual, const char *expected, const char *file,
                     const int line) {
	return qCompare(t1.B, t2, actual, expected, file, line);
}
static bool qCompare(const LiteralThrows& t1, const int& t2, const char *actual, const char *expected, const char *file,
                     const int line) {
	return qCompare(t1, LiteralThrows{t2}, actual, expected, file, line);
}
template<typename T1, typename T2>
static bool qCompare(const CountInstances<T1>& t1, const T2& t2, const char *actual, const char *expected,
                     const char *file, const int line) {
	return qCompare(static_cast<const T1&>(t1), t2, actual, expected, file, line);
}
} //namespace QTest

class TestConstexprStd : public QObject {
	Q_OBJECT
	private slots:
	//Algorithm lib
	//Non-modifying sequence operations
	void testCount(void) const noexcept;
	void testCountIf(void) const noexcept;
	void testEqual(void) const noexcept;
	void testUnequal(void) const noexcept;
	void testFind(void) const noexcept;
	void testFindIf(void) const noexcept;
	void testFindIfNot(void) const noexcept;
	
	//Modifying sequence operations
	void testCopy(void) const noexcept;
	
	//Permutations
	void testIsPermutation(void) const noexcept;
	
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
	void testNext(void) const noexcept;
	
	//Utility lib
	//Variant
	void testUninitialized(void) const noexcept;
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
	
	//Swap, forward and move
	void testExchange(void) const noexcept;
	void testSwap(void) const noexcept;
	
	public:
	explicit TestConstexprStd(QObject *parent = nullptr) : QObject(parent) { return; }
};

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
	
	//Reset for future tests
	count() = 0;
	return;
}

void TestConstexprStd::testVariantStaticAsserts(void) const noexcept {
	static_assert(constexprStd::variant_size_v<RV1> == 3);
	static_assert(constexprStd::variant_size_v<RV2> == 3);
	static_assert(constexprStd::variant_size_v<RV3> == 5);
	static_assert(constexprStd::variant_size_v<RV4> == 4);
	static_assert(constexprStd::variant_size_v<RV5> == 3);
	static_assert(constexprStd::variant_size_v<RV6> == 3);
	
	static_assert(std::variant_size_v<RV1> == 3);
	static_assert(std::variant_size_v<RV2> == 3);
	static_assert(std::variant_size_v<RV3> == 5);
	static_assert(std::variant_size_v<RV4> == 4);
	static_assert(std::variant_size_v<RV5> == 3);
	static_assert(std::variant_size_v<RV6> == 3);
	
	static_assert(constexprStd::variant_size_v<SV1> == 3);
	static_assert(constexprStd::variant_size_v<SV2> == 3);
	static_assert(constexprStd::variant_size_v<SV3> == 5);
	static_assert(constexprStd::variant_size_v<SV4> == 4);
	static_assert(constexprStd::variant_size_v<SV5> == 3);
	static_assert(constexprStd::variant_size_v<SV6> == 3);
	
	static_assert(std::variant_size_v<SV1> == 3);
	static_assert(std::variant_size_v<SV2> == 3);
	static_assert(std::variant_size_v<SV3> == 5);
	static_assert(std::variant_size_v<SV4> == 4);
	static_assert(std::variant_size_v<SV5> == 3);
	static_assert(std::variant_size_v<SV6> == 3);
	
	static_assert(std::is_same_v<std::variant_alternative_t<0, RV1>, std::string>);
	static_assert(std::is_same_v<std::variant_alternative_t<1, RV1>, bool>);
	static_assert(std::is_same_v<std::variant_alternative_t<2, RV4>, std::forward_list<int>>);
	
	static_assert(std::is_same_v<constexprStd::variant_alternative_t<0, RV1>, std::string>);
	static_assert(std::is_same_v<constexprStd::variant_alternative_t<1, RV1>, bool>);
	static_assert(std::is_same_v<constexprStd::variant_alternative_t<2, RV4>, std::forward_list<int>>);
	
	static_assert(std::is_same_v<std::variant_alternative_t<0, SV1>, std::string>);
	static_assert(std::is_same_v<std::variant_alternative_t<1, SV1>, bool>);
	static_assert(std::is_same_v<std::variant_alternative_t<2, SV4>, std::forward_list<int>>);
	
	static_assert(std::is_same_v<constexprStd::variant_alternative_t<0, SV1>, std::string>);
	static_assert(std::is_same_v<constexprStd::variant_alternative_t<1, SV1>, bool>);
	static_assert(std::is_same_v<constexprStd::variant_alternative_t<2, SV4>, std::forward_list<int>>);
	
	//Constructors
	//(1)
	static_assert( std::is_default_constructible_v<CV1>);
	static_assert( std::is_default_constructible_v<CV2>);
	static_assert(!std::is_default_constructible_v<CV3>);
	static_assert( std::is_default_constructible_v<CV4>);
	static_assert( std::is_default_constructible_v<CV5>);
	static_assert(!std::is_default_constructible_v<CV6>);
	
	static_assert( std::is_default_constructible_v<RV1>);
	static_assert( std::is_default_constructible_v<RV2>);
	static_assert(!std::is_default_constructible_v<RV3>);
	static_assert( std::is_default_constructible_v<RV4>);
	static_assert( std::is_default_constructible_v<RV5>);
	static_assert(!std::is_default_constructible_v<RV6>);
	
	static_assert( std::is_default_constructible_v<SV1>);
	static_assert( std::is_default_constructible_v<SV2>);
	static_assert(!std::is_default_constructible_v<SV3>);
	static_assert( std::is_default_constructible_v<SV4>);
	static_assert( std::is_default_constructible_v<SV5>);
	static_assert(!std::is_default_constructible_v<SV6>);
	
	//(2)
	static_assert( std::is_copy_constructible_v<CV1>);
	static_assert(!std::is_copy_constructible_v<CV2>);
	static_assert(!std::is_copy_constructible_v<CV3>);
	static_assert( std::is_copy_constructible_v<CV4>);
	static_assert(!std::is_copy_constructible_v<CV5>);
	static_assert( std::is_copy_constructible_v<CV6>);
	
	static_assert( std::is_copy_constructible_v<RV1>);
	static_assert(!std::is_copy_constructible_v<RV2>);
	static_assert(!std::is_copy_constructible_v<RV3>);
	static_assert( std::is_copy_constructible_v<RV4>);
	static_assert(!std::is_copy_constructible_v<RV5>);
	static_assert( std::is_copy_constructible_v<RV6>);
	
	static_assert( std::is_copy_constructible_v<SV1>);
	static_assert(!std::is_copy_constructible_v<SV2>);
	static_assert(!std::is_copy_constructible_v<SV3>);
	static_assert( std::is_copy_constructible_v<SV4>);
	static_assert(!std::is_copy_constructible_v<SV5>);
	static_assert( std::is_copy_constructible_v<SV6>);
	
	//(3)
	static_assert( std::is_move_constructible_v<CV1>);
	static_assert( std::is_move_constructible_v<CV2>);
	static_assert( std::is_move_constructible_v<CV3>);
	static_assert( std::is_move_constructible_v<CV4>);
	static_assert(!std::is_move_constructible_v<CV5>);
	static_assert( std::is_move_constructible_v<CV6>);
	
	static_assert( std::is_move_constructible_v<RV1>);
	static_assert( std::is_move_constructible_v<RV2>);
	static_assert( std::is_move_constructible_v<RV3>);
	static_assert( std::is_move_constructible_v<RV4>);
	static_assert(!std::is_move_constructible_v<RV5>);
	static_assert( std::is_move_constructible_v<RV6>);
	
	static_assert( std::is_move_constructible_v<SV1>);
	static_assert( std::is_move_constructible_v<SV2>);
	static_assert( std::is_move_constructible_v<SV3>);
	static_assert( std::is_move_constructible_v<SV4>);
	static_assert(!std::is_move_constructible_v<SV5>);
	static_assert( std::is_move_constructible_v<SV6>);
	
	//(4)
	static_assert( std::is_constructible_v<CV2, std::array<int, 10>>);
	static_assert( std::is_constructible_v<CV2, std::initializer_list<int>>);
	static_assert(!std::is_constructible_v<CV2, std::string>);
	
	static_assert( std::is_constructible_v<RV4, std::string>);
	static_assert( std::is_constructible_v<RV4, std::initializer_list<int>>);
	static_assert(!std::is_constructible_v<RV4, bool>);
	static_assert(!std::is_constructible_v<RV4, int>);
	
	static_assert( std::is_constructible_v<SV4, std::string>);
	static_assert( std::is_constructible_v<SV4, std::initializer_list<int>>);
	static_assert(!std::is_constructible_v<SV4, bool>);
	static_assert(!std::is_constructible_v<SV4, int>);
	
	//(5)
	static_assert( std::is_constructible_v<CV2, std::in_place_type_t<TestContainer>, std::array<int, 10>>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_type_t<int>, std::array<int, 10>>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_type_t<bool(*)(const int) noexcept>,
	                                       std::array<int, 10>>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_type_t<NotCopyable>, std::array<int, 10>>);
	
	static_assert(!std::is_constructible_v<RV4, std::in_place_type_t<std::monostate>, int, int>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_type_t<NoDefault>, int, int>);
	static_assert( std::is_constructible_v<RV4, std::in_place_type_t<std::forward_list<int>>, int, int>);
	static_assert( std::is_constructible_v<RV4, std::in_place_type_t<std::string>, int, int>);
	static_assert( std::is_constructible_v<RV4, std::in_place_type_t<std::monostate>>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_type_t<NoDefault>>);
	static_assert( std::is_constructible_v<RV4, std::in_place_type_t<std::forward_list<int>>>);
	static_assert( std::is_constructible_v<RV4, std::in_place_type_t<std::string>>);
	
	static_assert(!std::is_constructible_v<SV4, std::in_place_type_t<std::monostate>, int, int>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_type_t<NoDefault>, int, int>);
	static_assert( std::is_constructible_v<SV4, std::in_place_type_t<std::forward_list<int>>, int, int>);
	static_assert( std::is_constructible_v<SV4, std::in_place_type_t<std::string>, int, int>);
	static_assert( std::is_constructible_v<SV4, std::in_place_type_t<std::monostate>>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_type_t<NoDefault>>);
	static_assert( std::is_constructible_v<SV4, std::in_place_type_t<std::forward_list<int>>>);
	static_assert( std::is_constructible_v<SV4, std::in_place_type_t<std::string>>);
	
	//(6)
	using iil = std::initializer_list<int>;
	using cil = std::initializer_list<char>;
	
	static_assert( std::is_constructible_v<CV2, std::in_place_type_t<TestContainer>,               iil&>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_type_t<int>,                         iil&>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_type_t<bool(*)(const int) noexcept>, iil&>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_type_t<NotCopyable>,                 iil&>);
	
	static_assert(!std::is_constructible_v<RV4, std::in_place_type_t<std::monostate>,         iil&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_type_t<std::monostate>,         cil&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_type_t<NoDefault>,              iil&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_type_t<NoDefault>,              cil&>);
	static_assert( std::is_constructible_v<RV4, std::in_place_type_t<std::forward_list<int>>, iil&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_type_t<std::forward_list<int>>, cil&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_type_t<std::string>,            iil&>);
	static_assert( std::is_constructible_v<RV4, std::in_place_type_t<std::string>,            cil&>);
	
	static_assert(!std::is_constructible_v<SV4, std::in_place_type_t<std::monostate>,         iil&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_type_t<std::monostate>,         cil&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_type_t<NoDefault>,              iil&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_type_t<NoDefault>,              cil&>);
	static_assert( std::is_constructible_v<SV4, std::in_place_type_t<std::forward_list<int>>, iil&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_type_t<std::forward_list<int>>, cil&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_type_t<std::string>,            iil&>);
	static_assert( std::is_constructible_v<SV4, std::in_place_type_t<std::string>,            cil&>);
	
	//(7)
	static_assert( std::is_constructible_v<CV2, std::in_place_index_t<0>, std::array<int, 10>>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_index_t<1>, std::array<int, 10>>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_index_t<2>, std::array<int, 10>>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_index_t<3>, std::array<int, 10>>);
	
	static_assert(!std::is_constructible_v<RV4, std::in_place_index_t<0>, int, int>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_index_t<1>, int, int>);
	static_assert( std::is_constructible_v<RV4, std::in_place_index_t<2>, int, int>);
	static_assert( std::is_constructible_v<RV4, std::in_place_index_t<3>, int, int>);
	static_assert( std::is_constructible_v<RV4, std::in_place_index_t<0>>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_index_t<1>>);
	static_assert( std::is_constructible_v<RV4, std::in_place_index_t<2>>);
	static_assert( std::is_constructible_v<RV4, std::in_place_index_t<3>>);
	
	static_assert(!std::is_constructible_v<SV4, std::in_place_index_t<0>, int, int>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_index_t<1>, int, int>);
	static_assert( std::is_constructible_v<SV4, std::in_place_index_t<2>, int, int>);
	static_assert( std::is_constructible_v<SV4, std::in_place_index_t<3>, int, int>);
	static_assert( std::is_constructible_v<SV4, std::in_place_index_t<0>>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_index_t<1>>);
	static_assert( std::is_constructible_v<SV4, std::in_place_index_t<2>>);
	static_assert( std::is_constructible_v<SV4, std::in_place_index_t<3>>);
	
	//(8)
	static_assert( std::is_constructible_v<CV2, std::in_place_index_t<0>, std::initializer_list<int>&>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_index_t<1>, std::initializer_list<int>&>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_index_t<2>, std::initializer_list<int>&>);
	static_assert(!std::is_constructible_v<CV2, std::in_place_index_t<3>, std::initializer_list<int>&>);
	
	static_assert(!std::is_constructible_v<RV4, std::in_place_index_t<0>, std::initializer_list<int>&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_index_t<0>, std::initializer_list<char>&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_index_t<1>, std::initializer_list<int>&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_index_t<1>, std::initializer_list<char>&>);
	static_assert( std::is_constructible_v<RV4, std::in_place_index_t<2>, std::initializer_list<int>&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_index_t<2>, std::initializer_list<char>&>);
	static_assert(!std::is_constructible_v<RV4, std::in_place_index_t<3>, std::initializer_list<int>&>);
	static_assert( std::is_constructible_v<RV4, std::in_place_index_t<3>, std::initializer_list<char>&>);
	
	static_assert(!std::is_constructible_v<SV4, std::in_place_index_t<0>, std::initializer_list<int>&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_index_t<0>, std::initializer_list<char>&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_index_t<1>, std::initializer_list<int>&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_index_t<1>, std::initializer_list<char>&>);
	static_assert( std::is_constructible_v<SV4, std::in_place_index_t<2>, std::initializer_list<int>&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_index_t<2>, std::initializer_list<char>&>);
	static_assert(!std::is_constructible_v<SV4, std::in_place_index_t<3>, std::initializer_list<int>&>);
	static_assert( std::is_constructible_v<SV4, std::in_place_index_t<3>, std::initializer_list<char>&>);
	
	//Getter
	static_assert(std::is_same_v<decltype(std::get<0>(std::declval<      RV4>())),
	                             decltype(std::get<0>(std::declval<      SV4>()))>);
	static_assert(std::is_same_v<decltype(std::get<0>(std::declval<const RV4>())),
	                             decltype(std::get<0>(std::declval<const SV4>()))>);
	static_assert(std::is_same_v<decltype(std::get<0>(std::declval<      RV4&>())),
	                             decltype(std::get<0>(std::declval<      SV4&>()))>);
	static_assert(std::is_same_v<decltype(std::get<0>(std::declval<const RV4&>())),
	                             decltype(std::get<0>(std::declval<const SV4&>()))>);
	static_assert(std::is_same_v<decltype(std::get<0>(std::declval<      RV4&&>())),
	                             decltype(std::get<0>(std::declval<      SV4&&>()))>);
	static_assert(std::is_same_v<decltype(std::get<0>(std::declval<const RV4&&>())),
	                             decltype(std::get<0>(std::declval<const SV4&&>()))>);
	
	static_assert(std::is_same_v<decltype(std::get<1>(std::declval<      RV4>())),
	                             decltype(std::get<1>(std::declval<      SV4>()))>);
	static_assert(std::is_same_v<decltype(std::get<1>(std::declval<const RV4>())),
	                             decltype(std::get<1>(std::declval<const SV4>()))>);
	static_assert(std::is_same_v<decltype(std::get<1>(std::declval<      RV4&>())),
	                             decltype(std::get<1>(std::declval<      SV4&>()))>);
	static_assert(std::is_same_v<decltype(std::get<1>(std::declval<const RV4&>())),
	                             decltype(std::get<1>(std::declval<const SV4&>()))>);
	static_assert(std::is_same_v<decltype(std::get<1>(std::declval<      RV4&&>())),
	                             decltype(std::get<1>(std::declval<      SV4&&>()))>);
	static_assert(std::is_same_v<decltype(std::get<1>(std::declval<const RV4&&>())),
	                             decltype(std::get<1>(std::declval<const SV4&&>()))>);
	
	static_assert(std::is_same_v<decltype(std::get<2>(std::declval<      RV4>())),
	                             decltype(std::get<2>(std::declval<      SV4>()))>);
	static_assert(std::is_same_v<decltype(std::get<2>(std::declval<const RV4>())),
	                             decltype(std::get<2>(std::declval<const SV4>()))>);
	static_assert(std::is_same_v<decltype(std::get<2>(std::declval<      RV4&>())),
	                             decltype(std::get<2>(std::declval<      SV4&>()))>);
	static_assert(std::is_same_v<decltype(std::get<2>(std::declval<const RV4&>())),
	                             decltype(std::get<2>(std::declval<const SV4&>()))>);
	static_assert(std::is_same_v<decltype(std::get<2>(std::declval<      RV4&&>())),
	                             decltype(std::get<2>(std::declval<      SV4&&>()))>);
	static_assert(std::is_same_v<decltype(std::get<2>(std::declval<const RV4&&>())),
	                             decltype(std::get<2>(std::declval<const SV4&&>()))>);
	
	static_assert(std::is_same_v<decltype(std::get<3>(std::declval<      RV4>())),
	                             decltype(std::get<3>(std::declval<      SV4>()))>);
	static_assert(std::is_same_v<decltype(std::get<3>(std::declval<const RV4>())),
	                             decltype(std::get<3>(std::declval<const SV4>()))>);
	static_assert(std::is_same_v<decltype(std::get<3>(std::declval<      RV4&>())),
	                             decltype(std::get<3>(std::declval<      SV4&>()))>);
	static_assert(std::is_same_v<decltype(std::get<3>(std::declval<const RV4&>())),
	                             decltype(std::get<3>(std::declval<const SV4&>()))>);
	static_assert(std::is_same_v<decltype(std::get<3>(std::declval<      RV4&&>())),
	                             decltype(std::get<3>(std::declval<      SV4&&>()))>);
	static_assert(std::is_same_v<decltype(std::get<3>(std::declval<const RV4&&>())),
	                             decltype(std::get<3>(std::declval<const SV4&&>()))>);
	
	//Assignments
	//Copy assignment
	static_assert( std::is_copy_assignable_v<CV1>);
	static_assert(!std::is_copy_assignable_v<CV2>);
	static_assert(!std::is_copy_assignable_v<CV3>);
	static_assert( std::is_copy_assignable_v<CV4>);
	static_assert(!std::is_copy_assignable_v<CV5>);
	static_assert( std::is_copy_assignable_v<CV6>);
	
	static_assert( std::is_copy_assignable_v<RV1>);
	static_assert(!std::is_copy_assignable_v<RV2>);
	static_assert(!std::is_copy_assignable_v<RV3>);
	static_assert( std::is_copy_assignable_v<RV4>);
	static_assert(!std::is_copy_assignable_v<RV5>);
	static_assert( std::is_copy_assignable_v<RV6>);
	
	static_assert( std::is_copy_assignable_v<SV1>);
	static_assert(!std::is_copy_assignable_v<SV2>);
	static_assert(!std::is_copy_assignable_v<SV3>);
	static_assert( std::is_copy_assignable_v<SV4>);
	static_assert(!std::is_copy_assignable_v<SV5>);
	static_assert( std::is_copy_assignable_v<SV6>);
	
	//Move assignment
	static_assert( std::is_move_assignable_v<CV1>);
	static_assert( std::is_move_assignable_v<CV2>);
	static_assert( std::is_move_assignable_v<CV3>);
	static_assert( std::is_move_assignable_v<CV4>);
	static_assert(!std::is_move_assignable_v<CV5>);
	static_assert( std::is_move_assignable_v<CV6>);
	
	static_assert( std::is_move_assignable_v<RV1>);
	static_assert( std::is_move_assignable_v<RV2>);
	static_assert( std::is_move_assignable_v<RV3>);
	static_assert( std::is_move_assignable_v<RV4>);
	static_assert(!std::is_move_assignable_v<RV5>);
	static_assert( std::is_move_assignable_v<RV6>);
	
	static_assert( std::is_move_assignable_v<SV1>);
	static_assert( std::is_move_assignable_v<SV2>);
	static_assert( std::is_move_assignable_v<SV3>);
	static_assert( std::is_move_assignable_v<SV4>);
	static_assert(!std::is_move_assignable_v<SV5>);
	static_assert( std::is_move_assignable_v<SV6>);
	return;
}

void TestConstexprStd::testVariantConstructor(void) const noexcept {
	//(1)
	constexpr CV1 cv1;
	constexpr CV2 cv2;
	//constexpr CV3 cv3; Is not default constructible!
	constexpr CV4 cv4;
	constexpr CV5 cv5;
	//constexpr CV6 cv6;
	constexpr CV6 cv6{true}; //To have something to copy or move
	
	RV1 rv1;
	RV2 rv2;
	//RV3 cv3; Is not default constructible!
	RV4 rv4;
	RV5 rv5;
	RV6 rv6{fooStrings}; //To have something to copy or move
	
	//(2)
	constexpr CV1 ccv1{cv1};
	//constexpr CV2 ccv2{cv2}; Not copy constructible!
	//constexpr CV3 ccv3{cv3};
	constexpr CV4 ccv4{cv4};
	//constexpr CV5 ccv5{cv5};
	constexpr CV6 ccv6{cv6};
	
	RV1 crv1{rv1};
	//RV2 crv2{rv2}; Not copy constructible!
	//RV3 crv3{rv3};
	RV4 crv4{rv4};
	//RV5 crv5{rv5};
	RV6 crv6{rv6};
	
	//(3)
	/* Calling std::move() on a temporary is in general not a good idea, here we use it to force the compile to choose
	 * the move constructor. Without it the compiler would facllback to the copy constructor, if we don't have a move
	 * constructor. */
	constexpr CV1 mcv1{std::move(CV1{})};
	constexpr CV2 mcv2{std::move(CV2{})};
	//constexpr CV3 mcv3{std::move(CV3{})}; No default constructor...
	constexpr CV4 mcv4{std::move(CV4{})};
	//constexpr CV5 mcv5{std::move(CV5{})}; No move constructor...
	constexpr CV6 mcv6{std::move(CV6{true})};
	
	RV1 mrv1{std::move(RV1{})};
	RV2 mrv2{std::move(RV2{})};
	//RV3 mrv3{std::move(RV3{})}; No default constructor...
	RV4 mrv4{std::move(RV4{})};
	//RV5 mrv5{std::move(RV5{})}; No move constructor...
	RV6 mrv6{std::move(RV6{std::string(longString)})};
	
	//(4)
	constexpr CV2 tcv21{std::array<int, 10>{}};
	constexpr CV2 tcv22{7};
	constexpr CV2 tcv23{isOdd};
	constexpr CV2 tcv24{std::initializer_list<int>{5, 4, 3}};
	RV4 trv41{fooString};
	RV4 trv42{fooStrings};
	RV4 trv43{std::initializer_list<int>{5, 4, 3}};
	
	//(5)
	constexpr CV2 itcv21{std::in_place_type<TestContainer>};
	constexpr CV2 itcv22{std::in_place_type<TestContainer>, std::array<int, 10>{}};
	constexpr CV2 itcv23{std::in_place_type<int>, 7.6};
	RV4 itrv41{std::in_place_type<std::string>, fooString};
	RV4 itrv42{std::in_place_type<std::string>, fooStrings};
	RV4 itrv43{std::in_place_type<std::forward_list<int>>, 5, 4};
	RV4 itrv44{std::in_place_type<NoDefault>, 3};
	
	constexpr constexprStd::variant<NoDefault, TestContainer> ittcv{std::in_place_type<TestContainer>, std::array<int, 10>{}};
	constexprStd::variant<NoDefault, std::list<int>> ittrv{std::in_place_type<std::list<int>>, 4, 5};
	
	//(6)
	constexpr CV2 itlcv2{std::in_place_type<TestContainer>, {1, 2, 3}};
	RV4 itlrv41{std::in_place_type<std::forward_list<int>>, {1, 2, 3}};
	RV4 itlrv42{std::in_place_type<std::string>, {'f', 'o', 'o'}};
	
	constexpr constexprStd::variant<NoDefault, TestContainer> itltcv{std::in_place_type<TestContainer>, {1, 2}};
	constexprStd::variant<NoDefault, std::list<int>> itltrv{std::in_place_type<std::list<int>>, {1, 2}};
	
	//(7)
	constexpr CV2 iicv21{std::in_place_index<0>};
	constexpr CV2 iicv22{std::in_place_index<0>, std::array<int, 10>{}};
	constexpr CV2 iicv23{std::in_place_index<1>, 9};
	constexpr CV2 iicv24{std::in_place_index<2>, isOdd};
	RV4 iirv41{std::in_place_index<0>};
	RV4 iirv42{std::in_place_index<1>, 1};
	RV4 iirv43{std::in_place_index<2>, 1};
	RV4 iirv44{std::in_place_index<3>, 1, 'c'};
	RV4 iirv45{std::in_place_index<3>, fooString};
	RV4 iirv46{std::in_place_index<3>, fooStrings};
	
	constexpr constexprStd::variant<NoDefault, TestContainer> iitcv{std::in_place_index<1>, std::array<int, 10>{}};
	constexprStd::variant<NoDefault, std::list<int>> iitrv{std::in_place_index<1>, 4, 5};
	
	//(8)
	constexpr CV2 iilcv2{std::in_place_index<0>, {1, 2, 3}};
	RV4 iilrv41{std::in_place_index<2>, {1, 2, 3}};
	RV4 iilrv42{std::in_place_index<3>, {'f', 'o', 'o'}};
	
	constexpr constexprStd::variant<NoDefault, TestContainer> iiltcv{std::in_place_index<1>, {1, 2}};
	constexprStd::variant<NoDefault, std::list<int>> iiltrv{std::in_place_index<1>, {1, 2}};
	return;
}

void TestConstexprStd::testVariantDestructor(void) const noexcept {
	int& instances = CountInstances<Bool>::Instances;
	if ( instances != 0 ) {
		QWARN("Had to reset CountInstances<Bool>::Instances.");
		instances = 0;
	} //if ( instances != 0 )
	
	CountInstances<Bool>{true};
	QCOMPARE(instances, 0);
	
	{
		CountInstances<Bool> ci{false};
		QCOMPARE(instances, 1);
	}
	QCOMPARE(instances, 0);
	
	SV3{true};
	QCOMPARE(instances, 0);
	
	{
		SV3 sv3{false};
		QCOMPARE(instances, 1);
	}
	QCOMPARE(instances, 0);
	
	RV3{true};
	QCOMPARE(instances, 0);
	
	{
		RV3 rv3{false};
		QCOMPARE(instances, 1);
	}
	QCOMPARE(instances, 0);
	return;
}

void TestConstexprStd::testVariantInteraction(void) const noexcept {
	constexpr CV6 cv1 = [](void) {
			CV6 ret{3}, foo{9};
			ret.swap(foo);
			return ret;
		}();
	static_assert(!cv1.valueless_by_exception());
	static_assert(cv1.index() == 1);
	
	static_assert( std::holds_alternative<int>(cv1));
	static_assert(!std::holds_alternative<NoDefault>(cv1));
	static_assert(!std::holds_alternative<bool>(cv1));
	
	static_assert( constexprStd::holds_alternative<int>(cv1));
	static_assert(!constexprStd::holds_alternative<NoDefault>(cv1));
	static_assert(!constexprStd::holds_alternative<bool>(cv1));
	
	static_assert(std::get<1>(cv1) == 9);
	static_assert(std::get<int>(cv1) == 9);
	static_assert(std::get_if<0>(&cv1) == nullptr);
	static_assert(*std::get_if<1>(&cv1) == 9);
	static_assert(std::get_if<2>(&cv1) == nullptr);
	static_assert(std::get_if<NoDefault>(&cv1) == nullptr);
	static_assert(*std::get_if<int>(&cv1) == 9);
	static_assert(std::get_if<bool>(&cv1) == nullptr);
	
	static_assert(constexprStd::get<1>(cv1) == 9);
	static_assert(constexprStd::get<int>(cv1) == 9);
	static_assert(constexprStd::get_if<0>(&cv1) == nullptr);
	static_assert(*constexprStd::get_if<1>(&cv1) == 9);
	static_assert(constexprStd::get_if<2>(&cv1) == nullptr);
	static_assert(constexprStd::get_if<NoDefault>(&cv1) == nullptr);
	static_assert(*constexprStd::get_if<int>(&cv1) == 9);
	static_assert(constexprStd::get_if<bool>(&cv1) == nullptr);
	
	constexpr CV6 cv2 = [](void) {
			CV6 ret{3}, foo{7};
			constexprStd::swap(ret, foo);
			return ret;
		}();
	static_assert(cv2.index() == 1);
	static_assert(std::get<1>(cv2) == 7);
	
	constexpr CV6 cv3 = [](void) {
		CV6 ret{true}, foo{7};
		ret.swap(foo);
		return ret;
	}();
	static_assert(cv3.index() == 1);
	static_assert(std::get<1>(cv3) == 7);
	
	constexpr CV6 cv4 = [](void) {
		CV6 ret{true}, foo{7};
		constexprStd::swap(ret, foo);
		return ret;
	}();
	static_assert(cv4.index() == 1);
	static_assert(std::get<1>(cv4) == 7);
	
	RV6 rv1{fooStrings}, rv2{barStrings};
	SV6 sv1{fooStrings}, sv2{barStrings};
	QCOMPARE(std::get<std::string>(rv1), fooString);
	QCOMPARE(std::get<1>(rv2), barString);
	QCOMPARE(std::get<std::string>(sv1), fooString);
	QCOMPARE(std::get<1>(sv2), barString);
	
	rv1.swap(rv2);
	sv1.swap(sv2);
	QCOMPARE(std::get<1>(rv1), barString);
	QCOMPARE(std::get<std::string>(rv2), fooString);
	QCOMPARE(std::get<1>(sv1), barString);
	QCOMPARE(std::get<std::string>(sv2), fooString);
	
	std::swap(rv1, rv2);
	std::swap(sv1, sv2);
	QCOMPARE(std::get<1>(rv1), fooString);
	QCOMPARE(std::get<1>(rv2), barString);
	QCOMPARE(std::get<1>(sv1), fooString);
	QCOMPARE(std::get<1>(sv2), barString);
	
	constexprStd::swap(rv1, rv2);
	constexprStd::swap(sv1, sv2);
	QCOMPARE(std::get<1>(rv1), barString);
	QCOMPARE(std::get<1>(rv2), fooString);
	QCOMPARE(std::get<1>(sv1), barString);
	QCOMPARE(std::get<1>(sv2), fooString);
	return;
}

void TestConstexprStd::testVariantEmplace(void) const noexcept {
	//Constexprness
	constexpr CV2 cv1 = [](void) noexcept {
			CV2 ret{7};
			ret.emplace<0>(Tag{}, 1, 2);
			return ret;
		}();
	static_assert(std::get<0>(cv1) == std::array<int, 10>{1, 2});
	
	constexpr CV2 cv2 = [](void) noexcept {
			CV2 ret{7};
			ret.emplace<0>(Tag{}, 8, 65, 2);
			return ret;
		}();
	static_assert(std::get<0>(cv2) == std::array<int, 10>{8, 10, 12, 14, 16, 18, 20, 22, 24, 26});
	
	constexpr CV2 cv3 = [](void) noexcept {
			CV2 ret{7};
			ret.emplace<0>({5, 7, 8, 9});
			return ret;
		}();
	static_assert(std::get<0>(cv3) == std::array<int, 10>{5, 7, 8, 9});
	
	constexpr CV2 cv4 = [](void) noexcept {
			CV2 ret{7};
			ret.emplace<TestContainer>(Tag{}, 1, 2);
			return ret;
		}();
	static_assert(std::get<0>(cv4) == std::array<int, 10>{1, 2});
	
	constexpr CV2 cv5 = [](void) noexcept {
			CV2 ret{7};
			ret.emplace<TestContainer>(Tag{}, 8, 65, 2);
			return ret;
		}();
	static_assert(std::get<0>(cv5) == std::array<int, 10>{8, 10, 12, 14, 16, 18, 20, 22, 24, 26});
	
	constexpr CV2 cv6 = [](void) noexcept {
			CV2 ret{7};
			ret.emplace<TestContainer>({5, 7, 8, 9});
			return ret;
		}();
	static_assert(std::get<0>(cv6) == std::array<int, 10>{5, 7, 8, 9});
	
	//Go into valueless state
	constexprStd::variant<LiteralThrows> nscv;
	QCOMPARE(std::get<0>(nscv), 0);
	
	nscv.emplace<0>(7);
	QCOMPARE(std::get<0>(nscv), 7);
	
	try {
		nscv.emplace<0>(6.7);
		QVERIFY(false);
	} //try
	catch ( const std::exception& ) {
		QVERIFY(true);
	} //catch ( const std::exception& )
	
	QVERIFY(nscv.valueless_by_exception());
	
	nscv.emplace<LiteralThrows>(7);
	QCOMPARE(std::get<LiteralThrows>(nscv), 7);
	
	try {
		nscv.emplace<LiteralThrows>(6.7);
		QVERIFY(false);
	} //try
	catch ( const std::exception& ) {
		QVERIFY(true);
	} //catch ( const std::exception& )
	
	QVERIFY(nscv.valueless_by_exception());
	
	int& instances = CountInstances<std::string>::Instances;
	if ( instances != 0 ) {
		instances = 0;
		QWARN("Had to reset CountInstances<std::string>::Instances.");
	} //if ( instances != 0 )
	
	constexprStd::variant<CountInstances<std::string>, int, double, TestContainer, LiteralThrows> rv;
	         std::variant<CountInstances<std::string>, int, double, TestContainer, LiteralThrows> sv;
	
	QCOMPARE(instances, 2);
	rv.emplace<1>(7);
	sv.emplace<1>(7);
	QCOMPARE(std::get<int>(rv), 7);
	QCOMPARE(std::get<int>(sv), 7);
	QCOMPARE(instances, 0);
	
	rv.emplace<3>({7, 8, 9});
	sv.emplace<3>({7, 8, 9});
	QCOMPARE(std::get<TestContainer>(rv), (TestContainer{std::array<int, 10>{7, 8, 9}}));
	QCOMPARE(std::get<TestContainer>(sv), (TestContainer{std::array<int, 10>{7, 8, 9}}));
	
	rv.emplace<CountInstances<std::string>>(fooString);
	sv.emplace<CountInstances<std::string>>(fooString);
	QCOMPARE(std::get<0>(rv), fooString);
	QCOMPARE(std::get<0>(sv), fooString);
	QCOMPARE(instances, 2);
	
	rv.emplace<TestContainer>({7, 8, 9});
	sv.emplace<TestContainer>({7, 8, 9});
	QCOMPARE(std::get<3>(rv), (TestContainer{std::array<int, 10>{7, 8, 9}}));
	QCOMPARE(std::get<3>(sv), (TestContainer{std::array<int, 10>{7, 8, 9}}));
	QCOMPARE(instances, 0);
	
	rv.emplace<0>();
	sv.emplace<0>();
	QCOMPARE(std::get<0>(rv), emptyString);
	QCOMPARE(std::get<0>(sv), emptyString);
	QCOMPARE(instances, 2);
	
	try {
		rv.emplace<LiteralThrows>(6.7);
		QVERIFY(false);
	} //try
	catch ( const std::exception& ) {
		QVERIFY(true);
	} //catch ( const std::exception& )
	QVERIFY(rv.valueless_by_exception());
	
	try {
		sv.emplace<LiteralThrows>(6.7);
		QVERIFY(false);
	} //try
	catch ( const std::exception& ) {
		QVERIFY(true);
	} //catch ( const std::exception& )
	QVERIFY(sv.valueless_by_exception());
	QCOMPARE(instances, 0);
	return;
}

void TestConstexprStd::testVariantCopyAssign(void) const noexcept {
	int& instances = CountInstances<std::string>::Instances;
	
	if ( instances != 0 ) {
		QWARN("Had to reset CountInstances<std::string>::Instances.");
		instances = 0;
	} //if ( instances != 0 )
	
	//Case 1: Both valueless
	//Since valuelessness is only achieved through exceptions, this can not be true in constexpr mode
	constexprStd::variant<int, const char*,                 LiteralThrows, std::pair<double, double>> cv1, cv2;
	constexprStd::variant<int, CountInstances<std::string>, LiteralThrows, std::pair<double, double>> rv1, rv2;
	         std::variant<int, CountInstances<std::string>, LiteralThrows, std::pair<double, double>> sv1, sv2;
	
	try { cv1.emplace<2>(6.7); } catch ( ... ) { }
	try { cv2.emplace<2>(6.7); } catch ( ... ) { }
	try { rv1.emplace<2>(6.7); } catch ( ... ) { }
	try { rv2.emplace<2>(6.7); } catch ( ... ) { }
	try { sv1.emplace<2>(6.7); } catch ( ... ) { }
	try { sv2.emplace<2>(6.7); } catch ( ... ) { }
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(cv2.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(rv2.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	QVERIFY(sv2.valueless_by_exception());
	
	//Self assignment, while valueless
	cv1 = cv1;
	rv1 = rv1;
	sv1 = sv1;
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	
	cv1 = cv2;
	rv1 = rv2;
	sv1 = sv2;
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	
	//Case 2: LHS is not, and RHS is valueless
	//Also no constexpr
	cv1.emplace<1>(fooString);
	rv1.emplace<1>(fooString);
	sv1.emplace<1>(fooString);
	QCOMPARE(instances, 2);
	
	//Self assignment, while not valueless
	constexpr CV1 c1 = []{ CV1 ret{7}; ret = ret; return ret; }();
	static_assert(std::get<int>(c1) == 7);
	cv1 = cv1;
	rv1 = rv1;
	sv1 = sv1;
	QCOMPARE(instances, 2);
	QCOMPARE(std::get<1>(cv1), fooString);
	QCOMPARE(std::get<1>(rv1), fooString);
	QCOMPARE(std::get<1>(sv1), fooString);
	
	cv1 = cv2;
	rv1 = rv2;
	sv1 = sv2;
	QCOMPARE(instances, 0);
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	
	//Case 3: Both holding the same alternative
	constexpr CV1 c2 = []{ CV1 ret{9}, temp{17}; ret = temp; return ret; }();
	static_assert(std::get<int>(c2) == 17);
	
	cv1.emplace<1>(fooString);
	rv1.emplace<1>(fooString);
	sv1.emplace<1>(fooString);
	cv2.emplace<1>(longString);
	rv2.emplace<1>(longString);
	sv2.emplace<1>(longString);
	QCOMPARE(instances, 4);
	
	cv1 = cv2;
	rv1 = rv2;
	sv1 = sv2;
	QCOMPARE(std::get<1>(cv1), longString);
	QCOMPARE(std::get<1>(rv1), longString);
	QCOMPARE(std::get<1>(sv1), longString);
	QCOMPARE(instances, 4);
	
	//Case 3b: Same alternative, throwing on assignment
	cv1.emplace<2>(4);
	rv1.emplace<2>(4);
	sv1.emplace<2>(4);
	cv2.emplace<2>(42);
	rv2.emplace<2>(42);
	sv2.emplace<2>(42);
	QCOMPARE(instances, 0);
	
	try {
		cv1 = cv2;
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(!cv1.valueless_by_exception());
	try {
		rv1 = rv2;
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(!rv1.valueless_by_exception());
	try {
		sv1 = sv2;
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(!sv1.valueless_by_exception());
	
	//Case 4: Different alternatives and the alternative hold by the right hand side is:
	static_assert(std::is_nothrow_copy_constructible_v<std::pair<double, double>> || !std::is_nothrow_move_constructible_v<std::pair<double, double>>);
	static_assert(std::is_nothrow_copy_constructible_v<int> || !std::is_nothrow_move_constructible_v<int>);
	constexpr CV1 c3 = [](void) {
			CV1 ret{true};
			const CV1 foo{8};
			ret = foo;
			return ret;
		}();
	static_assert(std::get<int>(c3) == 8);
	
	//Case 4.a: Assigning to not valueless
	cv2.emplace<3>(7, 3.14);
	rv2.emplace<3>(7, 3.14);
	sv2.emplace<3>(7, 3.14);
	
	cv1 = cv2;
	rv1 = rv2;
	sv1 = sv2;
	QCOMPARE(std::get<3>(cv1), (std::pair<double, double>{7, 3.14}));
	QCOMPARE(std::get<3>(rv1), (std::pair<double, double>{7, 3.14}));
	QCOMPARE(std::get<3>(sv1), (std::pair<double, double>{7, 3.14}));
	
	//Case 4.b: Assigning to valueless
	try { cv1.emplace<2>(6.7); } catch ( ... ) { }
	try { rv1.emplace<2>(6.7); } catch ( ... ) { }
	try { sv1.emplace<2>(6.7); } catch ( ... ) { }
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	
	cv1 = cv2;
	rv1 = rv2;
	sv1 = sv2;
	QCOMPARE(std::get<3>(cv1), (std::pair<double, double>{7, 3.14}));
	QCOMPARE(std::get<3>(rv1), (std::pair<double, double>{7, 3.14}));
	QCOMPARE(std::get<3>(sv1), (std::pair<double, double>{7, 3.14}));
	
	//Case 5 uses the move assignment, is tested in the next function
	return;
}

void TestConstexprStd::testVariantMoveAssign(void) const noexcept {
	int& instances = CountInstances<std::string>::Instances;
	
	if ( instances != 0 ) {
		QWARN("Had to reset CountInstances<std::string>::Instances.");
		instances = 0;
	} //if ( instances != 0 )
	
	//Case 1: Both valueless
	//Since valuelessness is only achieved through exceptions, this can not be true in constexpr mode
	constexprStd::variant<int, const char*,                 LiteralThrows, std::pair<double, double>> cv1, cv2;
	constexprStd::variant<int, CountInstances<std::string>, LiteralThrows, std::pair<double, double>> rv1, rv2;
	         std::variant<int, CountInstances<std::string>, LiteralThrows, std::pair<double, double>> sv1, sv2;
	
	try { cv1.emplace<2>(6.7); } catch ( ... ) { }
	try { cv2.emplace<2>(6.7); } catch ( ... ) { }
	try { rv1.emplace<2>(6.7); } catch ( ... ) { }
	try { rv2.emplace<2>(6.7); } catch ( ... ) { }
	try { sv1.emplace<2>(6.7); } catch ( ... ) { }
	try { sv2.emplace<2>(6.7); } catch ( ... ) { }
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(cv2.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(rv2.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	QVERIFY(sv2.valueless_by_exception());
	
	//Self assignment, while valueless
	cv1 = std::move(cv1);
	rv1 = std::move(rv1);
	sv1 = std::move(sv1);
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	
	cv1 = std::move(cv2);
	rv1 = std::move(rv2);
	sv1 = std::move(sv2);
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	QVERIFY(cv2.valueless_by_exception());
	QVERIFY(rv2.valueless_by_exception());
	QVERIFY(sv2.valueless_by_exception());
	
	//Case 2: LHS is not, and RHS is valueless
	//Also no constexpr
	cv1.emplace<1>(fooString);
	rv1.emplace<1>(fooString);
	sv1.emplace<1>(fooString);
	QCOMPARE(instances, 2);
	
	//Self assignment, while not valueless
	constexpr CV1 c1 = []{ CV1 ret{7}; ret = std::move(ret); return ret; }();
	static_assert(std::get<int>(c1) == 7);
	cv1 = std::move(cv1);
	rv1 = std::move(rv1);
	sv1 = std::move(sv1);
	QCOMPARE(instances, 2);
	QCOMPARE(std::get<1>(cv1), fooString);
	QCOMPARE(std::get<1>(rv1), emptyString);
	QCOMPARE(std::get<1>(sv1), emptyString);
	
	cv1 = std::move(cv2);
	rv1 = std::move(rv2);
	sv1 = std::move(sv2);
	QCOMPARE(instances, 0);
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	QVERIFY(cv2.valueless_by_exception());
	QVERIFY(rv2.valueless_by_exception());
	QVERIFY(sv2.valueless_by_exception());
	
	//Case 3: Both holding the same alternative
	constexpr CV1 c2 = []{ CV1 ret{9}, temp{17}; ret = std::move(temp); return ret; }();
	static_assert(std::get<int>(c2) == 17);
	
	cv1.emplace<1>(fooString);
	rv1.emplace<1>(fooString);
	sv1.emplace<1>(fooString);
	cv2.emplace<1>(longString);
	rv2.emplace<1>(longString);
	sv2.emplace<1>(longString);
	QCOMPARE(instances, 4);
	
	cv1 = std::move(cv2);
	rv1 = std::move(rv2);
	sv1 = std::move(sv2);
	QCOMPARE(std::get<1>(cv1), longString);
	QCOMPARE(std::get<1>(rv1), longString);
	QCOMPARE(std::get<1>(sv1), longString);
	QCOMPARE(std::get<1>(cv2), longString);
	QCOMPARE(std::get<1>(rv2), emptyString);
	QCOMPARE(std::get<1>(sv2), emptyString);
	QCOMPARE(instances, 4);
	
	//Case 3b: Same alternative, throwing on assignment
	cv1.emplace<2>(666);
	rv1.emplace<2>(666);
	sv1.emplace<2>(666);
	cv2.emplace<2>(42);
	rv2.emplace<2>(42);
	sv2.emplace<2>(42);
	QCOMPARE(instances, 0);
	
	LiteralThrowsMoveThrower mt;
	
	try {
		cv1 = std::move(cv2);
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(!cv1.valueless_by_exception());
	try {
		rv1 = std::move(rv2);
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(!rv1.valueless_by_exception());
	try {
		sv1 = std::move(sv2);
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(!sv1.valueless_by_exception());
	
	mt.deactivate();
	
	//Case 4: Different alternatives
	constexpr CV1 c3 = [](void) {
			CV1 ret{true}, foo{8};
			ret = std::move(foo);
			return ret;
		}();
	static_assert(std::get<int>(c3) == 8);
	
	//Case 4.a: Assigning to not valueless
	cv1.emplace<1>(fooString);
	rv1.emplace<1>(fooString);
	sv1.emplace<1>(fooString);
	cv2.emplace<2>(7);
	rv2.emplace<2>(7);
	sv2.emplace<2>(7);
	
	cv1 = std::move(cv2);
	rv1 = std::move(rv2);
	sv1 = std::move(sv2);
	QCOMPARE(std::get<2>(cv1), 7);
	QCOMPARE(std::get<2>(rv1), 7);
	QCOMPARE(std::get<2>(sv1), 7);
	//I don't really understand why this isn't true, but it is in both implementations.
	//QCOMPARE(std::get<2>(cv2), -1);
	//QCOMPARE(std::get<2>(rv2), -1);
	//QCOMPARE(std::get<2>(sv2), -1);
	
	//Case 4.b: Assigning to valueless
	try { cv1.emplace<2>(6.7); } catch ( ... ) { }
	try { rv1.emplace<2>(6.7); } catch ( ... ) { }
	try { sv1.emplace<2>(6.7); } catch ( ... ) { }
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(rv1.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	cv2.emplace<2>(7);
	rv2.emplace<2>(7);
	sv2.emplace<2>(7);
	
	cv1 = std::move(cv2);
	rv1 = std::move(rv2);
	sv1 = std::move(sv2);
	QCOMPARE(std::get<2>(cv1), 7);
	QCOMPARE(std::get<2>(rv1), 7);
	QCOMPARE(std::get<2>(sv1), 7);
	//See above
	//QCOMPARE(std::get<2>(cv2), -1);
	//QCOMPARE(std::get<2>(rv2), -1);
	//QCOMPARE(std::get<2>(sv2), -1);
	
	//Case 4c: Throw during assign
	cv1.emplace<1>(fooString);
	rv1.emplace<1>(fooString);
	sv1.emplace<1>(fooString);
	cv2.emplace<2>(42);
	rv2.emplace<2>(42);
	sv2.emplace<2>(42);
	
	mt.activate();
	
	try {
		cv1 = std::move(cv2);
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(cv1.valueless_by_exception());
	try {
		rv1 = std::move(rv2);
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(rv1.valueless_by_exception());
	try {
		sv1 = std::move(sv2);
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(sv1.valueless_by_exception());
	QCOMPARE(std::get<2>(cv2), 42);
	QCOMPARE(std::get<2>(rv2), 42);
	QCOMPARE(std::get<2>(sv2), 42);
	
	mt.deactivate();
	
	//Copy assign Case 5: Same alternative and the alternative hold by the right hand side is:
	struct Temp : public LiteralThrows {
		using LiteralThrows::LiteralThrows;
		Temp(const Temp& that) : LiteralThrows(that) {
			return;
		}
		
		Temp(Temp&& that) noexcept {
			I = constexprStd::exchange(that.I, -1);;
			return;
		}
		
		Temp& operator=(const Temp&) = default;
		Temp& operator=(Temp&&) = default;
		
		int toInt(void) const noexcept {
			return I;
		}
	};
	
	static_assert(!(std::is_nothrow_copy_constructible_v<Temp> || !std::is_nothrow_move_constructible_v<Temp>));
	
	constexprStd::variant<int, const char*,                 Temp, std::pair<double, double>> cv3, cv4;
	constexprStd::variant<int, CountInstances<std::string>, Temp, std::pair<double, double>> rv3, rv4;
	         std::variant<int, CountInstances<std::string>, Temp, std::pair<double, double>> sv3, sv4;
	
	cv3.emplace<2>(17);
	rv3.emplace<2>(17);
	sv3.emplace<2>(17);
	cv4.emplace<2>(99);
	rv4.emplace<2>(99);
	sv4.emplace<2>(99);
	
	cv3 = cv4;
	rv3 = rv4;
	sv3 = sv4;
	QCOMPARE(std::get<2>(cv3).toInt(), 99);
	QCOMPARE(std::get<2>(rv3).toInt(), 99);
	QCOMPARE(std::get<2>(sv3).toInt(), 99);
	return;
}

void TestConstexprStd::testVariantConvertAssign(void) const noexcept {
	constexprStd::variant<bool,                 LiteralThrows, std::pair<double, double>> cv;
	constexprStd::variant<CountInstances<Bool>, LiteralThrows, std::pair<double, double>> rv;
	         std::variant<CountInstances<Bool>, LiteralThrows, std::pair<double, double>> sv;
	
	//Case 1: Holds already a suitable type
	constexpr CV1 c1 = []{ CV1 ret{9}; ret = 77; return ret; }();
	static_assert(std::get<0>(c1) == 77);
	
	cv.emplace<0>(fooString);
	rv.emplace<0>(fooString);
	sv.emplace<0>(fooString);
	
	cv = false;
	rv = false;
	sv = false;
	QCOMPARE(std::get<0>(cv), false);
	QCOMPARE(std::get<0>(rv), false);
	QCOMPARE(std::get<0>(sv), false);
	
	//Case 2: Different type and:
	static_assert(std::is_nothrow_constructible_v<LiteralThrows, int> ||
	              !std::is_nothrow_move_constructible_v<LiteralThrows>);
	constexpr CV1 c2 = []{ CV1 ret{true}; ret = 77; return ret; }();
	static_assert(std::get<0>(c2) == 77);
	
	cv = LiteralThrows{365}; //Without this it would assign to the bool
	rv = LiteralThrows{365};
	sv = LiteralThrows{365};
	QCOMPARE(std::get<1>(cv), 365);
	QCOMPARE(std::get<1>(rv), 365);
	QCOMPARE(std::get<1>(sv), 365);
	
	LiteralThrowsMoveThrower mt;
	
	//Case 1 and throw in assignment
	try {
		cv = LiteralThrows{42};
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(!cv.valueless_by_exception());
	try {
		rv = LiteralThrows{42};
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(!rv.valueless_by_exception());
	try {
		sv = LiteralThrows{42};
		QVERIFY(false);
	} //try
	catch ( const std::bad_cast& ) {
		QVERIFY(true);
	} //catch ( const std::bad_cast& )
	QVERIFY(!sv.valueless_by_exception());
	
	mt.deactivate();
	
	//Case 3:
	struct Temp : public LiteralThrows {
		Temp(const Temp& that) : LiteralThrows(that) {
			return;
		}
		
		Temp(const int i) {
			I = i;
			return;
		}
		
		Temp(Temp&& that) noexcept {
			I = constexprStd::exchange(that.I, -1);;
			return;
		}
		
		Temp& operator=(const Temp&) = default;
		Temp& operator=(Temp&&) = default;
		
		int toInt(void) const noexcept {
			return I;
		}
	};
	
	static_assert(!(std::is_nothrow_constructible_v<Temp, int> ||
	                !std::is_nothrow_move_constructible_v<Temp>));
	
	constexprStd::variant<Temp, std::pair<double, double>> cv2{88};
	constexprStd::variant<Temp, std::pair<double, double>> rv2{88};
	         std::variant<Temp, std::pair<double, double>> sv2{88};
	
	cv2 = 99;
	rv2 = 99;
	sv2 = 99;
	QCOMPARE(std::get<0>(cv2).toInt(), 99);
	QCOMPARE(std::get<0>(rv2).toInt(), 99);
	QCOMPARE(std::get<0>(sv2).toInt(), 99);
	return;
}

void TestConstexprStd::testVariantCompare(void) const noexcept {
	constexprStd::variant<int, double, char, LiteralThrows> cv1{3.6}, cv2{'f'};
	         std::variant<int, double, char, LiteralThrows> sv1{3.6}, sv2{'f'};
	
	//Different indices, no valueless, v1.i < v2.i
	QVERIFY(!(sv1 == sv2));
	QVERIFY(!(cv1 == cv2));
	QVERIFY( (sv1 != sv2));
	QVERIFY( (cv1 != cv2));
	QVERIFY( (sv1 <  sv2));
	QVERIFY( (cv1 <  cv2));
	QVERIFY(!(sv1 >  sv2));
	QVERIFY(!(cv1 >  cv2));
	QVERIFY( (sv1 <= sv2));
	QVERIFY( (cv1 <= cv2));
	QVERIFY(!(sv1 >= sv2));
	QVERIFY(!(cv1 >= cv2));
	
	//Different indices, no valueless, v1.i > v2.i
	cv2 = 7;
	sv2 = 7;
	QVERIFY(!(sv1 == sv2));
	QVERIFY(!(cv1 == cv2));
	QVERIFY( (sv1 != sv2));
	QVERIFY( (cv1 != cv2));
	QVERIFY(!(sv1 <  sv2));
	QVERIFY(!(cv1 <  cv2));
	QVERIFY( (sv1 >  sv2));
	QVERIFY( (cv1 >  cv2));
	QVERIFY(!(sv1 <= sv2));
	QVERIFY(!(cv1 <= cv2));
	QVERIFY( (sv1 >= sv2));
	QVERIFY( (cv1 >= cv2));
	
	//Different indices, v2 valueless
	try { cv2.emplace<LiteralThrows>(4.2); } catch ( ... ) { }
	try { sv2.emplace<LiteralThrows>(4.2); } catch ( ... ) { }
	QVERIFY(cv2.valueless_by_exception());
	QVERIFY(sv2.valueless_by_exception());
	QVERIFY(!(sv1 == sv2));
	QVERIFY(!(cv1 == cv2));
	QVERIFY( (sv1 != sv2));
	QVERIFY( (cv1 != cv2));
	QVERIFY(!(sv1 <  sv2));
	QVERIFY(!(cv1 <  cv2));
	QVERIFY( (sv1 >  sv2));
	QVERIFY( (cv1 >  cv2));
	QVERIFY(!(sv1 <= sv2));
	QVERIFY(!(cv1 <= cv2));
	QVERIFY( (sv1 >= sv2));
	QVERIFY( (cv1 >= cv2));
	
	//Same indices, both valueless
	try { cv1.emplace<LiteralThrows>(4.2); } catch ( ... ) { }
	try { sv1.emplace<LiteralThrows>(4.2); } catch ( ... ) { }
	QVERIFY(cv1.valueless_by_exception());
	QVERIFY(sv1.valueless_by_exception());
	QVERIFY( (sv1 == sv2));
	QVERIFY( (cv1 == cv2));
	QVERIFY(!(sv1 != sv2));
	QVERIFY(!(cv1 != cv2));
	QVERIFY(!(sv1 <  sv2));
	QVERIFY(!(cv1 <  cv2));
	QVERIFY(!(sv1 >  sv2));
	QVERIFY(!(cv1 >  cv2));
	QVERIFY( (sv1 <= sv2));
	QVERIFY( (cv1 <= cv2));
	QVERIFY( (sv1 >= sv2));
	QVERIFY( (cv1 >= cv2));
	
	//Different indices, v1 valueless
	cv2 = 17;
	sv2 = 17;
	QVERIFY(!(sv1 == sv2));
	QVERIFY(!(cv1 == cv2));
	QVERIFY( (sv1 != sv2));
	QVERIFY( (cv1 != cv2));
	QVERIFY( (sv1 <  sv2));
	QVERIFY( (cv1 <  cv2));
	QVERIFY(!(sv1 >  sv2));
	QVERIFY(!(cv1 >  cv2));
	QVERIFY( (sv1 <= sv2));
	QVERIFY( (cv1 <= cv2));
	QVERIFY(!(sv1 >= sv2));
	QVERIFY(!(cv1 >= cv2));
	
	//Same indices, both not valueless, same values
	cv1 = 17;
	sv1 = 17;
	QVERIFY( (sv1 == sv2));
	QVERIFY( (cv1 == cv2));
	QVERIFY(!(sv1 != sv2));
	QVERIFY(!(cv1 != cv2));
	QVERIFY(!(sv1 <  sv2));
	QVERIFY(!(cv1 <  cv2));
	QVERIFY(!(sv1 >  sv2));
	QVERIFY(!(cv1 >  cv2));
	QVERIFY( (sv1 <= sv2));
	QVERIFY( (cv1 <= cv2));
	QVERIFY( (sv1 >= sv2));
	QVERIFY( (cv1 >= cv2));
	
	//Same indices, both not valueless, different values, v1 < v2
	cv1 = 5;
	sv1 = 5;
	QVERIFY(!(sv1 == sv2));
	QVERIFY(!(cv1 == cv2));
	QVERIFY( (sv1 != sv2));
	QVERIFY( (cv1 != cv2));
	QVERIFY( (sv1 <  sv2));
	QVERIFY( (cv1 <  cv2));
	QVERIFY(!(sv1 >  sv2));
	QVERIFY(!(cv1 >  cv2));
	QVERIFY( (sv1 <= sv2));
	QVERIFY( (cv1 <= cv2));
	QVERIFY(!(sv1 >= sv2));
	QVERIFY(!(cv1 >= cv2));
	
	//Same indices, both not valueless, different values, v1 > v2
	cv2 = 1;
	sv2 = 1;
	QVERIFY(!(sv1 == sv2));
	QVERIFY(!(cv1 == cv2));
	QVERIFY( (sv1 != sv2));
	QVERIFY( (cv1 != cv2));
	QVERIFY(!(sv1 <  sv2));
	QVERIFY(!(cv1 <  cv2));
	QVERIFY( (sv1 >  sv2));
	QVERIFY( (cv1 >  cv2));
	QVERIFY(!(sv1 <= sv2));
	QVERIFY(!(cv1 <= cv2));
	QVERIFY( (sv1 >= sv2));
	QVERIFY( (cv1 >= cv2));
	return;
}

void TestConstexprStd::testDifferentVariantCompare(void ) const noexcept {
	using V1 = constexprStd::variant<int, bool, LiteralThrows>;
	using V2 = constexprStd::variant<char, int, bool, double, LiteralThrows>;
	V1 v1{7};
	V2 v2{7};
	
	//Same type, equal value
	QVERIFY( (v1 == v2));
	QVERIFY(!(v1 != v2));
	static_assert( (V1{7} == V2{7}));
	static_assert(!(V1{7} != V2{7}));
	
	//Same type, unequal value
	v2 = 9;
	QVERIFY(!(v1 == v2));
	QVERIFY( (v1 != v2));
	static_assert(!(V1{7} == V2{9}));
	static_assert( (V1{7} != V2{9}));
	
	//Different type
	v2 = 7.9;
	QVERIFY(!(v1 == v2));
	QVERIFY( (v1 != v2));
	static_assert(!(V1{7} == V2{7.9}));
	static_assert( (V1{7} != V2{7.9}));
	
	//Lefthand side valueless
	try { v1.emplace<LiteralThrows>(.3); } catch ( ... ) { }
	QVERIFY(v1.valueless_by_exception());
	QVERIFY(!(v1 == v2));
	QVERIFY( (v1 != v2));
	
	//Both valueless
	try { v2.emplace<LiteralThrows>(.3); } catch ( ... ) { }
	QVERIFY(v2.valueless_by_exception());
	QVERIFY( (v1 == v2));
	QVERIFY(!(v1 != v2));
	
	//Righthand side valueless
	v1 = true;
	QVERIFY(!(v1 == v2));
	QVERIFY( (v1 != v2));
	
	//The other way around
	v1 = 7;
	v2 = 7;
	//Same type, equal value
	QVERIFY( (v2 == v1));
	QVERIFY(!(v2 != v1));
	static_assert( (V2{7} == V1{7}));
	static_assert(!(V2{7} != V1{7}));
	
	//Same type, unequal value
	v2 = 9;
	QVERIFY(!(v2 == v1));
	QVERIFY( (v2 != v1));
	static_assert(!(V2{7} == V1{9}));
	static_assert( (V2{7} != V1{9}));
	
	//Different type
	v2 = 7.9;
	QVERIFY(!(v2 == v1));
	QVERIFY( (v2 != v1));
	static_assert(!(V2{7.9} == V1{7}));
	static_assert( (V2{7.9} != V1{7}));
	
	//Lefthand side valueless
	try { v2.emplace<LiteralThrows>(.3); } catch ( ... ) { }
	QVERIFY(v2.valueless_by_exception());
	QVERIFY(!(v2 == v1));
	QVERIFY( (v2 != v1));
	
	//Both valueless
	try { v1.emplace<LiteralThrows>(.3); } catch ( ... ) { }
	QVERIFY(v1.valueless_by_exception());
	QVERIFY( (v2 == v1));
	QVERIFY(!(v2 != v1));
	
	//Righthand side valueless
	v2 = true;
	QVERIFY(!(v2 == v1));
	QVERIFY( (v2 != v1));
	return;
}

void TestConstexprStd::testDifferentVariantAssign(void) const noexcept {
	using V1 = constexprStd::variant<int, bool, unsigned long long, LiteralThrows>;
	using V2 = constexprStd::variant<char, int, bool, double, LiteralThrows>;
	V1 v1{7};
	V2 v2{9};
	
	static_assert(!noexcept(v1 = v2));
	
	static_assert(noexcept(std::declval<constexprStd::variant<int, double>>() =
	                       std::declval<constexprStd::variant<double, int>>()));
	
	//Same type
	v1 = v2;
	QCOMPARE(std::get<int>(v1), 9);
	static_assert(std::get<int>(V1{7} = V2{9}) == 9);
	
	//Different type, contained
	v2 = true;
	v1 = v2;
	QCOMPARE(std::get<bool>(v1), true);
	static_assert(std::get<bool>(V1{9} = V2{true}) == true);
	
	//Different type, not contained
	v2 = 7.9;
	try {
		v1 = v2;
		QVERIFY(false);
	} //try
	catch ( const constexprStd::BadVariantAssignment& ) {
		QVERIFY(true);
	} //catch ( const constexprStd::BadVariantAssignment& )
	QVERIFY(!v1.valueless_by_exception());
	
	//Lefthand side valueless, not contained type
	try { v1.emplace<LiteralThrows>(.3); } catch ( ... ) { }
	QVERIFY(v1.valueless_by_exception());
	try {
		v1 = v2;
		QVERIFY(false);
	} //try
	catch ( const constexprStd::BadVariantAssignment& ) {
		QVERIFY(true);
	} //catch ( const constexprStd::BadVariantAssignment& )
	QVERIFY(v1.valueless_by_exception());
	
	//Lefthand side valueless, contained type
	v2 = 9;
	v1 = v2;
	QCOMPARE(std::get<int>(v1), 9);
	
	//Righthand side valueless
	try { v2.emplace<LiteralThrows>(.3); } catch ( ... ) { }
	QVERIFY(v2.valueless_by_exception());
	v1 = v2;
	QVERIFY(v1.valueless_by_exception());
	
	//Both valueless
	v1 = v2;
	QVERIFY(v1.valueless_by_exception());
	
	//The other way around
	//Same type
	v1 = 9;
	v2 = 7;
	v2 = v1;
	QCOMPARE(std::get<int>(v2), 9);
	static_assert(std::get<int>(V2{7} = V1{9}) == 9);
	
	//Different type, contained
	v1 = true;
	v2 = v1;
	QCOMPARE(std::get<bool>(v2), true);
	static_assert(std::get<bool>(V2{9} = V1{true}) == true);
	
	//Different type, not contained
	v1 = 9ull;
	try {
		v2 = v1;
		QVERIFY(false);
	} //try
	catch ( const constexprStd::BadVariantAssignment& ) {
		QVERIFY(true);
	} //catch ( const constexprStd::BadVariantAssignment& )
	QVERIFY(!v2.valueless_by_exception());
	
	//Lefthand side valueless, not contained type
	try { v2.emplace<LiteralThrows>(.3); } catch ( ... ) { }
	QVERIFY(v2.valueless_by_exception());
	try {
		v2 = v1;
		QVERIFY(false);
	} //try
	catch ( const constexprStd::BadVariantAssignment& ) {
		QVERIFY(true);
	} //catch ( const constexprStd::BadVariantAssignment& )
	QVERIFY(v2.valueless_by_exception());
	
	//Lefthand side valueless, contained type
	v1 = 9;
	v2 = v1;
	QCOMPARE(std::get<int>(v2), 9);
	
	//Righthand side valueless
	try { v1.emplace<LiteralThrows>(.3); } catch ( ... ) { }
	QVERIFY(v1.valueless_by_exception());
	v2 = v1;
	QVERIFY(v2.valueless_by_exception());
	
	//Both valueless
	v2 = v1;
	QVERIFY(v2.valueless_by_exception());
	return;
}

void TestConstexprStd::testExchange(void) const noexcept {
	//Test the acutal constexprness
	auto l = [](int i) constexpr noexcept {
			return constexprStd::exchange(i, 5);
		};
	static_assert(l(8) == 8);
	
	//Test runtime behavior and compare against std::exchange.
	std::string cstr1 = fooString;
	std::string sstr1 = fooString;
	QCOMPARE(cstr1, fooString);
	QCOMPARE(sstr1, fooString);
	
	auto cstr2 = constexprStd::exchange(cstr1, std::string{barString});
	auto sstr2 =          std::exchange(sstr1, std::string{barString});
	QCOMPARE(cstr1, barString);
	QCOMPARE(sstr1, barString);
	QCOMPARE(cstr2, fooString);
	QCOMPARE(sstr2, fooString);
	
	cstr2 = constexprStd::exchange(cstr1, bazString);
	sstr2 =          std::exchange(sstr1, bazString);
	QCOMPARE(cstr1, bazString);
	QCOMPARE(sstr1, bazString);
	QCOMPARE(cstr2, barString);
	QCOMPARE(sstr2, barString);
	return;
}

void TestConstexprStd::testSwap(void) const noexcept {
	//Test the acutal constexprness
	auto l = [](void) constexpr noexcept {
		int ret = 5, foo = 8;
		constexprStd::swap(ret, foo);
		return ret;
	};
	static_assert(l() == 8);
	
	//Test runtime behavior
	std::string str1 = fooString, str2 = barString;
	
	constexprStd::swap(str1, str2);
	
	QCOMPARE(str1, barString);
	QCOMPARE(str2, fooString);
	return;
}

QTEST_APPLESS_MAIN(TestConstexprStd)

#include "test_main.moc"
