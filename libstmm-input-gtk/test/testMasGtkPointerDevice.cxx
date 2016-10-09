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
 * File:   testMasGtkPointerDevice.cc
 */

#include "fixtureMasDM.h"

#include <gtest/gtest.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
TEST_F(MasDMOneWinOneAccOneListenerFixture, ButtonPressPress)
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateButtonPress(777.77, 888.88, 1, p0PointerDevice);

	refWinData1->simulateButtonPress(1777.77, 1888.88, 1, p0PointerDevice);

	// Either the second press is ignored or a cancel is sent for the first press
	EXPECT_TRUE((m_aReceivedEvents1.size() == 1) || (m_aReceivedEvents1.size() == 3));

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent.operator bool());
	EXPECT_TRUE(refEvent->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent = static_cast<stmi::PointerEvent*>(refEvent.get());
	EXPECT_TRUE(p0PointerEvent->getType() == stmi::PointerEvent::BUTTON_PRESS);
	EXPECT_TRUE(p0PointerEvent->getX() == 777.77);
	EXPECT_TRUE(p0PointerEvent->getY() == 888.88);
	EXPECT_TRUE(p0PointerEvent->getButton() >= 1);
	EXPECT_TRUE(p0PointerEvent->getAccessor() == m_refGtkAccessor1);
}

