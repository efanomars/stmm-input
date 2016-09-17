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
 * File:   testJsGtkDeviceManager.cc
 */

#include <gtest/gtest.h>
#include <stmm-input/callifs.h>

#include "fixtureJsDM.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
TEST_F(GlibAppFixture, CreateFakeJsGtkDeviceManager)
{
	auto refJsDM = FakeJsGtkDeviceManager::create(false, {});
	EXPECT_TRUE(refJsDM.operator bool());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(JsDMFixture, AddAccessor)
{
	auto refWin1 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
	EXPECT_TRUE(refWin1.operator bool());
	auto refAccessor = std::make_shared<stmi::GtkAccessor>(refWin1);
	const bool bAdded = m_refAllEvDM->addAccessor(refAccessor);

	EXPECT_TRUE(bAdded);
	EXPECT_TRUE(m_refAllEvDM->hasAccessor(refAccessor));
	const bool bRemoved = m_refAllEvDM->removeAccessor(refAccessor);
	EXPECT_TRUE(bRemoved);
	EXPECT_FALSE(m_refAllEvDM->hasAccessor(refAccessor));
}

TEST_F(JsDMFixture, AddListener)
{
	auto refListener = std::make_shared<stmi::EventListener>(
			[](const shared_ptr<stmi::Event>& /*refEvent*/)
			{
			});
	//TODO WHY is SECOND param needed !!!??? inline doesn't seem to work
	const bool bAdded = m_refAllEvDM->addEventListener(refListener, std::shared_ptr<stmi::CallIf>{});
	EXPECT_TRUE(bAdded);
	const bool bAdded2 = m_refAllEvDM->addEventListener(refListener, std::shared_ptr<stmi::CallIf>{});
	EXPECT_FALSE(bAdded2);

	//TODO WHY is SECOND param needed !!!??? inline doesn't seem to work
	const bool bRemoved = m_refAllEvDM->removeEventListener(refListener, true);
	EXPECT_TRUE(bRemoved);
	const bool bRemoved2 = m_refAllEvDM->removeEventListener(refListener, true);
	EXPECT_FALSE(bRemoved2);
}

TEST_F(JsDMFixture, DeviceAddAndRemoveChange)
{
	std::vector< shared_ptr<stmi::Event> > aReceivedEvents;
	auto refListener = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents.emplace_back(refEvent);
			});
	const bool bListenerAdded = m_refAllEvDM->addEventListener(refListener, std::shared_ptr<stmi::CallIf>{});
	EXPECT_TRUE(bListenerAdded);

	auto refFakeBackend = m_refAllEvDM->getBackend();

	const int32_t nSimuId1 = refFakeBackend->simulateNewDevice("TestJoy1"
							, {BTN_A, BTN_B, BTN_X, BTN_Y, BTN_TL, BTN_TR, BTN_SELECT, BTN_START, BTN_MODE}
							, 1
							, {ABS_X, ABS_Y, ABS_RX, ABS_RY, ABS_Z, ABS_RZ, ABS_HAT0X, ABS_HAT0Y});
	EXPECT_TRUE(nSimuId1 >= 0);
	EXPECT_TRUE(aReceivedEvents.size() == 1);
	int32_t nDeviceId1;
	{
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[0];
		EXPECT_TRUE(refEvent.operator bool());
		nDeviceId1 = refEvent->getCapabilityId();
		EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		EXPECT_TRUE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED);
	}

	const int32_t nSimuId2 = refFakeBackend->simulateNewDevice("TestJoy2"
							, {BTN_A, BTN_B, BTN_X, BTN_Y, BTN_TL, BTN_TR, BTN_SELECT, BTN_START, BTN_MODE}
							, 0
							, {ABS_X, ABS_Y, ABS_RX, ABS_RY, ABS_Z, ABS_RZ});
	EXPECT_TRUE(nSimuId2 >= 0);
	EXPECT_TRUE(aReceivedEvents.size() == 2);
	int32_t nDeviceId2;
	{
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[1];
		EXPECT_TRUE(refEvent.operator bool());
		nDeviceId2 = refEvent->getCapabilityId();
		EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		EXPECT_TRUE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED);
	}

	refFakeBackend->simulateRemoveDevice(nSimuId1);
	EXPECT_TRUE(aReceivedEvents.size() == 3);
	{
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[2];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getCapabilityId() == nDeviceId1);
		EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		EXPECT_TRUE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED);
		// when removed device persists only within callback
		EXPECT_FALSE(refDeviceMgmtEvent->getDevice().operator bool());
	}

	refFakeBackend->simulateRemoveDevice(nSimuId2);
	EXPECT_TRUE(aReceivedEvents.size() == 4);
	{
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[3];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getCapabilityId() == nDeviceId2);
		EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		EXPECT_TRUE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED);
		// when removed device persists only within callback
		EXPECT_FALSE(refDeviceMgmtEvent->getDevice().operator bool());
	}
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(JsDMOneWinOneAccOneDevOneListenerFixture, SingleButtonPressRelease)
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Js::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);
	assert(refWinData1->isWindowActive());

	EXPECT_TRUE(m_aReceivedEvents1.size() == 0);

	js_event oEv;
	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 3; 
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // BTN_Y press

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent1.operator bool());
	EXPECT_TRUE(refEvent1->getEventClass() == typeid(stmi::JoystickButtonEvent));
	auto p0ButtonEvent1 = static_cast<stmi::JoystickButtonEvent*>(refEvent1.get());
	EXPECT_TRUE(p0ButtonEvent1->getButton() == stmi::JoystickCapability::BUTTON_Y);
	EXPECT_TRUE(p0ButtonEvent1->getType() == stmi::JoystickButtonEvent::BUTTON_PRESS);
	auto refJCapa1 = p0ButtonEvent1->getJoystickCapability();
	EXPECT_TRUE(refJCapa1->isButtonPressed(JoystickCapability::BUTTON_Y));

	oEv.value = 0;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // BTN_Y release

	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);

	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	EXPECT_TRUE(refEvent2.operator bool());
	EXPECT_TRUE(refEvent2->getEventClass() == typeid(stmi::JoystickButtonEvent));
	auto p0ButtonEvent2 = static_cast<stmi::JoystickButtonEvent*>(refEvent2.get());
	EXPECT_TRUE(p0ButtonEvent2->getButton() == stmi::JoystickCapability::BUTTON_Y);
	EXPECT_TRUE(p0ButtonEvent2->getType() == stmi::JoystickButtonEvent::BUTTON_RELEASE);
	auto refJCapa2 = p0ButtonEvent2->getJoystickCapability();
	EXPECT_FALSE(refJCapa2->isButtonPressed(JoystickCapability::BUTTON_Y));
	EXPECT_TRUE(refJCapa1 == refJCapa2);
}

