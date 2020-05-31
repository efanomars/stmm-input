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
 * File:   testJoystickButtonEventClass.cc
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

class JoystickButtonEventClassFixture
{
public:
	JoystickButtonEventClassFixture()
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

TEST_CASE_METHOD(JoystickButtonEventClassFixture, "WorkingSetUp")
{
	REQUIRE(m_refJoystickDevice.operator bool());
}

TEST_CASE_METHOD(JoystickButtonEventClassFixture, "ConstructEvent")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	JoystickButtonEvent oJoystickButtonEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refJoystickCapability
							, JoystickButtonEvent::BUTTON_PRESS, JoystickCapability::BUTTON_A);
	REQUIRE(JoystickButtonEvent::getClass() == typeid(JoystickButtonEvent));
	REQUIRE(oJoystickButtonEvent.getEventClass() == typeid(JoystickButtonEvent));
	REQUIRE(oJoystickButtonEvent.getButton() == JoystickCapability::BUTTON_A);
	REQUIRE(oJoystickButtonEvent.getType() == JoystickButtonEvent::BUTTON_PRESS);
	REQUIRE(oJoystickButtonEvent.getTimeUsec() == nTimeUsec);
	REQUIRE(oJoystickButtonEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	REQUIRE(oJoystickButtonEvent.getJoystickCapability() == m_refJoystickCapability);
	REQUIRE(oJoystickButtonEvent.getCapability() == m_refJoystickCapability);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	REQUIRE(oJoystickButtonEvent.getAsKey(eK, eAsType, bMoreThanOne));
	REQUIRE(eK == HK_BTN_A);
	REQUIRE(eAsType == Event::AS_KEY_PRESS);
	REQUIRE_FALSE(bMoreThanOne);
	auto aAsKeys = oJoystickButtonEvent.getAsKeys();
	REQUIRE(aAsKeys.size() == 1);
	REQUIRE(aAsKeys[0].first == HK_BTN_A);
	REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	//REQUIRE(false); //just to check
}

TEST_CASE_METHOD(JoystickButtonEventClassFixture, "KeySimulation")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	JoystickButtonEvent oJoystickButtonEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refJoystickCapability
							, JoystickButtonEvent::BUTTON_RELEASE_CANCEL, JoystickCapability::BUTTON_TL);
	REQUIRE(oJoystickButtonEvent.getButton() == JoystickCapability::BUTTON_TL);
	REQUIRE(oJoystickButtonEvent.getType() == JoystickButtonEvent::BUTTON_RELEASE_CANCEL);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_PRESS;
	bool bMoreThanOne = true;
	REQUIRE(oJoystickButtonEvent.getAsKey(eK, eAsType, bMoreThanOne));
	REQUIRE(eK == HK_BTN_TL);
	REQUIRE(eAsType == Event::AS_KEY_RELEASE_CANCEL);
	REQUIRE_FALSE(bMoreThanOne);
	auto aAsKeys = oJoystickButtonEvent.getAsKeys();
	REQUIRE(aAsKeys.size() == 1);
	REQUIRE(aAsKeys[0].first == HK_BTN_TL);
	REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE_CANCEL);
	//REQUIRE(false); //just to check
}

} // namespace testing

} // namespace stmi
