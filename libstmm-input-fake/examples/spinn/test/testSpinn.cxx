/*
 *  Copyright © 2016   Stefano Marsili, <stemars@gmx.ch>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */
/* 
 * File:   testSpinn.cc
 */

#include "spinn.h"

#include "fakedevicemanager.h"

/* from the fit library (github.com/pfultz2/Fit) */
#define EXPECT_TRUE(...) if (!(__VA_ARGS__)) { std::cout << "***FAILED:  EXPECT_TRUE(" << #__VA_ARGS__ << ")\n     File: " << __FILE__ << ": " << __LINE__ << '\n'; return 1; }

#define EXECUTE_TEST(...) if ((__VA_ARGS__) != 0) { return 1; } else { std::cout << "ok " << #__VA_ARGS__ << '\n'; }

namespace example
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

int testConstruction()
{
	auto refFakeDM = std::make_shared<stmi::testing::FakeDeviceManager>();
	spinn::Spinn oSpinn(refFakeDM);
	EXPECT_TRUE(oSpinn.getValue() == 0);
	//
	return 0;
}
int testKeys()
{
	auto refFakeDM = std::make_shared<stmi::testing::FakeDeviceManager>();
	spinn::Spinn oSpinn(refFakeDM);

	auto nKeyDevId = refFakeDM->simulateNewDevice<stmi::testing::FakeKeyDevice>();
	//
	refFakeDM->simulateKeyEvent(nKeyDevId, stmi::KeyEvent::KEY_PRESS, stmi::HK_UP);
	EXPECT_TRUE(oSpinn.getValue() == 1);
	//
	refFakeDM->simulateKeyEvent(nKeyDevId, stmi::KeyEvent::KEY_RELEASE, stmi::HK_UP);
	EXPECT_TRUE(oSpinn.getValue() == 1);
	//
	refFakeDM->simulateKeyEvent(nKeyDevId, stmi::KeyEvent::KEY_PRESS, stmi::HK_LEFT);
	EXPECT_TRUE(oSpinn.getValue() == 1);
	//
	refFakeDM->simulateKeyEvent(nKeyDevId, stmi::KeyEvent::KEY_PRESS, stmi::HK_UP);
	EXPECT_TRUE(oSpinn.getValue() == 2);
	//
	refFakeDM->simulateKeyEvent(nKeyDevId, stmi::KeyEvent::KEY_PRESS, stmi::HK_DOWN);
	EXPECT_TRUE(oSpinn.getValue() == 1);
	//
	refFakeDM->simulateKeyEvent(nKeyDevId, stmi::KeyEvent::KEY_RELEASE, stmi::HK_UP);
	EXPECT_TRUE(oSpinn.getValue() == 1);
	//
	refFakeDM->simulateKeyEvent(nKeyDevId, stmi::KeyEvent::KEY_RELEASE, stmi::HK_DOWN);
	EXPECT_TRUE(oSpinn.getValue() == 1);
	//
	return 0;
}
int testPointer()
{
	auto refFakeDM = std::make_shared<stmi::testing::FakeDeviceManager>();
	spinn::Spinn oSpinn(refFakeDM);

	auto nPointerDevId = refFakeDM->simulateNewDevice<stmi::testing::FakePointerDevice>();
	//
	refFakeDM->simulatePointerEvent(nPointerDevId, 102, 103, stmi::PointerEvent::BUTTON_PRESS, 1, true, false);
	EXPECT_TRUE(oSpinn.getValue() == 10);
	//
	refFakeDM->simulatePointerEvent(nPointerDevId, 102, 103, stmi::PointerEvent::BUTTON_RELEASE, 1, false, true);
	EXPECT_TRUE(oSpinn.getValue() == 10);
	//
	return 0;
}

} // namespace testing
} // namespace example

int main(int /*argc*/, char** /*argv*/)
{
	EXECUTE_TEST(example::testing::testConstruction());
	EXECUTE_TEST(example::testing::testKeys());
	EXECUTE_TEST(example::testing::testPointer());
	//
	std::cout << "Spinn Tests successful!" << '\n';
	return 0;
}
