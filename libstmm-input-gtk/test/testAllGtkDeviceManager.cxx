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
 * File:   testAllGtkDeviceManager.cc
 */

#include "fixtureAllDM.h"

#include <stmm-input/callifs.h>

#include <gtest/gtest.h>


namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
TEST_F(GlibAppFixture, CreateFakeAllGtkDeviceManager)
{
	auto refFakeFloDM = FakeFloGtkDeviceManager::create(false, {}, KEY_REPEAT_MODE_SUPPRESS
												, shared_ptr<GdkKeyConverter>{}, Glib::RefPtr<Gdk::Display>{});
	auto refFakeMasDM = FakeMasGtkDeviceManager::create(false, {}, KEY_REPEAT_MODE_SUPPRESS
												, shared_ptr<GdkKeyConverter>{}, Glib::RefPtr<Gdk::DeviceManager>{});
	auto refFakeJsDM = FakeJsGtkDeviceManager::create(false, {});
	//
	auto refAllEvDM = StdParentDeviceManager::create({refFakeMasDM, refFakeFloDM, refFakeJsDM});
	EXPECT_TRUE(refAllEvDM.operator bool());
}

////////////////////////////////////////////////////////////////////////////////
TEST_F(AllEvAllDMFixture, AddAccessor)
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

TEST_F(AllEvAllDMFixture, AddListener)
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

