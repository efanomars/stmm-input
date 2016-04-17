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
 * File:   joystickcapability.h
 */

#ifndef _STMI_JOYSTICK_CAPABILITY_H_
#define _STMI_JOYSTICK_CAPABILITY_H_

#include <cassert>
#include <iostream>

#include <stmm-input/capability.h>

namespace stmi
{

/** The joystick capability of a device.
 * Devices with this capability can generate joystick events.
 */
class JoystickCapability : public Capability
{
public:
	/** The valid states of a hat.
	 */
	enum HAT_VALUE {
		HAT_VALUE_NOT_SET = -1
		, HAT_CENTER = 0
		, HAT_UP = 4
		, HAT_RIGHT = 2
		, HAT_DOWN = 8
		, HAT_LEFT = 1
		, HAT_RIGHTUP = (HAT_RIGHT | HAT_UP)
		, HAT_RIGHTDOWN = (HAT_RIGHT | HAT_DOWN)
		, HAT_LEFTUP = (HAT_LEFT | HAT_UP)
		, HAT_LEFTDOWN = (HAT_LEFT | HAT_DOWN)
		, HAT_CENTER_CANCEL = 16
	};
	static bool isValidHatValue(HAT_VALUE eValue)
	{
		switch (eValue) {
			case HAT_CENTER:
			case HAT_UP:
			case HAT_RIGHT:
			case HAT_DOWN:
			case HAT_LEFT:
			case HAT_RIGHTUP:
			case HAT_RIGHTDOWN:
			case HAT_LEFTUP:
			case HAT_LEFTDOWN:
			case HAT_CENTER_CANCEL:
				return true;
			default:
				return false;
		}
	}
	// See <linux/input.h>
	/** The valid buttons.
	 */
	enum BUTTON {
		BUTTON_TRIGGER = 0x120
		, BUTTON_THUMB = 0x121
		, BUTTON_THUMB2 = 0x122
		, BUTTON_TOP = 0x123
		, BUTTON_TOP2 = 0x124
		, BUTTON_PINKIE = 0x125
		, BUTTON_BASE = 0x126
		, BUTTON_A = 0x130
		, BUTTON_B = 0x131
		, BUTTON_C = 0x132
		, BUTTON_X = 0x133
		, BUTTON_Y = 0x134
		, BUTTON_Z = 0x135
		, BUTTON_TL = 0x136
		, BUTTON_TR = 0x137
		, BUTTON_TL2 = 0x138
		, BUTTON_TR2 = 0x139
		, BUTTON_SELECT = 0x13a
		, BUTTON_START = 0x13b
		, BUTTON_MODE = 0x13c
		, BUTTON_THUMBL = 0x13d
		, BUTTON_THUMBR = 0x13e
		//
		, BUTTON_GEAR_DOWN = 0x150
		, BUTTON_GEAR_UP = 0x151
	};
	static bool isValidButton(BUTTON eButton)
	{
		switch (eButton) {
			case BUTTON_TRIGGER:
			case BUTTON_THUMB:
			case BUTTON_THUMB2:
			case BUTTON_TOP:
			case BUTTON_TOP2:
			case BUTTON_PINKIE:
			case BUTTON_BASE:
			//
			case BUTTON_A:
			case BUTTON_B:
			case BUTTON_C:
			case BUTTON_X:
			case BUTTON_Y:
			case BUTTON_Z:
			case BUTTON_TL:
			case BUTTON_TR:
			case BUTTON_TL2:
			case BUTTON_TR2:
			case BUTTON_SELECT:
			case BUTTON_START:
			case BUTTON_MODE:
			case BUTTON_THUMBL:
			case BUTTON_THUMBR:
			//
			case BUTTON_GEAR_DOWN:
			case BUTTON_GEAR_UP:
				return true;
			default:
				return false;
		}
	}
	// See <linux/input.h>
	/** The valid axes.
	 */
	enum AXIS {
		AXIS_X = 0x00
		, AXIS_Y = 0x01
		, AXIS_Z = 0x02
		, AXIS_RX = 0x03
		, AXIS_RY = 0x04
		, AXIS_RZ = 0x05
		, AXIS_THROTTLE = 0x06
		, AXIS_RUDDER = 0x07
		, AXIS_WHEEL = 0x08
		, AXIS_GAS = 0x09
		, AXIS_BRAKE = 0x0a
		, AXIS_PRESSURE = 0x18
		, AXIS_DISTANCE = 0x19
		, AXIS_TILT_X = 0x1a
		, AXIS_TILT_Y = 0x1b
	};
	static bool isValidAxis(AXIS eAxis)
	{
		switch (eAxis) {
			case AXIS_X:
			case AXIS_Y:
			case AXIS_Z:
			case AXIS_RX:
			case AXIS_RY:
			case AXIS_RZ:
			case AXIS_THROTTLE:
			case AXIS_RUDDER:
			case AXIS_WHEEL:
			case AXIS_GAS:
			case AXIS_BRAKE:
			case AXIS_PRESSURE:
			case AXIS_DISTANCE:
			case AXIS_TILT_X:
			case AXIS_TILT_Y:
				return true;
			default:
				return false;
		}
	}
	/** Whether a device implementing this capability has the given button. */
	virtual bool getHasButton(JoystickCapability::BUTTON eButton) const = 0;
	/** The number of hats the device implementing this capability has. */
	virtual int32_t getTotHats() const = 0;
	/** Whether a device implementing this capability has the given axis. */
	virtual bool getHasAxis(JoystickCapability::AXIS eAxis) const = 0;
	/** Whether the given button is pressed.
	 * If the button is not supported false is returned.
	 */
	virtual bool isButtonPressed(JoystickCapability::BUTTON eButton) const = 0;
	/** The given hat value.
	 */
	virtual HAT_VALUE getHatValue(int32_t nHat) const = 0;
	/** The given axis` value.
	 * If the axis is not supported 0 is returned.
	 */
	virtual int32_t getAxisValue(JoystickCapability::AXIS eAxis) const = 0;

	//TODO Here you could insert force feedback methods??? Or in a separate Capability
	//
	static const char* const s_sClassId;
	static const Capability::Class& getClass() { return s_oInstall.getCapabilityClass(); }
protected:
	/** Constructor. */
	JoystickCapability() : Capability(s_oInstall.getCapabilityClass()) {}
	inline int32_t getMinButtonValue() const { return BUTTON_TRIGGER; } /**< The minimal value of enum JoystickCapability::BUTTON. */
	inline int32_t getMaxButtonValue() const { return BUTTON_GEAR_UP; } /**< The maximal value of enum JoystickCapability::BUTTON. */
	inline int32_t getMinAxisValue() const { return AXIS_X; } /**< The minimal value of enum JoystickCapability::AXIS. */
	inline int32_t getMaxAxisValue() const { return AXIS_TILT_Y; } /**< The maximal value of enum JoystickCapability::AXIS. */
private:
	static RegisterClass<JoystickCapability> s_oInstall;
};

} // namespace stmi

#endif	/* _STMI_JOYSTICK_CAPABILITY_H_ */
