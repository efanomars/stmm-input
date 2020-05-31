/*
 * Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>
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

#include <string>
#include <vector>

namespace stmi
{

namespace libconfig
{

namespace dl
{

/** The stmm-input-dl library version.
 * @return The version string. Cannot be empty.
 */
const char* getVersion() noexcept;

/** The system config plug-in paths.
 *
 * The plugin file names within the paths must satisfy regex `[0-9][0-9](.+)\.dlp`.
 * If they don't they are ignored. The (.+) captures the name of the plugin.
 * The first two digits of the file name represent the rank and are used to order
 * (ascending) the plugins. Of the files with the same plugin name, the one associated
 * with the lowest rank value is loaded (the others are skipped).
 *
 * The path can also contain a "disabled-dlps.txt" file which lists plugin names
 * that should not be loaded.
 *
 * Example paths: "/usr/local/etc/stmm-input-dl" or "/etc/stmm-input-dl".
 * @return The system plug-in directories.
 */
std::vector<std::string> getDataDirPaths() noexcept;

/** The user plug-in path.
 *
 * Currently this path can only contain a "disabled-dlps.txt" file, which
 * lists additional plugin names that should not be loaded by this library.
 *
 * Example path: "/home/myname/.local/etc/stmm-input-dl".
 * @return The user plug-in directory.
 */
std::string getUserDataDirPath() noexcept;

} // namespace dl

} // namespace libconfig

} // namespace stmi

#endif /* STMI_STMM_INPUT_DL_LIB_CONFIG_H */

