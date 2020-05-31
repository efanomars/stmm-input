/*
 * Copyright © 2016-2018  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testHardwareKeys.cc
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <algorithm>

#include "hardwarekey.h"

namespace stmi
{

namespace testing
{

class HardwareKeysFixture
{
};

TEST_CASE_METHOD(HardwareKeysFixture, "NotEmpty")
{
	REQUIRE_FALSE(HardwareKeys::get().empty());
}

TEST_CASE_METHOD(HardwareKeysFixture, "KeysOrdered")
{
	auto& oAllKeys = HardwareKeys::get();
	REQUIRE(std::is_sorted(oAllKeys.begin(), oAllKeys.end()));
}

TEST_CASE_METHOD(HardwareKeysFixture, "FirstAndLastExist")
{
	auto& oAllKeys = HardwareKeys::get();
	REQUIRE(std::find(oAllKeys.begin(), oAllKeys.end(), stmi::HK_ESC) != oAllKeys.end());
	REQUIRE(std::find(oAllKeys.begin(), oAllKeys.end(), stmi::HK_X_SCROLL_RIGHT) != oAllKeys.end());

	REQUIRE(HardwareKeys::isValid(stmi::HK_ESC));
	REQUIRE_FALSE(HardwareKeys::isValid(stmi::HK_NULL));
}

} // namespace testing

} // namespace stmi
