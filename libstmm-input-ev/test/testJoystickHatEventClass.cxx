/*
 * Copyright © 2016-2018  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/* 
 * File:   testJoystickHatEventClass.cc
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "fakejoystickdevice.h"
#include "joystickevent.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class JoystickHatEventClassFixture
{
public:
	JoystickHatEventClassFixture()
	{
		m_refJoystickDevice = std::make_shared<FakeJoystickDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refJoystickDevice->getCapability(m_refJoystickCapability);
		assert(bFound);
	}

	shared_ptr<Device> m_refJoystickDevice;
	shared_ptr<JoystickCapability> m_refJoystickCapability;
};

TEST_CASE_METHOD(JoystickHatEventClassFixture, "WorkingSetUp")
{
	REQUIRE(m_refJoystickDevice.operator bool());
}

TEST_CASE_METHOD(JoystickHatEventClassFixture, "ConstructEvent")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int32_t nHat = 0;
	const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_UP;
	const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_CENTER;
	JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refJoystickCapability
										, nHat, eValue, ePreviousValue);
	REQUIRE(JoystickHatEvent::getClass() == typeid(JoystickHatEvent));
	REQUIRE(oJoystickHatEvent.getEventClass() == typeid(JoystickHatEvent));
	REQUIRE(oJoystickHatEvent.getHat() == nHat);
	REQUIRE(oJoystickHatEvent.getDeltaXY() == std::make_pair(0,-1));
	REQUIRE(oJoystickHatEvent.getTimeUsec() == nTimeUsec);
	REQUIRE(oJoystickHatEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	REQUIRE(oJoystickHatEvent.getJoystickCapability() == m_refJoystickCapability);
	REQUIRE(oJoystickHatEvent.getCapability() == m_refJoystickCapability);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	REQUIRE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
	REQUIRE(eK == HK_UP);
	REQUIRE(eAsType == Event::AS_KEY_PRESS);
	REQUIRE_FALSE(bMoreThanOne);
	auto aAsKeys = oJoystickHatEvent.getAsKeys();
	REQUIRE(aAsKeys.size() == 1);
	REQUIRE(aAsKeys[0].first == HK_UP);
	REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	//REQUIRE(false); //just to check
}
TEST_CASE_METHOD(JoystickHatEventClassFixture, "TransitionsDistOne")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int32_t nHat = 0;
	{
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_RIGHTDOWN;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_RIGHT;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
										, m_refJoystickCapability, nHat, eValue, ePreviousValue);
		REQUIRE(oJoystickHatEvent.getDeltaXY() == std::make_pair(+1,+1));
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
		bool bMoreThanOne = true;
		REQUIRE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		REQUIRE(eK == HK_DOWN);
		REQUIRE(eAsType == Event::AS_KEY_PRESS);
		REQUIRE_FALSE(bMoreThanOne);
		auto aAsKeys = oJoystickHatEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 1);
		REQUIRE(aAsKeys[0].first == HK_DOWN);
		REQUIRE(aAsKeys[0].second == Event::AS_KEY_PRESS);
	}
	{
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_CENTER_CANCEL;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_UP;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
											, m_refJoystickCapability, nHat, eValue, ePreviousValue);
		REQUIRE(oJoystickHatEvent.getDeltaXY() == std::make_pair(0,0));
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_PRESS;
		bool bMoreThanOne = true;
		REQUIRE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		REQUIRE(eK == HK_UP);
		REQUIRE(eAsType == Event::AS_KEY_RELEASE_CANCEL);
		REQUIRE_FALSE(bMoreThanOne);
		auto aAsKeys = oJoystickHatEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 1);
		REQUIRE(aAsKeys[0].first == HK_UP);
		REQUIRE(aAsKeys[0].second == Event::AS_KEY_RELEASE_CANCEL);
	}
	//REQUIRE(false); //just to check
}
TEST_CASE_METHOD(JoystickHatEventClassFixture, "TransitionsDistTwo")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int32_t nHat = 0;
	{
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_RIGHTUP;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_CENTER;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
											, m_refJoystickCapability, nHat, eValue, ePreviousValue);
		REQUIRE(oJoystickHatEvent.getDeltaXY() == std::make_pair(+1,-1));
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
		bool bMoreThanOne = false;
		REQUIRE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		REQUIRE( ((eK == HK_RIGHT) || (eK == HK_UP)) );
		REQUIRE(eAsType == Event::AS_KEY_PRESS);
		REQUIRE(bMoreThanOne);
		auto aAsKeys = oJoystickHatEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 2);
		REQUIRE( ((aAsKeys[0].first == HK_RIGHT) || (aAsKeys[0].first == HK_UP)) );
		REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
		REQUIRE( ((aAsKeys[1].first == HK_RIGHT) || (aAsKeys[1].first == HK_UP)) );
		REQUIRE(aAsKeys[1].second ==  Event::AS_KEY_PRESS);
		//
		REQUIRE( (aAsKeys[0].first != aAsKeys[1].first) );
	}
	{
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_RIGHTDOWN;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_LEFTDOWN;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
											, m_refJoystickCapability, nHat, eValue, ePreviousValue);
		REQUIRE(oJoystickHatEvent.getDeltaXY() == std::make_pair(+1,+1));
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
		bool bMoreThanOne = false;
		REQUIRE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		REQUIRE(eK == HK_LEFT);
		REQUIRE(eAsType == Event::AS_KEY_RELEASE);
		REQUIRE(bMoreThanOne);
		auto aAsKeys = oJoystickHatEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 2);
		REQUIRE(aAsKeys[0].first == HK_LEFT);
		REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE);
		REQUIRE(aAsKeys[1].first == HK_RIGHT);
		REQUIRE(aAsKeys[1].second ==  Event::AS_KEY_PRESS);
	}
	//REQUIRE(false); //just to check
}
TEST_CASE_METHOD(JoystickHatEventClassFixture, "HatKeys")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (int32_t nHat = 0; nHat < 2; ++nHat) {
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_DOWN;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_CENTER;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
										, m_refJoystickCapability, nHat, eValue, ePreviousValue);
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
		bool bMoreThanOne = true;
		REQUIRE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		REQUIRE(eK == (nHat == 0 ? HK_DOWN : HK_BTN_DPAD_DOWN));
		REQUIRE(eAsType == Event::AS_KEY_PRESS);
		REQUIRE_FALSE(bMoreThanOne);
	}
}

} // namespace testing

} // namespace stmi
