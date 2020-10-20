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
 * File:   main.cc
 */

#include "showevswindow.h"

#include <stmm-input-gtk-dm/gtkdevicemanager.h>

#include <gtkmm.h>

#include <iostream>

namespace example
{

namespace showevs
{

int showevsAppMain(int argc, char** argv)
{
	const Glib::ustring sAppName = "net.exampleappsnirvana.showevs";
	const Glib::ustring sWindowTitle = "Show libstmm-input-gtk-dm events";
	// Also initializes Gtk
	Glib::RefPtr<Gtk::Application> refApp = Gtk::Application::create(argc, argv, sAppName);
	//
	stmi::GtkDeviceManager::Init oInit;
	oInit.m_sAppName = "showevs";
	const auto oPairDM = stmi::GtkDeviceManager::create(oInit);
	const shared_ptr<stmi::DeviceManager>& refDM = oPairDM.first;
	if (!refDM) {
		std::cout << "Error: Couldn't create device manager" << '\n';
		return EXIT_FAILURE;
	}
	auto refShowEvsWindow = Glib::RefPtr<ShowEvsWindow>(new ShowEvsWindow(refDM, sWindowTitle));
	auto refAccessor = std::make_shared<stmi::GtkAccessor>(refShowEvsWindow);
	refDM->addAccessor(refAccessor);

	return refApp->run(*(refShowEvsWindow.operator->()));
}

} // namespace showevs

} // namespace example

int main(int argc, char** argv)
{
	return example::showevs::showevsAppMain(argc, argv);
}

