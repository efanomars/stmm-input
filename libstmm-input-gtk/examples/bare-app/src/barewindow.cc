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
 * File:   barewindow.cc
 */

#include "barewindow.h"

namespace example
{

namespace bareapp
{

BareWindow::BareWindow(const Glib::ustring sTitle)
{
	set_title(sTitle);
	set_default_size(350, 250);
	set_resizable(true);
}
bool BareWindow::on_key_press_event(GdkEventKey* /*p0Event*/)
{
	return false; // propagate
}
bool BareWindow::on_key_release_event(GdkEventKey* /*p0Event*/)
{
	return false; // propagate
}

} // namespace bareapp

} // namespace example

