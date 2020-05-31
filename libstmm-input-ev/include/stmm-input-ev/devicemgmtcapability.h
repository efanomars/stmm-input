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
 * File:   devicemgmtcapability.h
 */

#ifndef STMI_DEVICE_MGMT_CAPABILITY_H
#define STMI_DEVICE_MGMT_CAPABILITY_H

#include <stmm-input/capability.h>

#include <vector>
#include <memory>
#include <type_traits>
#include <cassert>

namespace stmi { class DeviceManager; }

namespace stmi
{

using std::shared_ptr;

/** Device manager's capability for tree like composed device managers.
 * The capabilities of leaf device managers might not be directly accessible.
 * This device manager capability allows to retrieve other capabilities from
 * inner and leaf node device managers. An example could be a ServerCapability
 * which allows to start, stop the server or request status information.
 *
 * This capability is also used by DeviceMgmtEvent to inform listeners of added,
 * removed and changed devices.
 */
class DeviceMgmtCapability : public DeviceManagerCapability
{
public:
	/** The root device manager.
	 * If the owner device manager of this capability is an inner node of a tree like structure
	 * of device managers the root of it is returned otherwise the same as
	 * DeviceManagerCapability::getDeviceManager() is returned.
	 *
	 * DeviceManagerCapability::getDeviceManager() of an instance of this class should
	 * return the device manager owning the device involved in the DeviceMgmtEvent,
	 * which usually is a leaf in a tree like structure of device managers.
	 * @return The root device manager. Cannot be null.
	 */
	virtual shared_ptr<DeviceManager> getRootDeviceManager() const noexcept = 0;

	static const char* const s_sClassId;
	static const Capability::Class& getClass() noexcept { return s_oInstall.getCapabilityClass(); }
protected:
	DeviceMgmtCapability() noexcept;
private:
	static RegisterClass<DeviceMgmtCapability> s_oInstall;
private:
};

} // namespace stmi

#endif /* STMI_DEVICE_MGMT_CAPABILITY_H */
