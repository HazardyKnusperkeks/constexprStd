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

#include <array>
#include <set>

#include <constexprStd/iterator>

class TestContainer : public std::array<int, 10> {
	public:
	constexpr TestContainer(void) noexcept : std::array<int, 10>{{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}} {
		return;
	}
	
	constexpr TestContainer(std::array<int, 10> a) noexcept : std::array<int, 10>{a} {
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


class TestConstexprStd : public QObject {
	Q_OBJECT
	private slots:
	void testInsert_iterator(void) const noexcept;
	
	public:
	explicit TestConstexprStd(QObject *parent = nullptr) : QObject(parent) { return; }
};

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

QTEST_APPLESS_MAIN(TestConstexprStd)

#include "test_main.moc"
