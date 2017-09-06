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
 * @brief Contains details for the constexpr variants of <functional>
 */

#ifndef CONSTEXPRSTD_DETAILS_FUNCTIONAL_HPP
#define CONSTEXPRSTD_DETAILS_FUNCTIONAL_HPP

#include <type_traits>
#include <utility>

namespace constexprStd::details {
template<typename MemberFunction, typename Class, typename... Args,
         std::enable_if_t<std::is_member_function_pointer_v<MemberFunction>>* = nullptr>
constexpr decltype(auto) invokeImpl(MemberFunction&& func, Class&& object, Args&&... args)
		noexcept(std::is_nothrow_invocable_v<MemberFunction, Class, Args...>) {
	return (std::forward<Class>(object).*func)(std::forward<Args>(args)...);
}

template<typename Member, typename Class, std::enable_if_t<std::is_member_object_pointer_v<Member>>* = nullptr>
constexpr decltype(auto) invokeImpl(Member&& func, Class&& object)
		noexcept(std::is_nothrow_invocable_v<Member, Class>) {
	return std::forward<Class>(object).*std::forward<Member>(func);
}

template<typename Function, typename... Args,
         std::enable_if_t<std::negation_v<std::is_member_pointer<Function>>>* = nullptr>
constexpr decltype(auto) invokeImpl(Function&& func, Args&&... args)
		noexcept(std::is_nothrow_invocable_v<Function, Args...>) {
	return std::forward<Function>(func)(std::forward<Args>(args)...);
}
} //namespace constexprStd::details

#endif
