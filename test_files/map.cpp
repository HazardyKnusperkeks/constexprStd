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
 * @brief Checks constexprStd/map for self-containment and contains the tests.
 */

#include <constexprStd/map>

#include "../test.hpp"

#include <map>
#include <string>
#include <tuple>

#include "test_constants.hpp"
#include "test_count_instances.hpp"

void TestConstexprStd::testMap(void) const noexcept {
	int& instances = CountInstances<std::string>::Instances;
	if ( instances != 0 ) {
		QWARN("Had to reset CountInstances<std::string>::Instances");
		instances = 0;
	} //if ( instances != 0 )
	
	constexprStd::map<int, CountInstances<std::string>, 15> cmap;
	std::map<int, CountInstances<std::string>> smap;
	QCOMPARE(instances, 0);
	
	//Test empty clear
	cmap.clear();
	smap.clear();
	QCOMPARE(instances, 0);
	
	auto cins = cmap.insert({1, fooString});
	auto sins = smap.insert({1, fooString});
	QVERIFY(cins.second);
	QVERIFY(sins.second);
	QCOMPARE(instances, 2);
	
	cmap.clear();
	smap.clear();
	QCOMPARE(instances, 0);
	{
		constexprStd::mapDestroy<int, CountInstances<std::string>, 15> dmap;
	}
	QCOMPARE(instances, 0);
	return;
}
