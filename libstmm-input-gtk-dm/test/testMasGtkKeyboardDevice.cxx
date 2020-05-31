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
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/* 
 * File:   testMasGtkKeyboardDevice.cc
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "fixtureMasDM.h"

#include <gdk/gdkkeysyms.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
class MasGtkDMPressPressSuppressedFixture : public MasDMOneWinOneAccOneListenerFixture
{
};

TEST_CASE_METHOD(STFX<MasGtkDMPressPressSuppressedFixture>, "PressPress")
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1); //refWinData1->simulateWindowSetActive(true);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	// m_refAllEvDM is created with mode KeyRepeat::MODE_SUPPRESS
	REQUIRE(m_aReceivedEvents1.size() == 1);

	const shared_ptr<stmi::Event>& refEvent = m_aReceivedEvents1[0];
	REQUIRE(refEvent.operator bool());
	REQUIRE(refEvent->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
	REQUIRE(p0KeyEvent->getType() == stmi::KeyEvent::KEY_PRESS);
	REQUIRE(p0KeyEvent->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent->getAccessor() == m_refGtkAccessor1);
}

////////////////////////////////////////////////////////////////////////////////
class MasGtkDMPressPressAddReleaseFixture : public MasDMOneWinOneAccOneListenerFixture
											, public FixtureVariantKeyRepeatMode_AddRelease
{
};
TEST_CASE_METHOD(STFX<MasGtkDMPressPressAddReleaseFixture>, "PressPressAddRelease")
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	REQUIRE(m_aReceivedEvents1.size() == 3);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	REQUIRE(refEvent1.operator bool());
	REQUIRE(refEvent1->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent1 = static_cast<stmi::KeyEvent*>(refEvent1.get());
	REQUIRE(p0KeyEvent1->getType() == stmi::KeyEvent::KEY_PRESS);
	REQUIRE(p0KeyEvent1->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent1->getAccessor() == m_refGtkAccessor1);

	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	REQUIRE(refEvent2.operator bool());
	REQUIRE(refEvent2->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent2 = static_cast<stmi::KeyEvent*>(refEvent2.get());
	REQUIRE(p0KeyEvent2->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent2->getType() == stmi::KeyEvent::KEY_RELEASE);

	const shared_ptr<stmi::Event>& refEvent3 = m_aReceivedEvents1[2];
	REQUIRE(refEvent3.operator bool());
	REQUIRE(refEvent3->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent3 = static_cast<stmi::KeyEvent*>(refEvent3.get());
	REQUIRE(p0KeyEvent3->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent3->getType() == stmi::KeyEvent::KEY_PRESS);
}

////////////////////////////////////////////////////////////////////////////////
class MasGtkDMPressPressAddCancelFixture : public MasDMOneWinOneAccOneListenerFixture
											, public FixtureVariantKeyRepeatMode_AddReleaseCancel
{
};
TEST_CASE_METHOD(STFX<MasGtkDMPressPressAddCancelFixture>, "PressPressAddReleaseCancel")
{
	auto refFakeFactory = m_refAllEvDM->getFactory();
	const std::shared_ptr<Mas::FakeGtkWindowData>& refWinData1 = refFakeFactory->getFakeWindowData(m_refWin1.operator->());
	REQUIRE(refWinData1.operator bool());

	m_refAllEvDM->makeWindowActive(m_refGtkAccessor1);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	refWinData1->simulateKeyPress(GDK_KEY_F1, 67);

	REQUIRE(m_aReceivedEvents1.size() == 3);

	const shared_ptr<stmi::Event>& refEvent1 = m_aReceivedEvents1[0];
	REQUIRE(refEvent1.operator bool());
	REQUIRE(refEvent1->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent1 = static_cast<stmi::KeyEvent*>(refEvent1.get());
	REQUIRE(p0KeyEvent1->getType() == stmi::KeyEvent::KEY_PRESS);
	REQUIRE(p0KeyEvent1->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent1->getAccessor() == m_refGtkAccessor1);

	const shared_ptr<stmi::Event>& refEvent2 = m_aReceivedEvents1[1];
	REQUIRE(refEvent2.operator bool());
	REQUIRE(refEvent2->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent2 = static_cast<stmi::KeyEvent*>(refEvent2.get());
	REQUIRE(p0KeyEvent2->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent2->getType() == stmi::KeyEvent::KEY_RELEASE_CANCEL);

	const shared_ptr<stmi::Event>& refEvent3 = m_aReceivedEvents1[2];
	REQUIRE(refEvent3.operator bool());
	REQUIRE(refEvent3->getEventClass() == typeid(stmi::KeyEvent));
	auto p0KeyEvent3 = static_cast<stmi::KeyEvent*>(refEvent3.get());
	REQUIRE(p0KeyEvent3->getKey() == stmi::HK_F1);
	REQUIRE(p0KeyEvent3->getType() == stmi::KeyEvent::KEY_PRESS);
}

} // namespace testing

} // namespace stmi