TEST_F(JsDMOneWinOneAccOneDevOneListenerFixture, TwoAccessorButtonCancel)
{
	auto refWin2 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
	assert(refWin2.operator bool());
	auto refGtkAccessor2 = std::make_shared<stmi::GtkAccessor>(refWin2);
	assert(refGtkAccessor2.operator bool());
	#ifndef NDEBUG
	const bool bAdded = 
	#endif
	m_refAllEvDM->addAccessor(refGtkAccessor2);
	assert(bAdded);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Js::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());
	const std::shared_ptr<Js::FakeGtkWindowData>& refWinData2 = refFakeFactory->getFakeWindowData(refWin2.operator->());
	EXPECT_TRUE(refWinData2.operator bool());

	EXPECT_TRUE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_TRUE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	js_event oEv;
	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 6; // Index into JsDMOneWinOneAccOneDevFixture buttons
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // press BTN_SELECT

	m_refAllEvDM->makeWindowActive(refGtkAccessor2);

	oEv.number = 4; // Index into JsDMOneWinOneAccOneDevFixture buttons
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // press BTN_TL

	m_refAllEvDM->removeAccessor(refGtkAccessor2);

	EXPECT_TRUE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_FALSE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	EXPECT_TRUE(m_aReceivedEvents1.size() == 4); // received both BTN_SELECT and BTN_TL Press + Cancel to active window

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::JoystickButtonEvent));
		auto p0ButtonEvent = static_cast<stmi::JoystickButtonEvent*>(refEvent.get());
		if (nIdx == 0) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_SELECT);
			EXPECT_TRUE(p0ButtonEvent->getType() == stmi::JoystickButtonEvent::BUTTON_PRESS);
		} else if (nIdx == 1) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_SELECT);
			EXPECT_TRUE(p0ButtonEvent->getType() == stmi::JoystickButtonEvent::BUTTON_RELEASE_CANCEL);
		} else if (nIdx == 2) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_TL);
			EXPECT_TRUE(p0ButtonEvent->getType() == stmi::JoystickButtonEvent::BUTTON_PRESS);
		} else if (nIdx == 3) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_TL);
			EXPECT_TRUE(p0ButtonEvent->getType() == stmi::JoystickButtonEvent::BUTTON_RELEASE_CANCEL);
		}
	}
}

