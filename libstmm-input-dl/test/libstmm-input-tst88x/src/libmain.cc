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
 * File:   libmain.cc
 */

#include "libmain.h"

#include "tst88xdevicemanager.h"

#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

shared_ptr<stmi::ChildDeviceManager> createPlugin(const std::string& /*sAppName*/
												, bool bEnableEventClasses, const std::vector<stmi::Event::Class>& aEnDisableEventClasses)
{
//std::cout << "libmain.cc: createPlugin" << '\n';
	return std::make_shared<stmi::Tst88XDeviceManager>(bEnableEventClasses, aEnDisableEventClasses);
}

#ifdef __cplusplus
}
#endif
