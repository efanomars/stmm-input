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
 * File:   testCallIfSimplifier.cc
 */

#include "callifsimplifier.h"

#include "fakekeydevice.h"
#include "fakepointerdevice.h"

#include "keyevent.h"
#include "pointerevent.h"

#include <gtest/gtest.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class CallIfSimplifierFixture : public ::testing::Test
{
	void SetUp() override
	{
		m_refKeyDevice = std::make_shared<FakeKeyDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refKeyDevice->getCapability(m_refKeyCapability);
		assert(bFound);
	}
	void TearDown() override
	{
		m_refKeyCapability.reset();
		m_refKeyDevice.reset();
	}
public:
	shared_ptr<Device> m_refKeyDevice;
	shared_ptr<KeyCapability> m_refKeyCapability;
};

TEST_F(CallIfSimplifierFixture, TrueFalseSingletons)
{
	EXPECT_TRUE(CallIfSimplifier::simplify(CallIfTrue::getInstance(), KeyEvent::getClass()) == CallIfTrue::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(CallIfFalse::getInstance(), KeyEvent::getClass()) == CallIfFalse::getInstance());
}

TEST_F(CallIfSimplifierFixture, NotSingletons)
{
	auto refCINotTrue = std::make_shared<CallIfNot>(CallIfTrue::getInstance());
	auto refCINotFalse = std::make_shared<CallIfNot>(CallIfFalse::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCINotTrue, KeyEvent::getClass()) == CallIfFalse::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCINotFalse, KeyEvent::getClass()) == CallIfTrue::getInstance());
}

TEST_F(CallIfSimplifierFixture, NotNotSingletons)
{
	auto refCINotTrue = std::make_shared<CallIfNot>(CallIfTrue::getInstance());
	auto refCINotFalse = std::make_shared<CallIfNot>(CallIfFalse::getInstance());
	auto refCINotNotTrue = std::make_shared<CallIfNot>(refCINotTrue);
	auto refCINotNotFalse = std::make_shared<CallIfNot>(refCINotFalse);
	EXPECT_TRUE(CallIfSimplifier::simplify(refCINotNotTrue, KeyEvent::getClass()) == CallIfTrue::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCINotNotFalse, KeyEvent::getClass()) == CallIfFalse::getInstance());
}

TEST_F(CallIfSimplifierFixture, EventClass)
{
	auto refCIKeyEventClass = std::make_shared<CallIfEventClass>(typeid(KeyEvent));
	EXPECT_TRUE(refCIKeyEventClass->getClass() == typeid(KeyEvent));
	auto refCIPointerEventClass = std::make_shared<CallIfEventClass>(typeid(PointerEvent));
	EXPECT_TRUE(CallIfSimplifier::simplify(refCIKeyEventClass, KeyEvent::getClass()) == CallIfTrue::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCIPointerEventClass, KeyEvent::getClass()) == CallIfFalse::getInstance());
}

TEST_F(CallIfSimplifierFixture, Or)
{
	auto refCIOrFalseFalse = std::make_shared<CallIfOr>(CallIfFalse::getInstance(), CallIfFalse::getInstance());
	auto refCIOrFalseTrue = std::make_shared<CallIfOr>(CallIfFalse::getInstance(), CallIfTrue::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCIOrFalseFalse, KeyEvent::getClass()) == CallIfFalse::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCIOrFalseTrue, KeyEvent::getClass()) == CallIfTrue::getInstance());
}

TEST_F(CallIfSimplifierFixture, And)
{
	auto refCIAndFalseFalse = std::make_shared<CallIfAnd>(CallIfFalse::getInstance(), CallIfFalse::getInstance());
	auto refCIAndFalseTrue = std::make_shared<CallIfAnd>(CallIfFalse::getInstance(), CallIfTrue::getInstance());
	auto refCIAndTrueTrue = std::make_shared<CallIfAnd>(CallIfTrue::getInstance(), CallIfTrue::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCIAndFalseFalse, KeyEvent::getClass()) == CallIfFalse::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCIAndFalseTrue, KeyEvent::getClass()) == CallIfFalse::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCIAndTrueTrue, KeyEvent::getClass()) == CallIfTrue::getInstance());
}

TEST_F(CallIfSimplifierFixture, DeviceInstance)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	auto refKeyEvent = std::make_shared<KeyEvent>(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_PRESS, HK_U);
	auto refCIKeyDeviceId = std::make_shared<stmi::CallIfDeviceId>(m_refKeyDevice->getId());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCIKeyDeviceId, KeyEvent::getClass()) == refCIKeyDeviceId);
	EXPECT_TRUE((*refCIKeyDeviceId)(refKeyEvent));
	shared_ptr<Device> refKeyDevice2 = std::make_shared<FakeKeyDevice>();
	shared_ptr<KeyCapability> refKeyCapability2;
	const bool bFound = refKeyDevice2->getCapability(refKeyCapability2);
	EXPECT_TRUE(bFound);
	auto refKeyEvent2 = std::make_shared<KeyEvent>(nTimeUsec, Accessor::s_refEmptyAccessor, refKeyCapability2, KeyEvent::KEY_RELEASE, HK_U);
	EXPECT_FALSE((*refCIKeyDeviceId)(refKeyEvent2));
}

TEST_F(CallIfSimplifierFixture, CapabilityClass)
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	auto refKeyEvent = std::make_shared<KeyEvent>(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_PRESS, HK_U);
	auto refCIKeyCapabilityClass = std::make_shared<stmi::CallIfCapabilityClass>(KeyCapability::getClass());
	EXPECT_TRUE(CallIfSimplifier::simplify(refCIKeyCapabilityClass, KeyEvent::getClass()) == refCIKeyCapabilityClass);
	EXPECT_TRUE((*refCIKeyCapabilityClass)(refKeyEvent));
	shared_ptr<Device> refPointerDevice2 = std::make_shared<FakePointerDevice>();
	shared_ptr<PointerCapability> refPointerCapability2;
	const bool bFound = refPointerDevice2->getCapability(refPointerCapability2);
	EXPECT_TRUE(bFound);
	auto refPointerEvent2 = std::make_shared<PointerEvent>(nTimeUsec, Accessor::s_refEmptyAccessor, refPointerCapability2, 100.0, 200.0, PointerEvent::BUTTON_PRESS, 1, true, false);
	EXPECT_FALSE((*refCIKeyCapabilityClass)(refPointerEvent2));
}

} // namespace testing

} // namespace stmi
