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
 * File:   testTouchEventClass.cc
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "faketouchdevice.h"
#include "touchevent.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class TouchEventClassFixture
{
public:
	TouchEventClassFixture()
	{
		m_refTouchDevice = std::make_shared<FakeTouchDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refTouchDevice->getCapability(m_refTouchCapability);
		assert(bFound);
	}
public:
	shared_ptr<Device> m_refTouchDevice;
	shared_ptr<TouchCapability> m_refTouchCapability;
};

TEST_CASE_METHOD(TouchEventClassFixture, "WorkingSetUp")
{
	REQUIRE(m_refTouchDevice.operator bool());
}

TEST_CASE_METHOD(TouchEventClassFixture, "ConstructEvent")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int64_t nFingerId = 678;
	TouchEvent oTouchEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refTouchCapability
							, TouchEvent::TOUCH_BEGIN, 77.7, -88.8, nFingerId);
	REQUIRE(TouchEvent::getClass() == typeid(TouchEvent));
	REQUIRE(oTouchEvent.getEventClass() == typeid(TouchEvent));
	REQUIRE(oTouchEvent.getFingerId() == nFingerId);
	REQUIRE(oTouchEvent.getType() == TouchEvent::TOUCH_BEGIN);
	REQUIRE(oTouchEvent.getX() == 77.7);
	REQUIRE(oTouchEvent.getY() == -88.8);
	REQUIRE(oTouchEvent.getTimeUsec() == nTimeUsec);
	REQUIRE(oTouchEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	REQUIRE(oTouchEvent.getTouchCapability() == m_refTouchCapability);
	REQUIRE(oTouchEvent.getCapability() == m_refTouchCapability);
	//
	REQUIRE_FALSE(oTouchEvent.getIsModified());
	REQUIRE(oTouchEvent.getXYGrabId() == nFingerId);
	REQUIRE(oTouchEvent.getXYGrabType() == XYEvent::XY_GRAB);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	REQUIRE_FALSE(oTouchEvent.getAsKey(eK, eAsType, bMoreThanOne));
	auto aAsKeys = oTouchEvent.getAsKeys();
	REQUIRE(aAsKeys.size() == 0);
	//REQUIRE(false); //just to check
}
TEST_CASE_METHOD(TouchEventClassFixture, "AllTypes")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int64_t nFingerId = 678;
	{
		TouchEvent oTouchEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refTouchCapability
								, TouchEvent::TOUCH_END, 77.7, -88.8, nFingerId);
		REQUIRE(oTouchEvent.getType() == TouchEvent::TOUCH_END);
		//
		REQUIRE(oTouchEvent.getXYGrabId() == nFingerId);
		REQUIRE(oTouchEvent.getXYGrabType() == XYEvent::XY_UNGRAB);
	}
	{
		TouchEvent oTouchEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refTouchCapability
								, TouchEvent::TOUCH_UPDATE, 77.7, -88.8, nFingerId);
		REQUIRE(oTouchEvent.getType() == TouchEvent::TOUCH_UPDATE);
		//
		REQUIRE(oTouchEvent.getXYGrabId() == nFingerId);
		REQUIRE(oTouchEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		TouchEvent oTouchEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refTouchCapability
								, TouchEvent::TOUCH_CANCEL, 77.7, -88.8, nFingerId);
		REQUIRE(oTouchEvent.getType() == TouchEvent::TOUCH_CANCEL);
		//
		REQUIRE(oTouchEvent.getXYGrabId() == nFingerId);
		REQUIRE(oTouchEvent.getXYGrabType() == XYEvent::XY_UNGRAB_CANCEL);
	}
}

} // namespace testing

} // namespace stmi
