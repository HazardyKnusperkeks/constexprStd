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

#ifndef TEST_MOVE_INT_HPP
#define TEST_MOVE_INT_HPP

struct MoveInt {
	int I;
	bool Moved = false;
	
	constexpr MoveInt(const int i = 0) noexcept : I{i} {
		return;
	}
	
	constexpr MoveInt(const MoveInt&) = default;
	
	constexpr MoveInt(MoveInt&& that) noexcept : I{that.I} {
		that.Moved = true;
		that.I = 0;
		return;
	}
	
	constexpr MoveInt& operator=(const MoveInt&) = default;
	
	constexpr MoveInt& operator=(MoveInt&& that) noexcept {
		I = that.I;
		Moved = that.Moved;
		that.Moved = true;
		that.I = 0;
		return *this;
	}
	
	constexpr operator int(void) const & noexcept {
		return I;
	}
	
	constexpr operator int(void) && noexcept {
		Moved = true;
		int ret = I;
		I = 0;
		return ret;
	}
};

#endif
