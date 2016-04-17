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
 * File:   testCallIfSimplifier.cc
 */

#include <gtest/gtest.h>

#include "callifsimplifier.h"

#include <stmm-input-ev/keyevent.h>
#include <stmm-input-ev/pointerevent.h>

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
	}
	void TearDown() override
	{	
	}
};

TEST_F(CallIfSimplifierFixture, TrueFalseSingletons)
{
	EXPECT_TRUE(CallIfSimplifier::simplify(CallIfTrue::getInstance(), KeyEvent::getClass()) == CallIfTrue::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(CallIfFalse::getInstance(), KeyEvent::getClass()) == CallIfFalse::getInstance());
}

TEST_F(CallIfSimplifierFixture, NotSingletons)
{
	auto refNotTrue = std::make_shared<CallIfNot>(CallIfTrue::getInstance());
	auto refNotFalse = std::make_shared<CallIfNot>(CallIfFalse::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refNotTrue, KeyEvent::getClass()) == CallIfFalse::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refNotFalse, KeyEvent::getClass()) == CallIfTrue::getInstance());
}

TEST_F(CallIfSimplifierFixture, NotNotSingletons)
{
	auto refNotTrue = std::make_shared<CallIfNot>(CallIfTrue::getInstance());
	auto refNotFalse = std::make_shared<CallIfNot>(CallIfFalse::getInstance());
	auto refNotNotTrue = std::make_shared<CallIfNot>(refNotTrue);
	auto refNotNotFalse = std::make_shared<CallIfNot>(refNotFalse);
	EXPECT_TRUE(CallIfSimplifier::simplify(refNotNotTrue, KeyEvent::getClass()) == CallIfTrue::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refNotNotFalse, KeyEvent::getClass()) == CallIfFalse::getInstance());
}

TEST_F(CallIfSimplifierFixture, EventClass)
{
	auto refKeyEvent = std::make_shared<CallIfEventClass>(typeid(KeyEvent));
	EXPECT_TRUE(refKeyEvent->getClass() == typeid(KeyEvent));
	auto refPointerEvent = std::make_shared<CallIfEventClass>(typeid(PointerEvent));
	EXPECT_TRUE(CallIfSimplifier::simplify(refKeyEvent, KeyEvent::getClass()) == CallIfTrue::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refPointerEvent, KeyEvent::getClass()) == CallIfFalse::getInstance());
}

TEST_F(CallIfSimplifierFixture, Or)
{
	auto refOrFalseFalse = std::make_shared<CallIfOr>(CallIfFalse::getInstance(), CallIfFalse::getInstance());
	auto refOrFalseTrue = std::make_shared<CallIfOr>(CallIfFalse::getInstance(), CallIfTrue::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refOrFalseFalse, KeyEvent::getClass()) == CallIfFalse::getInstance());
	EXPECT_TRUE(CallIfSimplifier::simplify(refOrFalseTrue, KeyEvent::getClass()) == CallIfTrue::getInstance());
}

} // namespace testing

} // namespace stmi
