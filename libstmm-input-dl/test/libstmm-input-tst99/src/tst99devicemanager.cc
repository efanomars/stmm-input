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
 * File:   tst99devicemanager.cc
 */

#include "tst99devicemanager.h"

#include "devicemgmtevent.h"

#include <iostream>

namespace stmi
{

Tst99DeviceManager::Tst99DeviceManager(bool bEnableEventClasses, const std::vector<stmi::Event::Class>& aEnDisableEventClasses) noexcept
: StdDeviceManager({Tst99Capability::getClass()}
					, {DeviceMgmtEvent::getClass(), Tst99Event::getClass()}
					, bEnableEventClasses, aEnDisableEventClasses)
{
//std::cout << "Tst99DeviceManager::Tst99DeviceManager()" << '\n';
}

} // namespace stmi