TEST_F(MasDMOneWinOneAccOneListenerFixture, PointerXYGrabId)
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateButtonPress(777.77, 888.88, 1, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent1.operator bool());
	EXPECT_TRUE(refEvent1->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent1 = static_cast<stmi::PointerEvent*>(refEvent1.get());
	EXPECT_TRUE(p0PointerEvent1->getType() == stmi::PointerEvent::BUTTON_PRESS);
	const int32_t nButton1 = p0PointerEvent1->getButton();
	EXPECT_TRUE(nButton1 >= 1);
	EXPECT_TRUE(p0PointerEvent1->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0PointerEvent1->getXYGrabType() == XYEvent::XY_GRAB);
	EXPECT_TRUE(p0PointerEvent1->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateMotionNotify(777.77, 888.88, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);
	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	EXPECT_TRUE(refEvent2.operator bool());
	EXPECT_TRUE(refEvent2->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent2 = static_cast<PointerEvent*>(refEvent2.get());
	EXPECT_TRUE(p0PointerEvent2->getType() == PointerEvent::POINTER_MOVE);
	EXPECT_TRUE(p0PointerEvent2->getButton() == PointerEvent::s_nNoButton);
	EXPECT_TRUE(p0PointerEvent2->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0PointerEvent2->getXYGrabType() == XYEvent::XY_MOVE);
	EXPECT_TRUE(p0PointerEvent2->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateButtonRelease(777.77, 888.88, 1, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 3);

	const shared_ptr<stmi::Event>& refEvent3 = m_aReceivedEvents1[2];
	EXPECT_TRUE(refEvent3.operator bool());
	EXPECT_TRUE(refEvent3->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent3 = static_cast<stmi::PointerEvent*>(refEvent3.get());
	EXPECT_TRUE(p0PointerEvent3->getType() == stmi::PointerEvent::BUTTON_RELEASE);
	EXPECT_TRUE(p0PointerEvent3->getButton() == nButton1);
	EXPECT_TRUE(p0PointerEvent3->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0PointerEvent3->getXYGrabType() == XYEvent::XY_UNGRAB);
	EXPECT_TRUE(p0PointerEvent3->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateMotionNotify(777.77, 888.88, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 4);

	const shared_ptr<stmi::Event>& refEvent4 = m_aReceivedEvents1[3];
	EXPECT_TRUE(refEvent4.operator bool());
	EXPECT_TRUE(refEvent4->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent4 = static_cast<PointerEvent*>(refEvent4.get());
	EXPECT_TRUE(p0PointerEvent4->getType() == PointerEvent::POINTER_HOVER);
	EXPECT_TRUE(p0PointerEvent4->getButton() == PointerEvent::s_nNoButton);
	EXPECT_TRUE(p0PointerEvent4->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0PointerEvent4->getXYGrabType() == XYEvent::XY_HOVER);
	EXPECT_TRUE(p0PointerEvent4->getXYGrabId() == PointerEvent::s_nPointerNotGrabbedId);
}

TEST_F(MasDMOneWinOneAccOneListenerFixture, PointerXYGrabMultiButton)
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateButtonPress(777.77, 888.88, 1, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent1.operator bool());
	EXPECT_TRUE(refEvent1->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent1 = static_cast<stmi::PointerEvent*>(refEvent1.get());
	EXPECT_TRUE(p0PointerEvent1->getType() == stmi::PointerEvent::BUTTON_PRESS);
	const int32_t nButton1 = p0PointerEvent1->getButton();
	EXPECT_TRUE(nButton1 >= 1);
	EXPECT_TRUE(p0PointerEvent1->getXYGrabType() == XYEvent::XY_GRAB);

	refWinData1->simulateButtonPress(333.33, 444.44, 2, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);

	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	EXPECT_TRUE(refEvent2.operator bool());
	EXPECT_TRUE(refEvent2->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent2 = static_cast<stmi::PointerEvent*>(refEvent2.get());
	EXPECT_TRUE(p0PointerEvent2->getType() == stmi::PointerEvent::BUTTON_PRESS);
	const int32_t nButton2 = p0PointerEvent2->getButton();
	EXPECT_TRUE(nButton2 >= 1);
	EXPECT_TRUE(nButton2 != nButton1);
	EXPECT_TRUE(p0PointerEvent2->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0PointerEvent2->getXYGrabType() == XYEvent::XY_MOVE);
	EXPECT_TRUE(p0PointerEvent2->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateButtonRelease(111.11, 999.99, 1, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 3);

	const shared_ptr<stmi::Event>& refEvent3 = m_aReceivedEvents1[2];
	EXPECT_TRUE(refEvent3.operator bool());
	EXPECT_TRUE(refEvent3->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent3 = static_cast<stmi::PointerEvent*>(refEvent3.get());
	EXPECT_TRUE(p0PointerEvent3->getType() == stmi::PointerEvent::BUTTON_RELEASE);
	const int32_t nButton3 = p0PointerEvent3->getButton();
	EXPECT_TRUE(nButton3 == nButton1);
	EXPECT_TRUE(p0PointerEvent3->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0PointerEvent3->getXYGrabType() == XYEvent::XY_MOVE);
	EXPECT_TRUE(p0PointerEvent3->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateButtonRelease(111.11, 999.99, 2, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 4);

	const shared_ptr<stmi::Event>& refEvent4 = m_aReceivedEvents1[3];
	EXPECT_TRUE(refEvent4.operator bool());
	EXPECT_TRUE(refEvent4->getEventClass() == typeid(stmi::PointerEvent));
	auto p0PointerEvent4 = static_cast<stmi::PointerEvent*>(refEvent4.get());
	EXPECT_TRUE(p0PointerEvent4->getType() == stmi::PointerEvent::BUTTON_RELEASE);
	const int32_t nButton4 = p0PointerEvent4->getButton();
	EXPECT_TRUE(nButton4 == nButton2);
	EXPECT_TRUE(p0PointerEvent4->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0PointerEvent4->getXYGrabType() == XYEvent::XY_UNGRAB);
	EXPECT_TRUE(p0PointerEvent4->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);
}

TEST_F(MasDMOneWinOneAccOneListenerFixture, PointerScroll)
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateButtonPress(777.77, 888.88, 1, p0PointerDevice);

	refWinData1->simulateScroll(777.77, 888.88, GDK_SCROLL_DOWN, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);

	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	EXPECT_TRUE(refEvent2.operator bool());
	EXPECT_TRUE(refEvent2->getEventClass() == typeid(stmi::PointerScrollEvent));
	auto p0PointerScrollEvent2 = static_cast<stmi::PointerScrollEvent*>(refEvent2.get());
	EXPECT_TRUE(p0PointerScrollEvent2->getScrollDir() == PointerScrollEvent::SCROLL_DOWN);
	EXPECT_TRUE(p0PointerScrollEvent2->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0PointerScrollEvent2->getXYGrabType() == XYEvent::XY_MOVE);
	EXPECT_TRUE(p0PointerScrollEvent2->getXYGrabId() == PointerEvent::s_nPointerGrabbedId);

	refWinData1->simulateButtonRelease(777.77, 888.88, 1, p0PointerDevice);

	refWinData1->simulateScroll(777.77, 888.88, GDK_SCROLL_DOWN, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 4);

	const shared_ptr<stmi::Event>& refEvent4 = m_aReceivedEvents1[3];
	EXPECT_TRUE(refEvent4.operator bool());
	EXPECT_TRUE(refEvent4->getEventClass() == typeid(stmi::PointerScrollEvent));
	auto p0PointerScrollEvent4 = static_cast<stmi::PointerScrollEvent*>(refEvent4.get());
	EXPECT_TRUE(p0PointerScrollEvent4->getScrollDir() == PointerScrollEvent::SCROLL_DOWN);
	EXPECT_TRUE(p0PointerScrollEvent4->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0PointerScrollEvent4->getXYGrabType() == XYEvent::XY_HOVER);
	EXPECT_TRUE(p0PointerScrollEvent4->getXYGrabId() == PointerEvent::s_nPointerNotGrabbedId);
}

TEST_F(MasDMOneWinOneAccOneListenerFixture, TouchSequence)
{
	auto refFakeBackend = m_refAllEvDM->getBackend();
	auto p0PointerDevice = refFakeBackend->getPointerBackendDevice();
	assert(p0PointerDevice != nullptr);

	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	EXPECT_TRUE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	int64_t nDontKnow = 12121212;
	auto p0Seq = reinterpret_cast<GdkEventSequence*>(&nDontKnow);
	refWinData1->simulateTouch(GDK_TOUCH_BEGIN, 777.77, 888.88, p0Seq, p0PointerDevice);
	//GDK_TOUCH_UPDATE GDK_TOUCH_END GDK_TOUCH_CANCEL

	EXPECT_TRUE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	EXPECT_TRUE(refEvent1.operator bool());
	EXPECT_TRUE(refEvent1->getEventClass() == typeid(stmi::TouchEvent));
	auto p0TouchEvent1 = static_cast<stmi::TouchEvent*>(refEvent1.get());
	EXPECT_TRUE(p0TouchEvent1->getType() == stmi::TouchEvent::TOUCH_BEGIN);
	EXPECT_TRUE(p0TouchEvent1->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0TouchEvent1->getXYGrabType() == XYEvent::XY_GRAB);
	const auto nGrabId1 = p0TouchEvent1->getXYGrabId();

	refWinData1->simulateTouch(GDK_TOUCH_UPDATE, 777.77, 888.88, p0Seq, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 2);
	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	EXPECT_TRUE(refEvent2.operator bool());
	EXPECT_TRUE(refEvent2->getEventClass() == typeid(stmi::TouchEvent));
	auto p0TouchEventEvent2 = static_cast<TouchEvent*>(refEvent2.get());
	EXPECT_TRUE(p0TouchEventEvent2->getType() == TouchEvent::TOUCH_UPDATE);
	EXPECT_TRUE(p0TouchEventEvent2->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0TouchEventEvent2->getXYGrabType() == XYEvent::XY_MOVE);
	EXPECT_TRUE(p0TouchEventEvent2->getXYGrabId() == nGrabId1);

	refWinData1->simulateTouch(GDK_TOUCH_END, 777.77, 888.88, p0Seq, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 3);

	const shared_ptr<stmi::Event>& refEvent3 = m_aReceivedEvents1[2];
	EXPECT_TRUE(refEvent3.operator bool());
	EXPECT_TRUE(refEvent3->getEventClass() == typeid(stmi::TouchEvent));
	auto p0TouchEvent3 = static_cast<TouchEvent*>(refEvent3.get());
	EXPECT_TRUE(p0TouchEvent3->getType() == TouchEvent::TOUCH_END);
	EXPECT_TRUE(p0TouchEvent3->getAccessor() == m_refGtkAccessor1);
	EXPECT_TRUE(p0TouchEvent3->getXYGrabType() == XYEvent::XY_UNGRAB);
	EXPECT_TRUE(p0TouchEvent3->getXYGrabId() == nGrabId1);

	// Should be ignored
	refWinData1->simulateTouch(GDK_TOUCH_UPDATE, 777.77, 888.88, p0Seq, p0PointerDevice);

	EXPECT_TRUE(m_aReceivedEvents1.size() == 3);
}

} // namespace testing

} // namespace stmi
