/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   devicemgmtevent.cc
 */

#include "devicemgmtevent.h"

namespace stmi { class Device; }

namespace stmi
{

const char* const DeviceMgmtEvent::s_sClassId = "stmi::DeviceMgmt:DeviceMgmtEvent";
Event::RegisterClass<DeviceMgmtEvent> DeviceMgmtEvent::s_oInstall(s_sClassId);

DeviceMgmtEvent::DeviceMgmtEvent(int64_t nTimeUsec, const shared_ptr<DeviceMgmtCapability>& refDeviceMgmtCapability
								, DEVICE_MGMT_TYPE eDeviceMgmtType, const shared_ptr<Device>& refDevice) noexcept
: Event(s_oInstall.getEventClass(), nTimeUsec, (refDeviceMgmtCapability ? refDeviceMgmtCapability->getId() : -1))
, m_refDevice(refDevice)
, m_eDeviceMgmtType(eDeviceMgmtType)
, m_refDeviceMgmtCapability(refDeviceMgmtCapability)
{
	assert(refDeviceMgmtCapability);
	assert(refDevice);
	assert((eDeviceMgmtType >= DEVICE_MGMT_ADDED) && (eDeviceMgmtType <= DEVICE_MGMT_CHANGED));
}

} // namespace stmi

