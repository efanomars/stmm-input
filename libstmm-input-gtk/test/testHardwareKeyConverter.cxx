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
 * File:   testHardwareKeyConverter.cc
 */

#include <gtest/gtest.h>
#include "gdkkeyconverterevdev.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

TEST(testHardwareKeyConverter, InitialNotInstalled)
{
	EXPECT_FALSE(GdkKeyConverter::isConverterInstalled());
}

TEST(testHardwareKeyConverter, GetConverterIsInstall)
{
	EXPECT_FALSE(GdkKeyConverter::isConverterInstalled());
	const auto refConverter = GdkKeyConverter::getConverter();
	EXPECT_TRUE(refConverter.operator bool());
	EXPECT_TRUE(GdkKeyConverter::isConverterInstalled());
}

} // namespace testing

} // namespace stmi
