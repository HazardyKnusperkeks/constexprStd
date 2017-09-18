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
 * @brief Checks constexprStd/variant for self-containment and contains the tests.
 */

#include <constexprStd/variant>

#include "../test.hpp"

#include <array>
#include <forward_list>
#include <string>
#include <utility>
#include <variant>

//Forward declare before inclidung count instances
namespace {
struct Bool;
} //namespace

namespace QTest {
static bool qCompare(const Bool& t1, const bool& t2, const char *actual, const char *expected, const char *file,
                     const int line);
} //namespace QTest

#include "test_constants.hpp"
#include "test_container.hpp"
#include "test_count_instances.hpp"
#include "test_helper_functions.hpp"

namespace {
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
} //namespace

namespace QTest {
static bool qCompare(const Bool& t1, const bool& t2, const char *actual, const char *expected, const char *file,
                     const int line) {
	return qCompare(t1.B, t2, actual, expected, file, line);
}
static bool qCompare(const LiteralThrows& t1, const int& t2, const char *actual, const char *expected, const char *file,
                     const int line) {
	return qCompare(t1, LiteralThrows{t2}, actual, expected, file, line);
}
} //namespace QTest

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
