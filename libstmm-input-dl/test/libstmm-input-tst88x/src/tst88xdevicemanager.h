/*
 * Copyright © 2017-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   tst88xdevicemanager.h
 */

#ifndef _TST88X_TST88_DEVICE_MANAGER_H_
#define	_TST88X_TST88_DEVICE_MANAGER_H_

#include "tst88capability.h"

#include "stddevicemanager.h"
#include "keycapability.h"

#include <stmm-input/device.h>

namespace stmi
{

class Tst88XDeviceManager : public stmi::StdDeviceManager
{
public:
	Tst88XDeviceManager(bool bEnableEventClasses, const std::vector<stmi::Event::Class>& aEnDisableEventClasses) noexcept;
	virtual ~Tst88XDeviceManager() noexcept;

	class Tst88XDevice;

	class Tst88XKeyCapability : public KeyCapability
	{
	public:
		/** Constructor.
		 * @param p0Device The fake device the instance belongs to. Cannot be null.
		 */
		explicit Tst88XKeyCapability(Tst88XDevice* p0Device) noexcept
		: m_p0Device(p0Device)
		{
			assert(p0Device != nullptr);
		}
		shared_ptr<Device> getDevice() const noexcept override
		{
			return m_p0Device->shared_from_this();
		}
		/** Not implemented. */
		bool isKeyboard() const noexcept override
		{
			return true;
		}
	private:
		Tst88XDevice* m_p0Device;
	};
	////////////////////////////////////////////////////////////////////////////
	class Tst88XDevice : public Device, public test::Tst88Capability, public std::enable_shared_from_this<Tst88XDevice>
	{
	public:
		Tst88XDevice(const shared_ptr<DeviceManager>& refDeviceManager, const std::string& sName, int32_t nTestData) noexcept
		: Device()
		, m_sName(sName)
		, m_refDeviceManager(refDeviceManager)
		, m_refKeyCapa(std::make_shared<Tst88XKeyCapability>(this))
		, m_nTestData(nTestData)
		{
		}
		shared_ptr<Device> getDevice() const noexcept override
		{
			shared_ptr<const Tst88XDevice> refConstThis = shared_from_this();
			shared_ptr<Tst88XDevice> refThis = std::const_pointer_cast<Tst88XDevice>(refConstThis);
			return refThis;
		}
		int32_t getData() const noexcept override
		{
			return m_nTestData;
		}
		std::string getName() const noexcept override
		{
			return m_sName;
		}
		shared_ptr<DeviceManager> getDeviceManager() const noexcept override
		{
			return m_refDeviceManager.lock();
		}
		shared_ptr<Capability> getCapability(const Capability::Class& oClass) const noexcept override
		{
			if (oClass == this->getCapabilityClass()) {
				shared_ptr<const test::Tst88Capability> refConstThis = shared_from_this();
				shared_ptr<test::Tst88Capability> refThis = std::const_pointer_cast<test::Tst88Capability>(refConstThis);
				return refThis;
			} else if (oClass == m_refKeyCapa->getCapabilityClass()) {
				return m_refKeyCapa;
			} else {
				return shared_ptr<Capability>{};
			}
		}
		std::vector<Capability::Class> getCapabilityClasses() const noexcept override
		{
			return {KeyCapability::getClass(), test::Tst88Capability::getClass()};
		}
		shared_ptr<Capability> getCapability(int32_t nCapabilityId) const noexcept override
		{
			if (nCapabilityId == test::Tst88Capability::getId()) {
				shared_ptr<const test::Tst88Capability> refConstThis = shared_from_this();
				shared_ptr<test::Tst88Capability> refThis = std::const_pointer_cast<test::Tst88Capability>(refConstThis);
				return refThis;
			} else if (nCapabilityId == m_refKeyCapa->getId()) {
				return m_refKeyCapa;
			} else {
				return shared_ptr<Capability>{};
			}
		}
		std::vector<int32_t> getCapabilities() const noexcept override
		{
			return {test::Tst88Capability::getId(), m_refKeyCapa->getId()};
		}
	private:
		std::string m_sName;
		weak_ptr<DeviceManager> m_refDeviceManager;
		shared_ptr<Tst88XKeyCapability> m_refKeyCapa;
		int32_t m_nTestData;
	private:
		Tst88XDevice() = delete;
	};

	bool addAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override
	{
//std::cout << "Tst88XDeviceManager::addAccessor" << '\n';
		StdDeviceManager::addAccessor(refAccessor);
		static int32_t s_nCount = 0;
		simulateNewDevice("Dev" + std::to_string(s_nCount), 70 + s_nCount);
		return true;
	}
	void enableEventClass(const Event::Class& oEventClass) noexcept override
	{
		StdDeviceManager::enableEventClass(oEventClass);
	}

	int32_t simulateNewDevice(const std::string& sName, int32_t nData) noexcept
	{
		auto refChildThis = shared_from_this();
		auto refDevice = std::make_shared<Tst88XDevice>(refChildThis, sName, nData);
		#ifndef NDEBUG
		const bool bAdded = 
		#endif
		StdDeviceManager::addDevice(refDevice);
		assert(bAdded);
		sendDeviceMgmtToListeners(stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED, refDevice);
		Device* p0Device = refDevice.get();
		return p0Device->getId();
	}

protected:
	/* Does nothing. */
	void finalizeListener(ListenerData& /*oListenerData*/) noexcept override
	{
		//StdDeviceManager::finalizeListener(oListenerData);
	}
private:
	Tst88XDeviceManager() = delete;
	Tst88XDeviceManager(const Tst88XDeviceManager& oSource) = delete;
	Tst88XDeviceManager& operator=(const Tst88XDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif /* _TST88X_TST88_DEVICE_MANAGER_H_ */

