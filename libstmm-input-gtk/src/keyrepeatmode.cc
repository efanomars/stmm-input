/*
 * Copyright © 2017-2019  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   keyrepeatmode.cc
 */

#include "keyrepeatmode.h"

#include <cassert>

namespace stmi
{

KeyRepeat::MODE KeyRepeat::s_eMode{MODE_NOT_SET};

KeyRepeat::MODE KeyRepeat::getMode() noexcept
{
	if (s_eMode == MODE_NOT_SET) {
		// Set default
		s_eMode = MODE_SUPPRESS;
	}
	return s_eMode;
}

bool KeyRepeat::isModeSet() noexcept
{
	return (s_eMode != MODE_NOT_SET);
}
bool KeyRepeat::setMode(MODE eMode) noexcept
{
	assert((eMode == MODE_SUPPRESS) || (eMode == MODE_ADD_RELEASE) || (eMode == MODE_ADD_RELEASE_CANCEL));
	if (isModeSet()) {
		return false;
	}
	s_eMode = eMode;
	return true;
}

} // namespace stmi
