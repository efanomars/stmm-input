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
 * File:   testMasGtkPointerDevice.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "fixtureMasDM.h"

#include <stmm-input-ev/stmm-input-ev.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "ButtonPressPress")
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateButtonPress(777.77, 888.88, 1, p0PointerDevice);

	refWinData1->simulateButtonPress(1777.77, 1888.88, 1, p0PointerDevice);

	// Either the second press is ignored or a cancel is sent for the first press
	REQUIRE( ((m_aReceivedEvents1.size() == 1) || (m_aReceivedEvents1.size() == 3)) );

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	REQUIRE(refEvent.operator bool());
	REQUIRE(refEvent->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent = static_cast<stmi::PointerEvent*>(refEvent.get());
	REQUIRE(p0PointerEvent->getType() == stmi::PointerEvent::BUTTON_PRESS);
	REQUIRE(p0PointerEvent->getX() == 777.77);
	REQUIRE(p0PointerEvent->getY() == 888.88);
	REQUIRE(p0PointerEvent->getButton() >= 1);
	REQUIRE(p0PointerEvent->getAccessor() == m_refGtkAccessor1);
}

TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "PointerXYGrabId")
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateButtonPress(777.77, 888.88, 1, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	REQUIRE(refEvent1.operator bool());
	REQUIRE(refEvent1->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent1 = static_cast<stmi::PointerEvent*>(refEvent1.get());
	REQUIRE(p0PointerEvent1->getType() == stmi::PointerEvent::BUTTON_PRESS);
	const int32_t nButton1 = p0PointerEvent1->getButton();
	REQUIRE(nButton1 >= 1);
	REQUIRE(p0PointerEvent1->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0PointerEvent1->getXYGrabType() == XYEvent::XY_GRAB);
	REQUIRE(p0PointerEvent1->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateMotionNotify(777.77, 888.88, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 2);
	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	REQUIRE(refEvent2.operator bool());
	REQUIRE(refEvent2->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent2 = static_cast<PointerEvent*>(refEvent2.get());
	REQUIRE(p0PointerEvent2->getType() == PointerEvent::POINTER_MOVE);
	REQUIRE(p0PointerEvent2->getButton() == PointerEvent::s_nNoButton);
	REQUIRE(p0PointerEvent2->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0PointerEvent2->getXYGrabType() == XYEvent::XY_MOVE);
	REQUIRE(p0PointerEvent2->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateButtonRelease(777.77, 888.88, 1, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 3);

	const shared_ptr<stmi::Event>& refEvent3 = m_aReceivedEvents1[2];
	REQUIRE(refEvent3.operator bool());
	REQUIRE(refEvent3->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent3 = static_cast<stmi::PointerEvent*>(refEvent3.get());
	REQUIRE(p0PointerEvent3->getType() == stmi::PointerEvent::BUTTON_RELEASE);
	REQUIRE(p0PointerEvent3->getButton() == nButton1);
	REQUIRE(p0PointerEvent3->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0PointerEvent3->getXYGrabType() == XYEvent::XY_UNGRAB);
	REQUIRE(p0PointerEvent3->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateMotionNotify(777.77, 888.88, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 4);

	const shared_ptr<stmi::Event>& refEvent4 = m_aReceivedEvents1[3];
	REQUIRE(refEvent4.operator bool());
	REQUIRE(refEvent4->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent4 = static_cast<PointerEvent*>(refEvent4.get());
	REQUIRE(p0PointerEvent4->getType() == PointerEvent::POINTER_HOVER);
	REQUIRE(p0PointerEvent4->getButton() == PointerEvent::s_nNoButton);
	REQUIRE(p0PointerEvent4->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0PointerEvent4->getXYGrabType() == XYEvent::XY_HOVER);
	REQUIRE(p0PointerEvent4->getXYGrabId() == PointerEvent::s_nPointerNotGrabbedId);
}

TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "PointerXYGrabMultiButton")
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateButtonPress(777.77, 888.88, 1, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	REQUIRE(refEvent1.operator bool());
	REQUIRE(refEvent1->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent1 = static_cast<stmi::PointerEvent*>(refEvent1.get());
	REQUIRE(p0PointerEvent1->getType() == stmi::PointerEvent::BUTTON_PRESS);
	const int32_t nButton1 = p0PointerEvent1->getButton();
	REQUIRE(nButton1 >= 1);
	REQUIRE(p0PointerEvent1->getXYGrabType() == XYEvent::XY_GRAB);

	refWinData1->simulateButtonPress(333.33, 444.44, 2, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 2);

	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	REQUIRE(refEvent2.operator bool());
	REQUIRE(refEvent2->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent2 = static_cast<stmi::PointerEvent*>(refEvent2.get());
	REQUIRE(p0PointerEvent2->getType() == stmi::PointerEvent::BUTTON_PRESS);
	const int32_t nButton2 = p0PointerEvent2->getButton();
	REQUIRE(nButton2 >= 1);
	REQUIRE(nButton2 != nButton1);
	REQUIRE(p0PointerEvent2->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0PointerEvent2->getXYGrabType() == XYEvent::XY_MOVE);
	REQUIRE(p0PointerEvent2->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateButtonRelease(111.11, 999.99, 1, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 3);

	const shared_ptr<stmi::Event>& refEvent3 = m_aReceivedEvents1[2];
	REQUIRE(refEvent3.operator bool());
	REQUIRE(refEvent3->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent3 = static_cast<stmi::PointerEvent*>(refEvent3.get());
	REQUIRE(p0PointerEvent3->getType() == stmi::PointerEvent::BUTTON_RELEASE);
	const int32_t nButton3 = p0PointerEvent3->getButton();
	REQUIRE(nButton3 == nButton1);
	REQUIRE(p0PointerEvent3->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0PointerEvent3->getXYGrabType() == XYEvent::XY_MOVE);
	REQUIRE(p0PointerEvent3->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateButtonRelease(111.11, 999.99, 2, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 4);

	const shared_ptr<stmi::Event>& refEvent4 = m_aReceivedEvents1[3];
	REQUIRE(refEvent4.operator bool());
	REQUIRE(refEvent4->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent4 = static_cast<stmi::PointerEvent*>(refEvent4.get());
	REQUIRE(p0PointerEvent4->getType() == stmi::PointerEvent::BUTTON_RELEASE);
	const int32_t nButton4 = p0PointerEvent4->getButton();
	REQUIRE(nButton4 == nButton2);
	REQUIRE(p0PointerEvent4->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0PointerEvent4->getXYGrabType() == XYEvent::XY_UNGRAB);
	REQUIRE(p0PointerEvent4->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
}

TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "PointerScroll")
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateButtonPress(777.77, 888.88, 1, p0PointerDevice);

	refWinData1->simulateScroll(777.77, 888.88, GDK_SCROLL_DOWN, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 2);

	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	REQUIRE(refEvent2.operator bool());
	REQUIRE(refEvent2->getEventClass() == typeid(stmi::PointerScrollEvent));
	auto p0PointerScrollEvent2 = static_cast<stmi::PointerScrollEvent*>(refEvent2.get());
	REQUIRE(p0PointerScrollEvent2->getScrollDir() == PointerScrollEvent::SCROLL_DOWN);
	REQUIRE(p0PointerScrollEvent2->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0PointerScrollEvent2->getXYGrabType() == XYEvent::XY_MOVE);
	REQUIRE(p0PointerScrollEvent2->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateButtonRelease(777.77, 888.88, 1, p0PointerDevice);

	refWinData1->simulateScroll(777.77, 888.88, GDK_SCROLL_DOWN, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 4);

	const shared_ptr<stmi::Event>& refEvent4 = m_aReceivedEvents1[3];
	REQUIRE(refEvent4.operator bool());
	REQUIRE(refEvent4->getEventClass() == typeid(stmi::PointerScrollEvent));
	auto p0PointerScrollEvent4 = static_cast<stmi::PointerScrollEvent*>(refEvent4.get());
	REQUIRE(p0PointerScrollEvent4->getScrollDir() == PointerScrollEvent::SCROLL_DOWN);
	REQUIRE(p0PointerScrollEvent4->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0PointerScrollEvent4->getXYGrabType() == XYEvent::XY_HOVER);
	REQUIRE(p0PointerScrollEvent4->getXYGrabId() == PointerEvent::s_nPointerNotGrabbedId);
}

TEST_CASE_METHOD(STFX<MasDMOneWinOneAccOneListenerFixture>, "TouchSequence")
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	int64_t nDontKnow = 12121212;
	auto p0Seq = reinterpret_cast<GdkEventSequence*>(&nDontKnow);
	refWinData1->simulateTouch(GDK_TOUCH_BEGIN, 777.77, 888.88, p0Seq, p0PointerDevice);
	//GDK_TOUCH_UPDATE GDK_TOUCH_END GDK_TOUCH_CANCEL

	REQUIRE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	REQUIRE(refEvent1.operator bool());
	REQUIRE(refEvent1->getEventClass() == typeid(stmi::TouchEvent));
	auto p0TouchEvent1 = static_cast<stmi::TouchEvent*>(refEvent1.get());
	REQUIRE(p0TouchEvent1->getType() == stmi::TouchEvent::TOUCH_BEGIN);
	REQUIRE(p0TouchEvent1->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0TouchEvent1->getXYGrabType() == XYEvent::XY_GRAB);
	const auto nGrabId1 = p0TouchEvent1->getXYGrabId();

	refWinData1->simulateTouch(GDK_TOUCH_UPDATE, 777.77, 888.88, p0Seq, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 2);
	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	REQUIRE(refEvent2.operator bool());
	REQUIRE(refEvent2->getEventClass() == typeid(stmi::TouchEvent));
	auto p0TouchEventEvent2 = static_cast<TouchEvent*>(refEvent2.get());
	REQUIRE(p0TouchEventEvent2->getType() == TouchEvent::TOUCH_UPDATE);
	REQUIRE(p0TouchEventEvent2->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0TouchEventEvent2->getXYGrabType() == XYEvent::XY_MOVE);
	REQUIRE(p0TouchEventEvent2->getXYGrabId() == nGrabId1);

	refWinData1->simulateTouch(GDK_TOUCH_END, 777.77, 888.88, p0Seq, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 3);

	const shared_ptr<stmi::Event>& refEvent3 = m_aReceivedEvents1[2];
	REQUIRE(refEvent3.operator bool());
	REQUIRE(refEvent3->getEventClass() == typeid(stmi::TouchEvent));
	auto p0TouchEvent3 = static_cast<TouchEvent*>(refEvent3.get());
	REQUIRE(p0TouchEvent3->getType() == TouchEvent::TOUCH_END);
	REQUIRE(p0TouchEvent3->getAccessor() == m_refGtkAccessor1);
	REQUIRE(p0TouchEvent3->getXYGrabType() == XYEvent::XY_UNGRAB);
	REQUIRE(p0TouchEvent3->getXYGrabId() == nGrabId1);

	// Should be ignored
	refWinData1->simulateTouch(GDK_TOUCH_UPDATE, 777.77, 888.88, p0Seq, p0PointerDevice);

	REQUIRE(m_aReceivedEvents1.size() == 3);
}

} // namespace testing

} // namespace stmi