TEST_F(AllEvAllDMOneWinOneAccOneDevsListenerFixture, TwoAccessorKeyCancel)
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

	auto refFakeFloFactory = m_refFloAllEvDM->getFactory();
	const std::shared_ptr<Flo::FakeGtkWindowData>& refFloWinData1 = refFakeFloFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refFloWinData1.operator bool());
	const std::shared_ptr<Flo::FakeGtkWindowData>& refFloWinData2 = refFakeFloFactory->getFakeWindowData(refWin2.operator->());
	EXPECT_TRUE(refFloWinData2.operator bool());
	auto refFakeMasFactory = m_refMasAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refMasWinData1 = refFakeMasFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refFloWinData1.operator bool());
	const std::shared_ptr<Mas::FakeGtkWindowData>& refMasWinData2 = refFakeMasFactory->getFakeWindowData(refWin2.operator->());
	EXPECT_TRUE(refFloWinData2.operator bool());
	auto refFakeJsFactory = m_refJsAllEvDM->getFactory();
	const std::shared_ptr<Js::FakeGtkWindowData>& refJsWinData1 = refFakeJsFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refJsWinData1.operator bool());
	const std::shared_ptr<Js::FakeGtkWindowData>& refJsWinData2 = refFakeJsFactory->getFakeWindowData(refWin2.operator->());
	EXPECT_TRUE(refJsWinData2.operator bool());

	EXPECT_TRUE(m_refFloAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_TRUE(m_refFloAllEvDM->hasAccessor(refGtkAccessor2));
	EXPECT_TRUE(m_refMasAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_TRUE(m_refMasAllEvDM->hasAccessor(refGtkAccessor2));
	EXPECT_TRUE(m_refJsAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_TRUE(m_refJsAllEvDM->hasAccessor(refGtkAccessor2));

	m_refFloAllEvDM->makeWindowActive(m_refGtkAccessor1);
	m_refMasAllEvDM->makeWindowActive(m_refGtkAccessor1);
	m_refJsAllEvDM->makeWindowActive(m_refGtkAccessor1);

	m_p0FakeFloBackend->simulateKeyEvent(m_nXDeviceId, refFloWinData1->getXWindow(), true, 67); // press F1 evdev key
	refMasWinData1->simulateButtonPress(55.1, 55.2, 1, m_p0FakeMasBackend->getPointerBackendDevice());
	js_event oEv;
	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 3; 
	oEv.value = 1;
	m_p0FakeJsBackend->simulateJsEvent(m_nJsDeviceId, &oEv); // BTN_Y press

	m_refFloAllEvDM->makeWindowActive(refGtkAccessor2);
	m_refMasAllEvDM->makeWindowActive(refGtkAccessor2);
	m_refJsAllEvDM->makeWindowActive(refGtkAccessor2);

	m_p0FakeFloBackend->simulateKeyEvent(m_nXDeviceId, refFloWinData2->getXWindow(), true, 68); // press F1 evdev key
	refMasWinData2->simulateButtonPress(55.1, 55.2, 2, m_p0FakeMasBackend->getPointerBackendDevice());
	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 2; 
	oEv.value = 1;
	m_p0FakeJsBackend->simulateJsEvent(m_nJsDeviceId, &oEv); // BTN_X press

	m_refAllEvDM->removeAccessor(refGtkAccessor2);

	EXPECT_TRUE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	EXPECT_FALSE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	// received 
	// - F1 and F2 Press + Cancel to active window
	// - pointer button 1 and 2 Press + Cancel to active window
	// - joystick button BTN_Y and BTN_X Press + Cancel to active window
	EXPECT_TRUE(m_aReceivedEvents1.size() == 12);

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		HARDWARE_KEY eKey;
		Event::AS_KEY_INPUT_TYPE eType;
		bool bMoreThanOne;
		const bool bSimu = refEvent->getAsKey(eKey, eType, bMoreThanOne);
		EXPECT_TRUE(bSimu);
		EXPECT_FALSE(bMoreThanOne);
		if (nIdx < 3) {
			//EXPECT_TRUE((eKey == stmi::HK_F1) || () || ());
			EXPECT_TRUE(eType == stmi::Event::AS_KEY_PRESS);
		} else if ((nIdx >= 3) && (nIdx < 6)) {
			EXPECT_TRUE(eType == stmi::Event::AS_KEY_RELEASE_CANCEL);
		} else if ((nIdx >= 6) && (nIdx < 9)) {
			EXPECT_TRUE(eType == stmi::Event::AS_KEY_PRESS);
		} else if (nIdx >= 9) {
			EXPECT_TRUE(eType == stmi::Event::AS_KEY_RELEASE_CANCEL);
		}
	}
}

TEST_F(AllEvAllDMOneWinOneAccOneDevsListenerFixture, ListenerAndNestedAccessorRemoval)
{
	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
				EXPECT_TRUE(refEvent.operator bool());
				EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
				auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
				if ((p0KeyEvent->getType() == KeyEvent::KEY_RELEASE_CANCEL) && (p0KeyEvent->getKey() == stmi::HK_F1)) {
					//
					#ifndef NDEBUG
					const bool bRemoved =
					#endif
					m_refAllEvDM->removeAccessor(m_refGtkAccessor1);
					assert(bRemoved);
				}
			});

	auto refFakeFloFactory = m_refFloAllEvDM->getFactory();
	const std::shared_ptr<Flo::FakeGtkWindowData>& refFloWinData1 = refFakeFloFactory->getFakeWindowData(m_refWin1.operator->());
	auto refFakeMasFactory = m_refMasAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refMasWinData1 = refFakeMasFactory->getFakeWindowData(m_refWin1.operator->());
	// auto refFakeJsFactory = m_refJsAllEvDM->getFactory();
	// const std::shared_ptr<Js::FakeGtkWindowData>& refJsWinData1 = refFakeJsFactory->getFakeWindowData(m_refWin1.operator->());

	m_refFloAllEvDM->makeWindowActive(m_refGtkAccessor1);
	m_refMasAllEvDM->makeWindowActive(m_refGtkAccessor1);
	m_refJsAllEvDM->makeWindowActive(m_refGtkAccessor1);

	m_p0FakeFloBackend->simulateKeyEvent(m_nXDeviceId, refFloWinData1->getXWindow(), true, 67); // press F1 evdev key
	refMasWinData1->simulateButtonPress(55.1, 55.2, 1, m_p0FakeMasBackend->getPointerBackendDevice());
	refMasWinData1->simulateKeyPress(GDK_KEY_F2, 68);
	js_event oEv;
	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 3; 
	oEv.value = 1;
	m_p0FakeJsBackend->simulateJsEvent(m_nJsDeviceId, &oEv); // BTN_Y press

	m_refAllEvDM->removeEventListener(m_refListener1, true);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 8);

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		HARDWARE_KEY eKey;
		Event::AS_KEY_INPUT_TYPE eType;
		bool bMoreThanOne;
		const bool bSimu = refEvent->getAsKey(eKey, eType, bMoreThanOne);
		EXPECT_TRUE(bSimu);
		EXPECT_FALSE(bMoreThanOne);
		if (nIdx < 4) {
			//EXPECT_TRUE((eKey == stmi::HK_F1) || () || ());
			EXPECT_TRUE(eType == stmi::Event::AS_KEY_PRESS);
		} else if (nIdx >= 4) {
			EXPECT_TRUE(eType == stmi::Event::AS_KEY_RELEASE_CANCEL);
		}
	}
}

