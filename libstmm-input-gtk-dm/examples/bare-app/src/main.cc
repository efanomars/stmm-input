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
 * File:   main.cc
 */

#include "barewindow.h"

#include <stmm-input-gtk-dm/stmm-input-gtk-dm.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/event.h>

#include <gtkmm.h>

#include <iostream>

namespace example
{

namespace bareapp
{

int bareAppMain(int argc, char** argv)
{
	const Glib::ustring sAppName = "net.exampleappsnirvana.bareapp";
	const Glib::ustring sWindoTitle = "bare-app - run from terminal";
	// Also initializes Gtk
	Glib::RefPtr<Gtk::Application> refApp = Gtk::Application::create(argc, argv, sAppName);
	std::cout << "bare-app prints hardware key events from keyboards and simulated keys events" << '\n';
	std::cout << "of buttons, hats, etc. from mice and joysticks to the terminal" << '\n';
	std::cout << "bare-app uses libstmm-input        version " << stmi::libconfig::core::getVersion() << '\n';
	std::cout << "              libstmm-input-base   version " << stmi::libconfig::base::getVersion() << '\n';
	std::cout << "              libstmm-input-ev     version " << stmi::libconfig::ev::getVersion() << '\n';
	std::cout << "              libstmm-input-gtk    version " << stmi::libconfig::gtk::getVersion() << '\n';
	std::cout << "              libstmm-input-gtk-dm version " << stmi::libconfig::gtkdm::getVersion() << '\n';
	// device manager has to be created after gtk is initialized!
	stmi::GtkDeviceManager::Init oInit{};
	oInit.m_sAppName = "bare-app";
	shared_ptr<stmi::DeviceManager> refDM = stmi::GtkDeviceManager::create(oInit);
	if (!refDM) {
		//throw std::runtime_error("Couldn't create device manager");
		std::cout << "Error: Couldn't create device manager" << '\n';
		return EXIT_FAILURE;
	}
	auto refBareWindow = Glib::RefPtr<BareWindow>(new BareWindow(sWindoTitle));
	auto refAccessor = std::make_shared<stmi::GtkAccessor>(refBareWindow);
	refDM->addAccessor(refAccessor);

	auto refListener = std::make_shared<stmi::EventListener>(
	[](const shared_ptr<stmi::Event>& refEvent)
	{
		std::vector< std::pair<stmi::HARDWARE_KEY, stmi::Event::AS_KEY_INPUT_TYPE> > aKeys = refEvent->getAsKeys();
		if (aKeys.empty()) {
			return;
		}
		std::cout << "Event: " << refEvent->getEventClass().getId();
		std::cout << "  Time: " << refEvent->getTimeUsec();
		auto refCapability = refEvent->getCapability();
		if (refCapability) {
			auto refDevice = refCapability->getDevice();
			if (refDevice) {
				std::cout << "  Device: " << refDevice->getName();
			}
		}
		std::cout << '\n';
		bool bFirst = true;
		for (auto& oPair : aKeys) {
			stmi::HARDWARE_KEY eHardwareKey = oPair.first;
			stmi::Event::AS_KEY_INPUT_TYPE eType = oPair.second;
			if (!bFirst) {
				std::cout << " - ";
			} else {
				std::cout << "  ";
				bFirst = false;
			}
			std::cout << "(" << static_cast<int32_t>(eHardwareKey) << ",";
			if (eType == stmi::Event::AS_KEY_PRESS) {
				std::cout << "PRESS";
			} else if (eType == stmi::Event::AS_KEY_RELEASE) {
				std::cout << "RELEASE";
			} else {
				std::cout << "CANCEL";
			}
			std::cout << ")";
		}
		std::cout << '\n';
	});
	refDM->addEventListener(refListener);

	return refApp->run(*(refBareWindow.operator->()));
}

} // namespace bareapp

} // namespace example

int main(int argc, char** argv)
{
	return example::bareapp::bareAppMain(argc, argv);
}

