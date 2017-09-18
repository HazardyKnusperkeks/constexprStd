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
 * @brief Checks constexprStd/bitset for self-containment and contains the tests.
 */

#include <constexprStd/bitset>

#include "../test.hpp"

#include <bitset>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

#include "test_constants.hpp"

void TestConstexprStd::testBitset(void) const noexcept {
	using cbitset = constexprStd::bitset<19>;
	using sbitset =          std::bitset<19>;
	
	#define testZero(cb, sb, bit)\
		QVERIFY(!cb.test(bit));\
		QVERIFY(!sb.test(bit));\
		QVERIFY(!cb[bit]);\
		QVERIFY(!sb[bit]);\
		QVERIFY(!std::as_const(cb)[bit]);\
		QVERIFY(!std::as_const(sb)[bit])
	
	#define testOne(cb, sb, bit)\
		QVERIFY(cb.test(bit));\
		QVERIFY(sb.test(bit));\
		QVERIFY(cb[bit]);\
		QVERIFY(sb[bit]);\
		QVERIFY(std::as_const(cb)[bit]);\
		QVERIFY(std::as_const(sb)[bit])
	
	constexpr unsigned long long bitMask1  = 0b0101'0110; //bits 1, 2, 4 and 6 set
	constexpr unsigned long long bitMask2  = 0b1010'0110; //bits 1, 2, 5 and 7 set
	constexpr unsigned long long andResult = 0b0000'0110;
	constexpr unsigned long long orResult  = 0b1111'0110;
	constexpr unsigned long long xorResult = 0b1111'0000;
	
	constexpr unsigned long long onlyMSB   = 0b100'0000'0000'0000'0000;
	constexpr unsigned long long firstBolB = 0b001'0000'0000'0000'0000;
	
	const char *bitMaskString1 = "01010110";
	const char *bitMaskString2 = "10100110";
	const char *bitMaskString3 = "nynynyyn";
	const char *bitMaskString4 = "ynynnyyn";
	const char *throwString    = fooString;
	
	const std::string bitMaskStdString1 = bitMaskString1;
	const std::string bitMaskStdString2 = bitMaskString2;
	const std::string bitMaskStdString3 = bitMaskString3;
	const std::string bitMaskStdString4 = bitMaskString4;
	const std::string throwStdString    = fooStrings;
	
	cbitset cb1;
	sbitset sb1;
	
	QVERIFY(cb1.size() == sb1.size());
	
	for ( std::size_t i = 0; i < cb1.size(); ++i ) {
		testZero(cb1, sb1, i);
	} //for ( std::size_t i = 0; i < b1.size(); ++i )
	
	QVERIFY(!cb1.all());
	QVERIFY(!sb1.all());
	QVERIFY(!cb1.any());
	QVERIFY(!sb1.any());
	QVERIFY( cb1.none());
	QVERIFY( sb1.none());
	QVERIFY(cb1.count() == sb1.count());
	
	cbitset cb2{bitMask1};
	sbitset sb2{bitMask1};
	
	for ( auto i : {0u, 3u, 5u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u} ) {
		testZero(cb2, sb2, i);
	} //for ( auto i : {0u, 3u, 5u, 7u, 8u, 9u, 10u, 11u, 12u, 13u, 14u, 15u, 16u, 17u, 18u} )
	
	for ( auto i : {1u, 2u, 4u, 6u} ) {
		testOne(cb2, sb2, i);
	} //for ( auto i : {1u, 2u, 4u, 6u} )
	
	QVERIFY(!cb2.all());
	QVERIFY(!sb2.all());
	QVERIFY( cb2.any());
	QVERIFY( sb2.any());
	QVERIFY(!cb2.none());
	QVERIFY(!sb2.none());
	QVERIFY(cb2.count() == sb2.count());
	
	cbitset cb3{bitMaskString1};
	sbitset sb3{bitMaskString1};
	
	QVERIFY(cb3 == cb2);
	QVERIFY(sb3 == sb2);
	
	QVERIFY(!cb3.all());
	QVERIFY(!sb3.all());
	QVERIFY( cb3.any());
	QVERIFY( sb3.any());
	QVERIFY(!cb3.none());
	QVERIFY(!sb3.none());
	QVERIFY(cb3.count() == sb3.count());
	
	try {
		cbitset{throwString};
		QVERIFY(false);
	} //try
	catch ( const std::invalid_argument& ) { }
	
	try {
		sbitset{throwString};
		QVERIFY(false);
	} //try
	catch ( const std::invalid_argument& ) { }
	
	cbitset cb4{bitMaskStdString2};
	sbitset sb4{bitMaskStdString2};
	
	QVERIFY(cb4 == cbitset{bitMask2});
	QVERIFY(sb4 == sbitset{bitMask2});
	
	QVERIFY(!cb4.all());
	QVERIFY(!sb4.all());
	QVERIFY( cb4.any());
	QVERIFY( sb4.any());
	QVERIFY(!cb4.none());
	QVERIFY(!sb4.none());
	QVERIFY(cb4.count() == sb4.count());
	
	try {
		cbitset{throwStdString};
		QVERIFY(false);
	} //try
	catch ( const std::invalid_argument& ) { }
	
	try {
		sbitset{throwStdString};
		QVERIFY(false);
	} //try
	catch ( const std::invalid_argument& ) { }
	
	try {
		cbitset{bitMaskStdString3, 9};
		QVERIFY(false);
	} //try
	catch ( const std::out_of_range& ) { }
	
	try {
		sbitset{bitMaskStdString3, 9};
		QVERIFY(false);
	} //try
	catch ( const std::out_of_range& ) { }
	
	cbitset cb5{bitMaskString4, 8, 'n', 'y'};
	sbitset sb5{bitMaskString4, 8, 'n', 'y'};
	
	QVERIFY(cb5 == cb4);
	QVERIFY(sb5 == sb4);
	
	QVERIFY(!cb5.all());
	QVERIFY(!sb5.all());
	QVERIFY( cb5.any());
	QVERIFY( sb5.any());
	QVERIFY(!cb5.none());
	QVERIFY(!sb5.none());
	QVERIFY(cb5.count() == sb5.count());
	
	cbitset cb6{onlyMSB};
	sbitset sb6{onlyMSB};
	
	QVERIFY(!cb6.all());
	QVERIFY(!sb6.all());
	QVERIFY( cb6.any());
	QVERIFY( sb6.any());
	QVERIFY(!cb6.none());
	QVERIFY(!sb6.none());
	QVERIFY(cb6.count() == sb6.count());
	
	//Manually inverse
	for ( std::size_t i = 0; i < cb1.size(); ++i ) {
		auto cref = cb1[i];
		auto sref = sb1[i];
		
		QVERIFY(!cref);
		QVERIFY(!sref);
		
		QVERIFY(~cref);
		QVERIFY(~sref);
		
		QVERIFY(cref.flip());
		QVERIFY(sref.flip());
	} //for ( std::size_t i = 0; i < b1.size(); ++i )
	
	QVERIFY( cb1.all());
	QVERIFY( sb1.all());
	QVERIFY( cb1.any());
	QVERIFY( sb1.any());
	QVERIFY(!cb1.none());
	QVERIFY(!sb1.none());
	QVERIFY(cb1.count() == sb1.count());
	QVERIFY(cb1 == cbitset{}.flip());
	QVERIFY(sb1 == sbitset{}.flip());
	
	QVERIFY((cb2 & cb4) == cbitset{andResult});
	QVERIFY((sb2 & sb4) == sbitset{andResult});
	
	QVERIFY((cb2 | cb4) == cbitset{orResult});
	QVERIFY((sb2 | sb4) == sbitset{orResult});
	
	QVERIFY((cb2 ^ cb4) == cbitset{xorResult});
	QVERIFY((sb2 ^ sb4) == sbitset{xorResult});
	
	QVERIFY(cb1 == ~cbitset{});
	QVERIFY(sb1 == ~sbitset{});
	
	auto value = bitMask1;
	auto cb{cb2};
	auto sb{sb2};
	for ( std::size_t i = 0; i < cb2.size(); ++i, value <<= 1, cb <<= 1, sb <<= 1 ) {
		QVERIFY(cb == value);
		QVERIFY(sb == value);
		
		QVERIFY((cb2 << i) == cb);
		QVERIFY((sb2 << i) == sb);
		
		QVERIFY((cbitset{cb2} << i) == cb);
		QVERIFY((sbitset{sb2} << i) == sb);
	} //for ( std::size_t i = 0; i < cb2.size(); ++i, value <<= 1 )
	
	value = onlyMSB;
	cb = value;
	sb = value;
	for ( std::size_t i = 0; i < cb6.size(); ++i, value >>= 1, cb >>= 1, sb >>= 1 ) {
		QVERIFY(cb == value);
		QVERIFY(sb == value);
		
		QVERIFY((cb6 >> i) == cb);
		QVERIFY((sb6 >> i) == sb);
		
		QVERIFY((cbitset{cb6} >> i) == cb);
		QVERIFY((sbitset{sb6} >> i) == sb);
	} //for ( std::size_t i = 0; i < cb6.size(); ++i, value >>= 1 )
	
	std::string zeroOneString(cb2.size(), '0');
	std::string yesNoString(cb5.size(), 'n');
	zeroOneString.replace(zeroOneString.size() - bitMaskStdString1.size(), bitMaskStdString1.size(), bitMaskStdString1);
	yesNoString.replace(yesNoString.size()     - bitMaskStdString4.size(), bitMaskStdString4.size(), bitMaskStdString4);
	QVERIFY(cb2.to_string() == zeroOneString);
	QVERIFY(sb2.to_string() == zeroOneString);
	QVERIFY(cb5.to_string('n', 'y') == yesNoString);
	QVERIFY(sb5.to_string('n', 'y') == yesNoString);
	
	const unsigned long ul = std::numeric_limits<unsigned long>::max();
	constexprStd::bitset<std::numeric_limits<unsigned long long>::digits + 1> longCB{ul};
	         std::bitset<std::numeric_limits<unsigned long long>::digits + 1> longSB{ul};
	
	QVERIFY(longCB.to_ulong() == ul);
	QVERIFY(longSB.to_ulong() == ul);
	
	try {
		longCB <<= 1;
		[[maybe_unused]] auto t = longCB.to_ulong();
		QVERIFY(false);
	} //try
	catch ( const std::overflow_error& ) { }
	
	try {
		longSB <<= 1;
		[[maybe_unused]] auto t = longSB.to_ulong();
		QVERIFY(false);
	} //try
	catch ( const std::overflow_error& ) { }
	
	const unsigned long long ull = std::numeric_limits<unsigned long long>::max();
	longCB = ull;
	longSB = ull;
	
	QVERIFY(longCB.to_ullong() == ull);
	QVERIFY(longSB.to_ullong() == ull);
	
	try {
		longCB <<= 1;
		[[maybe_unused]] auto t = longCB.to_ullong();
		QVERIFY(false);
	} //try
	catch ( const std::overflow_error& ) { }
	
	try {
		longSB <<= 1;
		[[maybe_unused]] auto t = longSB.to_ullong();
		QVERIFY(false);
	} //try
	catch ( const std::overflow_error& ) { }
	
	std::ostringstream costream;
	std::ostringstream sostream;
	
	costream<<cb1<<cb2<<cb3<<cb4<<cb5<<cb6;
	sostream<<sb1<<sb2<<sb3<<sb4<<sb5<<sb6;
	
	QVERIFY(costream.str() == sostream.str());
	
	std::istringstream cistream(cb2.to_string() + "  " + cb3.to_string() + "\r\t\n " + " 001 1010 12");
	std::istringstream sistream(sb2.to_string() + "  " + sb3.to_string() + "\r\t\n " + " 001 1010 12");
	
	cistream>>cb;
	sistream>>sb;
	QVERIFY(cb == cb2);
	QVERIFY(sb == sb2);
	QVERIFY(cistream.good());
	QVERIFY(sistream.good());
	
	cistream>>cb;
	sistream>>sb;
	QVERIFY(cb == cb3);
	QVERIFY(sb == sb3);
	QVERIFY(cistream.good());
	QVERIFY(sistream.good());
	
	cistream>>cb;
	sistream>>sb;
	QVERIFY(cb == 1);
	QVERIFY(sb == 1);
	QVERIFY(cistream.good());
	QVERIFY(sistream.good());
	
	cistream>>cb;
	sistream>>sb;
	QVERIFY(cb == 0b1010);
	QVERIFY(sb == 0b1010);
	QVERIFY(cistream.good());
	QVERIFY(sistream.good());
	
	cistream>>cb;
	sistream>>sb;
	QVERIFY(cb == 1);
	QVERIFY(sb == 1);
	QVERIFY(cistream.good());
	QVERIFY(sistream.good());
	
	cistream>>cb;
	sistream>>sb;
	QVERIFY(cb == 1);
	QVERIFY(sb == 1);
	QVERIFY(!cistream.good());
	QVERIFY(!sistream.good());
	
	cistream.str("1010010");
	sistream.str("1010010");
	cistream.clear(std::ios_base::goodbit);
	sistream.clear(std::ios_base::goodbit);
	
	constexprStd::bitset<4> scb;
	         std::bitset<4> ssb;
	
	cistream>>scb;
	sistream>>ssb;
	QVERIFY(scb == 0b1010);
	QVERIFY(ssb == 0b1010);
	QVERIFY(cistream.good());
	QVERIFY(sistream.good());
	
	cistream>>scb;
	sistream>>ssb;
	QVERIFY(scb == 0b10);
	QVERIFY(ssb == 0b10);
	QVERIFY(cistream.eof());
	QVERIFY(sistream.eof());
	
	#undef testZero
	#undef testOne
	
	static_assert(cbitset{firstBolB}.any());
	return;
}
