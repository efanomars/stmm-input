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
 * File:   testEventClass.cc
 */

#include "devicemgmtevent.h"
#include "joystickevent.h"
#include "keyevent.h"
#include "pointerevent.h"
#include "touchevent.h"

#include <gtest/gtest.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

TEST(testEventClass, StaticBadEventClassId)
{
	EXPECT_FALSE(Event::getEventClassIdClass("SOmeRanDOMNameTHatIsntAClassId"));
}

TEST(testEventClass, StaticStdEventsExist)
{
	EXPECT_TRUE(Event::getEventClassIdClass(DeviceMgmtEvent::s_sClassId));
	EXPECT_TRUE(Event::getEventClassIdClass(JoystickHatEvent::s_sClassId));
	EXPECT_TRUE(Event::getEventClassIdClass(JoystickButtonEvent::s_sClassId));
	EXPECT_TRUE(Event::getEventClassIdClass(JoystickAxisEvent::s_sClassId));
	EXPECT_TRUE(Event::getEventClassIdClass(KeyEvent::s_sClassId));
	EXPECT_TRUE(Event::getEventClassIdClass(PointerEvent::s_sClassId));
	EXPECT_TRUE(Event::getEventClassIdClass(PointerScrollEvent::s_sClassId));
	EXPECT_TRUE(Event::getEventClassIdClass(TouchEvent::s_sClassId));
}

TEST(testEventClass, StaticStdEventsExistInverse)
{
	EXPECT_TRUE(Event::getEventClassIdClass(DeviceMgmtEvent::s_sClassId).getId() == DeviceMgmtEvent::s_sClassId);
	EXPECT_TRUE(Event::getEventClassIdClass(JoystickHatEvent::s_sClassId).getId() == JoystickHatEvent::s_sClassId);
	EXPECT_TRUE(Event::getEventClassIdClass(JoystickButtonEvent::s_sClassId).getId() == JoystickButtonEvent::s_sClassId);
	EXPECT_TRUE(Event::getEventClassIdClass(JoystickAxisEvent::s_sClassId).getId() == JoystickAxisEvent::s_sClassId);
	EXPECT_TRUE(Event::getEventClassIdClass(KeyEvent::s_sClassId).getId() == KeyEvent::s_sClassId);
	EXPECT_TRUE(Event::getEventClassIdClass(PointerEvent::s_sClassId).getId() == PointerEvent::s_sClassId);
	EXPECT_TRUE(Event::getEventClassIdClass(PointerScrollEvent::s_sClassId).getId() == PointerScrollEvent::s_sClassId);
	EXPECT_TRUE(Event::getEventClassIdClass(TouchEvent::s_sClassId).getId() == TouchEvent::s_sClassId);
}

TEST(testEventClass, StaticStdEventsTypeInfo)
{
	EXPECT_TRUE(Event::getEventClassIdClass(DeviceMgmtEvent::s_sClassId).getTypeInfo() == typeid(DeviceMgmtEvent));
	EXPECT_TRUE(Event::getEventClassIdClass(JoystickHatEvent::s_sClassId).getTypeInfo() == typeid(JoystickHatEvent));
	EXPECT_TRUE(Event::getEventClassIdClass(JoystickButtonEvent::s_sClassId).getTypeInfo() == typeid(JoystickButtonEvent));
	EXPECT_TRUE(Event::getEventClassIdClass(JoystickAxisEvent::s_sClassId).getTypeInfo() == typeid(JoystickAxisEvent));
	EXPECT_TRUE(Event::getEventClassIdClass(KeyEvent::s_sClassId).getTypeInfo() == typeid(KeyEvent));
	EXPECT_TRUE(Event::getEventClassIdClass(PointerEvent::s_sClassId).getTypeInfo() == typeid(PointerEvent));
	EXPECT_TRUE(Event::getEventClassIdClass(PointerScrollEvent::s_sClassId).getTypeInfo() == typeid(PointerScrollEvent));
	EXPECT_TRUE(Event::getEventClassIdClass(TouchEvent::s_sClassId).getTypeInfo() == typeid(TouchEvent));
}

TEST(testEventClass, EventIsXYEvent)
{
	EXPECT_FALSE(Event::getEventClassIdClass(DeviceMgmtEvent::s_sClassId).isXYEvent());
	EXPECT_FALSE(Event::getEventClassIdClass(JoystickHatEvent::s_sClassId).isXYEvent());
	EXPECT_FALSE(Event::getEventClassIdClass(JoystickButtonEvent::s_sClassId).isXYEvent());
	EXPECT_FALSE(Event::getEventClassIdClass(JoystickAxisEvent::s_sClassId).isXYEvent());
	EXPECT_FALSE(Event::getEventClassIdClass(KeyEvent::s_sClassId).isXYEvent());
	EXPECT_TRUE(Event::getEventClassIdClass(PointerEvent::s_sClassId).isXYEvent());
	EXPECT_TRUE(Event::getEventClassIdClass(PointerScrollEvent::s_sClassId).isXYEvent());
	EXPECT_TRUE(Event::getEventClassIdClass(TouchEvent::s_sClassId).isXYEvent());
}

} // namespace testing

} // namespace stmi
