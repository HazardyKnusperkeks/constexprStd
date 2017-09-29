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
 * @brief Checks constexprStd/functional for self-containment and contains the tests.
 */

#include <constexprStd/functional>

#include "../test.hpp"

#include <functional>

#include <constexprStd/iterator>

#include "test_constants.hpp"
#include "test_helper_functions.hpp"

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

void TestConstexprStd::testDefaultSeracher(void) const noexcept {
	auto lambda = [](void) constexpr noexcept {
			std::array a{1, 2, 3, 4, 5, 1, 2, 5, 4};
			std::array s1{1, 2};
			std::array s2{5};
			std::array s3{7};
			auto p1 = constexprStd::default_searcher{s1.begin(), s1.end()}(a);
			auto p2 = constexprStd::default_searcher{s2}(a);
			auto p3 = constexprStd::default_searcher{s3}(a);
			auto d11 = constexprStd::distance(a.begin(), p1.first);
			auto d12 = constexprStd::distance(a.begin(), p1.second);
			auto d21 = constexprStd::distance(a.begin(), p2.first);
			auto d22 = constexprStd::distance(a.begin(), p2.second);
			auto d31 = constexprStd::distance(a.begin(), p3.first);
			auto d32 = constexprStd::distance(a.begin(), p3.second);
			return std::tuple{d11, d12, d21, d22, d31, d32};
		};
	
	static_assert(lambda() == std::tuple{0, 2, 4, 5, 9, 9});
	
	std::string s = "123 hallo 123";
	std::string s1 = "123";
	std::string s2 = "l";
	std::string s3 = fooString;
	std::string s4 = " 123";
	std::string s5 = "123 hallo 123 bar";
	std::string s6 = "x";
	std::string s7 = "";
	
	constexpr std::iterator_traits<std::string::iterator>::difference_type d11 =  0;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d12 =  3;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d21 =  6;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d22 =  7;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d31 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d32 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d41 =  9;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d42 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d51 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d52 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d61 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d62 = 13;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d71 =  0;
	constexpr std::iterator_traits<std::string::iterator>::difference_type d72 =  0;
	
	auto sp1 =          std::default_searcher{s1.begin(), s1.end()}(s.begin(), s.end());
	auto sp2 =          std::default_searcher{s2.begin(), s2.end()}(s.begin(), s.end());
	auto sp3 =          std::default_searcher{s3.begin(), s3.end()}(s.begin(), s.end());
	auto sp4 =          std::default_searcher{s4.begin(), s4.end()}(s.begin(), s.end());
	auto sp5 =          std::default_searcher{s5.begin(), s5.end()}(s.begin(), s.end());
	auto sp6 =          std::default_searcher{s6.begin(), s6.end()}(s.begin(), s.end());
	auto sp7 =          std::default_searcher{s7.begin(), s7.end()}(s.begin(), s.end());
	auto cp1 = constexprStd::default_searcher{s1.begin(), s1.end()}(s.begin(), s.end());
	auto cp2 = constexprStd::default_searcher{s2.begin(), s2.end()}(s.begin(), s.end());
	auto cp3 = constexprStd::default_searcher{s3.begin(), s3.end()}(s.begin(), s.end());
	auto cp4 = constexprStd::default_searcher{s4.begin(), s4.end()}(s.begin(), s.end());
	auto cp5 = constexprStd::default_searcher{s5.begin(), s5.end()}(s.begin(), s.end());
	auto cp6 = constexprStd::default_searcher{s6.begin(), s6.end()}(s.begin(), s.end());
	auto cp7 = constexprStd::default_searcher{s7.begin(), s7.end()}(s.begin(), s.end());
	
	QCOMPARE(std::distance(s.begin(), sp1.first),  d11);
	QCOMPARE(std::distance(s.begin(), sp1.second), d12);
	QCOMPARE(std::distance(s.begin(), sp2.first),  d21);
	QCOMPARE(std::distance(s.begin(), sp2.second), d22);
	QCOMPARE(std::distance(s.begin(), sp3.first),  d31);
	QCOMPARE(std::distance(s.begin(), sp3.second), d32);
	QCOMPARE(std::distance(s.begin(), sp4.first),  d41);
	QCOMPARE(std::distance(s.begin(), sp4.second), d42);
	QCOMPARE(std::distance(s.begin(), sp5.first),  d51);
	QCOMPARE(std::distance(s.begin(), sp5.second), d52);
	QCOMPARE(std::distance(s.begin(), sp6.first),  d61);
	QCOMPARE(std::distance(s.begin(), sp6.second), d62);
	QCOMPARE(std::distance(s.begin(), sp7.first),  d71);
	QCOMPARE(std::distance(s.begin(), sp7.second), d72);
	QCOMPARE(std::distance(s.begin(), cp1.first),  d11);
	QCOMPARE(std::distance(s.begin(), cp1.second), d12);
	QCOMPARE(std::distance(s.begin(), cp2.first),  d21);
	QCOMPARE(std::distance(s.begin(), cp2.second), d22);
	QCOMPARE(std::distance(s.begin(), cp3.first),  d31);
	QCOMPARE(std::distance(s.begin(), cp3.second), d32);
	QCOMPARE(std::distance(s.begin(), cp4.first),  d41);
	QCOMPARE(std::distance(s.begin(), cp4.second), d42);
	QCOMPARE(std::distance(s.begin(), cp5.first),  d51);
	QCOMPARE(std::distance(s.begin(), cp5.second), d52);
	QCOMPARE(std::distance(s.begin(), cp6.first),  d61);
	QCOMPARE(std::distance(s.begin(), cp6.second), d62);
	QCOMPARE(std::distance(s.begin(), cp7.first),  d71);
	QCOMPARE(std::distance(s.begin(), cp7.second), d72);
	return;
}
