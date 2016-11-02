/*
 * Copyright © 2016  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   devicemgmtcapability.h
 */

#ifndef _STMI_DEVICE_MGMT_CAPABILITY_H_
#define _STMI_DEVICE_MGMT_CAPABILITY_H_

#include <stmm-input/capability.h>

namespace stmi
{

/** Device manager's capability to inform listeners of
 * added, removed and changed devices.
 */
class DeviceMgmtCapability : public DeviceManagerCapability
{
public:
	//
	static const char* const s_sClassId;
	static const Capability::Class& getClass() { return s_oInstall.getCapabilityClass(); }
protected:
	DeviceMgmtCapability();
private:
	static RegisterClass<DeviceMgmtCapability> s_oInstall;
private:
};

} // namespace stmi

#endif	/* _STMI_DEVICE_MGMT_CAPABILITY_H_ */