TEST_F(JsDMOneWinOneAccOneDevOneListenerFixture, HatPressRelease)
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Js::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);
	assert(refWinData1->isWindowActive());

	EXPECT_TRUE(m_aReceivedEvents1.size() == 0);

	js_event oEv;
	oEv.type = JS_EVENT_AXIS;
	oEv.number = 6; // ABS_HAT0X
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to right

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent1.operator bool());
	EXPECT_TRUE(refEvent1->getEventClass() == typeid(JoystickHatEvent));
	auto p0HatEvent1 = static_cast<JoystickHatEvent*>(refEvent1.get());
	EXPECT_TRUE(p0HatEvent1->getHat() == 0);
	EXPECT_TRUE(p0HatEvent1->getValue() == JoystickCapability::HAT_RIGHT);
	EXPECT_TRUE(p0HatEvent1->getDeltaXY().first == 1);
	EXPECT_TRUE(p0HatEvent1->getDeltaXY().second == 0);
	auto refJCapa1 = p0HatEvent1->getJoystickCapability();
	EXPECT_TRUE(refJCapa1->getHatValue(0) == JoystickCapability::HAT_RIGHT);

	oEv.number = 7; // ABS_HAT0Y
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to right-down

	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);

	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	EXPECT_TRUE(refEvent2.operator bool());
	EXPECT_TRUE(refEvent2->getEventClass() == typeid(JoystickHatEvent));
	auto p0HatEvent2 = static_cast<JoystickHatEvent*>(refEvent2.get());
	EXPECT_TRUE(p0HatEvent2->getValue() == JoystickCapability::HAT_RIGHTDOWN);
	EXPECT_TRUE(p0HatEvent2->getDeltaXY().first == 1);
	EXPECT_TRUE(p0HatEvent2->getDeltaXY().second == 1);
	auto refJCapa2 = p0HatEvent2->getJoystickCapability();
	EXPECT_TRUE(refJCapa2->getHatValue(0) == JoystickCapability::HAT_RIGHTDOWN);

	oEv.number = 6; // ABS_HAT0X
	oEv.value = 0;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to down

	EXPECT_TRUE(m_aReceivedEvents1.size() == 3);

	const shared_ptr<stmi::Event>& refEvent3 = m_aReceivedEvents1[2];
	EXPECT_TRUE(refEvent3.operator bool());
	EXPECT_TRUE(refEvent3->getEventClass() == typeid(JoystickHatEvent));
	auto p0HatEvent3 = static_cast<JoystickHatEvent*>(refEvent3.get());
	EXPECT_TRUE(p0HatEvent3->getValue() == JoystickCapability::HAT_DOWN);
	EXPECT_TRUE(p0HatEvent3->getDeltaXY().first == 0);
	EXPECT_TRUE(p0HatEvent3->getDeltaXY().second == 1);
	auto refJCapa3 = p0HatEvent3->getJoystickCapability();
	EXPECT_TRUE(refJCapa3->getHatValue(0) == JoystickCapability::HAT_DOWN);

	oEv.number = 7; // ABS_HAT0Y
	oEv.value = 0;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to center

	EXPECT_TRUE(m_aReceivedEvents1.size() == 4);

	const shared_ptr<stmi::Event>& refEvent4 = m_aReceivedEvents1[3];
	EXPECT_TRUE(refEvent4.operator bool());
	EXPECT_TRUE(refEvent4->getEventClass() == typeid(JoystickHatEvent));
	auto p0HatEvent4 = static_cast<JoystickHatEvent*>(refEvent4.get());
	EXPECT_TRUE(p0HatEvent4->getValue() == JoystickCapability::HAT_CENTER);
	EXPECT_TRUE(p0HatEvent4->getDeltaXY().first == 0);
	EXPECT_TRUE(p0HatEvent4->getDeltaXY().second == 0);
	auto refJCapa4 = p0HatEvent4->getJoystickCapability();
	EXPECT_TRUE(refJCapa4->getHatValue(0) == JoystickCapability::HAT_CENTER);
}

