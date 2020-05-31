/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   config.h
 */

#ifndef STMI_CONFIG_H
#define STMI_CONFIG_H

#include <string>
#include <vector>

namespace stmi
{

namespace Config
{

const std::string& getVersionString() noexcept;

// returns absolute paths
std::vector<std::string> getDataDirPaths() noexcept;

// Beware! this is the directory relative to the home directory
// which has to be retrieved by the caller
std::string getUserDataDirRelPath() noexcept;

} // namespace Config

} // namespace stmi

#endif /* STMI_CONFIG_H */

