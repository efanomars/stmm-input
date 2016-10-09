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
 * File:   fakekeydevice.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef _STMI_TESTING_FAKE_KEY_DEVICE_H_
#define _STMI_TESTING_FAKE_KEY_DEVICE_H_

#include "fakedevice.h"

#ifndef STMI_TESTING_ADD_EVS
#include <stmm-input-ev/keycapability.h>
#else
#include "keycapability.h"
#endif
#include <stmm-input/devicemanager.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
/** Fake key capability. */
class FakeKeyCapability : public KeyCapability
{
public:
	/** Constructor.
	 * @param p0Device The fake device the instance belongs to. Cannot be null.
	 */
	FakeKeyCapability(FakeDevice<FakeKeyCapability>* p0Device)
	: m_p0Device(p0Device)
	{
		assert(p0Device != nullptr);
	}
	shared_ptr<Device> getDevice() const override
	{
		return m_p0Device->shared_from_this();
	}
	shared_ptr<Device> getDevice() override
	{
		return m_p0Device->shared_from_this();
	}
	/** Not implemented. */
	bool isKeyboard() const override
	{
		return true;
	}
private:
	FakeDevice<FakeKeyCapability>* m_p0Device;
};

////////////////////////////////////////////////////////////////////////////////
/** Fake device that implements the key capability. */
class FakeKeyDevice : public FakeDevice<FakeKeyCapability>
{
public:
	/** @see FakeDevice<CAPA>::FakeDevice()
	 */
	FakeKeyDevice(const shared_ptr<DeviceManager>& refDeviceManager, const std::string& sName)
	: FakeDevice<FakeKeyCapability>(refDeviceManager, sName, std::make_shared<FakeKeyCapability>(this))
	{
	}
	/** @see FakeDevice<CAPA>::FakeDevice()
	 */
	FakeKeyDevice()
	: FakeKeyDevice(shared_ptr<DeviceManager>{}, "")
	{
	}
};

} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FAKE_KEY_DEVICE_H_ */
