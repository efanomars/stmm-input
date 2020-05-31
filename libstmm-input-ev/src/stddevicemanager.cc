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
 * File:   stddevicemanager.cc
 */

#include "stddevicemanager.h"

#include <stmm-input/devicemanager.h>
#include <stmm-input-base/parentdevicemanager.h>
#include <stmm-input-base/childdevicemanager.h>

#include <cassert>
#include <memory>

#ifndef NDEBUG
//#include <iostream>
#endif //NDEBUG

namespace stmi { class Device; }

namespace stmi
{

shared_ptr<DeviceManager> StdDeviceManager::PrivStdDeviceMgmtCapability::getDeviceManager() const noexcept
{
	return m_refWeakOwner.lock();
}
shared_ptr<DeviceManager> StdDeviceManager::PrivStdDeviceMgmtCapability::getRootDeviceManager() const noexcept
{
	shared_ptr<ChildDeviceManager> refChildDeviceManager = m_refWeakOwner.lock();
	return refChildDeviceManager->getRoot();
}


StdDeviceManager::StdDeviceManager(const std::vector<Capability::Class>& aDeviceCapabitityClasses
									 , const std::vector<Event::Class>& aEventClasses
									 , bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses) noexcept
: BasicDeviceManager({DeviceMgmtCapability::getClass()}
					, aDeviceCapabitityClasses, aEventClasses, bEnableEventClasses, aEnDisableEventClasses)
, m_nClassIdxDeviceMgmtEvent(getEventClassIndex(Event::Class{typeid(DeviceMgmtEvent)}))
, m_refDeviceMgmtCapability(std::make_shared<PrivStdDeviceMgmtCapability>())
{
}
void StdDeviceManager::checkWeakPtr() const noexcept
{
	if (m_refDeviceMgmtCapability->m_refWeakOwner.expired()) {
		StdDeviceManager* p0This = const_cast<StdDeviceManager*>(this);
		m_refDeviceMgmtCapability->m_refWeakOwner = p0This->shared_from_this();
	}
}
std::vector<Capability::Class> StdDeviceManager::getCapabilityClasses() const noexcept
{
	return {DeviceMgmtCapability::getClass()};
}
shared_ptr<Capability> StdDeviceManager::getCapability(const Capability::Class& oClass) const noexcept
{
//std::cout << "StdDeviceManager::getCapability " << oClass.getId() << '\n';
	if (oClass == typeid(DeviceMgmtCapability)) {
		checkWeakPtr();
		return m_refDeviceMgmtCapability;
	}
	return shared_ptr<Capability>{};
}
shared_ptr<Capability> StdDeviceManager::getCapability(int32_t nCapabilityId) const noexcept
{
	const auto nDeviceMgmtCapaId = m_refDeviceMgmtCapability->getId();
	if (nCapabilityId != nDeviceMgmtCapaId) {
		return shared_ptr<Capability>{};
	}
	checkWeakPtr();
	return m_refDeviceMgmtCapability;
}
shared_ptr<DeviceMgmtCapability> StdDeviceManager::getDeviceMgmtCapability() noexcept
{
	checkWeakPtr();
	return m_refDeviceMgmtCapability;
}
void StdDeviceManager::sendDeviceMgmtToListeners(const DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice) noexcept
{
	if (!isEventClassEnabled(Event::Class{typeid(DeviceMgmtEvent)})) {
		return;
	}
	shared_ptr<Capability> refCapa = getCapability(DeviceMgmtCapability::getClass());
	auto refMgmtCapa = std::static_pointer_cast<DeviceMgmtCapability>(refCapa);
	assert(refMgmtCapa);
	//
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	//
	shared_ptr<DeviceMgmtEvent> refEvent = std::make_shared<DeviceMgmtEvent>(nTimeUsec, refMgmtCapa, eMgmtType, refDevice);
	auto refListeners = getListeners();
	for (auto& p0ListenerData : *refListeners) {
		p0ListenerData->handleEventCallIf(m_nClassIdxDeviceMgmtEvent, refEvent);
	}
}

} // namespace stmi
