/*
 * Copyright © 2016  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
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
 * File:   gtkdevicemanager.cc
 */

#include "gtkdevicemanager.h"

#include "gdkkeyconverterevdev.h"
#include "masgtkdevicemanager.h"
#include "flogtkdevicemanager.h"
#include "jsgtkdevicemanager.h"

namespace stmi
{

namespace GtkDeviceManager
{

shared_ptr<DeviceManager> create()
{
	return create(false, {});
}
shared_ptr<DeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass)
{
	return create(bEnableEventClasses, aEnDisableEventClass, KEY_REPEAT_MODE_SUPPRESS);
}
shared_ptr<DeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
								, KEY_REPEAT_MODE eKeyRepeatMode)
{
	return create(bEnableEventClasses, aEnDisableEventClass, eKeyRepeatMode, shared_ptr<GdkKeyConverter>{});
}
shared_ptr<DeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
								, KEY_REPEAT_MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter)
{
	return create(bEnableEventClasses, aEnDisableEventClass, eKeyRepeatMode, refGdkConverter, Glib::RefPtr<Gdk::Display>{});
}
shared_ptr<DeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
								, KEY_REPEAT_MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter
								, const Glib::RefPtr<Gdk::Display>& refDisplay)
{
	shared_ptr<DeviceManager> refRes;

	std::vector< shared_ptr<ChildDeviceManager> > aManagers;
	aManagers.reserve(3);
	try {
		auto refMDM = MasGtkDeviceManager::create(bEnableEventClasses, aEnDisableEventClass, eKeyRepeatMode
												, (refGdkConverter ? refGdkConverter : GdkKeyConverterEvDev::getConverter())
												, (refDisplay ? refDisplay->get_device_manager() : Glib::RefPtr<Gdk::DeviceManager>{}));
		aManagers.push_back(refMDM);
	} catch (const std::runtime_error& oErr) {
		std::cerr << "GtkDeviceManager initialization error: " << oErr.what() << std::endl;
		return refRes;
	}
	try {
		auto refFDM = FloGtkDeviceManager::create(bEnableEventClasses, aEnDisableEventClass, eKeyRepeatMode
												, (refGdkConverter ? refGdkConverter : GdkKeyConverterEvDev::getConverter())
												, refDisplay);
		aManagers.push_back(refFDM);
	} catch (const std::runtime_error& oErr) {
		std::cerr << "FloGtkDeviceManager initialization error: " << oErr.what() << std::endl;
		return refRes;
	}
	try {
		JsGtkDeviceManager::DeviceFiles oDeviceFiles;
		oDeviceFiles.addBaseNrFiles("/dev/input/js");
		auto refJDM = JsGtkDeviceManager::create(bEnableEventClasses, aEnDisableEventClass, oDeviceFiles);
		aManagers.push_back(refJDM);
	} catch (const std::runtime_error& oErr) {
		std::cerr << "JsGtkDeviceManager initialization error: " << oErr.what() << std::endl;
		return refRes;
	}
	refRes = ParentDeviceManager::create(aManagers);
	return refRes;
}

} // namespace GtkDeviceManager

} // namespace stmi
