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
 * File:   testEventClass.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "devicemgmtevent.h"
#include "joystickevent.h"
#include "keyevent.h"
#include "pointerevent.h"
#include "touchevent.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

//TEST(testEventClass, StaticBadEventClassId)
TEST_CASE("testEventClass, StaticBadEventClassId")
{
	REQUIRE_FALSE(Event::getEventClassIdClass("SOmeRanDOMNameTHatIsntAClassId"));
}

TEST_CASE("testEventClass, StaticStdEventsExist")
{
	REQUIRE(Event::getEventClassIdClass(DeviceMgmtEvent::s_sClassId));
	REQUIRE(Event::getEventClassIdClass(JoystickHatEvent::s_sClassId));
	REQUIRE(Event::getEventClassIdClass(JoystickButtonEvent::s_sClassId));
	REQUIRE(Event::getEventClassIdClass(JoystickAxisEvent::s_sClassId));
	REQUIRE(Event::getEventClassIdClass(KeyEvent::s_sClassId));
	REQUIRE(Event::getEventClassIdClass(PointerEvent::s_sClassId));
	REQUIRE(Event::getEventClassIdClass(PointerScrollEvent::s_sClassId));
	REQUIRE(Event::getEventClassIdClass(TouchEvent::s_sClassId));
}

TEST_CASE("testEventClass, StaticStdEventsExistInverse")
{
	REQUIRE(Event::getEventClassIdClass(DeviceMgmtEvent::s_sClassId).getId() == DeviceMgmtEvent::s_sClassId);
	REQUIRE(Event::getEventClassIdClass(JoystickHatEvent::s_sClassId).getId() == JoystickHatEvent::s_sClassId);
	REQUIRE(Event::getEventClassIdClass(JoystickButtonEvent::s_sClassId).getId() == JoystickButtonEvent::s_sClassId);
	REQUIRE(Event::getEventClassIdClass(JoystickAxisEvent::s_sClassId).getId() == JoystickAxisEvent::s_sClassId);
	REQUIRE(Event::getEventClassIdClass(KeyEvent::s_sClassId).getId() == KeyEvent::s_sClassId);
	REQUIRE(Event::getEventClassIdClass(PointerEvent::s_sClassId).getId() == PointerEvent::s_sClassId);
	REQUIRE(Event::getEventClassIdClass(PointerScrollEvent::s_sClassId).getId() == PointerScrollEvent::s_sClassId);
	REQUIRE(Event::getEventClassIdClass(TouchEvent::s_sClassId).getId() == TouchEvent::s_sClassId);
}

TEST_CASE("testEventClass, StaticStdEventsTypeInfo")
{
	REQUIRE(Event::getEventClassIdClass(DeviceMgmtEvent::s_sClassId).getTypeInfo() == typeid(DeviceMgmtEvent));
	REQUIRE(Event::getEventClassIdClass(JoystickHatEvent::s_sClassId).getTypeInfo() == typeid(JoystickHatEvent));
	REQUIRE(Event::getEventClassIdClass(JoystickButtonEvent::s_sClassId).getTypeInfo() == typeid(JoystickButtonEvent));
	REQUIRE(Event::getEventClassIdClass(JoystickAxisEvent::s_sClassId).getTypeInfo() == typeid(JoystickAxisEvent));
	REQUIRE(Event::getEventClassIdClass(KeyEvent::s_sClassId).getTypeInfo() == typeid(KeyEvent));
	REQUIRE(Event::getEventClassIdClass(PointerEvent::s_sClassId).getTypeInfo() == typeid(PointerEvent));
	REQUIRE(Event::getEventClassIdClass(PointerScrollEvent::s_sClassId).getTypeInfo() == typeid(PointerScrollEvent));
	REQUIRE(Event::getEventClassIdClass(TouchEvent::s_sClassId).getTypeInfo() == typeid(TouchEvent));
}

TEST_CASE("testEventClass, EventIsXYEvent")
{
	REQUIRE_FALSE(Event::getEventClassIdClass(DeviceMgmtEvent::s_sClassId).isXYEvent());
	REQUIRE_FALSE(Event::getEventClassIdClass(JoystickHatEvent::s_sClassId).isXYEvent());
	REQUIRE_FALSE(Event::getEventClassIdClass(JoystickButtonEvent::s_sClassId).isXYEvent());
	REQUIRE_FALSE(Event::getEventClassIdClass(JoystickAxisEvent::s_sClassId).isXYEvent());
	REQUIRE_FALSE(Event::getEventClassIdClass(KeyEvent::s_sClassId).isXYEvent());
	REQUIRE(Event::getEventClassIdClass(PointerEvent::s_sClassId).isXYEvent());
	REQUIRE(Event::getEventClassIdClass(PointerScrollEvent::s_sClassId).isXYEvent());
	REQUIRE(Event::getEventClassIdClass(TouchEvent::s_sClassId).isXYEvent());
}

} // namespace testing

} // namespace stmi
