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

#ifndef TEST_COUNT_INSTANCES_HPP
#define TEST_COUNT_INSTANCES_HPP

#include <type_traits>
#include <utility>

#include <QTest>

template<typename T>
struct CountInstances : public T {
	static inline int Instances = 0;
	
	template<typename... Args>
	CountInstances(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args&&...>) :
			T{std::forward<Args>(args)...} {
		++Instances;
		return;
	}
	
	CountInstances(const CountInstances&) = default;
	CountInstances(CountInstances&&) = default;
	CountInstances& operator=(const CountInstances&) = default;
	CountInstances& operator=(CountInstances&&) = default;
	
	~CountInstances(void) noexcept(std::is_nothrow_destructible_v<T>) {
		--Instances;
		return;
	}
};

namespace QTest {
template<typename T1, typename T2>
inline bool qCompare(const CountInstances<T1>& t1, const T2& t2, const char *actual, const char *expected,
                     const char *file, const int line) {
	return qCompare(static_cast<const T1&>(t1), t2, actual, expected, file, line);
}
} //namespace QTest

#endif
