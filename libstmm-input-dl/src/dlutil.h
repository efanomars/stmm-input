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
 * File:   dlutil.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_STMMINPUTPLUGINS_CMAKE@   */

#ifndef STMI_DL_UTIL_H
#define STMI_DL_UTIL_H

#include <string>

namespace stmi
{

namespace DlUtil
{

std::string getEnvString(const char* p0Name) noexcept;

} // namespace DlUtil

} // namespace stmi

#endif /* STMI_DL_UTIL_H */
