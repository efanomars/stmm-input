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
 * File:   testCallIfSimplifier.cc
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "callifsimplifier.h"

#include "fakekeydevice.h"
#include "fakepointerdevice.h"

#include "keyevent.h"
#include "pointerevent.h"

#include <stmm-input/callifs.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class CallIfSimplifierFixture
{
public:
	CallIfSimplifierFixture()
	{
		m_refKeyDevice = std::make_shared<FakeKeyDevice>();
		#ifndef NDEBUG
		const bool bFound = 
		#endif
		m_refKeyDevice->getCapability(m_refKeyCapability);
		assert(bFound);
	}
protected:
	shared_ptr<Device> m_refKeyDevice;
	shared_ptr<KeyCapability> m_refKeyCapability;
};

TEST_CASE_METHOD(CallIfSimplifierFixture, "TrueFalseSingletons")
{
	REQUIRE(CallIfSimplifier::simplify(CallIfTrue::getInstance(), KeyEvent::getClass()) == CallIfTrue::getInstance());
	REQUIRE(CallIfSimplifier::simplify(CallIfFalse::getInstance(), KeyEvent::getClass()) == CallIfFalse::getInstance());
}

TEST_CASE_METHOD(CallIfSimplifierFixture, "NotSingletons")
{
	auto refCINotTrue = std::make_shared<CallIfNot>(CallIfTrue::getInstance());
	auto refCINotFalse = std::make_shared<CallIfNot>(CallIfFalse::getInstance());
	REQUIRE(CallIfSimplifier::simplify(refCINotTrue, KeyEvent::getClass()) == CallIfFalse::getInstance());
	REQUIRE(CallIfSimplifier::simplify(refCINotFalse, KeyEvent::getClass()) == CallIfTrue::getInstance());
}

TEST_CASE_METHOD(CallIfSimplifierFixture, "NotNotSingletons")
{
	auto refCINotTrue = std::make_shared<CallIfNot>(CallIfTrue::getInstance());
	auto refCINotFalse = std::make_shared<CallIfNot>(CallIfFalse::getInstance());
	auto refCINotNotTrue = std::make_shared<CallIfNot>(refCINotTrue);
	auto refCINotNotFalse = std::make_shared<CallIfNot>(refCINotFalse);
	REQUIRE(CallIfSimplifier::simplify(refCINotNotTrue, KeyEvent::getClass()) == CallIfTrue::getInstance());
	REQUIRE(CallIfSimplifier::simplify(refCINotNotFalse, KeyEvent::getClass()) == CallIfFalse::getInstance());
}

TEST_CASE_METHOD(CallIfSimplifierFixture, "EventClass")
{
	auto refCIKeyEventClass = std::make_shared<CallIfEventClass>(typeid(KeyEvent));
	REQUIRE(refCIKeyEventClass->getClass() == typeid(KeyEvent));
	auto refCIPointerEventClass = std::make_shared<CallIfEventClass>(typeid(PointerEvent));
	REQUIRE(CallIfSimplifier::simplify(refCIKeyEventClass, KeyEvent::getClass()) == CallIfTrue::getInstance());
	REQUIRE(CallIfSimplifier::simplify(refCIPointerEventClass, KeyEvent::getClass()) == CallIfFalse::getInstance());
}

TEST_CASE_METHOD(CallIfSimplifierFixture, "Or")
{
	auto refCIOrFalseFalse = std::make_shared<CallIfOr>(CallIfFalse::getInstance(), CallIfFalse::getInstance());
	auto refCIOrFalseTrue = std::make_shared<CallIfOr>(CallIfFalse::getInstance(), CallIfTrue::getInstance());
	REQUIRE(CallIfSimplifier::simplify(refCIOrFalseFalse, KeyEvent::getClass()) == CallIfFalse::getInstance());
	REQUIRE(CallIfSimplifier::simplify(refCIOrFalseTrue, KeyEvent::getClass()) == CallIfTrue::getInstance());
}

TEST_CASE_METHOD(CallIfSimplifierFixture, "And")
{
	auto refCIAndFalseFalse = std::make_shared<CallIfAnd>(CallIfFalse::getInstance(), CallIfFalse::getInstance());
	auto refCIAndFalseTrue = std::make_shared<CallIfAnd>(CallIfFalse::getInstance(), CallIfTrue::getInstance());
	auto refCIAndTrueTrue = std::make_shared<CallIfAnd>(CallIfTrue::getInstance(), CallIfTrue::getInstance());
	REQUIRE(CallIfSimplifier::simplify(refCIAndFalseFalse, KeyEvent::getClass()) == CallIfFalse::getInstance());
	REQUIRE(CallIfSimplifier::simplify(refCIAndFalseTrue, KeyEvent::getClass()) == CallIfFalse::getInstance());
	REQUIRE(CallIfSimplifier::simplify(refCIAndTrueTrue, KeyEvent::getClass()) == CallIfTrue::getInstance());
}

TEST_CASE_METHOD(CallIfSimplifierFixture, "DeviceInstance")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	auto refKeyEvent = std::make_shared<KeyEvent>(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_PRESS, HK_U);
	auto refCIKeyDeviceId = std::make_shared<stmi::CallIfDeviceId>(m_refKeyDevice->getId());
	REQUIRE(CallIfSimplifier::simplify(refCIKeyDeviceId, KeyEvent::getClass()) == refCIKeyDeviceId);
	REQUIRE((*refCIKeyDeviceId)(refKeyEvent));
	shared_ptr<Device> refKeyDevice2 = std::make_shared<FakeKeyDevice>();
	shared_ptr<KeyCapability> refKeyCapability2;
	const bool bFound = refKeyDevice2->getCapability(refKeyCapability2);
	REQUIRE(bFound);
	auto refKeyEvent2 = std::make_shared<KeyEvent>(nTimeUsec, Accessor::s_refEmptyAccessor, refKeyCapability2, KeyEvent::KEY_RELEASE, HK_U);
	REQUIRE_FALSE((*refCIKeyDeviceId)(refKeyEvent2));
}

TEST_CASE_METHOD(CallIfSimplifierFixture, "CapabilityClass")
{
	auto nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	auto refKeyEvent = std::make_shared<KeyEvent>(nTimeUsec, Accessor::s_refEmptyAccessor, m_refKeyCapability, KeyEvent::KEY_PRESS, HK_U);
	auto refCIKeyCapabilityClass = std::make_shared<stmi::CallIfCapabilityClass>(KeyCapability::getClass());
	REQUIRE(CallIfSimplifier::simplify(refCIKeyCapabilityClass, KeyEvent::getClass()) == refCIKeyCapabilityClass);
	REQUIRE((*refCIKeyCapabilityClass)(refKeyEvent));
	shared_ptr<Device> refPointerDevice2 = std::make_shared<FakePointerDevice>();
	shared_ptr<PointerCapability> refPointerCapability2;
	const bool bFound = refPointerDevice2->getCapability(refPointerCapability2);
	REQUIRE(bFound);
	auto refPointerEvent2 = std::make_shared<PointerEvent>(nTimeUsec, Accessor::s_refEmptyAccessor, refPointerCapability2, 100.0, 200.0, PointerEvent::BUTTON_PRESS, 1, true, false);
	REQUIRE_FALSE((*refCIKeyCapabilityClass)(refPointerEvent2));
}

} // namespace testing

} // namespace stmi
