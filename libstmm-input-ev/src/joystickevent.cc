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
 * File:   joystickevent.cc
 */

#include "joystickevent.h"

namespace stmi
{

const char* const JoystickHatEvent::s_sClassId = "stmi::Joystick:JoystickHatEvent";
Event::RegisterClass<JoystickHatEvent> JoystickHatEvent::s_oInstall(s_sClassId);

JoystickHatEvent::JoystickHatEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
									, const shared_ptr<JoystickCapability>& refJoystickCapability, int32_t nHat
									, JoystickCapability::HAT_VALUE eValue, JoystickCapability::HAT_VALUE ePreviousValue)
: Event(s_oInstall.getEventClass(), nTimeUsec, (refJoystickCapability ? refJoystickCapability->getId() : -1), refAccessor)
, m_nHat(nHat)
, m_eValue(eValue)
, m_ePreviousValue(ePreviousValue)
, m_bCancel(eValue == JoystickCapability::HAT_CENTER_CANCEL)
, m_refJoystickCapability(refJoystickCapability)
{
	assert(refJoystickCapability);
	assert(JoystickCapability::isValidHatValue(eValue));
	assert((ePreviousValue == JoystickCapability::HAT_VALUE_NOT_SET) || JoystickCapability::isValidHatValue(ePreviousValue));
	assert(ePreviousValue != JoystickCapability::HAT_CENTER_CANCEL);
	assert(nHat >= 0);
}
JoystickHatEvent::JoystickHatEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
									, const shared_ptr<JoystickCapability>& refJoystickCapability, int32_t nHat
									, JoystickCapability::HAT_VALUE eValue)
: JoystickHatEvent(nTimeUsec, refAccessor, refJoystickCapability, nHat, eValue , JoystickCapability::HAT_VALUE_NOT_SET)
{
}
bool JoystickHatEvent::getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const
{
	if ((m_eValue == m_ePreviousValue) || (m_ePreviousValue == JoystickCapability::HAT_VALUE_NOT_SET) || (m_nHat >= s_nHatsWithKeys)) {
		return false;
	}
	auto aKeys = getAsKeys();
	eKey = aKeys[0].first;
	eType = aKeys[0].second;
	bMoreThanOne = (aKeys.size() > 1);
	return true;
}
std::vector< std::pair<HARDWARE_KEY, Event::AS_KEY_INPUT_TYPE> > JoystickHatEvent::getAsKeys() const
{
	if ((m_eValue == m_ePreviousValue) || (m_ePreviousValue == JoystickCapability::HAT_VALUE_NOT_SET)
					|| (m_nHat >= s_nHatsWithKeys)) {
		return {}; //-----------------------------------------------------------
	}
	const bool bUp = ((m_eValue & JoystickCapability::HAT_UP) != 0);
	const bool bDown = ((m_eValue & JoystickCapability::HAT_DOWN) != 0);
	const bool bLeft = ((m_eValue & JoystickCapability::HAT_LEFT) != 0);
	const bool bRight = ((m_eValue & JoystickCapability::HAT_RIGHT) != 0);
	//
	const bool bPrevUp = ((m_ePreviousValue & JoystickCapability::HAT_UP) != 0);
	const bool bPrevDown = ((m_ePreviousValue & JoystickCapability::HAT_DOWN) != 0);
	const bool bPrevLeft = ((m_ePreviousValue & JoystickCapability::HAT_LEFT) != 0);
	const bool bPrevRight = ((m_ePreviousValue & JoystickCapability::HAT_RIGHT) != 0);

	const int32_t nUp = 0;
	const int32_t nDown = 1;
	const int32_t nLeft = 2;
	const int32_t nRight = 3;
	static_assert(s_nHatsWithKeys == 4,"");
	static const HARDWARE_KEY s_aHatDirKey[s_nHatsWithKeys][4] = {
		{ HK_UP, HK_DOWN, HK_LEFT, HK_RIGHT }
		, { HK_BTN_DPAD_UP, HK_BTN_DPAD_DOWN, HK_BTN_DPAD_LEFT, HK_BTN_DPAD_RIGHT }
		, { HK_BTN_TRIGGER_HAPPY1, HK_BTN_TRIGGER_HAPPY2, HK_BTN_TRIGGER_HAPPY3, HK_BTN_TRIGGER_HAPPY4 }
		, { HK_BTN_TRIGGER_HAPPY5, HK_BTN_TRIGGER_HAPPY6, HK_BTN_TRIGGER_HAPPY7, HK_BTN_TRIGGER_HAPPY8 }
	};

	auto& aDir = s_aHatDirKey[m_nHat];

	const Event::AS_KEY_INPUT_TYPE eReleaseType = (m_bCancel ? AS_KEY_RELEASE_CANCEL : AS_KEY_RELEASE);
	std::vector< std::pair<HARDWARE_KEY, AS_KEY_INPUT_TYPE> > aKeys;
	if (bPrevUp && !bUp) {
		aKeys.emplace_back(aDir[nUp], eReleaseType);
	} else if (bPrevDown && !bDown) {
		aKeys.emplace_back(aDir[nDown], eReleaseType);
	}
	if (bPrevLeft && !bLeft) {
		aKeys.emplace_back(aDir[nLeft], eReleaseType);
	} else if (bPrevRight && !bRight) {
		aKeys.emplace_back(aDir[nRight], eReleaseType);
	}
	if (bUp && !bPrevUp) {
		aKeys.emplace_back(aDir[nUp], AS_KEY_PRESS);
	} else if (bDown && !bPrevDown) {
		aKeys.emplace_back(aDir[nDown], AS_KEY_PRESS);
	}
	if (bLeft && !bPrevLeft) {
		aKeys.emplace_back(aDir[nLeft], AS_KEY_PRESS);
	} else if (bRight && !bPrevRight) {
		aKeys.emplace_back(aDir[nRight], AS_KEY_PRESS);
	}
	return aKeys;
}


