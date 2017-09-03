/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   testconfig.h
 */

#ifndef STMI_TEST_CONFIG_H
#define STMI_TEST_CONFIG_H

namespace stmi
{

namespace testing
{

namespace config
{

/** The stmm-input-dl test source directory.
 * @return The test source directory. Cannot be empty.
 */
const char* getTestSourceDir();

/** The absolute path of the executable test.
 * @return The executable path. Cannot be empty.
 */
const char* getTestExePath();

} // namespace config

} // namespace testing

} // namespace stmi

#endif /* STMI_TEST_CONFIG_H */

