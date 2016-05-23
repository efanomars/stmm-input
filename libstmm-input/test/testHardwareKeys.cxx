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
 * File:   testHardwareKeys.cc
 */

#include <gtest/gtest.h>

#include <stmm-input/hardwarekey.h>

namespace stmi
{

namespace testing
{

class HardwareKeysFixture : public ::testing::Test
{
	void SetUp() override
	{
	}
	void TearDown() override
	{	
	}
};

TEST_F(HardwareKeysFixture, NotEmpty)
{
	EXPECT_FALSE(HardwareKeys::get().empty());
}

TEST_F(HardwareKeysFixture, FirstAndLastExist)
{
	auto& oAllKeys = HardwareKeys::get();
	EXPECT_TRUE(oAllKeys.find(stmi::HK_ESC) != oAllKeys.end());
	EXPECT_TRUE(oAllKeys.find(stmi::HK_X_SCROLL_RIGHT) != oAllKeys.end());
}

} // namespace testing

} // namespace stmi
