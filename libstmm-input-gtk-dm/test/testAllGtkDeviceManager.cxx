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
 * File:   testAllGtkDeviceManager.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "fixtureAllDM.h"

#include <stmm-input/callifs.h>


namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<GlibAppFixture>, "CreateFakeAllGtkDeviceManager")
{
	auto refFakeMasDM = FakeMasGtkDeviceManager::create(false, {}, KeyRepeat::MODE_SUPPRESS
												, shared_ptr<GdkKeyConverter>{}, Glib::RefPtr<Gdk::DeviceManager>{});
	auto refFakeJsDM = FakeJsGtkDeviceManager::create(false, {});
	//
	auto refAllEvDM = TheParentDeviceManager::create(std::vector<shared_ptr<ChildDeviceManager>>{refFakeMasDM, refFakeJsDM});
	REQUIRE(refAllEvDM.operator bool());
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<AllEvAllDMFixture>, "AddAccessor")
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

TEST_CASE_METHOD(STFX<AllEvAllDMFixture>, "AddListener")
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

TEST_CASE_METHOD(STFX<AllEvAllDMOneWinOneAccOneDevsListenerFixture>, "TwoAccessorKeyCancel")
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

	auto refFakeMasFactory = m_refMasAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refMasWinData1 = refFakeMasFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refMasWinData1.operator bool());
	const std::shared_ptr<Mas::FakeGtkWindowData>& refMasWinData2 = refFakeMasFactory->getFakeWindowData(refWin2.operator->());
	REQUIRE(refMasWinData2.operator bool());
	auto refFakeJsFactory = m_refJsAllEvDM->getFactory();
	const std::shared_ptr<Js::FakeGtkWindowData>& refJsWinData1 = refFakeJsFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refJsWinData1.operator bool());
	const std::shared_ptr<Js::FakeGtkWindowData>& refJsWinData2 = refFakeJsFactory->getFakeWindowData(refWin2.operator->());
	REQUIRE(refJsWinData2.operator bool());

	REQUIRE(m_refMasAllEvDM->hasAccessor(m_refGtkAccessor1));
	REQUIRE(m_refMasAllEvDM->hasAccessor(refGtkAccessor2));
	REQUIRE(m_refJsAllEvDM->hasAccessor(m_refGtkAccessor1));
	REQUIRE(m_refJsAllEvDM->hasAccessor(refGtkAccessor2));

	m_refMasAllEvDM->makeWindowActive(m_refGtkAccessor1);
	m_refJsAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refMasWinData1->simulateButtonPress(55.1, 55.2, 1, m_p0FakeMasBackend->getPointerBackendDevice());
	js_event oEv;
	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 3; 
	oEv.value = 1;
	m_p0FakeJsBackend->simulateJsEvent(m_nJsDeviceId, &oEv); // BTN_Y press

	m_refMasAllEvDM->makeWindowActive(refGtkAccessor2);
	m_refJsAllEvDM->makeWindowActive(refGtkAccessor2);

	refMasWinData2->simulateButtonPress(55.1, 55.2, 2, m_p0FakeMasBackend->getPointerBackendDevice());
	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 2; 
	oEv.value = 1;
	m_p0FakeJsBackend->simulateJsEvent(m_nJsDeviceId, &oEv); // BTN_X press

	m_refAllEvDM->removeAccessor(refGtkAccessor2);

	REQUIRE(m_refAllEvDM->hasAccessor(m_refGtkAccessor1));
	REQUIRE_FALSE(m_refAllEvDM->hasAccessor(refGtkAccessor2));

	// received 
	// - F1 and F2 Press + Cancel to active window
	// - pointer button 1 and 2 Press + Cancel to active window
	// - joystick button BTN_Y and BTN_X Press + Cancel to active window
	REQUIRE(m_aReceivedEvents1.size() == 8);

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		HARDWARE_KEY eKey;
		Event::AS_KEY_INPUT_TYPE eType;
		bool bMoreThanOne;
		const bool bSimu = refEvent->getAsKey(eKey, eType, bMoreThanOne);
		REQUIRE(bSimu);
		REQUIRE_FALSE(bMoreThanOne);
		if (nIdx < 2) {
			//REQUIRE((eKey == stmi::HK_F1) || () || ());
			REQUIRE(eType == stmi::Event::AS_KEY_PRESS);
		} else if ((nIdx >= 2) && (nIdx < 4)) {
			REQUIRE(eType == stmi::Event::AS_KEY_RELEASE_CANCEL);
		} else if ((nIdx >= 4) && (nIdx < 6)) {
			REQUIRE(eType == stmi::Event::AS_KEY_PRESS);
		} else if (nIdx >= 6) {
			REQUIRE(eType == stmi::Event::AS_KEY_RELEASE_CANCEL);
		}
	}
}

