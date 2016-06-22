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

#include <stmm-input/capability.h>

//#include <cassert>
#include <algorithm>

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
	/** Returns the defined HAT_VALUE values as an ordered vector (`HAT_VALUE_NOT_SET` excluded).
	 * The singleton vector is only created when this function is first called.
	 * @return The hat values as a std::vector.
	 */
	static const std::vector<HAT_VALUE>& getHatValues()
	{
		static const std::vector<HAT_VALUE> s_oSet{
			HAT_CENTER
			, HAT_LEFT
			, HAT_RIGHT
			, HAT_UP
			, HAT_LEFTUP
			, HAT_RIGHTUP
			, HAT_DOWN
			, HAT_LEFTDOWN
			, HAT_RIGHTDOWN
			, HAT_CENTER_CANCEL
		};
		//assert(std::is_sorted(s_oSet.begin(), s_oSet.end()));
		return s_oSet;
	}
	/** Tells whether a hat value is valid.
	 * `HAT_VALUE_NOT_SET` is not considered a valid hat by this function!
	 * @param eValue The hat to check.
	 * @return Whether the hat is valid.
	 */
	static bool isValidHatValue(HAT_VALUE eValue)
	{
		auto& aHatValues = getHatValues();
		return std::binary_search(aHatValues.begin(), aHatValues.end(), eValue);
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
	/** Returns the defined BUTTON values as an ordered vector.
	 * The singleton vector is only created when this function is first called.
	 * @return The button values as a std::vector.
	 */
	static const std::vector<BUTTON>& getButtons()
	{
		static const std::vector<BUTTON> s_oSet{
			BUTTON_TRIGGER
			, BUTTON_THUMB
			, BUTTON_THUMB2
			, BUTTON_TOP
			, BUTTON_TOP2
			, BUTTON_PINKIE
			, BUTTON_BASE
			//
			, BUTTON_A
			, BUTTON_B
			, BUTTON_C
			, BUTTON_X
			, BUTTON_Y
			, BUTTON_Z
			, BUTTON_TL
			, BUTTON_TR
			, BUTTON_TL2
			, BUTTON_TR2
			, BUTTON_SELECT
			, BUTTON_START
			, BUTTON_MODE
			, BUTTON_THUMBL
			, BUTTON_THUMBR
			//
			, BUTTON_GEAR_DOWN
			, BUTTON_GEAR_UP
		};
		//assert(std::is_sorted(s_oSet.begin(), s_oSet.end()));
		return s_oSet;
	}
	/** Tells whether a button value is valid.
	 * @param eButton The button to check.
	 * @return Whether the button is valid.
	 */
	static bool isValidButton(BUTTON eButton)
	{
		auto& aButtons = getButtons();
		return std::binary_search(aButtons.begin(), aButtons.end(), eButton);
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
	/** Returns the defined AXIS values as an ordered vector.
	 * The singleton vector is only created when this function is first called.
	 * @return The axis values as a std::vector.
	 */
	static const std::vector<AXIS>& getAxes()
	{
		static const std::vector<AXIS> s_oSet{
			AXIS_X
			, AXIS_Y
			, AXIS_Z
			, AXIS_RX
			, AXIS_RY
			, AXIS_RZ
			, AXIS_THROTTLE
			, AXIS_RUDDER
			, AXIS_WHEEL
			, AXIS_GAS
			, AXIS_BRAKE
			, AXIS_PRESSURE
			, AXIS_DISTANCE
			, AXIS_TILT_X
			, AXIS_TILT_Y
		};
		//assert(std::is_sorted(s_oSet.begin(), s_oSet.end()));
		return s_oSet;
	}
	/** Tells whether an axis value is valid.
	 * @param eAxis The axis to check.
	 * @return Whether the axis is valid.
	 */
	static bool isValidAxis(AXIS eAxis)
	{
		auto& aAxes = getAxes();
		return std::binary_search(aAxes.begin(), aAxes.end(), eAxis);
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
