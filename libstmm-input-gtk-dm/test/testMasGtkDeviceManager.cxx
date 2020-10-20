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
 * File:   testMasGtkDeviceManager.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "fixtureMasDM.h"

#include <stmm-input/callifs.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<GlibAppFixture>, "CreateFakeMasGtkDeviceManager")
{
	auto refMasDM = FakeMasGtkDeviceManager::create(false, {}, KeyRepeat::MODE_SUPPRESS
												, shared_ptr<GdkKeyConverter>{}, Glib::RefPtr<Gdk::DeviceManager>{});
	REQUIRE(refMasDM.operator bool());
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<MasDMFixture>, "AddAccessor")
{
	auto refWin1 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
	REQUIRE(refWin1.operator bool());
	auto refAccessor = std::make_shared<stmi::GtkAccessor>(refWin1);
	const bool bAdded = m_refAllEvDM->addAccessor(refAccessor);

	REQUIRE(bAdded);
	REQUIRE(m_refAllEvDM->hasAccessor(refAccessor));
	const bool bRemoved = m_refAllEvDM->removeAccessor(refAccessor);
	REQUIRE(bRemoved);
	REQUIRE_FALSE(m_refAllEvDM->hasAccessor(refAccessor));
}

TEST_CASE_METHOD(STFX<MasDMFixture>, "AddListener")
{
	auto refListener = std::make_shared<stmi::EventListener>(
			[](const shared_ptr<stmi::Event>& /*refEvent*/)
			{
			});
	const bool bAdded = m_refAllEvDM->addEventListener(refListener);
	REQUIRE(bAdded);
	const bool bAdded2 = m_refAllEvDM->addEventListener(refListener);
	REQUIRE_FALSE(bAdded2);

	const bool bRemoved = m_refAllEvDM->removeEventListener(refListener);
	REQUIRE(bRemoved);
	const bool bRemoved2 = m_refAllEvDM->removeEventListener(refListener);
	REQUIRE_FALSE(bRemoved2);
}

