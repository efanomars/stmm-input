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
 * File:   tst88xdevicemanager.cc
 */


#include "tst88xdevicemanager.h"

#include "stmm-input-ev/keyevent.h"
#include "stmm-input-ev/devicemgmtevent.h"

#include <iostream>

namespace stmi
{

Tst88XDeviceManager::Tst88XDeviceManager(bool bEnableEventClasses, const std::vector<stmi::Event::Class>& aEnDisableEventClasses) noexcept
: StdDeviceManager({KeyCapability::getClass(), test::Tst88Capability::getClass()}
					, {DeviceMgmtEvent::getClass(), KeyEvent::getClass()}
					, bEnableEventClasses, aEnDisableEventClasses)
{
//std::cout << "Tst88XDeviceManager::Tst88XDeviceManager()" << '\n';
}

Tst88XDeviceManager::~Tst88XDeviceManager() noexcept
{
//std::cout << "Tst88XDeviceManager::~Tst88XDeviceManager()" << '\n';
}

} // namespace stmi
