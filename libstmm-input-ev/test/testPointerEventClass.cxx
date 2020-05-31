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
 * File:   testPointerEventClass.cc
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "fakepointerdevice.h"
#include "pointerevent.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class PointerEventClassFixture
{
public:
	PointerEventClassFixture()
	{
		m_refPointerDevice = std::make_shared<FakePointerDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refPointerDevice->getCapability(m_refPointerCapability);
		assert(bFound);
	}

	shared_ptr<Device> m_refPointerDevice;
	shared_ptr<PointerCapability> m_refPointerCapability;
};

TEST_CASE_METHOD(PointerEventClassFixture, "WorkingSetUp")
{
	REQUIRE(m_refPointerDevice.operator bool());
}

TEST_CASE_METHOD(PointerEventClassFixture, "ConstructEvent")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const bool bAnyButtonIsPressed = true;
	const bool bAnyButtonWasPressed = false;
	PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
				, PointerEvent::BUTTON_PRESS, 1, bAnyButtonIsPressed, bAnyButtonWasPressed);
	REQUIRE(PointerEvent::getClass() == typeid(PointerEvent));
	REQUIRE(oPointerEvent.getEventClass() == typeid(PointerEvent));
	REQUIRE(oPointerEvent.getButton() == 1);
	REQUIRE(oPointerEvent.getType() == PointerEvent::BUTTON_PRESS);
	REQUIRE(oPointerEvent.getX() == 77.7);
	REQUIRE(oPointerEvent.getY() == -88.8);
	REQUIRE(oPointerEvent.getTimeUsec() == nTimeUsec);
	REQUIRE(oPointerEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	REQUIRE(oPointerEvent.getPointerCapability() == m_refPointerCapability);
	REQUIRE(oPointerEvent.getCapability() == m_refPointerCapability);
	//
	REQUIRE_FALSE(oPointerEvent.getIsModified());
	REQUIRE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
	REQUIRE(oPointerEvent.getXYGrabType() == XYEvent::XY_GRAB);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	REQUIRE(oPointerEvent.getAsKey(eK, eAsType, bMoreThanOne));
	REQUIRE(eK == HK_BTN_LEFT);
	REQUIRE(eAsType == Event::AS_KEY_PRESS);
	REQUIRE_FALSE(bMoreThanOne);
	auto aAsKeys = oPointerEvent.getAsKeys();
	REQUIRE(aAsKeys.size() == 1);
	REQUIRE(aAsKeys[0].first == HK_BTN_LEFT);
	REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	//REQUIRE(false); //just to check
}

TEST_CASE_METHOD(PointerEventClassFixture, "GrabTransitions")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const int32_t nButton = 1;
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_PRESS, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		REQUIRE(oPointerEvent.getType() == PointerEvent::BUTTON_PRESS);
		REQUIRE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		REQUIRE(oPointerEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_MOVE, PointerEvent::s_nNoButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		REQUIRE(oPointerEvent.getType() == PointerEvent::POINTER_MOVE);
		REQUIRE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		REQUIRE(oPointerEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		REQUIRE(oPointerEvent.getType() == PointerEvent::BUTTON_RELEASE);
		REQUIRE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		REQUIRE(oPointerEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE_CANCEL, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		REQUIRE(oPointerEvent.getType() == PointerEvent::BUTTON_RELEASE_CANCEL);
		REQUIRE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		REQUIRE(oPointerEvent.getXYGrabType() == XYEvent::XY_MOVE);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		REQUIRE(oPointerEvent.getType() == PointerEvent::BUTTON_RELEASE);
		REQUIRE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		REQUIRE(oPointerEvent.getXYGrabType() == XYEvent::XY_UNGRAB);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE_CANCEL, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		REQUIRE(oPointerEvent.getType() == PointerEvent::BUTTON_RELEASE_CANCEL);
		REQUIRE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
		REQUIRE(oPointerEvent.getXYGrabType() == XYEvent::XY_UNGRAB_CANCEL);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = false;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_HOVER, PointerEvent::s_nNoButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		REQUIRE(oPointerEvent.getType() == PointerEvent::POINTER_HOVER);
		REQUIRE(oPointerEvent.getXYGrabId() == PointerEvent::s_nPointerNotGrabbedId);
		REQUIRE(oPointerEvent.getXYGrabType() == XYEvent::XY_HOVER);
	}
	//REQUIRE(false); //just to check
}

TEST_CASE_METHOD(PointerEventClassFixture, "KeySimulation")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = false;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_HOVER, PointerEvent::s_nNoButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 0);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_MOVE, PointerEvent::s_nNoButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 0);
	}
	{
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = true;
		const int32_t nButton = 1;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_PRESS, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 1);
		REQUIRE(aAsKeys[0].first == HK_BTN_LEFT);
		REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		const int32_t nButton = 2;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 1);
		REQUIRE(aAsKeys[0].first == HK_BTN_RIGHT);
		REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE);
	}
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		const int32_t nButton = 3;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE_CANCEL, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 1);
		REQUIRE(aAsKeys[0].first == stmi::HK_BTN_MIDDLE);
		REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE_CANCEL);
	}
	for (int32_t nButton = 4; nButton <= PointerEvent::s_nLastSimulatedButton; ++nButton) {
		const bool bAnyButtonIsPressed = true;
		const bool bAnyButtonWasPressed = false;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_PRESS, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		auto aAsKeys = oPointerEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 1);
		REQUIRE(aAsKeys[0].first == HK_BTN_0 + nButton);
		REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	}
	//REQUIRE(false); //just to check
}
TEST_CASE_METHOD(PointerEventClassFixture, "TranslateXY")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = true;
		const int32_t nButton = 1;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::BUTTON_RELEASE, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		REQUIRE_FALSE(oPointerEvent.getIsModified());
		oPointerEvent.translateXY(10, -10);
		REQUIRE(oPointerEvent.getIsModified());
		REQUIRE(Approx(oPointerEvent.getX()).epsilon(0.001) == 87.7);
		REQUIRE(Approx(oPointerEvent.getY()).epsilon(0.001) == -98.8);
		//EXPECT_NEAR(oPointerEvent.getX(), 87.7, 0.001);
		//EXPECT_NEAR(oPointerEvent.getY(), -98.8, 0.001);
	}
	//REQUIRE(false); //just to check
}
TEST_CASE_METHOD(PointerEventClassFixture, "ScaleXY")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = false;
		const bool bAnyButtonWasPressed = false;
		const int32_t nButton = PointerEvent::s_nNoButton;
		PointerEvent oPointerEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refPointerCapability, 77.7, -88.8
					, PointerEvent::POINTER_HOVER, nButton, bAnyButtonIsPressed, bAnyButtonWasPressed);
		REQUIRE_FALSE(oPointerEvent.getIsModified());
		oPointerEvent.scaleXY(1.2, 0.5);
		REQUIRE(oPointerEvent.getIsModified());
		REQUIRE(Approx(oPointerEvent.getX()).epsilon(0.001) == 77.7 * 1.2);
		REQUIRE(Approx(oPointerEvent.getY()).epsilon(0.001) == -88.8 * 0.5);
		//EXPECT_NEAR(oPointerEvent.getX(), 77.7 * 1.2, 0.001);
		//EXPECT_NEAR(oPointerEvent.getY(), -88.8 * 0.5, 0.001);
	}
	//REQUIRE(false); //just to check
}

} // namespace testing

} // namespace stmi
