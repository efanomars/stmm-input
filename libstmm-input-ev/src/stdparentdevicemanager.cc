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
 * File:   stdparentdevicemanager.cc
 */

#include "stdparentdevicemanager.h"

namespace stmi
{

shared_ptr<StdParentDeviceManager> StdParentDeviceManager::create(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager)
{
	for (auto& refChild : aChildDeviceManager) {
		auto refCapa = refChild->getCapability(DeviceMgmtCapability::getClass());
		if (!refCapa) {
			return shared_ptr<StdParentDeviceManager>{};
		}
	}
	shared_ptr<StdParentDeviceManager> refInstance(new StdParentDeviceManager());
	refInstance->init(aChildDeviceManager);
	return refInstance;
}

shared_ptr<Capability> StdParentDeviceManager::getCapability(const Capability::Class& oClass) const
{
	shared_ptr<Capability> refCapa;
	if (oClass == typeid(DeviceMgmtCapability)) {
		shared_ptr<ChildDeviceManager> refChildThis = std::const_pointer_cast<ChildDeviceManager>(shared_from_this());
		shared_ptr<StdParentDeviceManager> refThis = std::static_pointer_cast<StdParentDeviceManager>(refChildThis);
		refCapa = refThis;
	}
	return refCapa;
}
shared_ptr<Capability> StdParentDeviceManager::getCapability(int32_t nCapabilityId) const
{
	const auto nDeviceMgmtCapaId = DeviceMgmtCapability::getId();
	if (nCapabilityId != nDeviceMgmtCapaId) {
		return shared_ptr<Capability>{};
	}
	shared_ptr<ChildDeviceManager> refChildThis = std::const_pointer_cast<ChildDeviceManager>(shared_from_this());
	shared_ptr<StdParentDeviceManager> refThis = std::static_pointer_cast<StdParentDeviceManager>(refChildThis);
	return refThis;
}

std::vector<Capability::Class> StdParentDeviceManager::getCapabilityClasses() const
{
	return {DeviceMgmtCapability::getClass()};
}
shared_ptr<DeviceManager> StdParentDeviceManager::getDeviceManager() const
{
	return getRoot();
}
shared_ptr<DeviceManager> StdParentDeviceManager::getDeviceManager()
{
	return getRoot();
}

} // namespace stmi