TEST_F(JsDMOneWinOneAccOneDevOneListenerFixture, TwoAccessorHatCancel)
{
	auto refWin2 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
	assert(refWin2.operator bool());
	auto refGtkAccessor2 = std::make_shared<stmi::GtkAccessor>(refWin2);
	assert(refGtkAccessor2.operator bool());
	#ifndef NDEBUG
	const bool bAdded = 
	#endif
	m_refAllEvDM->addAccessor(refGtkAccessor2);
	assert(bAdded);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Js::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());
	const std::shared_ptr<Js::FakeGtkWindowData>& refWinData2 = refFakeFactory->getFakeWindowData(refWin2.operator->());
	EXPECT_TRUE(refWinData2.operator bool());

	EXPECT_TRUE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_TRUE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	js_event oEv;
	oEv.type = JS_EVENT_AXIS;
	oEv.number = 6; // ABS_HAT0X
	oEv.value = -1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to left

	oEv.number = 7; // ABS_HAT0Y
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to left-up

	m_refAllEvDM->makeWindowActive(refGtkAccessor2);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 3);

	oEv.number = 6; // ABS_HAT0X
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to right

	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to right, IGNORED repetition

	m_refAllEvDM->removeAccessor(refGtkAccessor2);

	EXPECT_TRUE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_FALSE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	EXPECT_TRUE(m_aReceivedEvents1.size() == 5); // received left, left-up, center-cancel; right, center-cancel

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getEventClass() == typeid(JoystickHatEvent));
		auto p0HatEvent = static_cast<JoystickHatEvent*>(refEvent.get());
		if (nIdx == 0) {
			EXPECT_TRUE(p0HatEvent->getValue() == JoystickCapability::HAT_LEFT);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().first == -1);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().second == 0);
		} else if (nIdx == 1) {
			EXPECT_TRUE(p0HatEvent->getValue() == JoystickCapability::HAT_LEFTUP);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().first == -1);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().second == -1);
		} else if (nIdx == 2) {
			EXPECT_TRUE(p0HatEvent->getValue() == JoystickCapability::HAT_CENTER_CANCEL);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().first == 0);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().second == 0);
		} else if (nIdx == 3) {
			EXPECT_TRUE(p0HatEvent->getValue() == JoystickCapability::HAT_RIGHT);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().first == +1);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().second == 0);
		} else if (nIdx == 4) {
			EXPECT_TRUE(p0HatEvent->getValue() == JoystickCapability::HAT_CENTER_CANCEL);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().first == 0);
			EXPECT_TRUE(p0HatEvent->getDeltaXY().second == 0);
		}
	}
}

TEST_F(JsDMOneWinOneAccOneDevOneListenerFixture, AxisEvents)
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Js::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);
	assert(refWinData1->isWindowActive());

	EXPECT_TRUE(m_aReceivedEvents1.size() == 0);

	js_event oEv;
	oEv.type = JS_EVENT_AXIS;
	oEv.number = 2; // ABS_RX
	oEv.value = 9999;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent1.operator bool());
	EXPECT_TRUE(refEvent1->getEventClass() == typeid(JoystickAxisEvent));
	auto p0AxisEvent1 = static_cast<JoystickAxisEvent*>(refEvent1.get());
	EXPECT_TRUE(p0AxisEvent1->getValue() == 9999);
	EXPECT_TRUE(p0AxisEvent1->getAxis() == JoystickCapability::AXIS_RX);
	auto refJCapa1 = p0AxisEvent1->getJoystickCapability();
	EXPECT_TRUE(refJCapa1->getAxisValue(JoystickCapability::AXIS_RX) == 9999);
}

