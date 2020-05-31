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
 * File:   joystickevent.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMI_JOYSTICK_EVENT_H
#define STMI_JOYSTICK_EVENT_H

#include "joystickcapability.h"

#include <stmm-input/event.h>
#include <stmm-input/hardwarekey.h>

#include <cassert>
#include <vector>
#include <utility>
#include <cstdint>
#include <memory>

namespace stmi { class Accessor; }
namespace stmi { class Capability; }


namespace stmi
{

using std::weak_ptr;

/** Event generated when the state of a joystick's hat is changed.
 * Note that the reference to the capability that generated this event is weak.
 */
class JoystickHatEvent : public Event
{
public:
	//TODO make ctor where you can pass the four keys that getAsKeys should "simulate"
	/** Constructor.
	 * If the previous value of the hat is not JoystickCapability::HAT_VALUE_NOT_SET
	 * this event simulates keys, but only for hats 0, 1, 2 and 3.
	 * - hat 0: HK_UP, HK_DOWN, HK_LEFT, HK_RIGHT.
	 * - hat 1: HK_BTN_DPAD_UP, HK_BTN_DPAD_DOWN, HK_BTN_DPAD_LEFT, HK_BTN_DPAD_RIGHT.
	 * - hat 2: HK_BTN_TRIGGER_HAPPY1, HK_BTN_TRIGGER_HAPPY2, HK_BTN_TRIGGER_HAPPY3, HK_BTN_TRIGGER_HAPPY4.
	 * - hat 3: HK_BTN_TRIGGER_HAPPY5, HK_BTN_TRIGGER_HAPPY6, HK_BTN_TRIGGER_HAPPY7, HK_BTN_TRIGGER_HAPPY8.
	 *
	 * @param nTimeUsec Time from epoch in microseconds.
	 * @param refAccessor The accessor used to generate the event. Can be null.
	 * @param refJoystickCapability The capability that generated this event. Cannot be null.
	 * @param nHat The hat that changed its state. Must be >= 0.
	 * @param eValue The new value for the hat. Can't be JoystickCapability::HAT_VALUE_NOT_SET.
	 * @param ePreviousValue The previous value for the hat. Can be JoystickCapability::HAT_VALUE_NOT_SET.
	 *        Can't be HAT_CENTER_CANCEL.
	 */
	JoystickHatEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
						, const shared_ptr<JoystickCapability>& refJoystickCapability, int32_t nHat
						, JoystickCapability::HAT_VALUE eValue, JoystickCapability::HAT_VALUE ePreviousValue) noexcept;
	/** Constructs instance without key simulation.
	 * @see JoystickHatEvent::JoystickHatEvent(int64_t, const shared_ptr<Accessor>&
	 *                                         , const shared_ptr<JoystickCapability>&, int32_t
	 *                                         , JoystickCapability::HAT_VALUE, JoystickCapability::HAT_VALUE);
	 */
	JoystickHatEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
						, const shared_ptr<JoystickCapability>& refJoystickCapability, int32_t nHat
						, JoystickCapability::HAT_VALUE eValue) noexcept;
	/** The hat that has changed.
	 * @return The hat (>= 0).
	 */
	inline int32_t getHat() const noexcept { return m_nHat; }
	/** The value of the hat.
	 * @return The new value. Cannot be JoystickCapability::HAT_VALUE_NOT_SET.
	 */
	inline JoystickCapability::HAT_VALUE getValue() const noexcept { return m_eValue; }
	/** The value of the hat in unary coordinates.
	 * Examples:
	 * - JoystickCapability::HAT_CENTER corresponds to pair (X=0, Y=0).
	 * - JoystickCapability::HAT_RIGHTUP corresponds to pair (X=1, Y=-1).
	 * - JoystickCapability::HAT_LEFTDOWN corresponds to pair (X=-1, Y=+1).
	 * - JoystickCapability::HAT_DOWN corresponds to pair (X=0, Y=+1).
	 *
	 * @return Pair containing X and Y with possible values `-1`, `0`, `1`.
	 */
	inline std::pair<int32_t,int32_t> getDeltaXY() const noexcept
	{
		const bool bUp = ((m_eValue & JoystickCapability::HAT_UP) != 0);
		const bool bDown = ((m_eValue & JoystickCapability::HAT_DOWN) != 0);
		const bool bLeft = ((m_eValue & JoystickCapability::HAT_LEFT) != 0);
		const bool bRight = ((m_eValue & JoystickCapability::HAT_RIGHT) != 0);
		const int32_t nDx = (bLeft ? -1 : (bRight ? +1 : 0));
		const int32_t nDy = (bUp ? -1 : (bDown ? +1 : 0));
		return std::make_pair(nDx, nDy);
	}
	/** The joystick capability.
	 * @return The capability or null if the capability was deleted.
	 */
	inline shared_ptr<JoystickCapability> getJoystickCapability() const noexcept { return m_refJoystickCapability.lock(); }
	// from Event class
	shared_ptr<Capability> getCapability() const noexcept override { return m_refJoystickCapability.lock(); }
	/** The first of one or more simulated keys.
	 * @see JoystickHatEvent::getAsKeys()
	 */
	bool getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const noexcept override;
	/** The generated simulated keys.
	 * @see JoystickHatEvent(int64_t, const shared_ptr<JoystickCapability>&, int32_t, JoystickCapability::HAT_VALUE, JoystickCapability::HAT_VALUE, bool)
	 * to know which keys are generated by which hats.
	 *
	 * Example 1: A hat `0` transition from HAT_CENTER to HAT_LEFT
	 * simulates the key
	 * `{ {HK_LEFT, AS_KEY_PRESS} }`
	 *
	 * Example 2: A hat `0` transition with canceling from HAT_LEFTUP to HAT_CENTER
	 * simulates the keys
	 * `{ {HK_UP, AS_KEY_RELEASE_CANCEL}, {HK_LEFT, AS_KEY_RELEASE_CANCEL} }`
	 * 
	 * Example 3: A (physically very unlikely) hat `0` transition from HAT_LEFTUP to HAT_RIGHTDOWN 
	 * simulates the keys
	 * `{ {HK_UP, AS_KEY_RELEASE}, {HK_LEFT, AS_KEY_RELEASE}, {HK_DOWN, AS_KEY_PRESS}, {HK_RIGHT, AS_KEY_PRESS} }`
	 */
	std::vector< std::pair<HARDWARE_KEY, AS_KEY_INPUT_TYPE> > getAsKeys() const noexcept override;
	//
	static const char* const s_sClassId;
	static const Event::Class& getClass() noexcept
	{
		static const Event::Class s_oJoyHatClass = s_oInstall.getEventClass();
		return s_oJoyHatClass;
	}
