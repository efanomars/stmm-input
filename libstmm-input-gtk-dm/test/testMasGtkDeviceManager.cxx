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
 * File:   testMasGtkDeviceManager.cc
 */

#include "fixtureMasDM.h"

#include <stmm-input/callifs.h>

#include <gtest/gtest.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
TEST_F(GlibAppFixture, CreateFakeMasGtkDeviceManager)
{
	auto refMasDM = FakeMasGtkDeviceManager::create(false, {}, KeyRepeat::MODE_SUPPRESS
												, shared_ptr<GdkKeyConverter>{}, Glib::RefPtr<Gdk::DeviceManager>{});
	EXPECT_TRUE(refMasDM.operator bool());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(MasDMFixture, AddAccessor)
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

TEST_F(MasDMFixture, AddListener)
{
	auto refListener = std::make_shared<stmi::EventListener>(
			[](const shared_ptr<stmi::Event>& /*refEvent*/)
			{
			});
	const bool bAdded = m_refAllEvDM->addEventListener(refListener);
	EXPECT_TRUE(bAdded);
	const bool bAdded2 = m_refAllEvDM->addEventListener(refListener);
	EXPECT_FALSE(bAdded2);

	const bool bRemoved = m_refAllEvDM->removeEventListener(refListener);
	EXPECT_TRUE(bRemoved);
	const bool bRemoved2 = m_refAllEvDM->removeEventListener(refListener);
	EXPECT_FALSE(bRemoved2);
}

TEST_F(MasDMFixture, NewMasterDevices)
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

	const auto sOldPointerName = refFakeBackend->getPointerDeviceName();
	const auto sOldKeyboardName = refFakeBackend->getKeyboardDeviceName();
	EXPECT_FALSE(sOldPointerName.empty());
	EXPECT_FALSE(sOldKeyboardName.empty());

	refFakeBackend->simulateNewMasterDevices();

	const auto sNewPointerName = refFakeBackend->getPointerDeviceName();
	const auto sNewKeyboardName = refFakeBackend->getKeyboardDeviceName();
	EXPECT_FALSE(sNewPointerName.empty());
	EXPECT_FALSE(sNewKeyboardName.empty());
	EXPECT_TRUE(sOldPointerName != sNewPointerName);
	EXPECT_TRUE(sOldKeyboardName != sNewKeyboardName);

	EXPECT_TRUE(aReceivedEvents.size() == 4);
	for (int32_t nIdx = 0; nIdx < 2; ++ nIdx) {
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[nIdx];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		EXPECT_TRUE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED);
		// when removed device persists only within callback
		EXPECT_FALSE(refDeviceMgmtEvent->getDevice().operator bool());
	}
	for (int32_t nIdx = 2; nIdx < 4; ++ nIdx) {
		const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[nIdx];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
		auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
		EXPECT_TRUE(refDeviceMgmtEvent->getDeviceMgmtType() == stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED);
		EXPECT_TRUE(refDeviceMgmtEvent->getDevice().operator bool());
		const auto& sDeviceName = refDeviceMgmtEvent->getDevice()->getName();
		EXPECT_TRUE((sDeviceName == sNewPointerName) || (sDeviceName == sNewKeyboardName));
	}
}
TEST_F(MasDMFixture, MasterDeviceChange)
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

	const auto sOldPointerName = refFakeBackend->getPointerDeviceName();
	EXPECT_FALSE(sOldPointerName.empty());

	refFakeBackend->simulateMasterDeviceChange(true);

	// Checking refFakeBackend is doing what it's supposed to.
	const auto sNewPointerName = refFakeBackend->getPointerDeviceName();
	EXPECT_TRUE(sOldPointerName == sNewPointerName);

	EXPECT_TRUE(aReceivedEvents.size() == 1);
	const shared_ptr<stmi::Event>& refEvent = aReceivedEvents[0];
	EXPECT_TRUE(refEvent.operator bool());
	EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::DeviceMgmtEvent));
	auto refDeviceMgmtEvent = std::static_pointer_cast<DeviceMgmtEvent>(refEvent);
	EXPECT_TRUE(refDeviceMgmtEvent->getDevice().operator bool());
	EXPECT_TRUE(refDeviceMgmtEvent->getDevice()->getName() == sNewPointerName);
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(MasDMOneWinOneAccFixture, SelfRemovingListener)
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());
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
					EXPECT_TRUE(bRemoved);
				}
			});
	const bool bListenerAdded = m_refAllEvDM->addEventListener(refListener, std::shared_ptr<stmi::CallIf>{});
	EXPECT_TRUE(bListenerAdded);
	
	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	EXPECT_TRUE(aReceivedEvents.size() == 2);

	const shared_ptr<stmi::Event>& refEvent1 = aReceivedEvents[0];
	EXPECT_TRUE(refEvent1.operator bool());
	EXPECT_TRUE(refEvent1->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent1 = static_cast<stmi::KeyEvent*>(refEvent1.get());
	EXPECT_TRUE(p0KeyEvent1->getKey() == stmi::HK_F1);
	EXPECT_TRUE(p0KeyEvent1->getType() == stmi::KeyEvent::KEY_PRESS);

	const shared_ptr<stmi::Event>& refEvent2 = aReceivedEvents[1];
	EXPECT_TRUE(refEvent2.operator bool());
	EXPECT_TRUE(refEvent2->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent2 = static_cast<stmi::KeyEvent*>(refEvent2.get());
	EXPECT_TRUE(p0KeyEvent2->getKey() == stmi::HK_F1);
	EXPECT_TRUE(p0KeyEvent2->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
}
////////////////////////////////////////////////////////////////////////////////
TEST_F(MasDMOneWinOneAccOneListenerFixture, KeyEvent)
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);
	assert(refWinData1->isWindowActive());

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent.operator bool());
	EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
	EXPECT_TRUE(p0KeyEvent->getKey() == stmi::HK_F1);
	EXPECT_TRUE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
}

