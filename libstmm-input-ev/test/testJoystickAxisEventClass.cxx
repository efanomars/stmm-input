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
 * File:   testJoystickAxisEventClass.cc
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

class JoystickAxisEventClassFixture
{
public:
	JoystickAxisEventClassFixture()
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

TEST_CASE_METHOD(JoystickAxisEventClassFixture, "WorkingSetUp")
{
	REQUIRE(m_refJoystickDevice.operator bool());
}

TEST_CASE_METHOD(JoystickAxisEventClassFixture, "ConstructEvent")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	JoystickAxisEvent oJoystickAxisEvent(nTimeUsec, Accessor::s_refEmptyAccessor
									, m_refJoystickCapability, JoystickCapability::AXIS_X, +32111);
	REQUIRE(JoystickAxisEvent::getClass() == typeid(JoystickAxisEvent));
	REQUIRE(oJoystickAxisEvent.getEventClass() == typeid(JoystickAxisEvent));
	REQUIRE(oJoystickAxisEvent.getAxis() == JoystickCapability::AXIS_X);
	REQUIRE(oJoystickAxisEvent.getValue() == 32111);
	REQUIRE(oJoystickAxisEvent.getTimeUsec() == nTimeUsec);
	REQUIRE(oJoystickAxisEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	REQUIRE(oJoystickAxisEvent.getJoystickCapability() == m_refJoystickCapability);
	REQUIRE(oJoystickAxisEvent.getCapability() == m_refJoystickCapability);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	REQUIRE_FALSE(oJoystickAxisEvent.getAsKey(eK, eAsType, bMoreThanOne));
	auto aAsKeys = oJoystickAxisEvent.getAsKeys();
	REQUIRE(aAsKeys.size() == 0);
	//REQUIRE(false); //just to check
}

TEST_CASE_METHOD(JoystickAxisEventClassFixture, "FromTo")
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
									, m_refJoystickCapability, JoystickCapability::AXIS_Z, std::get<0>(oTuple));
		REQUIRE(oJoystickAxisEvent.getAxis() == JoystickCapability::AXIS_Z);
		REQUIRE(oJoystickAxisEvent.getValue() == std::get<0>(oTuple));
		REQUIRE(Approx( oJoystickAxisEvent.getValueM1ToP1() ).epsilon(0.001) == std::get<1>(oTuple));
		REQUIRE(Approx( oJoystickAxisEvent.getValue0ToP1() ).epsilon(0.001) == std::get<2>(oTuple));
		//EXPECT_NEAR(oJoystickAxisEvent.getValueM1ToP1(), std::get<1>(oTuple), 0.001);
		//EXPECT_NEAR(oJoystickAxisEvent.getValue0ToP1(), std::get<2>(oTuple), 0.001);
	}
	//REQUIRE(false); //just to check
}

} // namespace testing

} // namespace stmi
