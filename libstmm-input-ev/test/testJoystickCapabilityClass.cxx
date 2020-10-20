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
 * File:   testJoystickCapabilityClass.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "joystickcapability.h"

#include <algorithm>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class JoystickCapabilityClassFixture
{
public:
	JoystickCapabilityClassFixture()
	{
	}
};

TEST_CASE_METHOD(JoystickCapabilityClassFixture, "NotEmpty")
{
	REQUIRE_FALSE(JoystickCapability::getHatValues().empty());
	REQUIRE_FALSE(JoystickCapability::getButtons().empty());
	REQUIRE_FALSE(JoystickCapability::getAxes().empty());
}

TEST_CASE_METHOD(JoystickCapabilityClassFixture, "HatsOrdered")
{
	auto& oAllHats = JoystickCapability::getHatValues();
	REQUIRE(std::is_sorted(oAllHats.begin(), oAllHats.end()));
}
TEST_CASE_METHOD(JoystickCapabilityClassFixture, "ButtonsOrdered")
{
	auto& oAllButtons = JoystickCapability::getButtons();
	REQUIRE(std::is_sorted(oAllButtons.begin(), oAllButtons.end()));
}
TEST_CASE_METHOD(JoystickCapabilityClassFixture, "AxesOrdered")
{
	auto& oAllAxes = JoystickCapability::getAxes();
	REQUIRE(std::is_sorted(oAllAxes.begin(), oAllAxes.end()));
}

TEST_CASE_METHOD(JoystickCapabilityClassFixture, "HatsFirstAndLastExist")
{
	auto& oAllHats = JoystickCapability::getHatValues();
	REQUIRE(std::find(oAllHats.begin(), oAllHats.end(), stmi::JoystickCapability::HAT_CENTER) != oAllHats.end());
	REQUIRE(std::find(oAllHats.begin(), oAllHats.end(), stmi::JoystickCapability::HAT_CENTER_CANCEL) != oAllHats.end());

	REQUIRE(stmi::JoystickCapability::isValidHatValue(stmi::JoystickCapability::HAT_LEFTUP));
	REQUIRE_FALSE(stmi::JoystickCapability::isValidHatValue(static_cast<stmi::JoystickCapability::HAT_VALUE>(-77)));
}

TEST_CASE_METHOD(JoystickCapabilityClassFixture, "ButtonsFirstAndLastExist")
{
	auto& oAllButtons = JoystickCapability::getButtons();
	REQUIRE(std::find(oAllButtons.begin(), oAllButtons.end(), stmi::JoystickCapability::BUTTON_TRIGGER) != oAllButtons.end());
	REQUIRE(std::find(oAllButtons.begin(), oAllButtons.end(), stmi::JoystickCapability::BUTTON_GEAR_UP) != oAllButtons.end());

	REQUIRE(stmi::JoystickCapability::isValidButton(stmi::JoystickCapability::BUTTON_A));
	REQUIRE_FALSE(stmi::JoystickCapability::isValidButton(static_cast<stmi::JoystickCapability::BUTTON>(-77)));
}

TEST_CASE_METHOD(JoystickCapabilityClassFixture, "AxesFirstAndLastExist")
{
	auto& oAllAxes = JoystickCapability::getAxes();
	REQUIRE(std::find(oAllAxes.begin(), oAllAxes.end(), stmi::JoystickCapability::AXIS_X) != oAllAxes.end());
	REQUIRE(std::find(oAllAxes.begin(), oAllAxes.end(), stmi::JoystickCapability::AXIS_TILT_Y) != oAllAxes.end());

	REQUIRE(stmi::JoystickCapability::isValidAxis(stmi::JoystickCapability::AXIS_THROTTLE));
	REQUIRE_FALSE(stmi::JoystickCapability::isValidAxis(static_cast<stmi::JoystickCapability::AXIS>(-77)));
}

} // namespace testing

} // namespace stmi