TEST_F(JsDMOneWinOneAccOneDevOneListenerFixture, ThreeListeners)
{
	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	std::vector<shared_ptr<stmi::Event> > aReceivedEvents3;
	auto refListener3 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents3.emplace_back(refEvent);
			});
	//
	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
				EXPECT_TRUE(refEvent.operator bool());
				EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::JoystickButtonEvent));
				auto p0ButtonEvent = static_cast<JoystickButtonEvent*>(refEvent.get());
				if ((p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_PRESS) && (p0ButtonEvent->getButton() == JoystickCapability::BUTTON_B)) {
					// Add third listener
					const bool bListenerAdded3 = m_refAllEvDM->addEventListener(refListener3, std::shared_ptr<stmi::CallIf>{});
					EXPECT_TRUE(bListenerAdded3);
				}
			});
	const bool bListenerAdded2 = m_refAllEvDM->addEventListener(refListener2, std::make_shared<stmi::CallIfEventClass>(JoystickButtonEvent::getClass()));
	EXPECT_TRUE(bListenerAdded2);

	js_event oEv;
	oEv.type = JS_EVENT_AXIS;
	oEv.number = 6; // ABS_HAT0X
	oEv.value = -1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to left

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);
	EXPECT_TRUE(aReceivedEvents2.size() == 0); // only listens to buttons
	EXPECT_TRUE(aReceivedEvents3.size() == 0);

	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 1; // Index into JsDMOneWinOneAccOneDevFixture buttons
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // press BTN_B

	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);
	EXPECT_TRUE(aReceivedEvents2.size() == 1);
	EXPECT_TRUE(aReceivedEvents3.size() == 0);

	oEv.value = 0;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // release BTN_B

	EXPECT_TRUE(m_aReceivedEvents1.size() == 3);
	EXPECT_TRUE(aReceivedEvents2.size() == 2);
	EXPECT_TRUE(aReceivedEvents3.size() == 0); // Since it wasn't pressed when added listener 3 doesn't receive release

	oEv.type = JS_EVENT_AXIS;
	oEv.number = 6; // ABS_HAT0X
	oEv.value = 0;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to center

	EXPECT_TRUE(m_aReceivedEvents1.size() == 4);
	EXPECT_TRUE(aReceivedEvents2.size() == 2);
	EXPECT_TRUE(aReceivedEvents3.size() == 0); // Since it wasn't pressed when added listener 3 doesn't receive center

	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 2;
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // press BTN_X

	EXPECT_TRUE(m_aReceivedEvents1.size() == 5);
	EXPECT_TRUE(aReceivedEvents2.size() == 3);
	EXPECT_TRUE(aReceivedEvents3.size() == 1);

	m_refAllEvDM->removeEventListener(m_refListener1, true);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 6); // release cancel BTN_X
	EXPECT_TRUE(aReceivedEvents2.size() == 3);
	EXPECT_TRUE(aReceivedEvents3.size() == 1);

	oEv.value = 0;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // release BTN_X

	EXPECT_TRUE(m_aReceivedEvents1.size() == 6); // no longer active
	EXPECT_TRUE(aReceivedEvents2.size() == 4);
	EXPECT_TRUE(aReceivedEvents3.size() == 2);

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		EXPECT_TRUE(refEvent.operator bool());
		if ((nIdx == 0) || (nIdx == 3)) {
			EXPECT_TRUE(refEvent->getEventClass() == typeid(JoystickHatEvent));
			auto p0HatEvent = static_cast<JoystickHatEvent*>(refEvent.get());
			if (nIdx == 0) {
				EXPECT_TRUE(p0HatEvent->getValue() == JoystickCapability::HAT_LEFT);
				EXPECT_TRUE(p0HatEvent->getDeltaXY().first == -1);
				EXPECT_TRUE(p0HatEvent->getDeltaXY().second == 0);
			} else if (nIdx == 3) {
				EXPECT_TRUE(p0HatEvent->getValue() == JoystickCapability::HAT_CENTER);
				EXPECT_TRUE(p0HatEvent->getDeltaXY().first == 0);
				EXPECT_TRUE(p0HatEvent->getDeltaXY().second == 0);
			}			
		} else {
			EXPECT_TRUE(refEvent->getEventClass() == typeid(JoystickButtonEvent));
			auto p0ButtonEvent = static_cast<JoystickButtonEvent*>(refEvent.get());
			if (nIdx == 1) {
				EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_B);
				EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_PRESS);
			} else if (nIdx == 2) {
				EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_B);
				EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_RELEASE);
			} else if (nIdx == 4) {
				EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_X);
				EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_PRESS);
			} else if (nIdx == 5) {
				EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_X);
				EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_RELEASE_CANCEL);
			}
		}
	}
	for (auto nIdx = 0u; nIdx < aReceivedEvents2.size(); ++nIdx) {
		auto& refEvent = aReceivedEvents2[nIdx];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getEventClass() == typeid(JoystickButtonEvent));
		auto p0ButtonEvent = static_cast<JoystickButtonEvent*>(refEvent.get());
		if (nIdx == 0) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_B);
			EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_PRESS);
		} else if (nIdx == 1) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_B);
			EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_RELEASE);
		} else if (nIdx == 2) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_X);
			EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_PRESS);
		} else if (nIdx == 3) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_X);
			EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_RELEASE);
		}
	}
	for (auto nIdx = 0u; nIdx < aReceivedEvents3.size(); ++nIdx) {
		auto& refEvent = aReceivedEvents3[nIdx];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getEventClass() == typeid(JoystickButtonEvent));
		auto p0ButtonEvent = static_cast<JoystickButtonEvent*>(refEvent.get());
		if (nIdx == 0) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_X);
			EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_PRESS);
		} else if (nIdx == 1) {
			EXPECT_TRUE(p0ButtonEvent->getButton() == JoystickCapability::BUTTON_X);
			EXPECT_TRUE(p0ButtonEvent->getType() == JoystickButtonEvent::BUTTON_RELEASE);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
class JsDMDelayedEventEnablingFixture : public JsDMOneWinOneAccOneDevOneListenerFixture
										, public FixtureVariantEventClassesEnable_True
										, public FixtureVariantEventClasses_JoystickAxisEvent
{
};
TEST_F(JsDMDelayedEventEnablingFixture, DelayedEventEnabling)
{
	EXPECT_TRUE(m_refAllEvDM->isEventClassEnabled(JoystickAxisEvent::getClass()));
	EXPECT_FALSE(m_refAllEvDM->isEventClassEnabled(JoystickHatEvent::getClass()));
	EXPECT_FALSE(m_refAllEvDM->isEventClassEnabled(JoystickButtonEvent::getClass()));

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	js_event oEv;
	oEv.type = JS_EVENT_AXIS;
	oEv.number = 6; // ABS_HAT0X
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to right

	EXPECT_TRUE(m_aReceivedEvents1.size() == 0); // hat not enabled yet

	m_refAllEvDM->enableEventClass(JoystickHatEvent::getClass());
	EXPECT_TRUE(m_refAllEvDM->isEventClassEnabled(JoystickHatEvent::getClass()));

	oEv.number = 7; // ABS_HAT0Y
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to down

	EXPECT_TRUE(m_aReceivedEvents1.size() == 0); // hat must go back to center first

	oEv.number = 6; // ABS_HAT0X
	oEv.value = 0;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0x to center

	EXPECT_TRUE(m_aReceivedEvents1.size() == 0); // hat must go back to center first

	oEv.number = 7; // ABS_HAT0Y
	oEv.value = 0;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0y to center

	EXPECT_TRUE(m_aReceivedEvents1.size() == 0);

	oEv.number = 6; // ABS_HAT0X
	oEv.value = 1;
	m_p0FakeBackend->simulateJsEvent(m_nDeviceId, &oEv); // hat 0 to right

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);
}

} // namespace testing

} // namespace stmi
