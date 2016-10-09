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
 * File:   testKeyEventClass.cc
 */

#include "fakekeydevice.h"
#include "keyevent.h"

#include <gtest/gtest.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class KeyEventClassFixture : public ::testing::Test
{
	void SetUp() override
	{
		m_refKeyDevice = std::make_shared<FakeKeyDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refKeyDevice->getCapability(m_refKeyCapability);
		assert(bFound);
	}
	void TearDown() override
	{
		m_refKeyCapability.reset();
		m_refKeyDevice.reset();
	}
public:
	shared_ptr<Device> m_refKeyDevice;
	shared_ptr<KeyCapability> m_refKeyCapability;
};

TEST_F(KeyEventClassFixture, WorkingSetUp)
{
	EXPECT_TRUE(m_refKeyDevice.operator bool());
}

TEST_F(KeyEventClassFixture, ConstructEvent)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	KeyEvent oKeyEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_PRESS, HK_U);
	EXPECT_TRUE(KeyEvent::getClass() == typeid(KeyEvent));
	EXPECT_TRUE(oKeyEvent.getEventClass() == typeid(KeyEvent));
	EXPECT_TRUE(oKeyEvent.getKey() == HK_U);
	EXPECT_TRUE(oKeyEvent.getType() == KeyEvent::KEY_PRESS);
	EXPECT_TRUE(oKeyEvent.getTimeUsec() == nTimeUsec);
	EXPECT_TRUE(oKeyEvent.getAccessor() == Accessor::s_refEmptyAccessor);
	EXPECT_TRUE(oKeyEvent.getKeyCapability() == m_refKeyCapability);
	EXPECT_TRUE(oKeyEvent.getCapability() == m_refKeyCapability);
	HARDWARE_KEY eK = HK_T;
	Event::AS_KEY_INPUT_TYPE eAsType = Event::AS_KEY_RELEASE_CANCEL;
	bool bMoreThanOne = true;
	EXPECT_TRUE(oKeyEvent.getAsKey(eK, eAsType, bMoreThanOne));
	EXPECT_TRUE(eK == HK_U);
	EXPECT_TRUE(eAsType == Event::AS_KEY_PRESS);
	EXPECT_FALSE(bMoreThanOne);
	auto aAsKeys = oKeyEvent.getAsKeys();
	EXPECT_TRUE(aAsKeys.size() == 1);
	EXPECT_TRUE(aAsKeys[0].first == HK_U);
	EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_PRESS);
	//EXPECT_TRUE(false); //just to check
}

TEST_F(KeyEventClassFixture, KeySimulation)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	{
		KeyEvent oKeyEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_RELEASE, HK_U);
		auto aAsKeys = oKeyEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 1);
		EXPECT_TRUE(aAsKeys[0].first == HK_U);
		EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE);
	}
	{
		KeyEvent oKeyEvent(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_RELEASE_CANCEL, HK_ESC);
		auto aAsKeys = oKeyEvent.getAsKeys();
		EXPECT_TRUE(aAsKeys.size() == 1);
		EXPECT_TRUE(aAsKeys[0].first == HK_ESC);
		EXPECT_TRUE(aAsKeys[0].second ==  Event::AS_KEY_RELEASE_CANCEL);
	}
}

} // namespace testing

} // namespace stmi
