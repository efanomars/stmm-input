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
 * File:   testJoystickAxisEventClass.cc
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

class JoystickAxisEventClassFixture : public ::testing::Test
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

TEST_F(JoystickAxisEventClassFixture, WorkingSetUp)
{
	EXPECT_TRUE(m_refJoystickDevice.operator bool());
}

TEST_F(JoystickAxisEventClassFixture, ConstructEvent)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	JoystickAxisEvent oJoystickAxisEvent(nTimeUsec, Accessor::s_refEmptyAccessor
									, m_refJoystickDevice, JoystickCapability::AXIS_X, +32111);
	EXPECT_TRUE(JoystickAxisEvent::getClass() == typeid(JoystickAxisEvent));
	EXPECT_TRUE(oJoystickAxisEvent.getEventClass() == typeid(JoystickAxisEvent));
	EXPECT_TRUE(oJoystickAxisEvent.getAxis() == JoystickCapability::AXIS_X);
	EXPECT_TRUE(oJoystickAxisEvent.getValue() == 32111);
	EXPECT_TRUE(oJoystickAxisEvent.getTimeUsec() == nTimeUsec);
	EXPECT_TRUE(oJoystickAxisEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	EXPECT_TRUE(oJoystickAxisEvent.getJoystickCapability() == m_refJoystickDevice);
	EXPECT_TRUE(oJoystickAxisEvent.getCapability() == m_refJoystickDevice);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	EXPECT_FALSE(oJoystickAxisEvent.getAsKey(eK, eAsType, bMoreThanOne));
	auto aAsKeys = oJoystickAxisEvent.getAsKeys();
	EXPECT_TRUE(aAsKeys.size() == 0);
	//EXPECT_TRUE(false); //just to check
}

TEST_F(JoystickAxisEventClassFixture, FromTo)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	std::vector< std::tuple<int32_t, double, double> > aTuple = {
		std::tuple<int32_t, double, double>{0, 0.0, 0.5}
		, std::tuple<int32_t, double, double>{-32767, -1.0, 0.0}
		, std::tuple<int32_t, double, double>{32767, 1.0, 1.0}
		, std::tuple<int32_t, double, double>{16383.5, 0.5, 0.75}
	};
	for (auto& oTuple : aTuple) {
		JoystickAxisEvent oJoystickAxisEvent(nTimeUsec, Accessor::s_refEmptyAccessor
									, m_refJoystickDevice, JoystickCapability::AXIS_Z, std::get<0>(oTuple));
		EXPECT_TRUE(oJoystickAxisEvent.getAxis() == JoystickCapability::AXIS_Z);
		EXPECT_TRUE(oJoystickAxisEvent.getValue() == std::get<0>(oTuple));
		EXPECT_NEAR(oJoystickAxisEvent.getValueM1ToP1(), std::get<1>(oTuple), 0.001);
		EXPECT_NEAR(oJoystickAxisEvent.getValue0ToP1(), std::get<2>(oTuple), 0.001);
	}
	//EXPECT_TRUE(false); //just to check
}

} // namespace testing

} // namespace stmi
