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
 * File:   fakejoystickdevice.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMI_TESTING_FAKE_JOYSTICK_DEVICE_H
#define STMI_TESTING_FAKE_JOYSTICK_DEVICE_H

#include "fakedevice.h"

#ifndef STMI_TESTING_ADD_EVS
#include <stmm-input-ev/joystickcapability.h>
#else
#include "joystickcapability.h"
#endif
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
/** Fake joystick capability. */
class FakeJoystickCapability : public JoystickCapability
{
public:
	/** Constructor.
	 * @param p0Device The fake device the instance belongs to. Cannot be null.
	 */
	FakeJoystickCapability(FakeDevice<FakeJoystickCapability>* p0Device) noexcept
	: m_p0Device(p0Device)
	{
		assert(p0Device != nullptr);
	}
	shared_ptr<Device> getDevice() const noexcept override
	{
		return m_p0Device->shared_from_this();
	}
	/** Not implemented. */
	bool getHasButton(JoystickCapability::BUTTON /*eButton*/) const noexcept override
	{
		return false;
	}
	/** Not implemented. */
	int32_t getTotHats() const noexcept override
	{
		return 0;
	}
	/** Not implemented. */
	bool getHasAxis(JoystickCapability::AXIS /*eAxis*/) const noexcept override
	{
		return false;
	}
	/** Not implemented. */
	bool isButtonPressed(JoystickCapability::BUTTON /*eButton*/) const noexcept override
	{
		return false;
	}
	/** Not implemented. */
	HAT_VALUE getHatValue(int32_t /*nHat*/) const noexcept override
	{
		return JoystickCapability::HAT_CENTER;
	}
	/** Not implemented. */
	int32_t getAxisValue(JoystickCapability::AXIS /*eAxis*/) const noexcept override
	{
		return 0;
	}
private:
	FakeDevice<FakeJoystickCapability>* m_p0Device;
};

////////////////////////////////////////////////////////////////////////////////
/** Fake device that implements the joystick capability. */
class FakeJoystickDevice : public FakeDevice<FakeJoystickCapability>
{
public:
	/** @see FakeDevice<CAPA>::FakeDevice()
	 */
	FakeJoystickDevice(const shared_ptr<DeviceManager>& refDeviceManager, const std::string& sName) noexcept
	: FakeDevice<FakeJoystickCapability>(refDeviceManager, sName, std::make_shared<FakeJoystickCapability>(this))
	{
	}
	/** @see FakeDevice<CAPA>::FakeDevice()
	 */
	FakeJoystickDevice() noexcept
	: FakeJoystickDevice(shared_ptr<DeviceManager>{}, "")
	{
	}
};

} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FAKE_JOYSTICK_DEVICE_H */