protected:
	/** Sets the hat.
	 * @param nHat The hat that changed its state. Must be >= 0.
	 */
	inline void setHat(int32_t nHat) noexcept
	{
		assert(nHat >= 0);
		m_nHat = nHat;
	}
	/** Sets the value of the hat.
	 * @param eValue The new value for the hat. Can't be JoystickCapability::HAT_VALUE_NOT_SET.
	 */
	inline void setValue(JoystickCapability::HAT_VALUE eValue) noexcept
	{
		setValue(eValue, JoystickCapability::HAT_VALUE_NOT_SET);
	}
	/** Sets the value of the hat for key simulation.
	 * @param eValue The new value for the hat. Can't be JoystickCapability::HAT_VALUE_NOT_SET.
	 * @param ePreviousValue The previous value for the hat. Can be JoystickCapability::HAT_VALUE_NOT_SET.
	 *        Can't be HAT_CENTER_CANCEL.
	 */
	inline void setValue(JoystickCapability::HAT_VALUE eValue, JoystickCapability::HAT_VALUE ePreviousValue) noexcept
	{
		assert(JoystickCapability::isValidHatValue(eValue));
		assert((ePreviousValue == JoystickCapability::HAT_VALUE_NOT_SET) || JoystickCapability::isValidHatValue(ePreviousValue));
		assert(ePreviousValue != JoystickCapability::HAT_CENTER_CANCEL);
		m_eValue = eValue;
		m_ePreviousValue = ePreviousValue;
		m_bCancel = (eValue == JoystickCapability::HAT_CENTER_CANCEL);
	}
	/** Sets the capability.
	 * @param refJoystickCapability The capability that generated this event. Cannot be null.
	 */
	inline void setJoystickCapability(const shared_ptr<JoystickCapability>& refJoystickCapability) noexcept
	{
		assert(refJoystickCapability);
		setCapabilityId(refJoystickCapability->getId());
		m_refJoystickCapability = refJoystickCapability;
	}
