/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   dlutil.cc
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_STMMINPUTPLUGINS_CMAKE@   */

#include "dlutil.h"

#include <stdlib.h>

namespace stmi
{

namespace DlUtil
{

std::string getEnvString(const char* p0Name) noexcept
{
	const char* p0Value = ::secure_getenv(p0Name);
	std::string sValue{p0Value == nullptr ? "" : p0Value};
	return sValue;
}


} // namespace DlUtil

} // namespace stmi
