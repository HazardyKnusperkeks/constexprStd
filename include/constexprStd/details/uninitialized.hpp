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
 * @brief Contains a struct for uninitialized storage.
 */

#ifndef CONSTEXPRSTD_DETAILS_UNINITIALIZED_HPP
#define CONSTEXPRSTD_DETAILS_UNINITIALIZED_HPP

#include <type_traits>
#include <utility>

namespace constexprStd::details {
template<typename T, bool = std::is_trivially_destructible_v<T>>
struct Uninitialized;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-conversion"
#pragma GCC diagnostic ignored "-Wnarrowing"
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

template<typename T>
struct Uninitialized<T, true> {
	T Storage;
	
	template<typename... Args>
	constexpr Uninitialized(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) :
			Storage{std::forward<Args>(args)...} {
		return;
	}
	
	template<typename... Args>
	constexpr T& init(Args&&... args) noexcept((!std::is_constructible_v<T, Args...> ||
	                                            std::is_nothrow_constructible_v<T, Args...>) &&
	                                           std::is_nothrow_move_assignable_v<T>) {
		if constexpr ( std::is_constructible_v<T, Args...> ) {
			Storage = T(std::forward<Args>(args)...);
		} //if constexpr ( std::is_constructible_v<T, Args...> )
		else {
			Storage = T{std::forward<Args>(args)...};
		} //else -> if constexpr ( std::is_constructible_v<T, Args...> )
		return Storage;
	}
	
	constexpr void defaultInit(void) noexcept {
		return;
	}
	
	constexpr void deinit(void) noexcept {
		return;
	}
	
	constexpr T& get(void) & noexcept {
		return Storage;
	}
	
	constexpr const T& get(void) const & noexcept {
		return Storage;
	}
	
	constexpr T&& get(void) && noexcept {
		return std::move(Storage);
	}
	
	constexpr const T&& get(void) const && noexcept {
		return std::move(Storage);
	}
};

template<typename T>
struct Uninitialized<T, false> {
	std::aligned_storage_t<sizeof(T), alignof(T)> Storage;
	
	Uninitialized(void) noexcept {
		return;
	}
	
	template<typename... Args>
	Uninitialized(Args&&... args) noexcept(std::is_nothrow_constructible_v<T, Args...>) {
		init(std::forward<Args>(args)...);
		return;
	}
	
	template<typename... Args>
	T& init(Args&&... args) noexcept(!std::is_constructible_v<T, Args...> ||
	                                 std::is_nothrow_constructible_v<T, Args...>) {
		if constexpr ( std::is_constructible_v<T, Args...> ) {
			::new (&Storage) T(std::forward<Args>(args)...);
		} //if constexpr ( std::is_constructible_v<T, Args...> )
		else {
			::new (&Storage) T{std::forward<Args>(args)...};
		} //else -> if constexpr ( std::is_constructible_v<T, Args...> )
		return get();
	}
	
	void defaultInit(void) noexcept(noexcept(std::declval<Uninitialized&>().init())) {
		init();
		return;
	}
	
	void deinit(void) noexcept(std::is_nothrow_destructible_v<T>) {
		get().~T();
		return;
	}
	
	T& get(void) & noexcept {
		return *reinterpret_cast<T*>(&Storage);
	}
	
	const T& get(void) const & noexcept {
		return *reinterpret_cast<const T*>(&Storage);
	}
	
	T&& get(void) && noexcept {
		return std::move(*reinterpret_cast<T*>(&Storage));
	}
	
	const T&& get(void) const && noexcept {
		return std::move(*reinterpret_cast<const T*>(&Storage));
	}
};

#pragma GCC diagnostic pop
} //namespace constexprStd::details

#endif
