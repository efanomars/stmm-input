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
 * File:   spinnwindow.cc
 */

#include "spinnwindow.h"

#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <gtkmm.h>
#include <pangomm.h>

namespace example
{

namespace spinn
{

SpinnWindow::SpinnWindow(const shared_ptr<stmi::DeviceManager>& refDM, const Glib::ustring sTitle)
: Gtk::Window()
, m_oMainVBox(Gtk::ORIENTATION_VERTICAL)
, m_oSpinn(refDM)
{
	//
	set_title(sTitle);
	set_default_size(s_nInitialWindowSizeW, s_nInitialWindowSizeH);
	set_resizable(true);

	m_oLabel1.set_text("-111");
	Glib::RefPtr<Gtk::StyleContext> refStyleCtx1 = m_oLabel1.get_style_context();
	Pango::FontDescription oFont = refStyleCtx1->get_font();
	oFont.set_size(oFont.get_size() * 3);
	oFont.set_family("Mono");
	m_oLabel1.override_font(oFont);

	m_oLabel2.set_text("Press keys to change value:\n"
						"Up: +1, Down: -1\n"
						"Mouse Button Left: +10, Right: -10\n"
						"Joystick Button A: +100, B: -100");

	m_oMainVBox.pack_start(m_oArea, true, true);
	m_oMainVBox.pack_start(m_oLabel1, false, true);
	m_oMainVBox.pack_start(m_oLabel2, false, true);

	add(m_oMainVBox);

	m_oSpinn.m_oVauleChangedSignal.connect( sigc::mem_fun(this, &SpinnWindow::valueChanged) );

	auto sValue = Glib::ustring::compose("%1", m_oSpinn.getValue());
	m_oLabel1.set_text(sValue);

	show_all_children();
}

void SpinnWindow::valueChanged()
{
	auto sValue = Glib::ustring::compose("%1", m_oSpinn.getValue());
	m_oLabel1.set_text(sValue);
}

} // namespace spinn

} // namespace example
