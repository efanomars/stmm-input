/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   stmm-input-fake-config.h
 */

#ifndef _STMI_STMM_INPUT_FAKE_LIB_CONFIG_
#define _STMI_STMM_INPUT_FAKE_LIB_CONFIG_

namespace stmi
{

namespace libconfig
{

namespace fake
{

/** The stmm-input-fake library version.
 * @return The version string. Cannot be empty.
 */
const char* getVersion();

} // namespace fake

} // namespace libconfig

} // namespace stmi

#endif /* _STMI_STMM_INPUT_FAKE_LIB_CONFIG_ */

