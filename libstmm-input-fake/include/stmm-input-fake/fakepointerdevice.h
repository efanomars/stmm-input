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
 * File:   fakepointerdevice.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMI_TESTING_FAKE_POINTER_DEVICE_H
#define STMI_TESTING_FAKE_POINTER_DEVICE_H

#include "fakedevice.h"

#include <stmm-input-ev/pointercapability.h>

#include <stmm-input/devicemanager.h>

#include <string>
#include <memory>


namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
/** Fake pointer capability. */
class FakePointerCapability : public PointerCapability
{
public:
	/** Constructor.
	 * @param p0Device The fake device the instance belongs to. Cannot be null.
	 */
	FakePointerCapability(FakeDevice<FakePointerCapability>* p0Device) noexcept
	: m_p0Device(p0Device)
	{
		assert(p0Device != nullptr);
	}
	shared_ptr<Device> getDevice() const noexcept override
	{
		return m_p0Device->shared_from_this();
	}
private:
	FakeDevice<FakePointerCapability>* m_p0Device;
};

////////////////////////////////////////////////////////////////////////////////
/** Fake device that implements the pointer capability. */
class FakePointerDevice : public FakeDevice<FakePointerCapability>
{
public:
	/** @see FakeDevice<CAPA>::FakeDevice()
	 */
	FakePointerDevice(const shared_ptr<DeviceManager>& refDeviceManager, const std::string& sName) noexcept
	: FakeDevice<FakePointerCapability>(refDeviceManager, sName, std::make_shared<FakePointerCapability>(this))
	{
	}
	/** @see FakeDevice<CAPA>::FakeDevice()
	 */
	FakePointerDevice() noexcept
	: FakePointerDevice(shared_ptr<DeviceManager>{}, "")
	{
	}
};

} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FAKE_POINTER_DEVICE_H */
