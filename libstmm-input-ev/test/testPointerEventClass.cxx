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
 * File:   testPointerEventClass.cc
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

class PointerEventClassFixture : public ::testing::Test
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

TEST_F(PointerEventClassFixture, WorkingSetUp)
{
	EXPECT_TRUE(m_refPointerDevice.operator bool());
}

TEST_F(PointerEventClassFixture, ConstructEvent)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const bool bAnyButtonIsPressed = true;
	const bool bAnyButtonWasPressed = false;
	PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
				, PointerEvent::BUTTON_PRESS, 1, bAnyButtonIsPressed, bAnyButtonWasPressed);
	EXPECT_TRUE(PointerEvent::getClass() == typeid(PointerEvent));
	EXPECT_TRUE(oPointerEvent.getEventClass() == typeid(PointerEvent));
	EXPECT_TRUE(oPointerEvent.getButton() == 1);
	EXPECT_TRUE(oPointerEvent.getType() == PointerEvent::BUTTON_PRESS);
	EXPECT_TRUE(oPointerEvent.getX() == 77.7);
	EXPECT_TRUE(oPointerEvent.getY() == -88.8);
	EXPECT_TRUE(oPointerEvent.getTimeUsec() == nTimeUsec);
	EXPECT_TRUE(oPointerEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	EXPECT_TRUE(oPointerEvent.getPointerCapability() == m_refPointerCapability);
	EXPECT_TRUE(oPointerEvent.getCapability() == m_refPointerCapability);
	//
	EXPECT_FALSE(oPointerEvent.getIsModified());
	EXPECT_TRUE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
	EXPECT_TRUE(oPointerEvent.getXYGrabType() == XYEvent::XY_GRAB);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	EXPECT_TRUE(oPointerEvent.getAsKey(eK, eAsType, bMoreThanOne));
	EXPECT_TRUE(eK == HK_BTN_LEFT);
	EXPECT_TRUE(eAsType == Event::AS_KEY_PRESS);
	EXPECT_FALSE(bMoreThanOne);
	auto aAsKeys = oPointerEvent.getAsKeys();
	EXPECT_TRUE(aAsKeys.size() == 1);
	EXPECT_TRUE(aAsKeys[0].first == HK_BTN_LEFT);
	EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	//EXPECT_TRUE(false); //just to check
}

TEST_F(PointerEventClassFixture, GrabTransitions)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int32_t nButton = 1;
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_PRESS, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		EXPECT_TRUE(oPointerEvent.getType() == PointerEvent::BUTTON_PRESS);
		EXPECT_TRUE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		EXPECT_TRUE(oPointerEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_MOVE, PointerEvent::s_nNoButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		EXPECT_TRUE(oPointerEvent.getType() == PointerEvent::POINTER_MOVE);
		EXPECT_TRUE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		EXPECT_TRUE(oPointerEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		EXPECT_TRUE(oPointerEvent.getType() == PointerEvent::BUTTON_RELEASE);
		EXPECT_TRUE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		EXPECT_TRUE(oPointerEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE_CANCEL, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		EXPECT_TRUE(oPointerEvent.getType() == PointerEvent::BUTTON_RELEASE_CANCEL);
		EXPECT_TRUE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		EXPECT_TRUE(oPointerEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		EXPECT_TRUE(oPointerEvent.getType() == PointerEvent::BUTTON_RELEASE);
		EXPECT_TRUE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		EXPECT_TRUE(oPointerEvent.getXYGrabType() == XYEvent::XY_UNGRAB);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE_CANCEL, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		EXPECT_TRUE(oPointerEvent.getType() == PointerEvent::BUTTON_RELEASE_CANCEL);
		EXPECT_TRUE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		EXPECT_TRUE(oPointerEvent.getXYGrabType() == XYEvent::XY_UNGRAB_CANCEL);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = false;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_HOVER, PointerEvent::s_nNoButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		EXPECT_TRUE(oPointerEvent.getType() == PointerEvent::POINTER_HOVER);
		EXPECT_TRUE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerNotGrabbedId);
		EXPECT_TRUE(oPointerEvent.getXYGrabType() == XYEvent::XY_HOVER);
	}
	//EXPECT_TRUE(false); //just to check
}

TEST_F(PointerEventClassFixture, KeySimulation)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = false;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_HOVER, PointerEvent::s_nNoButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 0);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_MOVE, PointerEvent::s_nNoButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 0);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		const int32_t nButton = 1;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_PRESS, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 1);
		EXPECT_TRUE(aAsKeys[0].first == HK_BTN_LEFT);
		EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		const int32_t nButton = 2;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 1);
		EXPECT_TRUE(aAsKeys[0].first == HK_BTN_RIGHT);
		EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		const int32_t nButton = 3;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE_CANCEL, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 1);
		EXPECT_TRUE(aAsKeys[0].first == stmi::HK_BTN_MIDDLE);
		EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE_CANCEL);
	}
	for (int32_t nButton = 4; nButton <= PointerEvent::s_nLastSimulatedButton; ++nButton) {
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = false;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_PRESS, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 1);
		EXPECT_TRUE(aAsKeys[0].first == HK_BTN_0 + nButton);
		EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	}
	//EXPECT_TRUE(false); //just to check
}
TEST_F(PointerEventClassFixture, TranslateXY)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		const int32_t nButton = 1;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		EXPECT_FALSE(oPointerEvent.getIsModified());
		oPointerEvent.translateXY(10, -10);
		EXPECT_TRUE(oPointerEvent.getIsModified());
		EXPECT_NEAR(oPointerEvent.getX(), 87.7, 0.001);
		EXPECT_NEAR(oPointerEvent.getY(), -98.8, 0.001);
	}
	//EXPECT_TRUE(false); //just to check
}
TEST_F(PointerEventClassFixture, ScaleXY)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = false;
		const int32_t nButton = PointerEvent::s_nNoButton;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_HOVER, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		EXPECT_FALSE(oPointerEvent.getIsModified());
		oPointerEvent.scaleXY(1.2, 0.5);
		EXPECT_TRUE(oPointerEvent.getIsModified());
		EXPECT_NEAR(oPointerEvent.getX(), 77.7 * 1.2, 0.001);
		EXPECT_NEAR(oPointerEvent.getY(), -88.8 * 0.5, 0.001);
	}
	//EXPECT_TRUE(false); //just to check
}

} // namespace testing

} // namespace stmi