TEST_F(MasDMOneWinOneAccOneListenerFixture, PointerEvent)
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1); //refWinData1->simulateWindowSetActive(true);
	assert(refWinData1->isWindowActive());

	refWinData1->simulateMotionNotify(777.77, 888.88, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent.operator bool());
	EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent = static_cast<stmi::PointerEvent*>(refEvent.get());
	EXPECT_TRUE(p0PointerEvent->getType() == stmi::PointerEvent::POINTER_HOVER);
	EXPECT_TRUE(p0PointerEvent->getX() == 777.77);
	EXPECT_TRUE(p0PointerEvent->getY() == 888.88);
	EXPECT_TRUE(p0PointerEvent->getAccessor() == m_refGtkAccessor1);
}

TEST_F(MasDMOneWinOneAccOneListenerFixture, DeleteWindow)
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1); //refWinData1->simulateWindowSetActive(true);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	m_refWin1.clear();

	EXPECT_TRUE(refWinData1->getAccessor()->isDeleted());

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1); // no new event was added

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent.operator bool());
	EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
}

TEST_F(MasDMOneWinOneAccOneListenerFixture, TwoListeners)
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
			});
	const bool bListenerAdded2 = m_refAllEvDM->addEventListener(refListener2, std::make_shared<stmi::CallIfEventClass>(stmi::KeyEvent::getClass()));
	EXPECT_TRUE(bListenerAdded2);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);
	refWinData1->simulateButtonPress(10, 20, 2, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 2); // received both KeyEvent and PointerEvent
	EXPECT_TRUE(aReceivedEvents2.size() == 1); // only received KeyEvent
}

TEST_F(MasDMOneWinOneAccOneListenerFixture, TwoAccessors)
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
	EXPECT_TRUE(refWinData1.operator bool());
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData2 = refFakeFactory->getFakeWindowData(refWin2.operator->());
	EXPECT_TRUE(refWinData2.operator bool());

	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
			});
	const bool bListenerAdded2 = m_refAllEvDM->addEventListener(refListener2, std::make_shared<stmi::CallIfAccessor>(refGtkAccessor2));
	EXPECT_TRUE(bListenerAdded2);

	EXPECT_TRUE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_TRUE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);
	refWinData2->simulateKeyPress(GDK_KEY_F2, 68); // This shouldn't be received by any listener because window inactive

	m_refAllEvDM->makeWindowActive(refGtkAccessor2);

	refWinData1->simulateKeyPress(GDK_KEY_F3, 69); // This shouldn't be received by any listener because window inactive
	refWinData2->simulateKeyPress(GDK_KEY_F4, 70);

	m_refAllEvDM->removeAccessor(refGtkAccessor2);

	EXPECT_TRUE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_FALSE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	EXPECT_TRUE(m_aReceivedEvents1.size() == 4); // received both F1 and F4 Press + Cancel to active window
	EXPECT_TRUE(aReceivedEvents2.size() == 2); // only received F4 Press + Cancel for refWin2 when active

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
		auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
		if (nIdx == 0) {
			EXPECT_TRUE(p0KeyEvent->getKey() == stmi::HK_F1);
			EXPECT_TRUE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 1) {
			EXPECT_TRUE(p0KeyEvent->getKey() == stmi::HK_F1);
			EXPECT_TRUE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		} else if (nIdx == 2) {
			EXPECT_TRUE(p0KeyEvent->getKey() == stmi::HK_F4);
			EXPECT_TRUE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 3) {
			EXPECT_TRUE(p0KeyEvent->getKey() == stmi::HK_F4);
			EXPECT_TRUE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		}
	}
	for (auto nIdx = 0u; nIdx < aReceivedEvents2.size(); ++nIdx) {
		auto& refEvent = aReceivedEvents2[nIdx];
		EXPECT_TRUE(refEvent.operator bool());
		EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
		auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
		EXPECT_TRUE(p0KeyEvent->getKey() == stmi::HK_F4);
		if (nIdx == 0) {
			EXPECT_TRUE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
		} else if (nIdx == 1) {
			EXPECT_TRUE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
class MasDMDelayedEventEnablingFixture : public MasDMOneWinOneAccOneListenerFixture
										, public FixtureVariantEventClassesEnable_True
										, public FixtureVariantEventClasses_KeyEvent
{
};
TEST_F(MasDMDelayedEventEnablingFixture, DelayedEventEnabling)
{
	EXPECT_FALSE(m_refAllEvDM->isEventClassEnabled(PointerEvent::getClass()));
	EXPECT_TRUE(m_refAllEvDM->isEventClassEnabled(KeyEvent::getClass()));

	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());
	assert(!refWinData1->isWindowActive());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);
	assert(refWinData1->isWindowActive());

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	m_refAllEvDM->enableEventClass(PointerEvent::getClass());

	EXPECT_TRUE(m_refAllEvDM->isEventClassEnabled(PointerEvent::getClass()));
	EXPECT_TRUE(m_refAllEvDM->isEventClassEnabled(KeyEvent::getClass()));

	refWinData1->simulateMotionNotify(777.77, 888.88, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 2); // PointerEvent is not enabled!
}

} // namespace testing

} // namespace stmi
