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
 * File:   plugininterface.h
 */

#ifndef _STMI_PLUGIN_INTERFACE_H_
#define _STMI_PLUGIN_INTERFACE_H_

#include <stmm-input-base/childdevicemanager.h>

#include <vector>
#include <string>


namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

/** The signature of the "createPlugin" function every plugin must provide.
 * Beware: the function shouldn't throw exceptions.
 *
 * The function definition must be surrounded by `extern "C"`. Example:
 *
 *     #ifdef __cplusplus
 *     extern "C" {
 *     #endif //__cplusplus
 *     
 *     shared_ptr<ChildDeviceManager> createPlugin(const std::string sAppName
 *                                               , bool bEnableEventClasses
 *                                               , const std::vector<Event::Class>& aEnDisableEventClasses);
 *
 *     #ifdef __cplusplus
 *     }
 *     #endif //__cplusplus
 *
 * Parameter `sAppName` The application name. Can be empty.
 *
 * Parameter `bEnableEventClasses` Whether to enable or disable event classes.
 *
 * Parameter `aEnDisableEventClasses` The event classes to enable or disable.
 *
 * Return The device manager or null if couldn't create one.
 */
typedef shared_ptr<ChildDeviceManager> createPlugin_t(const std::string& sAppName
													, bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses);

} // namespace stmi

#endif /* _STMI_PLUGIN_INTERFACE_H_ */

