/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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

#include <stmm-input-base/parentdevicemanager.h>

#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG

namespace stmi
{

StdDeviceManager::StdDeviceManager(const std::vector<Capability::Class>& aDeviceCapabitityClasses
									 , const std::vector<Event::Class>& aEventClasses
									 , bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses)
: BasicDeviceManager({DeviceMgmtCapability::getClass()}
					, aDeviceCapabitityClasses, aEventClasses, bEnableEventClasses, aEnDisableEventClasses)
, m_nClassIdxDeviceMgmtEvent(getEventClassIndex(typeid(DeviceMgmtEvent)))
{
}
std::vector<Capability::Class> StdDeviceManager::getCapabilityClasses() const
{
	return {DeviceMgmtCapability::getClass()};
}
shared_ptr<Capability> StdDeviceManager::getCapability(const Capability::Class& oClass) const
{
	shared_ptr<Capability> refCapa;
	if (oClass == typeid(DeviceMgmtCapability)) {
		shared_ptr<const ChildDeviceManager> refConstThis = shared_from_this();
		shared_ptr<ChildDeviceManager> refChildThis = std::const_pointer_cast<ChildDeviceManager>(refConstThis);
		shared_ptr<StdDeviceManager> refThis = std::static_pointer_cast<StdDeviceManager>(refChildThis);
		refCapa = refThis;
	}
	return refCapa;
}
shared_ptr<Capability> StdDeviceManager::getCapability(int32_t nCapabilityId) const
{
	const auto nDeviceMgmtCapaId = DeviceMgmtCapability::getId();
	if (nCapabilityId != nDeviceMgmtCapaId) {
		return shared_ptr<Capability>{};
	}
	shared_ptr<const ChildDeviceManager> refConstThis = shared_from_this();
	shared_ptr<ChildDeviceManager> refChildThis = std::const_pointer_cast<ChildDeviceManager>(refConstThis);
	shared_ptr<StdDeviceManager> refThis = std::static_pointer_cast<StdDeviceManager>(refChildThis);
	return refThis;
}
shared_ptr<DeviceManager> StdDeviceManager::getRootDeviceManager() const
{
	shared_ptr<ChildDeviceManager> refChildThis = std::const_pointer_cast<ChildDeviceManager>(shared_from_this());
	shared_ptr<StdDeviceManager> refThis = std::static_pointer_cast<StdDeviceManager>(refChildThis);
	return refThis;
}
std::vector<shared_ptr<Capability>> StdDeviceManager::getNodeDeviceManagerCapabilities(const Capability::Class& oClass) const
{
	std::vector<shared_ptr<Capability>> aCapas;
	shared_ptr<ChildDeviceManager> refChildThis = std::const_pointer_cast<ChildDeviceManager>(shared_from_this());
	getPriNodeDeviceManagerCapabilities(refChildThis, aCapas, oClass);
	return aCapas;
}
void StdDeviceManager::getPriNodeDeviceManagerCapabilities(const shared_ptr<ChildDeviceManager>& refCur
														, std::vector<shared_ptr<Capability>>& aCapas
														, const Capability::Class& oClass) const
{
	if (refCur->isParent()) {
		auto refParent = refCur->getAsParent();
		auto aChildren = refParent->getChildren();
		for (auto& refChild : aChildren) {
			getPriNodeDeviceManagerCapabilities(refChild, aCapas, oClass);
		}
	}
	auto refCapa = refCur->getCapability(oClass);
	if (refCapa) {
		aCapas.push_back(refCapa);
	}
}
shared_ptr<DeviceManager> StdDeviceManager::getDeviceManager() const
{
	return getRoot();
}
void StdDeviceManager::sendDeviceMgmtToListeners(const DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice)
{
	if (!isEventClassEnabled(typeid(DeviceMgmtEvent))) {
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
