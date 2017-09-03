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
 * File:   devicemgmtevent.h
 */

#ifndef STMI_DEVICE_MGMT_EVENT_H
#define STMI_DEVICE_MGMT_EVENT_H

#include "devicemgmtcapability.h"

#include <stmm-input/event.h>

#include <cassert>

namespace stmi
{

using std::weak_ptr;

/** Event informing about a device being added, removed or changed.
 */
class DeviceMgmtEvent : public Event
{
public:
	/** The event type. */
	enum DEVICE_MGMT_TYPE {
		DEVICE_MGMT_ADDED = 1
		, DEVICE_MGMT_REMOVED = 2
		, DEVICE_MGMT_CHANGED = 3  /**< The meaning is device specific.
									* Example: for a XInput master device it could mean a slave was added or removed.
									*/
	};
	/** The involved device.
	 * If a device is removed (DEVICE_MGMT_REMOVED), it no longer is attached to
	 * its device manager, but its methods should still work and its capabilities
	 * be accessible, at least during the listener`s callback.
	 * @return The device or null if it was deleted.
	 */
	inline shared_ptr<Device> getDevice() const { return m_refDevice.lock(); }
	/** Constructor.
	 * The device and the capability are stored by this instance as std::weak_ptr
	 * to avoid possible cyclic references.
	 *
	 * @param nTimeUsec The time in microseconds from time epoch.
	 * @param refDeviceMgmtCapability The device manager's capability. Can't be null.
	 * @param eDeviceMgmtType The type.
	 * @param refDevice The involved device. Can't be null.
	 */
	DeviceMgmtEvent(int64_t nTimeUsec, const shared_ptr<DeviceMgmtCapability>& refDeviceMgmtCapability
					, DEVICE_MGMT_TYPE eDeviceMgmtType, const shared_ptr<Device>& refDevice);
	/** What happens to the involved device.
	 * @return The type.
	 */
	inline DEVICE_MGMT_TYPE getDeviceMgmtType() const { return m_eDeviceMgmtType; }
	/** Returns the capability that generated this event.
	 * @return If null the capability (and probably the device manager) was deleted.
	 */
	inline shared_ptr<DeviceMgmtCapability> getDeviceMgmtCapability() const { return m_refDeviceMgmtCapability.lock(); }
	//
	shared_ptr<Capability> getCapability() const override { return m_refDeviceMgmtCapability.lock(); }
	/** The class id of this event. */
	static const char* const s_sClassId;
	static const Event::Class& getClass()
	{
		static const Event::Class s_oDeviceMgmtClass = s_oInstall.getEventClass();
		return s_oDeviceMgmtClass;
	}
protected:
	/** Sets the device.
	 * @param refDevice The involved device. Can't be null.
	 */
	inline void setDevice(const shared_ptr<Device>& refDevice)
	{
		assert(refDevice);
		m_refDevice = refDevice;
	}
	/** Sets the type.
	 * @param eDeviceMgmtType The type.
	 */
	inline void setDeviceMgmtType(DEVICE_MGMT_TYPE eDeviceMgmtType)
	{
		assert((eDeviceMgmtType >= DEVICE_MGMT_ADDED) && (eDeviceMgmtType <= DEVICE_MGMT_CHANGED));
		m_eDeviceMgmtType = eDeviceMgmtType;
	}
	/** Sets the capability.
	 * @param refDeviceMgmtCapability The device manager's capability. Can't be null.
	 */
	inline void setDeviceMgmtCapability(const shared_ptr<DeviceMgmtCapability>& refDeviceMgmtCapability)
	{
		assert(refDeviceMgmtCapability);
		setCapabilityId(refDeviceMgmtCapability->getId());
		m_refDeviceMgmtCapability = refDeviceMgmtCapability;
	}
private:
	weak_ptr<Device> m_refDevice;
	DEVICE_MGMT_TYPE m_eDeviceMgmtType;
	weak_ptr<DeviceMgmtCapability> m_refDeviceMgmtCapability;
	//
	static RegisterClass<DeviceMgmtEvent> s_oInstall;
private:
	DeviceMgmtEvent() = delete;
};

} // namespace stmi

#endif /* STMI_DEVICE_MGMT_EVENT_H */
