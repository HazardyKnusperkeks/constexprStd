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
 * @brief Contains details for the constexpr variants of <iterator>
 */

#ifndef CONSTEXPRSTD_DETAILS_ITERATOR_HPP
#define CONSTEXPRSTD_DETAILS_ITERATOR_HPP

#include <iterator>
#include <utility>

namespace constexprStd::details {
template<typename Iterator>
constexpr inline auto iteratorCategory = typename std::iterator_traits<Iterator>::iterator_category{};

template<typename InputIterator, typename Distance>
constexpr void advanceImpl(InputIterator& i, Distance n, const std::input_iterator_tag)
		noexcept(noexcept(++i) && noexcept(n--) && noexcept(static_cast<bool>(std::declval<decltype(n--)>()))) {
	while ( n-- ) {
		++i;
	} //while ( n-- )
	return;
}

template<typename BidirectionalIterator, typename Distance>
constexpr void advanceImpl(BidirectionalIterator& i, Distance n, const std::bidirectional_iterator_tag)
		noexcept(noexcept(++i) && noexcept(n--) && noexcept(static_cast<bool>(std::declval<decltype(n--)>())) &&
		         noexcept(--i) && noexcept(n++) && noexcept(static_cast<bool>(std::declval<decltype(n++)>()))) {
	if ( n > 0 ) {
		while ( n-- ) {
			++i;
		} //while ( n-- )
	} //if ( n > 0 )
	else {
		while ( n++ ) {
			--i;
		} //while ( n++ )
	} //else -> if ( n > 0 )
	return;
}

template<typename RandomAccessIterator, typename Distance>
constexpr void advanceImpl(RandomAccessIterator& i, const Distance n, std::random_access_iterator_tag)
		noexcept(noexcept(i += n)) {
	i += n;
	return;
}

template<typename InputIterator>
constexpr auto distanceImpl(InputIterator first, const InputIterator last, const std::input_iterator_tag)
		noexcept(noexcept(first != last) && noexcept(++first) &&
		         std::is_nothrow_constructible_v<typename std::iterator_traits<InputIterator>::difference_type, int> &&
		         noexcept(++std::declval<typename std::iterator_traits<InputIterator>::difference_type&>())) {
	typename std::iterator_traits<InputIterator>::difference_type n = 0;
	for ( ; first != last; ++first, ++n ) {
		
	} //for ( ; first != last; ++first, ++n )
	return n;
}

template<typename RandomAccessIterator>
constexpr auto distanceImpl(RandomAccessIterator first, const RandomAccessIterator last,
                            const std::random_access_iterator_tag)
		noexcept(noexcept(last - first)) {
	return last - first;
}

} //namespace constexprStd::details

#endif
