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
 * File:   testEventClass.cc
 */

#include "fakedevicemanager.h"

#include "fixtureFakeDM.h"

#include <gtest/gtest.h>

#include <cassert>
#include <iostream>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

TEST(testFakeDeviceManager, Constructor)
{
	auto refFakeDM = std::make_shared<FakeDeviceManager>();
	EXPECT_TRUE(refFakeDM->getDevices().empty());

	std::vector<Capability::Class> aDMCapaClasses = refFakeDM->getCapabilityClasses();
	EXPECT_TRUE(aDMCapaClasses.size() == 1);
	auto refMgmtCapa = refFakeDM->getCapability(aDMCapaClasses[0]);
	auto p0Capa = refMgmtCapa.get();
	EXPECT_TRUE(p0Capa != nullptr);
	EXPECT_TRUE(p0Capa->getCapabilityClass().isDeviceManagerCapability());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FakeDMFixture, AddKeyDevice)
{
	const int32_t nKeyDevId = m_refAllEvDM->simulateNewDevice<FakeKeyDevice>();

	DeviceManager* p0DM = m_refAllEvDM.get();
	auto refDevice = p0DM->getDevice(nKeyDevId);
	EXPECT_TRUE(refDevice.operator bool());
	EXPECT_FALSE(p0DM->getDevice(nKeyDevId + 1).operator bool());

	auto aCapaIds = refDevice->getCapabilities();
	EXPECT_TRUE(aCapaIds.size() == 1);
	const int32_t nCapaId = aCapaIds[0];
	shared_ptr<Capability> refCapa = refDevice->getCapability(nCapaId);
	EXPECT_TRUE(refCapa.operator bool());
	EXPECT_TRUE(refCapa->getId() == nCapaId);

	shared_ptr<KeyCapability> refKeyCapa;
	const bool bHasIt = refDevice->getCapability(refKeyCapa);
	EXPECT_TRUE(bHasIt);
	EXPECT_TRUE(refKeyCapa == refCapa);

	EXPECT_TRUE(refDevice->getDeviceManager() == m_refAllEvDM);

	auto aKeyDevIds = m_refAllEvDM->getDevicesWithCapabilityClass(KeyCapability::getClass());
	EXPECT_TRUE(aKeyDevIds.size() == 1);
	const int32_t nKeyDevId2 = aKeyDevIds[0];
	EXPECT_TRUE(nKeyDevId2 == nKeyDevId);
}

TEST_F(FakeDMFixture, RemoveDevice)
{
	const int32_t nPointerDevId = m_refAllEvDM->simulateNewDevice<FakePointerDevice>();

	auto aDeviceIds = m_refAllEvDM->getDevices();
	EXPECT_TRUE(aDeviceIds.size() == 1);
	EXPECT_TRUE(aDeviceIds[0] == nPointerDevId);

	const bool bRemoved = m_refAllEvDM->simulateRemoveDevice(nPointerDevId);
	EXPECT_TRUE(bRemoved);

	aDeviceIds = m_refAllEvDM->getDevices();
	EXPECT_TRUE(aDeviceIds.size() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(FakeDMOneListenerFixture, SendKeyEventToListeners)
{
	std::vector<shared_ptr<stmi::Event> > aReceivedEvents;
	auto refListener = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents.emplace_back(refEvent);
			});
	const bool bListenerAdded = m_refAllEvDM->addEventListener(refListener, std::make_shared<stmi::CallIfEventClass>(stmi::KeyEvent::getClass()));
	EXPECT_TRUE(bListenerAdded);

	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
			});
	const bool bListenerAdded2 = m_refAllEvDM->addEventListener(refListener2, std::make_shared<stmi::CallIfEventClass>(stmi::PointerEvent::getClass()));
	EXPECT_TRUE(bListenerAdded2);

	const int32_t nKeyDevId = m_refAllEvDM->simulateNewDevice<FakeKeyDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateKeyEvent(nKeyDevId, KeyEvent::KEY_PRESS, HK_A);

	EXPECT_TRUE(nTotSent == 2);
	EXPECT_TRUE(m_aReceivedEvents1.size() == 2); // It also received DeviceMgmtEvent
	EXPECT_TRUE(aReceivedEvents.size() == 1);
	EXPECT_TRUE(aReceivedEvents2.size() == 0);
}

TEST_F(FakeDMOneListenerFixture, SendPointerEventToListener)
{
	const int32_t nPointerDevId = m_refAllEvDM->simulateNewDevice<FakePointerDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulatePointerEvent(nPointerDevId, 101.1, 102, PointerEvent::BUTTON_PRESS, 2, true, false);

	EXPECT_TRUE(nTotSent == 1);
	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);
}

TEST_F(FakeDMOneListenerFixture, SendPointerScrollEventToListener)
{
	const int32_t nPointerDevId = m_refAllEvDM->simulateNewDevice<FakePointerDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulatePointerScrollEvent(nPointerDevId, PointerScrollEvent::SCROLL_DOWN, 101.1, 102, false);

	EXPECT_TRUE(nTotSent == 1);
	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);
}

TEST_F(FakeDMOneListenerFixture, SendTouchEventToListener)
{
	const int32_t nTouchDevId = m_refAllEvDM->simulateNewDevice<FakeTouchDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateTouchEvent(nTouchDevId, TouchEvent::TOUCH_BEGIN, 101.1, 102, 55555);

	EXPECT_TRUE(nTotSent == 1);
	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);
}

TEST_F(FakeDMOneListenerFixture, SendJoystickButtonEventToListener)
{
	const int32_t nJoystickDevId = m_refAllEvDM->simulateNewDevice<FakeJoystickDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateJoystickButtonEvent(nJoystickDevId, JoystickButtonEvent::BUTTON_PRESS, JoystickCapability::BUTTON_A);

	EXPECT_TRUE(nTotSent == 1);
	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);
}

TEST_F(FakeDMOneListenerFixture, SendJoystickHatEventToListener)
{
	const int32_t nJoystickDevId = m_refAllEvDM->simulateNewDevice<FakeJoystickDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateJoystickHatEvent(nJoystickDevId, 1, JoystickCapability::HAT_DOWN, JoystickCapability::HAT_CENTER);

	EXPECT_TRUE(nTotSent == 1);
	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);
}

TEST_F(FakeDMOneListenerFixture, SendJoystickAxisEventToListener)
{
	const int32_t nJoystickDevId = m_refAllEvDM->simulateNewDevice<FakeJoystickDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateJoystickAxisEvent(nJoystickDevId, JoystickCapability::AXIS_X, 5000);

	EXPECT_TRUE(nTotSent == 1);
	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);
}

} // namespace testing

} // namespace stmi
