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
 * File:   tst99devicemanager.h
 */

#ifndef _TST99_DEVICE_MANAGER_H_
#define	_TST99_DEVICE_MANAGER_H_

#include "stddevicemanager.h"

#include "tst99capability.h"
#include "tst99event.h"

namespace stmi
{

class Tst99DeviceManager : public stmi::StdDeviceManager
{
public:
	Tst99DeviceManager(bool bEnableEventClasses, const std::vector<stmi::Event::Class>& aEnDisableEventClasses);
	virtual ~Tst99DeviceManager();

	////////////////////////////////////////////////////////////////////////////
	class Tst99Device : public Device, public Tst99Capability, public std::enable_shared_from_this<Tst99Device>
	{
	public:
		Tst99Device(const shared_ptr<DeviceManager>& refDeviceManager, const std::string& sName, int32_t nTestData)
		: Device()
		, m_sName(sName)
		, m_refDeviceManager(refDeviceManager)
		, m_nTestData(nTestData)
		{
		}
		shared_ptr<Device> getDevice() const override
		{
			shared_ptr<const Tst99Device> refConstThis = shared_from_this();
			shared_ptr<Tst99Device> refThis = std::const_pointer_cast<Tst99Device>(refConstThis);
			return refThis;
		}
		int32_t getData() const override
		{
			return m_nTestData;
		}
		std::string getName() const override
		{
			return m_sName;
		}
		shared_ptr<DeviceManager> getDeviceManager() const override
		{
			return m_refDeviceManager.lock();
		}
		shared_ptr<Capability> getCapability(const Capability::Class& oClass) const override
		{
			if (oClass == this->getCapabilityClass()) {
				shared_ptr<const Tst99Capability> refConstThis = shared_from_this();
				shared_ptr<Tst99Capability> refThis = std::const_pointer_cast<Tst99Capability>(refConstThis);
				return refThis;
			} else {
				return shared_ptr<Capability>{};
			}
		}
		std::vector<Capability::Class> getCapabilityClasses() const override
		{
			return {Tst99Capability::getClass()};
		}
		shared_ptr<Capability> getCapability(int32_t nCapabilityId) const override
		{
			if (nCapabilityId == Tst99Capability::getId()) {
				shared_ptr<const Tst99Capability> refConstThis = shared_from_this();
				shared_ptr<Tst99Capability> refThis = std::const_pointer_cast<Tst99Capability>(refConstThis);
				return refThis;
			} else {
				return shared_ptr<Capability>{};
			}
		}
		std::vector<int32_t> getCapabilities() const override
		{
			return {Tst99Capability::getId()};
		}
	private:
		std::string m_sName;
		weak_ptr<DeviceManager> m_refDeviceManager;
		int32_t m_nTestData;
	private:
		Tst99Device() = delete;
	};

	bool addAccessor(const shared_ptr<Accessor>& refAccessor) override
	{
//std::cout << "Tst99DeviceManager::addAccessor" << '\n';
		StdDeviceManager::addAccessor(refAccessor);
		static int32_t s_nCount = 0;
		simulateNewDevice("Dev" + std::to_string(s_nCount), 70 + s_nCount);
		return true;
	}
	void enableEventClass(const Event::Class& oEventClass) override
	{
//std::cout << "Tst99DeviceManager::enableEventClass()" << '\n';
		StdDeviceManager::enableEventClass(oEventClass);
		auto aDeviceIds = getDevices();
		for (int32_t nDevId : aDeviceIds) {
//std::cout << "Tst99DeviceManager::enableEventClass()  nDevId=" << nDevId << '\n';
			simulateTst99Event(nDevId, 99);
		}
	}

	int32_t simulateNewDevice(const std::string& sName, int32_t nData)
	{
		auto refChildThis = shared_from_this();
		auto refDevice = std::make_shared<Tst99Device>(refChildThis, sName, nData);
		#ifndef NDEBUG
		const bool bAdded = 
		#endif
		StdDeviceManager::addDevice(refDevice);
		assert(bAdded);
		sendDeviceMgmtToListeners(stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED, refDevice);
		Device* p0Device = refDevice.get();
		return p0Device->getId();
	}

	int32_t simulateEvent(const shared_ptr<Event>& refEvent)
	{
		if (!isEventClassEnabled(refEvent->getEventClass())) {
//std::cout << "Tst99DeviceManager::simulateTst99Event()  refEvent->getEventClass()=" << refEvent->getEventClass().getId() << '\n';
			return -1;
		}
		int32_t nTotSent = 0;
		auto refListeners = getListeners();
		for (ListenerData* p0ListenerData : *refListeners) {
			const bool bSent = p0ListenerData->handleEventCallIf(-1, refEvent);
			if (bSent) {
				++nTotSent;
			}
		}
		return nTotSent;
	}
	int32_t simulateTst99Event(int32_t nDeviceId, int32_t nButton)
	{
		auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
		if (!refDevice) {
			return -1; //-------------------------------------------------------
		}
//std::cout << "Tst99DeviceManager::simulateTst99Event()  nButton=" << nButton << '\n';
		shared_ptr<Tst99Capability> refTst99Capability;
		const bool bFound = refDevice->getCapability(refTst99Capability);
		if (!bFound) {
			return -1; //-------------------------------------------------------
		}
		const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
//std::cout << "Tst99DeviceManager::simulateTst99Event()  nTimeUsec=" << nTimeUsec << '\n';
		shared_ptr<Event> refTst99Event = std::make_shared<Tst99Event>(nTimeUsec, refTst99Capability, nButton);
		return simulateEvent(refTst99Event);
	}

protected:
	/* Does nothing. */
	void finalizeListener(ListenerData& /*oListenerData*/) override
	{
		//StdDeviceManager::finalizeListener(oListenerData);
	}
private:
	Tst99DeviceManager() = delete;
	Tst99DeviceManager(const Tst99DeviceManager& oSource) = delete;
	Tst99DeviceManager& operator=(const Tst99DeviceManager& oSource) = delete;
};

} // namespace stmi

#endif /* _TST99_DEVICE_MANAGER_H_ */

