/*
 * Copyright © 2016  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software; you can redistribute it and/or
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
 * File:   barewindow.h
 */

#ifndef _BARE_WINDOW_
#define _BARE_WINDOW_

#include <string>
#include <memory>

#include <gtkmm.h>
#include <gdkmm.h>

#include <stmm-input/devicemanager.h>
#include <stmm-input-gtk/gtkaccessor.h>

namespace example
{

namespace bareapp
{

using std::shared_ptr;
using std::weak_ptr;

class BareWindow : public Gtk::Window
{
public:
	BareWindow(const Glib::ustring sTitle);
protected:
	// To receive most of the keys, like TAB, ENTER, etc. override default handlers
	bool on_key_press_event(GdkEventKey* p0Event) override;
	bool on_key_release_event(GdkEventKey* p0Event) override;
};

} // namespace bareapp

} // namespace example

#endif	/* _BARE_WINDOW_ */

