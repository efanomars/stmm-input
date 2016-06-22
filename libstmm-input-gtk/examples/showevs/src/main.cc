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