TEST_CASE_METHOD(STFX<MasDMFixture>, "NewMasterDevices")
{
	std::vector< shared_ptr<stmi::Event> > aReceivedEvents;
	auto refListener = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents.emplace_back(refEvent);
			});
	const bool bListenerAdded = m_refAllEvDM->addEventListener(refListener, std::shared_ptr<stmi::CallIf>{});
	REQUIRE(bListenerAdded);

	auto refFakeBackend = m_refAllEvDM->getBackend();

	const auto sOldPointerName = refFakeBackend->getPointerDeviceName();
	const auto sOldKeyboardName = refFakeBackend->getKeyboardDeviceName();
	REQUIRE_FALSE(sOldPointerName.empty());
	REQUIRE_FALSE(sOldKeyboardName.empty());

	refFakeBackend->simulateNewMasterDevices();

	const auto sNewPointerName = refFakeBackend->getPointerDeviceName();
	const auto sNewKeyboardName = refFakeBackend->getKeyboardDeviceName();
	REQUIRE_FALSE(sNewPointerName.empty());
	REQUIRE_FALSE(sNewKeyboardName.empty());
	REQUIRE(sOldPointerName != sNewPointerName);
	REQUIRE(sOldKeyboardName != sNewKeyboardName);

	REQUIRE(aReceivedEvents.size() == 4);
	for (int32_t nIdx = 0; nIdx < 2; ++ nIdx) {
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[nIdx];
		REQUIRE(refEvent.operator bool());
		REQUIRE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		REQUIRE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED);
		// when removed device persists only within callback
		REQUIRE_FALSE(refDeviceMgmtEvent->getDevice().operator bool());
	}
	for (int32_t nIdx = 2; nIdx < 4; ++ nIdx) {
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[nIdx];
		REQUIRE(refEvent.operator bool());
		REQUIRE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		REQUIRE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED);
		REQUIRE(refDeviceMgmtEvent->getDevice().operator bool());
		const auto& sDeviceName = refDeviceMgmtEvent->getDevice()->getName();
		REQUIRE( ((sDeviceName == sNewPointerName) || (sDeviceName == sNewKeyboardName)) );
	}
}
TEST_CASE_METHOD(STFX<MasDMFixture>, "MasterDeviceChange")
{
	std::vector< shared_ptr<stmi::Event> > aReceivedEvents;
	auto refListener = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents.emplace_back(refEvent);
			});
	const bool bListenerAdded = m_refAllEvDM->addEventListener(refListener, std::shared_ptr<stmi::CallIf>{});
	REQUIRE(bListenerAdded);

	auto refFakeBackend = m_refAllEvDM->getBackend();

	const auto sOldPointerName = refFakeBackend->getPointerDeviceName();
	REQUIRE_FALSE(sOldPointerName.empty());

	refFakeBackend->simulateMasterDeviceChange(true);

	// Checking refFakeBackend is doing what it's supposed to.
	const auto sNewPointerName = refFakeBackend->getPointerDeviceName();
	REQUIRE(sOldPointerName == sNewPointerName);

	REQUIRE(aReceivedEvents.size() == 1);
	const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[0];
	REQUIRE(refEvent.operator bool());
	REQUIRE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
	auto refDeviceMgmtEvent = std::static_pointer_cast<DeviceMgmtEvent>(refEvent);
	REQUIRE(refDeviceMgmtEvent->getDevice().operator bool());
	REQUIRE(refDeviceMgmtEvent->getDevice()->getName() == sNewPointerName);
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<MasDMOneWinOneAccFixture>, "SelfRemovingListener")
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	std::vector< shared_ptr<stmi::Event> > aReceivedEvents;
	shared_ptr<stmi::EventListener> refListener = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents.emplace_back(refEvent);
				assert(refEvent->getEventClass() == typeid(stmi::KeyEvent));
				auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
				if (p0KeyEvent->getType() == KeyEvent::KEY_PRESS) {
					const bool bRemoved = m_refAllEvDM->removeEventListener(refListener, true);
					REQUIRE(bRemoved);
				}
			});
	const bool bListenerAdded = m_refAllEvDM->addEventListener(refListener, std::shared_ptr<stmi::CallIf>{});
	REQUIRE(bListenerAdded);
	
	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	REQUIRE(aReceivedEvents.size() == 2);

	const shared_ptr<stmi::Event>& refEvent1 = aReceivedEvents[0];
	REQUIRE(refEvent1.operator bool());
	REQUIRE(refEvent1->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent1 = static_cast<stmi::KeyEvent*>(refEvent1.get());
	REQUIRE(p0KeyEvent1->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent1->getType() == stmi::KeyEvent::KEY_PRESS);

	const shared_ptr<stmi::Event>& refEvent2 = aReceivedEvents[1];
	REQUIRE(refEvent2.operator bool());
	REQUIRE(refEvent2->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent2 = static_cast<stmi::KeyEvent*>(refEvent2.get());
	REQUIRE(p0KeyEvent2->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent2->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
}
////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "KeyEvent")
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);
	assert(refWinData1->isWindowActive());

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	REQUIRE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	REQUIRE(refEvent.operator bool());
	REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
	REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
}

TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "PointerEvent")
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1); //refWinData1->simulateWindowSetActive(true);
	assert(refWinData1->isWindowActive());

	refWinData1->simulateMotionNotify(777.77, 888.88, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	REQUIRE(refEvent.operator bool());
	REQUIRE(refEvent->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent = static_cast<stmi::PointerEvent*>(refEvent.get());
	REQUIRE(p0PointerEvent->getType() == stmi::PointerEvent::POINTER_HOVER);
	REQUIRE(p0PointerEvent->getX() == 777.77);
	REQUIRE(p0PointerEvent->getY() == 888.88);
	REQUIRE(p0PointerEvent->getAccessor() == m_refGtkAccessor1);
}

TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "DeleteWindow")
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1); //refWinData1->simulateWindowSetActive(true);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	REQUIRE(m_aReceivedEvents1.size() == 1);

	m_refWin1.clear();

	REQUIRE(refWinData1->getAccessor()->isDeleted());

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	REQUIRE(m_aReceivedEvents1.size() == 1); // no new event was added

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	REQUIRE(refEvent.operator bool());
	REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
}

TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "TwoListeners")
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
			});
	const bool bListenerAdded2 = m_refAllEvDM->addEventListener(refListener2, std::make_shared<stmi::CallIfEventClass>(stmi::KeyEvent::getClass()));
	REQUIRE(bListenerAdded2);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);
	refWinData1->simulateButtonPress(10, 20, 2, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 2); // received both KeyEvent and PointerEvent
	REQUIRE(aReceivedEvents2.size() == 1); // only received KeyEvent
}

TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "TwoAccessors")
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
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData2 = refFakeFactory->getFakeWindowData(refWin2.operator->());
	REQUIRE(refWinData2.operator bool());

	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
			});
	const bool bListenerAdded2 = m_refAllEvDM->addEventListener(refListener2, std::make_shared<stmi::CallIfAccessor>(refGtkAccessor2));
	REQUIRE(bListenerAdded2);

	REQUIRE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	REQUIRE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);
	refWinData2->simulateKeyPress(GDK_KEY_F2, 68); // This shouldn't be received by any listener because window inactive

	m_refAllEvDM->makeWindowActive(refGtkAccessor2);

	refWinData1->simulateKeyPress(GDK_KEY_F3, 69); // This shouldn't be received by any listener because window inactive
	refWinData2->simulateKeyPress(GDK_KEY_F4, 70);

	m_refAllEvDM->removeAccessor(refGtkAccessor2);

	REQUIRE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	REQUIRE_FALSE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	REQUIRE(m_aReceivedEvents1.size() == 4); // received both F1 and F4 Press + Cancel to active window
	REQUIRE(aReceivedEvents2.size() == 2); // only received F4 Press + Cancel for refWin2 when active

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		REQUIRE(refEvent.operator bool());
		REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
		auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
		if (nIdx == 0) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 1) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		} else if (nIdx == 2) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F4);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 3) {
			REQUIRE(p0KeyEvent->getKey() == stmi::HK_F4);
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		}
	}
	for (auto nIdx = 0u; nIdx < aReceivedEvents2.size(); ++nIdx) {
		auto& refEvent = aReceivedEvents2[nIdx];
		REQUIRE(refEvent.operator bool());
		REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
		auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
		REQUIRE(p0KeyEvent->getKey() == stmi::HK_F4);
		if (nIdx == 0) {
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 1) {
			REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
class MasDMDelayedEventEnablingFixture : public MasDMOneWinOneAccOneListenerFixture
										, public FixtureVariantEventClassesEnable_True
										, public FixtureVariantEventClasses_KeyEvent
{
};
TEST_CASE_METHOD(STFX<MasDMDelayedEventEnablingFixture>, "DelayedEventEnabling")
{
	REQUIRE_FALSE(m_refAllEvDM->isEventClassEnabled(PointerEvent::getClass()));
	REQUIRE(m_refAllEvDM->isEventClassEnabled(KeyEvent::getClass()));

	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);
	assert(refWinData1->isWindowActive());

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	REQUIRE(m_aReceivedEvents1.size() == 1);

	m_refAllEvDM->enableEventClass(PointerEvent::getClass());

	REQUIRE(m_refAllEvDM->isEventClassEnabled(PointerEvent::getClass()));
	REQUIRE(m_refAllEvDM->isEventClassEnabled(KeyEvent::getClass()));

	refWinData1->simulateMotionNotify(777.77, 888.88, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 2); // PointerEvent is not enabled!
}

} // namespace testing

} // namespace stmi
