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
 * File:   testPointerScrollEventClass.cc
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

class PointerScrollEventClassFixture
{
public:
	PointerScrollEventClassFixture()
	{
		m_refPointerDevice = std::make_shared<FakePointerDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refPointerDevice->getCapability(m_refPointerCapability);
		assert(bFound);
	}
protected:
	shared_ptr<Device> m_refPointerDevice;
	shared_ptr<PointerCapability> m_refPointerCapability;
};

TEST_CASE_METHOD(PointerScrollEventClassFixture, "WorkingSetUp")
{
	REQUIRE(m_refPointerDevice.operator bool());
}

TEST_CASE_METHOD(PointerScrollEventClassFixture, "ConstructEvent")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const bool bAnyButtonIsPressed = false;
	PointerScrollEvent oPointerScrollEvent(nTimeUsec, Accessor::s_refEmptyAccessor
								, m_refPointerCapability, PointerScrollEvent::SCROLL_UP
								, 77.7, -88.8, bAnyButtonIsPressed);
	REQUIRE(PointerScrollEvent::getClass() == typeid(PointerScrollEvent));
	REQUIRE(oPointerScrollEvent.getEventClass() == typeid(PointerScrollEvent));
	REQUIRE(oPointerScrollEvent.getScrollDir() == PointerScrollEvent::SCROLL_UP);
	REQUIRE(oPointerScrollEvent.getX() == 77.7);
	REQUIRE(oPointerScrollEvent.getY() == -88.8);
	REQUIRE(oPointerScrollEvent.getTimeUsec() == nTimeUsec);
	REQUIRE(oPointerScrollEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	REQUIRE(oPointerScrollEvent.getPointerCapability() == m_refPointerCapability);
	REQUIRE(oPointerScrollEvent.getCapability() == m_refPointerCapability);
	//
	REQUIRE_FALSE(oPointerScrollEvent.getIsModified());
	REQUIRE(oPointerScrollEvent.getXYGrabId() == PointerEvent::s_nPointerNotGrabbedId);
	REQUIRE(oPointerScrollEvent.getXYGrabType() == XYEvent::XY_HOVER);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = false;
	REQUIRE(oPointerScrollEvent.getAsKey(eK, eAsType, bMoreThanOne));
	REQUIRE(eK == HK_SCROLLUP);
	REQUIRE(eAsType == Event::AS_KEY_PRESS);
	REQUIRE(bMoreThanOne);
	auto aAsKeys = oPointerScrollEvent.getAsKeys();
	REQUIRE(aAsKeys.size() == 2);
	REQUIRE(aAsKeys[0].first == HK_SCROLLUP);
	REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	REQUIRE(aAsKeys[1].first == HK_SCROLLUP);
	REQUIRE(aAsKeys[1].second ==  Event::AS_KEY_RELEASE);
	//REQUIRE(false); //just to check
}

TEST_CASE_METHOD(PointerScrollEventClassFixture, "GrabMove")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const bool bAnyButtonIsPressed = true;
	PointerScrollEvent oPointerScrollEvent(nTimeUsec, Accessor::s_refEmptyAccessor
								, m_refPointerCapability, PointerScrollEvent::SCROLL_DOWN
								, 77.7, -88.8, bAnyButtonIsPressed);
	REQUIRE(oPointerScrollEvent.getScrollDir() == PointerScrollEvent::SCROLL_DOWN);
	REQUIRE(oPointerScrollEvent.getX() == 77.7);
	REQUIRE(oPointerScrollEvent.getY() == -88.8);
	//
	REQUIRE_FALSE(oPointerScrollEvent.getIsModified());
	REQUIRE(oPointerScrollEvent.getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
	REQUIRE(oPointerScrollEvent.getXYGrabType() == XYEvent::XY_MOVE);
	//
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = false;
	REQUIRE(oPointerScrollEvent.getAsKey(eK, eAsType, bMoreThanOne));
	REQUIRE(eK == HK_SCROLLDOWN);
	REQUIRE(eAsType == Event::AS_KEY_PRESS);
	REQUIRE(bMoreThanOne);
	auto aAsKeys = oPointerScrollEvent.getAsKeys();
	REQUIRE(aAsKeys.size() == 2);
	REQUIRE(aAsKeys[0].first == HK_SCROLLDOWN);
	REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	REQUIRE(aAsKeys[1].first == HK_SCROLLDOWN);
	REQUIRE(aAsKeys[1].second ==  Event::AS_KEY_RELEASE);
	//REQUIRE(false); //just to check
}
TEST_CASE_METHOD(PointerScrollEventClassFixture, "TranslateXY")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = true;
		PointerScrollEvent oPointerScrollEvent(nTimeUsec, Accessor::s_refEmptyAccessor
									, m_refPointerCapability, PointerScrollEvent::SCROLL_DOWN
									, 77.7, -88.8, bAnyButtonIsPressed);
		REQUIRE_FALSE(oPointerScrollEvent.getIsModified());
		oPointerScrollEvent.translateXY(10, -10);
		REQUIRE(oPointerScrollEvent.getIsModified());
		REQUIRE(Approx(oPointerScrollEvent.getX()).epsilon(0.001) == 87.7);
		REQUIRE(Approx(oPointerScrollEvent.getY()).epsilon(0.001) == -98.8);
		//EXPECT_NEAR(oPointerScrollEvent.getX(), 87.7, 0.001);
		//EXPECT_NEAR(oPointerScrollEvent.getY(), -98.8, 0.001);
	}
	//REQUIRE(false); //just to check
}
TEST_CASE_METHOD(PointerScrollEventClassFixture, "ScaleXY")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		const bool bAnyButtonIsPressed = false;
		PointerScrollEvent oPointerScrollEvent(nTimeUsec, Accessor::s_refEmptyAccessor
									, m_refPointerCapability, PointerScrollEvent::SCROLL_DOWN
									, 77.7, -88.8, bAnyButtonIsPressed);
		REQUIRE_FALSE(oPointerScrollEvent.getIsModified());
		oPointerScrollEvent.scaleXY(1.2, 0.5);
		REQUIRE(oPointerScrollEvent.getIsModified());
		REQUIRE(Approx(oPointerScrollEvent.getX()).epsilon(0.001) == 77.7 * 1.2);
		REQUIRE(Approx(oPointerScrollEvent.getY()).epsilon(0.001) == -88.8 * 0.5);
		//EXPECT_NEAR(oPointerScrollEvent.getX(), 77.7 * 1.2, 0.001);
		//EXPECT_NEAR(oPointerScrollEvent.getY(), -88.8 * 0.5, 0.001);
	}
	//REQUIRE(false); //just to check
}

} // namespace testing

} // namespace stmi
