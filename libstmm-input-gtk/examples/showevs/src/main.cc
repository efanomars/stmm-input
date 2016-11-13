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
 * File:   main.cc
 */

#include "showevswindow.h"

#include <stmm-input-gtk/gtkdevicemanager.h>

#include <gtkmm.h>

#include <iostream>

namespace example
{

namespace showevs
{

int showevsAppMain(int argc, char** argv)
{
	const Glib::ustring sAppName = "net.exampleappsnirvana.showevs";
	const Glib::ustring sWindowTitle = "Show libstmm-input-gtk events";
	// Also initializes Gtk
	Glib::RefPtr<Gtk::Application> refApp = Gtk::Application::create(argc, argv, sAppName);
	//
	shared_ptr<stmi::DeviceManager> refDM = stmi::GtkDeviceManager::create();
	if (!refDM) {
		//throw std::runtime_error("Couldn't create device manager");
		std::cout << "Error: Couldn't create device manager" << std::endl;
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

