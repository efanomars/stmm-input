/*
 * Copyright © 2016-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testFakeDeviceManager.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "fakedevicemanager.h"
#include "fakekeydevice.h"
#include "fakepointerdevice.h"
#include "faketouchdevice.h"
#include "fakejoystickdevice.h"

#include "fixtureFakeDM.h"

#include <stmm-input/callifs.h>

#include <cassert>
#include <iostream>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

TEST_CASE("testFakeDeviceManager, Constructor")
{
	auto refFakeDM = std::make_shared<FakeDeviceManager>();
	REQUIRE(refFakeDM->getDevices().empty());

	std::vector<Capability::Class> aDMCapaClasses = refFakeDM->getCapabilityClasses();
	REQUIRE(aDMCapaClasses.size() == 1);
	auto refMgmtCapa = refFakeDM->getCapability(aDMCapaClasses[0]);
	auto p0Capa = refMgmtCapa.get();
	REQUIRE(p0Capa != nullptr);
	REQUIRE(p0Capa->getCapabilityClass().isDeviceManagerCapability());
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(FakeDMFixture, "AddKeyDevice")
{
	const int32_t nKeyDevId = m_refAllEvDM->simulateNewDevice<FakeKeyDevice>();

	DeviceManager* p0DM = m_refAllEvDM.get();
	auto refDevice = p0DM->getDevice(nKeyDevId);
	REQUIRE(refDevice.operator bool());
	REQUIRE_FALSE(p0DM->getDevice(nKeyDevId + 1).operator bool());

	auto aCapaIds = refDevice->getCapabilities();
	REQUIRE(aCapaIds.size() == 1);
	const int32_t nCapaId = aCapaIds[0];
	shared_ptr<Capability> refCapa = refDevice->getCapability(nCapaId);
	REQUIRE(refCapa.operator bool());
	REQUIRE(refCapa->getId() == nCapaId);

	shared_ptr<KeyCapability> refKeyCapa;
	const bool bHasIt = refDevice->getCapability(refKeyCapa);
	REQUIRE(bHasIt);
	REQUIRE(refKeyCapa == refCapa);

	REQUIRE(refDevice->getDeviceManager() == m_refAllEvDM);

	auto aKeyDevIds = m_refAllEvDM->getDevicesWithCapabilityClass(KeyCapability::getClass());
	REQUIRE(aKeyDevIds.size() == 1);
	const int32_t nKeyDevId2 = aKeyDevIds[0];
	REQUIRE(nKeyDevId2 == nKeyDevId);
}

TEST_CASE_METHOD(FakeDMFixture, "RemoveDevice")
{
	const int32_t nPointerDevId = m_refAllEvDM->simulateNewDevice<FakePointerDevice>();

	auto aDeviceIds = m_refAllEvDM->getDevices();
	REQUIRE(aDeviceIds.size() == 1);
	REQUIRE(aDeviceIds[0] == nPointerDevId);

	const bool bRemoved = m_refAllEvDM->simulateRemoveDevice(nPointerDevId);
	REQUIRE(bRemoved);

	aDeviceIds = m_refAllEvDM->getDevices();
	REQUIRE(aDeviceIds.size() == 0);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(FakeDMOneListenerFixture, "SendKeyEventToListeners")
{
	std::vector<shared_ptr<stmi::Event> > aReceivedEvents;
	auto refListener = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents.emplace_back(refEvent);
			});
	const bool bListenerAdded = m_refAllEvDM->addEventListener(refListener, std::make_shared<stmi::CallIfEventClass>(stmi::KeyEvent::getClass()));
	REQUIRE(bListenerAdded);

	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
			});
	const bool bListenerAdded2 = m_refAllEvDM->addEventListener(refListener2, std::make_shared<stmi::CallIfEventClass>(stmi::PointerEvent::getClass()));
	REQUIRE(bListenerAdded2);

	const int32_t nKeyDevId = m_refAllEvDM->simulateNewDevice<FakeKeyDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateKeyEvent(nKeyDevId, KeyEvent::KEY_PRESS, HK_A);

	REQUIRE(nTotSent == 2);
	REQUIRE(m_aReceivedEvents1.size() == 2); // It also received DeviceMgmtEvent
	REQUIRE(aReceivedEvents.size() == 1);
	REQUIRE(aReceivedEvents2.size() == 0);
}

TEST_CASE_METHOD(FakeDMOneListenerFixture, "SendPointerEventToListener")
{
	const int32_t nPointerDevId = m_refAllEvDM->simulateNewDevice<FakePointerDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulatePointerEvent(nPointerDevId, 101.1, 102, PointerEvent::BUTTON_PRESS, 2, true, false);

	REQUIRE(nTotSent == 1);
	REQUIRE(m_aReceivedEvents1.size() == 2);
}

TEST_CASE_METHOD(FakeDMOneListenerFixture, "SendPointerScrollEventToListener")
{
	const int32_t nPointerDevId = m_refAllEvDM->simulateNewDevice<FakePointerDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulatePointerScrollEvent(nPointerDevId, PointerScrollEvent::SCROLL_DOWN, 101.1, 102, false);

	REQUIRE(nTotSent == 1);
	REQUIRE(m_aReceivedEvents1.size() == 2);
}

TEST_CASE_METHOD(FakeDMOneListenerFixture, "SendTouchEventToListener")
{
	const int32_t nTouchDevId = m_refAllEvDM->simulateNewDevice<FakeTouchDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateTouchEvent(nTouchDevId, TouchEvent::TOUCH_BEGIN, 101.1, 102, 55555);

	REQUIRE(nTotSent == 1);
	REQUIRE(m_aReceivedEvents1.size() == 2);
}

TEST_CASE_METHOD(FakeDMOneListenerFixture, "SendJoystickButtonEventToListener")
{
	const int32_t nJoystickDevId = m_refAllEvDM->simulateNewDevice<FakeJoystickDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateJoystickButtonEvent(nJoystickDevId, JoystickButtonEvent::BUTTON_PRESS, JoystickCapability::BUTTON_A);

	REQUIRE(nTotSent == 1);
	REQUIRE(m_aReceivedEvents1.size() == 2);
}

TEST_CASE_METHOD(FakeDMOneListenerFixture, "SendJoystickHatEventToListener")
{
	const int32_t nJoystickDevId = m_refAllEvDM->simulateNewDevice<FakeJoystickDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateJoystickHatEvent(nJoystickDevId, 1, JoystickCapability::HAT_DOWN, JoystickCapability::HAT_CENTER);

	REQUIRE(nTotSent == 1);
	REQUIRE(m_aReceivedEvents1.size() == 2);
}

TEST_CASE_METHOD(FakeDMOneListenerFixture, "SendJoystickAxisEventToListener")
{
	const int32_t nJoystickDevId = m_refAllEvDM->simulateNewDevice<FakeJoystickDevice>();

	const int32_t nTotSent = m_refAllEvDM->simulateJoystickAxisEvent(nJoystickDevId, JoystickCapability::AXIS_X, 5000);

	REQUIRE(nTotSent == 1);
	REQUIRE(m_aReceivedEvents1.size() == 2);
}

} // namespace testing

} // namespace stmi
