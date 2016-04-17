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
 * File:   testJoystickButtonEventClass.cc
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

class JoystickButtonEventClassFixture : public ::testing::Test
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

TEST_F(JoystickButtonEventClassFixture, WorkingSetUp)
{
	EXPECT_TRUE(m_refJoystickDevice.operator bool());
}

TEST_F(JoystickButtonEventClassFixture, ConstructEvent)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	JoystickButtonEvent oJoystickButtonEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refJoystickDevice
							, JoystickButtonEvent::BUTTON_PRESS, JoystickCapability::BUTTON_A);
	EXPECT_TRUE(JoystickButtonEvent::getClass() == typeid(JoystickButtonEvent));
	EXPECT_TRUE(oJoystickButtonEvent.getEventClass() == typeid(JoystickButtonEvent));
	EXPECT_TRUE(oJoystickButtonEvent.getButton() == JoystickCapability::BUTTON_A);
	EXPECT_TRUE(oJoystickButtonEvent.getType() == JoystickButtonEvent::BUTTON_PRESS);
	EXPECT_TRUE(oJoystickButtonEvent.getTimeUsec() == nTimeUsec);
	EXPECT_TRUE(oJoystickButtonEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	EXPECT_TRUE(oJoystickButtonEvent.getJoystickCapability() == m_refJoystickDevice);
	EXPECT_TRUE(oJoystickButtonEvent.getCapability() == m_refJoystickDevice);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	EXPECT_TRUE(oJoystickButtonEvent.getAsKey(eK, eAsType, bMoreThanOne));
	EXPECT_TRUE(eK == HK_BTN_A);
	EXPECT_TRUE(eAsType == Event::AS_KEY_PRESS);
	EXPECT_FALSE(bMoreThanOne);
	auto aAsKeys = oJoystickButtonEvent.getAsKeys();
	EXPECT_TRUE(aAsKeys.size() == 1);
	EXPECT_TRUE(aAsKeys[0].first == HK_BTN_A);
	EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	//EXPECT_TRUE(false); //just to check
}

TEST_F(JoystickButtonEventClassFixture, KeySimulation)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	JoystickButtonEvent oJoystickButtonEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refJoystickDevice
							, JoystickButtonEvent::BUTTON_RELEASE_CANCEL, JoystickCapability::BUTTON_TL);
	EXPECT_TRUE(oJoystickButtonEvent.getButton() == JoystickCapability::BUTTON_TL);
	EXPECT_TRUE(oJoystickButtonEvent.getType() == JoystickButtonEvent::BUTTON_RELEASE_CANCEL);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_PRESS;
	bool bMoreThanOne = true;
	EXPECT_TRUE(oJoystickButtonEvent.getAsKey(eK, eAsType, bMoreThanOne));
	EXPECT_TRUE(eK == HK_BTN_TL);
	EXPECT_TRUE(eAsType == Event::AS_KEY_RELEASE_CANCEL);
	EXPECT_FALSE(bMoreThanOne);
	auto aAsKeys = oJoystickButtonEvent.getAsKeys();
	EXPECT_TRUE(aAsKeys.size() == 1);
	EXPECT_TRUE(aAsKeys[0].first == HK_BTN_TL);
	EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE_CANCEL);
	//EXPECT_TRUE(false); //just to check
}

} // namespace testing

} // namespace stmi