private:
	int32_t m_nHat;
	JoystickCapability::HAT_VALUE m_eValue;
	JoystickCapability::HAT_VALUE m_ePreviousValue;
	bool m_bCancel;
	weak_ptr<JoystickCapability> m_refJoystickCapability;
	//
	static constexpr int32_t s_nHatsWithKeys = 4;
	static RegisterClass<JoystickHatEvent> s_oInstall;
private:
	JoystickHatEvent() = delete;
};

////////////////////////////////////////////////////////////////////////////////
/** Event generated when the state of a joystick's button is changed.
 */
class JoystickButtonEvent : public Event
{
public:
	/** Describes what happened to the button.
	 */
	enum BUTTON_INPUT_TYPE {
		BUTTON_PRESS = AS_KEY_PRESS
		, BUTTON_RELEASE = AS_KEY_RELEASE
		, BUTTON_RELEASE_CANCEL = AS_KEY_RELEASE_CANCEL
	};
	/** Constructor.
	 * @see JoystickCapability::BUTTON for valid buttons.
	 * @param nTimeUsec Time from epoch in microseconds.
	 * @param refAccessor The accessor used to generate the event. Can be null.
	 * @param refJoystickCapability The capability that generated this event. Cannot be null.
	 * @param eType What happened to the button.
	 * @param eButton The button.
	 */
	JoystickButtonEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
						, const shared_ptr<JoystickCapability>& refJoystickCapability
						, BUTTON_INPUT_TYPE eType, JoystickCapability::BUTTON eButton) noexcept;
	inline BUTTON_INPUT_TYPE getType() const noexcept { return m_eType; }
	inline JoystickCapability::BUTTON getButton() const noexcept { return m_eButton; }
	/** The joystick capability.
	 * @return The capability or null if the capability was deleted.
	 */
	inline shared_ptr<JoystickCapability> getJoystickCapability() const noexcept { return m_refJoystickCapability.lock(); }
	/** Get the button as a key.
	 * All the buttons of JoystickCapability::BUTTON have a HARDWARE_KEY with the 'same' name
	 * and value.
	 */
	bool getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const noexcept override;
	// from Event class
	shared_ptr<Capability> getCapability() const noexcept override { return m_refJoystickCapability.lock(); }
	//
	static const char* const s_sClassId;
	static const Event::Class& getClass() noexcept
	{
		static const Event::Class s_oJoyButtonClass = s_oInstall.getEventClass();
		return s_oJoyButtonClass;
	}
protected:
	/** Sets the type.
	 * @param eType What happened to the button.
	 */
	inline void setType(BUTTON_INPUT_TYPE eType) noexcept
	{
		assert(isValidType(eType));
		m_eType = eType;
	}
	/** Sets the button.
	 * @param eButton The button.
	 */
	inline void setButton(JoystickCapability::BUTTON eButton) noexcept
	{
		assert(JoystickCapability::isValidButton(eButton));
		m_eButton = eButton;
	}
	/** Sets the capability.
	 * @param refJoystickCapability The capability that generated this event. Cannot be null.
	 */
	inline void setJoystickCapability(const shared_ptr<JoystickCapability>& refJoystickCapability) noexcept
	{
		assert(refJoystickCapability);
		setCapabilityId(refJoystickCapability->getId());
		m_refJoystickCapability = refJoystickCapability;
	}