TEST_CASE_METHOD(STFX<AllEvAllDMOneWinOneAccOneDevsListenerFixture>, "ListenerAndNestedAccessorRemoval")
{
	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
				REQUIRE(refEvent.operator bool());
				REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
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

	auto refFakeMasFactory = m_refMasAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refMasWinData1 = refFakeMasFactory->getFakeWindowData(m_refWin1.operator->());
	// auto refFakeJsFactory = m_refJsAllEvDM->getFactory();
	// const std::shared_ptr<Js::FakeGtkWindowData>& refJsWinData1 = refFakeJsFactory->getFakeWindowData(m_refWin1.operator->());

	m_refMasAllEvDM->makeWindowActive(m_refGtkAccessor1);
	m_refJsAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refMasWinData1->simulateButtonPress(55.1, 55.2, 1, m_p0FakeMasBackend->getPointerBackendDevice());
	refMasWinData1->simulateKeyPress(GDK_KEY_F2, 68);
	js_event oEv;
	oEv.type = JS_EVENT_BUTTON;
	oEv.number = 3; 
	oEv.value = 1;
	m_p0FakeJsBackend->simulateJsEvent(m_nJsDeviceId, &oEv); // BTN_Y press

	m_refAllEvDM->removeEventListener(m_refListener1, true);

	REQUIRE(m_aReceivedEvents1.size() == 6);

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		HARDWARE_KEY eKey;
		Event::AS_KEY_INPUT_TYPE eType;
		bool bMoreThanOne;
		const bool bSimu = refEvent->getAsKey(eKey, eType, bMoreThanOne);
		REQUIRE(bSimu);
		REQUIRE_FALSE(bMoreThanOne);
		if (nIdx < 3) {
			//REQUIRE((eKey == stmi::HK_F1) || () || ());
			REQUIRE(eType == stmi::Event::AS_KEY_PRESS);
		} else if (nIdx >= 3) {
			REQUIRE(eType == stmi::Event::AS_KEY_RELEASE_CANCEL);
		}
	}
}

TEST_CASE_METHOD(STFX<AllEvAllDMOneWinOneAccOneDevsListenerFixture>, "AccessorAndNestedListenerRemoval")
{
	std::vector<shared_ptr<stmi::Event> > aReceivedEvents2;
	auto refListener2 = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceivedEvents2.emplace_back(refEvent);
				REQUIRE(refEvent.operator bool());
				REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
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

	auto refFakeMasFactory = m_refMasAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refMasWinData1 = refFakeMasFactory->getFakeWindowData(m_refWin1.operator->());
	// auto refFakeJsFactory = m_refJsAllEvDM->getFactory();
	// const std::shared_ptr<Js::FakeGtkWindowData>& refJsWinData1 = refFakeJsFactory->getFakeWindowData(m_refWin1.operator->());

	m_refMasAllEvDM->makeWindowActive(m_refGtkAccessor1);
	m_refJsAllEvDM->makeWindowActive(m_refGtkAccessor1);

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

	REQUIRE(m_aReceivedEvents1.size() == 6);

	for (auto nIdx = 0u; nIdx < m_aReceivedEvents1.size(); ++nIdx) {
		auto& refEvent = m_aReceivedEvents1[nIdx];
		HARDWARE_KEY eKey;
		Event::AS_KEY_INPUT_TYPE eType;
		bool bMoreThanOne;
		const bool bSimu = refEvent->getAsKey(eKey, eType, bMoreThanOne);
		REQUIRE(bSimu);
		REQUIRE_FALSE(bMoreThanOne);
		if (nIdx < 3) {
			//REQUIRE((eKey == stmi::HK_F1) || () || ());
			REQUIRE(eType == stmi::Event::AS_KEY_PRESS);
		} else if (nIdx >= 3) {
			REQUIRE(eType == stmi::Event::AS_KEY_RELEASE_CANCEL);
		}
	}
}

} // namespace testing

} // namespace stmi