TEST_F(AllEvAllDMOneWinOneAccOneDevsListenerFixture, AccessorAndNestedListenerRemoval)
{
	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
				EXPECT_TRUE(refEvent.operator bool());
				EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
				auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
				if ((p0KeyEvent->getType() == KeyEvent::KEY_RELEASE_CANCEL) && (p0KeyEvent->getKey() == stmi::HK_F1)) {
					//
					#ifndef NDEBUG
					const bool bRemoved =
					#endif
					m_refAllEvDM->removeEventListener(m_refListener1, true);
					assert(bRemoved);
				}
			});

	auto refFakeFloFactory = m_refFloAllEvDM->getFactory();
	const std::shared_ptr<Flo::FakeGtkWindowData>& refFloWinData1 = refFakeFloFactory->getFakeWindowData(m_refWin1.operator->());
	auto refFakeMasFactory = m_refMasAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refMasWinData1 = refFakeMasFactory->getFakeWindowData(m_refWin1.operator->());
	// auto refFakeJsFactory = m_refJsAllEvDM->getFactory();
	// const std::shared_ptr<Js::FakeGtkWindowData>& refJsWinData1 = refFakeJsFactory->getFakeWindowData(m_refWin1.operator->());

	m_refFloAllEvDM->makeWindowActive(m_refGtkAccessor1);
	m_refMasAllEvDM->makeWindowActive(m_refGtkAccessor1);
	m_refJsAllEvDM->makeWindowActive(m_refGtkAccessor1);

	m_p0FakeFloBackend->simulateKeyEvent(m_nXDeviceId, refFloWinData1->getXWindow(), true, 67); // press F1 evdev key
	refMasWinData1->simulateButtonPress(55.1, 55.2, 1, m_p0FakeMasBackend->getPointerBackendDevice());
	refMasWinData1->simulateKeyPress(GDK_KEY_F2, 68);
	js_event oEv;
	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 3; 
	oEv.value = 1;
	m_p0FakeJsBackend->simulateJsEvent(m_nJsDeviceId, &oEv); // BTN_Y press

	#ifndef NDEBUG
	const bool bRemoved =
	#endif
	m_refAllEvDM->removeAccessor(m_refGtkAccessor1);
	assert(bRemoved);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 8);

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		HARDWARE_KEY eKey;
		Event::AS_KEY_INPUT_TYPE eType;
		bool bMoreThanOne;
		const bool bSimu = refEvent->getAsKey(eKey, eType, bMoreThanOne);
		EXPECT_TRUE(bSimu);
		EXPECT_FALSE(bMoreThanOne);
		if (nIdx < 4) {
			//EXPECT_TRUE((eKey == stmi::HK_F1) || () || ());
			EXPECT_TRUE(eType == stmi::Event::AS_KEY_PRESS);
		} else if (nIdx >= 4) {
			EXPECT_TRUE(eType == stmi::Event::AS_KEY_RELEASE_CANCEL);
		}
	}
}

} // namespace testing

} // namespace stmi
