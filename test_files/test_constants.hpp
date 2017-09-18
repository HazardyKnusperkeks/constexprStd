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

#ifndef TEST_CONSTANTS_HPP
#define TEST_CONSTANTS_HPP

#include <string>

constexpr inline const char *fooString    = "foo";
constexpr inline const char *barString    = "bar";
constexpr inline const char *bazString    = "baz";
constexpr inline const char *emptyString  = "";
constexpr inline const char *longString   = "This is a long string, to be allocated on the heap!";
const     inline std::string fooStrings   = fooString;
const     inline std::string barStrings   = barString;
const     inline std::string bazStrings   = bazString;
const     inline std::string emptyStrings = emptyString;
const     inline std::string longStrings  = longString;

#endif
