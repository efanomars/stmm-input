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
 * File:   testGdkKeyConverterEvdev.cc
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "gdkkeyconverterevdev.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

TEST_CASE("testGdkKeyConverterEvdev, SingletonNotNull")
{
	REQUIRE(GdkKeyConverterEvDev::getConverter().operator bool());
}

TEST_CASE("testGdkKeyConverterEvdev, IsReallySingleton")
{
	auto refConverter = GdkKeyConverterEvDev::getConverter();
	REQUIRE(refConverter == GdkKeyConverterEvDev::getConverter());
}

TEST_CASE("testGdkKeyConverterEvdev, KeyCodeConversionWorks")
{
	auto refConverter = GdkKeyConverterEvDev::getConverter();
	HARDWARE_KEY eKey;
	const bool bConverted = refConverter->convertKeyCodeToHardwareKey(9, eKey);
	REQUIRE(bConverted);
	REQUIRE(eKey == HK_ESC);
}

} // namespace testing

} // namespace stmi
