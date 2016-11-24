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
 * File:   main.cc
 */

#include "spinnwindow.h"

#include <stmm-input-gtk/gtkdevicemanager.h>

#include <gtkmm.h>

#include <iostream>

namespace example
{

namespace spinn
{

int spinnAppMain(int argc, char** argv)
{
	const Glib::ustring sAppName = "net.exampleappsnirvana.spinn";
	const Glib::ustring sWindowTitle = "Spinn: change number with keys, mouse, etc.";
	// Also initializes Gtk
	Glib::RefPtr<Gtk::Application> refApp = Gtk::Application::create(argc, argv, sAppName);
	//
	shared_ptr<stmi::DeviceManager> refDM = stmi::GtkDeviceManager::create();
	if (!refDM) {
		//throw std::runtime_error("Couldn't create device manager");
		std::cout << "Error: Couldn't create device manager" << std::endl;
		return EXIT_FAILURE;
	}
	auto refSpinnWindow = Glib::RefPtr<SpinnWindow>(new SpinnWindow(refDM, sWindowTitle));
	auto refAccessor = std::make_shared<stmi::GtkAccessor>(refSpinnWindow);
	refDM->addAccessor(refAccessor);

	return refApp->run(*(refSpinnWindow.operator->()));
}

} // namespace spinn

} // namespace example

int main(int argc, char** argv)
{
	return example::spinn::spinnAppMain(argc, argv);
}

