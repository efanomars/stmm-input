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
 * File:   testGtkAccessor.cc
 */

#include "gtkaccessor.h"

#include <gtkmm.h>

#include <gtest/gtest.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class GlibAppFixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
		m_refApp = Gtk::Application::create("net.testlibsnirvana.stmi");
	}
	void TearDown() override
	{
		m_refApp.clear();
	}
public:
	Glib::RefPtr<Gtk::Application> m_refApp;
};

////////////////////////////////////////////////////////////////////////////////
TEST_F(GlibAppFixture, Constructor)
{
	auto refWin1 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
	EXPECT_TRUE(refWin1.operator bool());
	auto refAccessor = std::make_shared<stmi::GtkAccessor>(refWin1);
	EXPECT_FALSE(refAccessor->isDeleted());
	auto p0Win1 = refWin1.operator->();
	EXPECT_TRUE(refAccessor->getGtkmmWindow() == p0Win1);

	refWin1.reset();
	EXPECT_TRUE(refAccessor->isDeleted());
	EXPECT_TRUE(refAccessor->getGtkmmWindow() == p0Win1);
}

} // namespace testing

} // namespace stmi
