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
 * File:   testPointerScrollEventClass.cc
 */

#include "fakepointerdevice.h"
#include "pointerevent.h"

#include <gtest/gtest.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class PointerScrollEventClassFixture : public ::testing::Test
{
	void SetUp() override
	{
		m_refPointerDevice = std::make_shared<FakePointerDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refPointerDevice->getCapability(m_refPointerCapability);
		assert(bFound);
	}
	void TearDown() override
	{
		m_refPointerCapability.reset();
		m_refPointerDevice.reset();
	}
public:
	shared_ptr<Device> m_refPointerDevice;
	shared_ptr<PointerCapability> m_refPointerCapability;
};

TEST_F(PointerScrollEventClassFixture, WorkingSetUp)
{
	EXPECT_TRUE(m_refPointerDevice.operator bool());
}

TEST_F(PointerScrollEventClassFixture, ConstructEvent)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const bool bAnyButtonIsPressed = false;
	PointerScrollEvent oPointerScrollEvent(nTimeUsec, Accessor::s_refEmptyAccessor
								, m_refPointerCapability, PointerScrollEvent::SCROLL_UP
								, 77.7, -88.8, bAnyButtonIsPressed);
	EXPECT_TRUE(PointerScrollEvent::getClass() == typeid(PointerScrollEvent));
	EXPECT_TRUE(oPointerScrollEvent.getEventClass() == typeid(PointerScrollEvent));
	EXPECT_TRUE(oPointerScrollEvent.getScrollDir() == PointerScrollEvent::SCROLL_UP);
	EXPECT_TRUE(oPointerScrollEvent.getX() == 77.7);
	EXPECT_TRUE(oPointerScrollEvent.getY() == -88.8);
	EXPECT_TRUE(oPointerScrollEvent.getTimeUsec() == nTimeUsec);
	EXPECT_TRUE(oPointerScrollEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	EXPECT_TRUE(oPointerScrollEvent.getPointerCapability() == m_refPointerCapability);
	EXPECT_TRUE(oPointerScrollEvent.getCapability() == m_refPointerCapability);
	//
	EXPECT_FALSE(oPointerScrollEvent.getIsModified());
	EXPECT_TRUE(oPointerScrollEvent.getXYGrabId() == PointerEvent::s_nPointerNotGrabbedId);
	EXPECT_TRUE(oPointerScrollEvent.getXYGrabType() == XYEvent::XY_HOVER);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = false;
	EXPECT_TRUE(oPointerScrollEvent.getAsKey(eK, eAsType, bMoreThanOne));
	EXPECT_TRUE(eK == HK_SCROLLUP);
	EXPECT_TRUE(eAsType == Event::AS_KEY_PRESS);
	EXPECT_TRUE(bMoreThanOne);
	auto aAsKeys = oPointerScrollEvent.getAsKeys();
	EXPECT_TRUE(aAsKeys.size() == 2);
	EXPECT_TRUE(aAsKeys[0].first == HK_SCROLLUP);
	EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	EXPECT_TRUE(aAsKeys[1].first == HK_SCROLLUP);
	EXPECT_TRUE(aAsKeys[1].second ==  Event::AS_KEY_RELEASE);
	//EXPECT_TRUE(false); //just to check
}

TEST_F(PointerScrollEventClassFixture, GrabMove)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const bool bAnyButtonIsPressed = true;
	PointerScrollEvent oPointerScrollEvent(nTimeUsec, Accessor::s_refEmptyAccessor
								, m_refPointerCapability, PointerScrollEvent::SCROLL_DOWN
								, 77.7, -88.8, bAnyButtonIsPressed);
	EXPECT_TRUE(oPointerScrollEvent.getScrollDir() == PointerScrollEvent::SCROLL_DOWN);
	EXPECT_TRUE(oPointerScrollEvent.getX() == 77.7);
	EXPECT_TRUE(oPointerScrollEvent.getY() == -88.8);
	//
	EXPECT_FALSE(oPointerScrollEvent.getIsModified());
	EXPECT_TRUE(oPointerScrollEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
	EXPECT_TRUE(oPointerScrollEvent.getXYGrabType() == XYEvent::XY_MOVE);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = false;
	EXPECT_TRUE(oPointerScrollEvent.getAsKey(eK, eAsType, bMoreThanOne));
	EXPECT_TRUE(eK == HK_SCROLLDOWN);
	EXPECT_TRUE(eAsType == Event::AS_KEY_PRESS);
	EXPECT_TRUE(bMoreThanOne);
	auto aAsKeys = oPointerScrollEvent.getAsKeys();
	EXPECT_TRUE(aAsKeys.size() == 2);
	EXPECT_TRUE(aAsKeys[0].first == HK_SCROLLDOWN);
	EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	EXPECT_TRUE(aAsKeys[1].first == HK_SCROLLDOWN);
	EXPECT_TRUE(aAsKeys[1].second ==  Event::AS_KEY_RELEASE);
	//EXPECT_TRUE(false); //just to check
}
TEST_F(PointerScrollEventClassFixture, TranslateXY)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = true;
		PointerScrollEvent oPointerScrollEvent(nTimeUsec, Accessor::s_refEmptyAccessor
									, m_refPointerCapability, PointerScrollEvent::SCROLL_DOWN
									, 77.7, -88.8, bAnyButtonIsPressed);
		EXPECT_FALSE(oPointerScrollEvent.getIsModified());
		oPointerScrollEvent.translateXY(10, -10);
		EXPECT_TRUE(oPointerScrollEvent.getIsModified());
		EXPECT_NEAR(oPointerScrollEvent.getX(), 87.7, 0.001);
		EXPECT_NEAR(oPointerScrollEvent.getY(), -98.8, 0.001);
	}
	//EXPECT_TRUE(false); //just to check
}
TEST_F(PointerScrollEventClassFixture, ScaleXY)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = false;
		PointerScrollEvent oPointerScrollEvent(nTimeUsec, Accessor::s_refEmptyAccessor
									, m_refPointerCapability, PointerScrollEvent::SCROLL_DOWN
									, 77.7, -88.8, bAnyButtonIsPressed);
		EXPECT_FALSE(oPointerScrollEvent.getIsModified());
		oPointerScrollEvent.scaleXY(1.2, 0.5);
		EXPECT_TRUE(oPointerScrollEvent.getIsModified());
		EXPECT_NEAR(oPointerScrollEvent.getX(), 77.7 * 1.2, 0.001);
		EXPECT_NEAR(oPointerScrollEvent.getY(), -88.8 * 0.5, 0.001);
	}
	//EXPECT_TRUE(false); //just to check
}

} // namespace testing

} // namespace stmi
