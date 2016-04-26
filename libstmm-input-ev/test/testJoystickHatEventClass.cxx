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
 * File:   testJoystickEventClass.cc
 */

#include <gtest/gtest.h>
#include "fakejoystickdevice.h"
#include "joystickevent.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class JoystickEventClassFixture : public ::testing::Test
{
	void SetUp() override
	{
		m_refJoystickDevice = std::make_shared<FakeJoystickDevice>();
	}
	void TearDown() override
	{
		m_refJoystickDevice.reset();
	}
public:
	shared_ptr<FakeJoystickDevice> m_refJoystickDevice;
};

TEST_F(JoystickEventClassFixture, WorkingSetUp)
{
	EXPECT_TRUE(m_refJoystickDevice.operator bool());
}

TEST_F(JoystickEventClassFixture, ConstructEvent)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int32_t nHat = 0;
	const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_UP;
	const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_CENTER;
	JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refJoystickDevice, nHat
										, eValue, ePreviousValue);
	EXPECT_TRUE(JoystickHatEvent::getClass() == typeid(JoystickHatEvent));
	EXPECT_TRUE(oJoystickHatEvent.getEventClass() == typeid(JoystickHatEvent));
	EXPECT_TRUE(oJoystickHatEvent.getHat() == nHat);
	EXPECT_TRUE(oJoystickHatEvent.getDeltaXY() == std::make_pair(0,-1));
	EXPECT_TRUE(oJoystickHatEvent.getTimeUsec() == nTimeUsec);
	EXPECT_TRUE(oJoystickHatEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	EXPECT_TRUE(oJoystickHatEvent.getJoystickCapability() == m_refJoystickDevice);
	EXPECT_TRUE(oJoystickHatEvent.getCapability() == m_refJoystickDevice);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	EXPECT_TRUE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
	EXPECT_TRUE(eK == HK_UP);
	EXPECT_TRUE(eAsType == Event::AS_KEY_PRESS);
	EXPECT_FALSE(bMoreThanOne);
	auto aAsKeys = oJoystickHatEvent.getAsKeys();
	EXPECT_TRUE(aAsKeys.size() == 1);
	EXPECT_TRUE(aAsKeys[0].first == HK_UP);
	EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	//EXPECT_TRUE(false); //just to check
}
TEST_F(JoystickEventClassFixture, TransitionsDistOne)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int32_t nHat = 0;
	{
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_RIGHTDOWN;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_RIGHT;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
										, m_refJoystickDevice, nHat, eValue, ePreviousValue);
		EXPECT_TRUE(oJoystickHatEvent.getDeltaXY() == std::make_pair(+1,+1));
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
		bool bMoreThanOne = true;
		EXPECT_TRUE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		EXPECT_TRUE(eK == HK_DOWN);
		EXPECT_TRUE(eAsType == Event::AS_KEY_PRESS);
		EXPECT_FALSE(bMoreThanOne);
		auto aAsKeys = oJoystickHatEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 1);
		EXPECT_TRUE(aAsKeys[0].first == HK_DOWN);
		EXPECT_TRUE(aAsKeys[0].second == Event::AS_KEY_PRESS);
	}
	{
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_CENTER_CANCEL;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_UP;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
											, m_refJoystickDevice, nHat, eValue, ePreviousValue);
		EXPECT_TRUE(oJoystickHatEvent.getDeltaXY() == std::make_pair(0,0));
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_PRESS;
		bool bMoreThanOne = true;
		EXPECT_TRUE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		EXPECT_TRUE(eK == HK_UP);
		EXPECT_TRUE(eAsType == Event::AS_KEY_RELEASE_CANCEL);
		EXPECT_FALSE(bMoreThanOne);
		auto aAsKeys = oJoystickHatEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 1);
		EXPECT_TRUE(aAsKeys[0].first == HK_UP);
		EXPECT_TRUE(aAsKeys[0].second == Event::AS_KEY_RELEASE_CANCEL);
	}
	//EXPECT_TRUE(false); //just to check
}
TEST_F(JoystickEventClassFixture, TransitionsDistTwo)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int32_t nHat = 0;
	{
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_RIGHTUP;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_CENTER;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
											, m_refJoystickDevice, nHat, eValue, ePreviousValue);
		EXPECT_TRUE(oJoystickHatEvent.getDeltaXY() == std::make_pair(+1,-1));
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
		bool bMoreThanOne = false;
		EXPECT_TRUE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		EXPECT_TRUE( (eK == HK_RIGHT) || (eK == HK_UP) );
		EXPECT_TRUE(eAsType == Event::AS_KEY_PRESS);
		EXPECT_TRUE(bMoreThanOne);
		auto aAsKeys = oJoystickHatEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 2);
		EXPECT_TRUE( (aAsKeys[0].first == HK_RIGHT) || (aAsKeys[0].first == HK_UP) );
		EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
		EXPECT_TRUE( (aAsKeys[1].first == HK_RIGHT) || (aAsKeys[1].first == HK_UP) );
		EXPECT_TRUE(aAsKeys[1].second ==  Event::AS_KEY_PRESS);
		//
		EXPECT_TRUE( (aAsKeys[0].first != aAsKeys[1].first) );
	}
	{
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_RIGHTDOWN;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_LEFTDOWN;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
											, m_refJoystickDevice, nHat, eValue, ePreviousValue);
		EXPECT_TRUE(oJoystickHatEvent.getDeltaXY() == std::make_pair(+1,+1));
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
		bool bMoreThanOne = false;
		EXPECT_TRUE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		EXPECT_TRUE(eK == HK_LEFT);
		EXPECT_TRUE(eAsType == Event::AS_KEY_RELEASE);
		EXPECT_TRUE(bMoreThanOne);
		auto aAsKeys = oJoystickHatEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 2);
		EXPECT_TRUE(aAsKeys[0].first == HK_LEFT);
		EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE);
		EXPECT_TRUE(aAsKeys[1].first == HK_RIGHT);
		EXPECT_TRUE(aAsKeys[1].second ==  Event::AS_KEY_PRESS);
	}
	//EXPECT_TRUE(false); //just to check
}
TEST_F(JoystickEventClassFixture, HatKeys)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (int32_t nHat = 0; nHat < 2; ++nHat) {
		const JoystickCapability::HAT_VALUE eValue = JoystickCapability::HAT_DOWN;
		const JoystickCapability::HAT_VALUE ePreviousValue = JoystickCapability::HAT_CENTER;
		JoystickHatEvent oJoystickHatEvent(nTimeUsec, Accessor::s_refEmptyAccessor
										, m_refJoystickDevice, nHat, eValue, ePreviousValue);
		//
		HARDWARE_KEY eK = HK_T;
		Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
		bool bMoreThanOne = true;
		EXPECT_TRUE(oJoystickHatEvent.getAsKey(eK, eAsType, bMoreThanOne));
		EXPECT_TRUE(eK == (nHat == 0 ? HK_DOWN : HK_BTN_DPAD_DOWN));
		EXPECT_TRUE(eAsType == Event::AS_KEY_PRESS);
		EXPECT_FALSE(bMoreThanOne);
	}
}

} // namespace testing

} // namespace stmi