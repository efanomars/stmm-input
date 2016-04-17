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
 * File:   fakejoystickdevice.h
 */

#ifndef _STMI_TESTING_FAKE_JOYSTICK_DEVICE_H_
#define _STMI_TESTING_FAKE_JOYSTICK_DEVICE_H_

#include "devicemanager.h"
#include "joystickcapability.h"
#include "fakedevice.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class FakeJoystickDevice : public FakeDevice<JoystickCapability>
{
public:
	bool getHasButton(JoystickCapability::BUTTON /*eButton*/) const override
	{
		return false;
	}
	int32_t getTotHats() const override
	{
		return 0;
	}
	bool getHasAxis(JoystickCapability::AXIS /*eAxis*/) const override
	{
		return false;
	}
	bool isButtonPressed(JoystickCapability::BUTTON /*eButton*/) const override
	{
		return false;
	}
	HAT_VALUE getHatValue(int32_t /*nHat*/) const override
	{
		return JoystickCapability::HAT_CENTER;
	}
	int32_t getAxisValue(JoystickCapability::AXIS /*eAxis*/) const override
	{
		return 0;
	}
};

} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FAKE_JOYSTICK_DEVICE_H_ */
