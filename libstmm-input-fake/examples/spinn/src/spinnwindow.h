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
 * File:   spinnwindow.h
 */

#ifndef _EXAMPLE_SPINN_SPINN_WINDOW_
#define _EXAMPLE_SPINN_SPINN_WINDOW_

#include "spinn.h"

#include <stmm-input-ev/stmm-input-ev.h>

#include <gtkmm.h>
#include <gdkmm.h>

#include <string>
#include <memory>

namespace example
{

namespace spinn
{

using std::shared_ptr;
using std::weak_ptr;

class SpinnWindow : public Gtk::Window
{
public:
	SpinnWindow(const shared_ptr<stmi::DeviceManager>& refDM, const Glib::ustring sTitle);
protected:
	// To receive most of the keys, like cursor keys, TAB, ENTER, etc. override default handlers
	bool on_key_press_event(GdkEventKey* /*p0Event*/) override
	{
		return false; // propagate
	}
	bool on_key_release_event(GdkEventKey* /*p0Event*/) override
	{
		return false; // propagate
	}

	void valueChanged();
private:
	Gtk::Box m_oMainVBox;
		Gtk::DrawingArea m_oArea;
		Gtk::Label m_oLabel1;
		Gtk::Label m_oLabel2;

	Spinn m_oSpinn;

	static constexpr int32_t s_nInitialWindowSizeW = 300;
	static constexpr int32_t s_nInitialWindowSizeH = 200;
};

} // namespace spinn

} // namespace example

#endif /* _EXAMPLE_SPINN_SPINN_WINDOW_ */