////////////////////////////////////////////////////////////////////////////////
const char* const JoystickButtonEvent::s_sClassId = "stmi::Joystick:JoystickButtonEvent";
Event::RegisterClass<JoystickButtonEvent> JoystickButtonEvent::s_oInstall(s_sClassId);

JoystickButtonEvent::JoystickButtonEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
										, const shared_ptr<JoystickCapability>& refJoystickCapability
										, BUTTON_INPUT_TYPE eType, JoystickCapability::BUTTON eButton)
: Event(s_oInstall.getEventClass(), nTimeUsec, (refJoystickCapability ? refJoystickCapability->getId() : -1), refAccessor)
, m_eButton(eButton)
, m_eType(eType)
, m_refJoystickCapability(refJoystickCapability)
{
	assert((eType >= BUTTON_PRESS) && (eType <= BUTTON_RELEASE_CANCEL));
	assert(JoystickCapability::isValidButton(eButton));
	assert(refJoystickCapability);
}
bool JoystickButtonEvent::getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const
{
	eKey = static_cast<HARDWARE_KEY>(m_eButton);
	eType = (AS_KEY_INPUT_TYPE)m_eType;
	bMoreThanOne = false;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
const char* const JoystickAxisEvent::s_sClassId = "stmi::Joystick:JoystickAxisEvent";
Event::RegisterClass<JoystickAxisEvent> JoystickAxisEvent::s_oInstall(s_sClassId);

JoystickAxisEvent::JoystickAxisEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
									, const shared_ptr<JoystickCapability>& refJoystickCapability
									, JoystickCapability::AXIS eAxis, int32_t nValue)
: Event(s_oInstall.getEventClass(), nTimeUsec, (refJoystickCapability ? refJoystickCapability->getId() : -1), refAccessor)
, m_eAxis(eAxis)
, m_nValue(nValue)
, m_refJoystickCapability(refJoystickCapability)
{
	assert(refJoystickCapability);
	assert((nValue >= -32767) && (nValue <= 32767));
	assert(JoystickCapability::isValidAxis(eAxis));
}

} // namespace stmi

