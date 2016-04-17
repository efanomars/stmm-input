/*
 * File:   barewindow.cc
 */

#include <cassert>
#include <iostream>

#include "barewindow.h"

namespace example
{

namespace bareapp
{

BareWindow::BareWindow(const Glib::ustring sTitle)
{
	//
	set_title(sTitle);
	set_default_size(350, 250);
	set_resizable(true);
}
BareWindow::~BareWindow()
{
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

