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

#include "xideviceswindow.h"
#include "config.h"

#include <cassert>
#include <iostream>

#include <cstdlib>

namespace stmi
{

void printVersion()
{
	std::cout << Config::getVersionString() << std::endl;
}
void printUsage()
{
	std::cout << "Usage: device-floater [OPTION]" << std::endl;
	std::cout << "GUI to safely float a keyboard or pointer slave device (see xinput)." << std::endl;
	std::cout << "Option:" << std::endl;
	std::cout << "  -h --help              Prints this message." << std::endl;
	std::cout << "  -v --version           Prints version." << std::endl;
	std::cout << "  -a --acquire-secs N    Acquire time is N seconds" << std::endl;
	std::cout << "                         (default " << XiDevicesWindow::s_nDefaultAcquireSeconds
			<< ", max. " << XiDevicesWindow::s_nMaxAcquireSeconds  << ")." << std::endl;
}
void printDirs()
{
	std::cout << "Dirs: user data dir:   " << Config::getDataDir() << std::endl;
}

int deviceFloaterMain(int argc, char** argv)
{
	int32_t nAcquireSeconds = XiDevicesWindow::s_nDefaultAcquireSeconds;
	//
	if (argc >= 2) {
		char* argvZeroSave = ((argc >= 1) ? argv[0] : nullptr);
		if ((strcmp("--version", argv[1]) == 0) || (strcmp("-v", argv[1]) == 0)) {
			printVersion();
			return EXIT_SUCCESS; //---------------------------------------------
		}
		if ((strcmp("--help", argv[1]) == 0) || (strcmp("-h", argv[1]) == 0)) {
			printUsage();
			return EXIT_SUCCESS; //---------------------------------------------
		}
		if (strcmp("--dirs", argv[1]) == 0) {
			printDirs();
			return EXIT_SUCCESS; //---------------------------------------------
		}
		if ((strcmp("--acquire-secs", argv[1]) == 0) || (strcmp("-a", argv[1]) == 0)) {
			--argc;
			++argv;
			if (argc == 1) {
				std::cout << "Error: missing argument: number of seconds" << std::endl;
				return EXIT_FAILURE; //-----------------------------------------
			} else {
				try {
					double fAcquireSeconds = Glib::Ascii::strtod(argv[1]);
					if (fAcquireSeconds >= XiDevicesWindow::s_nMaxAcquireSeconds) {
						nAcquireSeconds = XiDevicesWindow::s_nMaxAcquireSeconds;
					} else if (fAcquireSeconds >= 1) {
						nAcquireSeconds = std::ceil(fAcquireSeconds);
					}
				} catch (const std::runtime_error& oErr) {
					std::cout << "Error: " << oErr.what() << std::endl;
					return EXIT_FAILURE; //-------------------------------------
				}
				--argc;
				++argv;
			}
		}
		argv[0] = argvZeroSave;
	}

	int nRet = EXIT_SUCCESS;

	const Glib::ustring sAppName = "ch.efanomars.devicefloater";
	const Glib::ustring sWindoTitle = "device-floater " + Config::getVersionString();
	try {
		//
		Glib::RefPtr<Gtk::Application> refApp = Gtk::Application::create(argc, argv, sAppName);
		XiDevicesWindow oWindow(sWindoTitle, nAcquireSeconds);
		nRet = refApp->run(oWindow);
	} catch (const std::runtime_error& oErr) {
		std::cout << "Error: " << oErr.what() << std::endl;
		nRet = EXIT_FAILURE;
	}
	return nRet;
}
} // namespace stmi

int main(int argc, char** argv)
{
	return stmi::deviceFloaterMain(argc, argv);
}

