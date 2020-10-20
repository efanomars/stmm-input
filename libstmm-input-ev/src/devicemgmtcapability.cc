/*
 * Copyright © 2016-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   devicemgmtcapability.cc
 */

#include "stmm-input-ev/devicemgmtcapability.h"

namespace stmi
{

const char* const DeviceMgmtCapability::s_sClassId = "stmi::DeviceMgmt";
Capability::RegisterClass<DeviceMgmtCapability> DeviceMgmtCapability::s_oInstall(s_sClassId);

DeviceMgmtCapability::DeviceMgmtCapability() noexcept
: DeviceManagerCapability(s_oInstall.getCapabilityClass())
{
}

} // namespace stmi