private:
	inline bool isValidType(BUTTON_INPUT_TYPE eType) const noexcept
	{
		return ((eType == BUTTON_PRESS) || (eType == BUTTON_RELEASE) || (eType == BUTTON_RELEASE_CANCEL));
	}
private:
	JoystickCapability::BUTTON m_eButton;
	BUTTON_INPUT_TYPE m_eType;
	weak_ptr<JoystickCapability> m_refJoystickCapability;
	//
	static RegisterClass<JoystickButtonEvent> s_oInstall;
private:
	JoystickButtonEvent() = delete;
};

////////////////////////////////////////////////////////////////////////////////
/** Event generated when the state of a joystick's absolute axis is changed.
 */
class JoystickAxisEvent : public Event
{
public:
	/** Constructor.
	 * @param nTimeUsec Time from epoch in microseconds.
	 * @param refAccessor The accessor used to generate the event. Can be null.
	 * @param refJoystickCapability The capability that generated this event. Cannot be null.
	 * @param eAxis The axis.
	 * @param nValue Value normalized to [-32767, 32767].
	 */
	JoystickAxisEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
						, const shared_ptr<JoystickCapability>& refJoystickCapability
						, JoystickCapability::AXIS eAxis, int32_t nValue) noexcept;
	//
	inline JoystickCapability::AXIS getAxis() const noexcept { return m_eAxis; }
	/** Integer value normalized to [-32767, +32767].
	 * @return The value of the axis.
	 */
	inline int32_t getValue() const noexcept { return m_nValue; }
	/** Floating value normalized to [0, +1.0].
	 * @return The value of the axis.
	 */
	inline double getValue0ToP1() const noexcept { return (double{1.0} * (m_nValue + 32767.0) / (2 * 32767.0)); }
	/** Floating value normalized to [-1.0, +1.0].
	 * @return The value of the axis.
	 */
	inline double getValueM1ToP1() const noexcept { return (double{1.0} * m_nValue) / 32767.0; }
	/** The joystick capability.
	 * @return The capability or null if the capability was deleted.
	 */
	inline shared_ptr<JoystickCapability> getJoystickCapability() const noexcept { return m_refJoystickCapability.lock(); }
	// from Event class
	shared_ptr<Capability> getCapability() const noexcept override { return m_refJoystickCapability.lock(); }
	//
	static const char* const s_sClassId;
	static const Event::Class& getClass() noexcept
	{
		static const Event::Class s_oJoyAxisClass = s_oInstall.getEventClass();
		return s_oJoyAxisClass;
	}
protected:
	/** Sets the axis.
	 * @param eAxis The axis.
	 */ 
	inline void setAxis(JoystickCapability::AXIS eAxis) noexcept
	{
		assert(JoystickCapability::isValidAxis(eAxis));
		m_eAxis = eAxis;
	}
	/** Sets the axis value.
	 * @param nValue Value normalized to [-32767, 32767].
	 */
	inline void setValue(int32_t nValue) noexcept
	{
		assert((nValue >= -32767) && (nValue <= 32767));
		m_nValue = nValue;
	}
	/** Set the capability.
	 * @param refJoystickCapability The capability that generated this event. Cannot be null.
	 */
	inline void setJoystickCapability(const shared_ptr<JoystickCapability>& refJoystickCapability) noexcept
	{
		assert(refJoystickCapability);
		setCapabilityId(refJoystickCapability->getId());
		m_refJoystickCapability = refJoystickCapability;
	}
private:
	JoystickCapability::AXIS m_eAxis;
	int32_t m_nValue;
	weak_ptr<JoystickCapability> m_refJoystickCapability;
	//
	static RegisterClass<JoystickAxisEvent> s_oInstall;
private:
	JoystickAxisEvent() = delete;
};

} // namespace stmi

#endif /* STMI_JOYSTICK_EVENT_H */

