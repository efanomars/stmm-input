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
 * File:   testJoystickCapabilityClass.cc
 */

#include "joystickcapability.h"

#include <gtest/gtest.h>

#include <algorithm>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class JoystickCapabilityClassFixture : public ::testing::Test
{
	void SetUp() override
	{
	}
	void TearDown() override
	{
	}
};

TEST_F(JoystickCapabilityClassFixture, NotEmpty)
{
	EXPECT_FALSE(JoystickCapability::getHatValues().empty());
	EXPECT_FALSE(JoystickCapability::getButtons().empty());
	EXPECT_FALSE(JoystickCapability::getAxes().empty());
}

TEST_F(JoystickCapabilityClassFixture, HatsOrdered)
{
	auto& oAllHats = JoystickCapability::getHatValues();
	EXPECT_TRUE(std::is_sorted(oAllHats.begin(), oAllHats.end()));
}
TEST_F(JoystickCapabilityClassFixture, ButtonsOrdered)
{
	auto& oAllButtons = JoystickCapability::getButtons();
	EXPECT_TRUE(std::is_sorted(oAllButtons.begin(), oAllButtons.end()));
}
TEST_F(JoystickCapabilityClassFixture, AxesOrdered)
{
	auto& oAllAxes = JoystickCapability::getAxes();
	EXPECT_TRUE(std::is_sorted(oAllAxes.begin(), oAllAxes.end()));
}

TEST_F(JoystickCapabilityClassFixture, HatsFirstAndLastExist)
{
	auto& oAllHats = JoystickCapability::getHatValues();
	EXPECT_TRUE(std::find(oAllHats.begin(), oAllHats.end(), stmi::JoystickCapability::HAT_CENTER) != oAllHats.end());
	EXPECT_TRUE(std::find(oAllHats.begin(), oAllHats.end(), stmi::JoystickCapability::HAT_CENTER_CANCEL) != oAllHats.end());

	EXPECT_TRUE(stmi::JoystickCapability::isValidHatValue(stmi::JoystickCapability::HAT_LEFTUP));
	EXPECT_FALSE(stmi::JoystickCapability::isValidHatValue(static_cast<stmi::JoystickCapability::HAT_VALUE>(-77)));
}

TEST_F(JoystickCapabilityClassFixture, ButtonsFirstAndLastExist)
{
	auto& oAllButtons = JoystickCapability::getButtons();
	EXPECT_TRUE(std::find(oAllButtons.begin(), oAllButtons.end(), stmi::JoystickCapability::BUTTON_TRIGGER) != oAllButtons.end());
	EXPECT_TRUE(std::find(oAllButtons.begin(), oAllButtons.end(), stmi::JoystickCapability::BUTTON_GEAR_UP) != oAllButtons.end());

	EXPECT_TRUE(stmi::JoystickCapability::isValidButton(stmi::JoystickCapability::BUTTON_A));
	EXPECT_FALSE(stmi::JoystickCapability::isValidButton(static_cast<stmi::JoystickCapability::BUTTON>(-77)));
}

TEST_F(JoystickCapabilityClassFixture, AxesFirstAndLastExist)
{
	auto& oAllAxes = JoystickCapability::getAxes();
	EXPECT_TRUE(std::find(oAllAxes.begin(), oAllAxes.end(), stmi::JoystickCapability::AXIS_X) != oAllAxes.end());
	EXPECT_TRUE(std::find(oAllAxes.begin(), oAllAxes.end(), stmi::JoystickCapability::AXIS_TILT_Y) != oAllAxes.end());

	EXPECT_TRUE(stmi::JoystickCapability::isValidAxis(stmi::JoystickCapability::AXIS_THROTTLE));
	EXPECT_FALSE(stmi::JoystickCapability::isValidAxis(static_cast<stmi::JoystickCapability::AXIS>(-77)));
}

} // namespace testing

} // namespace stmi
