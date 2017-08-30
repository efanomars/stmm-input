/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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
#ifndef STMI_OMIT_X11
#include "flogtkdevicemanager.h"
#endif // STMI_OMIT_X11
#ifndef STMI_OMIT_PLUGINS
#include <stmm-input-dl/pluginsdevicemanager.h>
#endif // STMI_OMIT_PLUGINS
#include "jsgtkdevicemanager.h"

#include "parentdevicemanager.h"

namespace stmi
{

shared_ptr<GtkDeviceManager> GtkDeviceManager::create(const Init& oInit)
{
	shared_ptr<GtkDeviceManager> refRes;

	std::vector< shared_ptr<ChildDeviceManager> > aManagers;
	aManagers.reserve(3);
	try {
		auto refMDM = MasGtkDeviceManager::create(oInit.m_bEnableEventClasses, oInit.m_aEnDisableEventClasses, oInit.m_eKeyRepeatMode
												, (oInit.m_refGdkConverter ? oInit.m_refGdkConverter : GdkKeyConverterEvDev::getConverter())
												, (oInit.m_refDisplay ? oInit.m_refDisplay->get_device_manager() : Glib::RefPtr<Gdk::DeviceManager>{}));
		aManagers.push_back(refMDM);
	} catch (const std::runtime_error& oErr) {
		std::cerr << "GtkDeviceManager initialization error: " << oErr.what() << std::endl;
		return refRes;
	}
	#ifndef STMI_OMIT_X11
	try {
		auto refFDM = FloGtkDeviceManager::create(oInit.m_bEnableEventClasses, oInit.m_aEnDisableEventClasses, oInit.m_eKeyRepeatMode
												, (oInit.m_refGdkConverter ? oInit.m_refGdkConverter : GdkKeyConverterEvDev::getConverter())
												, oInit.m_refDisplay);
		aManagers.push_back(refFDM);
	} catch (const std::runtime_error& oErr) {
		std::cerr << "FloGtkDeviceManager initialization error: " << oErr.what() << std::endl;
		return refRes;
	}
	#endif //NOT STMI_OMIT_X11
	#ifndef STMI_OMIT_PLUGINS
	try {
		auto refPIDM = PluginsDeviceManager::create(oInit.m_bEnableEventClasses, oInit.m_aEnDisableEventClasses
													, oInit.m_sAdditionalPluginPath, oInit.m_bAdditionalPluginPathOnly
													, oInit.m_bEnablePlugins, oInit.m_aEnDisablePlugins
													, oInit.m_sAppName);
		if (refPIDM) {
			aManagers.push_back(refPIDM);
		}
	} catch (const std::runtime_error& oErr) {
		std::cerr << "PluginsDeviceManager initialization error: " << oErr.what() << std::endl;
		return refRes;
	}
	#endif //NOT STMI_OMIT_PLUGINS
	try {
		JsDeviceFiles oDeviceFiles;
		oDeviceFiles.addBaseNrFiles("/dev/input/js");
		auto refJDM = JsGtkDeviceManager::create(oInit.m_bEnableEventClasses, oInit.m_aEnDisableEventClasses, oDeviceFiles);
		aManagers.push_back(refJDM);
	} catch (const std::runtime_error& oErr) {
		std::cerr << "JsGtkDeviceManager initialization error: " << oErr.what() << std::endl;
		return refRes;
	}
	shared_ptr<GtkDeviceManager> refInstance(new GtkDeviceManager());
	refInstance->init(aManagers);
	return refInstance;
}

} // namespace stmi
