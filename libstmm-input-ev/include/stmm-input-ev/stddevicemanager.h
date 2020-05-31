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
 * File:   stddevicemanager.h
 */

#ifndef STMI_STD_DEVICE_MANAGER_H
#define STMI_STD_DEVICE_MANAGER_H

#include "devicemgmtevent.h"
#include "devicemgmtcapability.h"

#include <stmm-input-base/basicdevicemanager.h>
#include <stmm-input/capability.h>
#include <stmm-input/event.h>

#include <vector>
#include <cstdint>
#include <memory>

namespace stmi { class ChildDeviceManager; }
namespace stmi { class Device; }
namespace stmi { class DeviceManager; }

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

/** Helper class for implementing a device manager with DeviceMgmtCapability.
 */
class StdDeviceManager : public BasicDeviceManager
{
public:
	using BasicDeviceManager::getDevice;

	std::vector<Capability::Class> getCapabilityClasses() const noexcept override;

	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const noexcept override;
	shared_ptr<Capability> getCapability(int32_t nCapabilityId) const noexcept override;

protected:
	/** Constructor.
	 * If bEnableEventClasses is `true` then all event classes in aEnDisableEventClasses are enabled, all others disabled,
	 * if `false` then all event classes supported by this instance are enabled except those in aEnDisableEventClasses.
	 * StdDeviceManager doesn't allow disabling event classes once constructed, only enabling.
	 * Example: To enable all the event classes supported by this instance pass
	 *
	 *     bEnableEventClasses = false,  aEnDisableEventClasses = {}
	 *
	 * @param aDeviceCapabitityClasses Vector of registered (device) capability classes supported by this manager.
	 * @param aEventClasses Vector of registered event classes supported by this manager.
	 * @param bEnableEventClasses Whether to enable or disable all but aEnDisableEventClasses.
	 * @param aEnDisableEventClasses The event classes to be enabled or disabled according to bEnableEventClasses.
	 */
	StdDeviceManager(const std::vector<Capability::Class>& aDeviceCapabitityClasses
					, const std::vector<Event::Class>& aEventClasses
					, bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses) noexcept;
	/** Sends a device mgmt event to the listeners.
	 * The event is sent to a listener only if its callif permits it and
	 * the event class is enabled.
	 * @param eMgmtType The type.
	 * @param refDevice The device involved. Cannot be null.
	 */
	void sendDeviceMgmtToListeners(const DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice) noexcept;
protected:
	void checkWeakPtr() const noexcept;
	shared_ptr<DeviceMgmtCapability> getDeviceMgmtCapability() noexcept;
private:
	////
	//class ReDeviceMgmtEvent :public DeviceMgmtEvent
	//{
	//public:
	//	ReDeviceMgmtEvent(int64_t nTimeUsec, const shared_ptr<DeviceMgmtCapability>& refDeviceMgmtCapability
	//					, DEVICE_MGMT_TYPE eDeviceMgmtType, const shared_ptr<Device>& refDevice)
	//	: DeviceMgmtEvent(nTimeUsec, refDeviceMgmtCapability, eDeviceMgmtType, refDevice)
	//	{
	//	}
	//	void reInit(int64_t nTimeUsec, const shared_ptr<DeviceMgmtCapability>& refDeviceMgmtCapability
	//				, DEVICE_MGMT_TYPE eDeviceMgmtType, const shared_ptr<Device>& refDevice)
	//	{
	//		setTimeUsec(nTimeUsec);
	//		setAccessor({});
	//		setDeviceMgmtCapability(refDeviceMgmtCapability);
	//		setDeviceMgmtType(eDeviceMgmtType);
	//		setDevice(refDevice);
	//	}
	//};
	//Private::Recycler<ReDeviceMgmtEvent, Event> m_oDeviceMgmtRecycler;
	const int32_t m_nClassIdxDeviceMgmtEvent;

	friend class PrivStdDeviceMgmtCapability;
	class PrivStdDeviceMgmtCapability : public DeviceMgmtCapability
	{
	public:
		shared_ptr<DeviceManager> getDeviceManager() const noexcept override;
		shared_ptr<DeviceManager> getRootDeviceManager() const noexcept override;
	public:
		weak_ptr<ChildDeviceManager> m_refWeakOwner;
	};
	shared_ptr<PrivStdDeviceMgmtCapability> m_refDeviceMgmtCapability;
private:
	StdDeviceManager() = delete;
	StdDeviceManager(const StdDeviceManager& oSource) = delete;
	StdDeviceManager& operator=(const StdDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_STD_DEVICE_MANAGER_H */
