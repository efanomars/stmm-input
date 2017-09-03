/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   stmm-input-dl-config.h
 */

#ifndef STMI_STMM_INPUT_DL_LIB_CONFIG_H
#define STMI_STMM_INPUT_DL_LIB_CONFIG_H

namespace stmi
{

namespace libconfig
{

namespace dl
{

/** The stmm-input-dl library version.
 * @return The version string. Cannot be empty.
 */
const char* getVersion();

/** The system plug-in path.
 * @return The system plug-in directory.
 */
const char* getDataDir();

/** The user plug-in path.
 * Has precedence over system plug-in path.
 *
 * It is also the place where the "disabled-dlps.txt" file is stored.
 * @return The user plug-in directory.
 */
const char* getUserDataDir();

} // namespace dl

} // namespace libconfig

} // namespace stmi

#endif /* STMI_STMM_INPUT_DL_LIB_CONFIG_H */

