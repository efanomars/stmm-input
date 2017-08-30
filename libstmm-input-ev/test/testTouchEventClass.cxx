/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/* 
 * File:   testTouchEventClass.cc
 */

#include "faketouchdevice.h"
#include "touchevent.h"

#include <gtest/gtest.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class TouchEventClassFixture : public ::testing::Test
{
	void SetUp() override
	{
		m_refTouchDevice = std::make_shared<FakeTouchDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refTouchDevice->getCapability(m_refTouchCapability);
		assert(bFound);
	}
	void TearDown() override
	{
		m_refTouchCapability.reset();
		m_refTouchDevice.reset();
	}
public:
	shared_ptr<Device> m_refTouchDevice;
	shared_ptr<TouchCapability> m_refTouchCapability;
};

TEST_F(TouchEventClassFixture, WorkingSetUp)
{
	EXPECT_TRUE(m_refTouchDevice.operator bool());
}

TEST_F(TouchEventClassFixture, ConstructEvent)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int64_t nFingerId = 678;
	TouchEvent oTouchEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refTouchCapability
							, TouchEvent::TOUCH_BEGIN, 77.7, -88.8, nFingerId);
	EXPECT_TRUE(TouchEvent::getClass() == typeid(TouchEvent));
	EXPECT_TRUE(oTouchEvent.getEventClass() == typeid(TouchEvent));
	EXPECT_TRUE(oTouchEvent.getFingerId() == nFingerId);
	EXPECT_TRUE(oTouchEvent.getType() == TouchEvent::TOUCH_BEGIN);
	EXPECT_TRUE(oTouchEvent.getX() == 77.7);
	EXPECT_TRUE(oTouchEvent.getY() == -88.8);
	EXPECT_TRUE(oTouchEvent.getTimeUsec() == nTimeUsec);
	EXPECT_TRUE(oTouchEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	EXPECT_TRUE(oTouchEvent.getTouchCapability() == m_refTouchCapability);
	EXPECT_TRUE(oTouchEvent.getCapability() == m_refTouchCapability);
	//
	EXPECT_FALSE(oTouchEvent.getIsModified());
	EXPECT_TRUE(oTouchEvent.getXYGrabId() == nFingerId);
	EXPECT_TRUE(oTouchEvent.getXYGrabType() == XYEvent::XY_GRAB);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	EXPECT_FALSE(oTouchEvent.getAsKey(eK, eAsType, bMoreThanOne));
	auto aAsKeys = oTouchEvent.getAsKeys();
	EXPECT_TRUE(aAsKeys.size() == 0);
	//EXPECT_TRUE(false); //just to check
}
TEST_F(TouchEventClassFixture, AllTypes)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int64_t nFingerId = 678;
	{
		TouchEvent oTouchEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refTouchCapability
								, TouchEvent::TOUCH_END, 77.7, -88.8, nFingerId);
		EXPECT_TRUE(oTouchEvent.getType() == TouchEvent::TOUCH_END);
		//
		EXPECT_TRUE(oTouchEvent.getXYGrabId() == nFingerId);
		EXPECT_TRUE(oTouchEvent.getXYGrabType() == XYEvent::XY_UNGRAB);
	}
	{
		TouchEvent oTouchEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refTouchCapability
								, TouchEvent::TOUCH_UPDATE, 77.7, -88.8, nFingerId);
		EXPECT_TRUE(oTouchEvent.getType() == TouchEvent::TOUCH_UPDATE);
		//
		EXPECT_TRUE(oTouchEvent.getXYGrabId() == nFingerId);
		EXPECT_TRUE(oTouchEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		TouchEvent oTouchEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refTouchCapability
								, TouchEvent::TOUCH_CANCEL, 77.7, -88.8, nFingerId);
		EXPECT_TRUE(oTouchEvent.getType() == TouchEvent::TOUCH_CANCEL);
		//
		EXPECT_TRUE(oTouchEvent.getXYGrabId() == nFingerId);
		EXPECT_TRUE(oTouchEvent.getXYGrabType() == XYEvent::XY_UNGRAB_CANCEL);
	}
}

} // namespace testing

} // namespace stmi
