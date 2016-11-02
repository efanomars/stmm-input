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
 * File:   stdparentdevicemanager.h
 */

#ifndef _STMI_STD_PARENT_DEVICE_MANAGER_H_
#define _STMI_STD_PARENT_DEVICE_MANAGER_H_

#include "devicemgmtcapability.h"

#include <stmm-input-base/parentdevicemanager.h>

#include <vector>


namespace stmi
{

/** Device manager container class with DeviceMgmtCapability.
 * The contained device managers (the children) must be subclasses of ChildDeviceManager.
 */
class StdParentDeviceManager : public ParentDeviceManager, public DeviceMgmtCapability
{
public:
	/** Creates an instance of a parent device manager.
	 * The children must also have the DeviceMgmtCapability.
	 * @param aChildDeviceManager An array of non null child device managers.
	 * @return The created instance or null if creation failed.
	 */
	static shared_ptr<StdParentDeviceManager> create(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager);

	/** Returns {DeviceMgmtCapability::getClass()}. */
	std::vector<Capability::Class> getCapabilityClasses() const override;

	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const override;
	shared_ptr<Capability> getCapability(int32_t nCapabilityId) const override;

	// from DeviceMgmtCapability
	shared_ptr<DeviceManager> getDeviceManager() const override;
	shared_ptr<DeviceManager> getDeviceManager() override;
protected:
	StdParentDeviceManager() = default;

private:
	StdParentDeviceManager(const StdParentDeviceManager& oSource) = delete;
	StdParentDeviceManager& operator=(const StdParentDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif	/* _STMI_STD_PARENT_DEVICE_MANAGER_H_ */

