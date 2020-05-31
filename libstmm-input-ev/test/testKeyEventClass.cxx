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
 * File:   testKeyEventClass.cc
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "fakekeydevice.h"
#include "keyevent.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class KeyEventClassFixture
{
public:
	KeyEventClassFixture()
	{
		m_refKeyDevice = std::make_shared<FakeKeyDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refKeyDevice->getCapability(m_refKeyCapability);
		assert(bFound);
	}
protected:
	shared_ptr<Device> m_refKeyDevice;
	shared_ptr<KeyCapability> m_refKeyCapability;
};

TEST_CASE_METHOD(KeyEventClassFixture, "WorkingSetUp")
{
	REQUIRE(m_refKeyDevice.operator bool());
}

TEST_CASE_METHOD(KeyEventClassFixture, "ConstructEvent")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	KeyEvent oKeyEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_PRESS, HK_U);
	REQUIRE(KeyEvent::getClass() == typeid(KeyEvent));
	REQUIRE(oKeyEvent.getEventClass() == typeid(KeyEvent));
	REQUIRE(oKeyEvent.getKey() == HK_U);
	REQUIRE(oKeyEvent.getType() == KeyEvent::KEY_PRESS);
	REQUIRE(oKeyEvent.getTimeUsec() == nTimeUsec);
	REQUIRE(oKeyEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	REQUIRE(oKeyEvent.getKeyCapability() == m_refKeyCapability);
	REQUIRE(oKeyEvent.getCapability() == m_refKeyCapability);
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	REQUIRE(oKeyEvent.getAsKey(eK, eAsType, bMoreThanOne));
	REQUIRE(eK == HK_U);
	REQUIRE(eAsType == Event::AS_KEY_PRESS);
	REQUIRE_FALSE(bMoreThanOne);
	auto aAsKeys = oKeyEvent.getAsKeys();
	REQUIRE(aAsKeys.size() == 1);
	REQUIRE(aAsKeys[0].first == HK_U);
	REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	//REQUIRE(false); //just to check
}

TEST_CASE_METHOD(KeyEventClassFixture, "KeySimulation")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		KeyEvent oKeyEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_RELEASE, HK_U);
		auto aAsKeys = oKeyEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 1);
		REQUIRE(aAsKeys[0].first == HK_U);
		REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE);
	}
	{
		KeyEvent oKeyEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_RELEASE_CANCEL, HK_ESC);
		auto aAsKeys = oKeyEvent.getAsKeys();
		REQUIRE(aAsKeys.size() == 1);
		REQUIRE(aAsKeys[0].first == HK_ESC);
		REQUIRE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE_CANCEL);
	}
}

} // namespace testing

} // namespace stmi
