/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
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
#include "jsdevicefiles.h"

#include <stmm-input-gtk/gdkkeyconverterevdev.h>
#include "masgtkdevicemanager.h"

#ifndef STMI_OMIT_PLUGINS
#include <stmm-input-dl/pluginsdevicemanager.h>
#endif // STMI_OMIT_PLUGINS
#include "jsgtkdevicemanager.h"

#include <iostream>
#include <type_traits>

namespace stmi { class ChildDeviceManager; }

namespace stmi
{

std::pair<shared_ptr<GtkDeviceManager>, std::string> GtkDeviceManager::create(const Init& oInit) noexcept
{
	shared_ptr<GtkDeviceManager> refRes;

	std::vector< shared_ptr<ChildDeviceManager> > aManagers;
	aManagers.reserve(3);
	{
		auto oPairMDM = MasGtkDeviceManager::create(oInit.m_bEnableEventClasses, oInit.m_aEnDisableEventClasses, oInit.m_eKeyRepeatMode
												, (oInit.m_refGdkConverter ? oInit.m_refGdkConverter : GdkKeyConverterEvDev::getConverter())
												, (oInit.m_refDisplay ? oInit.m_refDisplay->get_device_manager() : Glib::RefPtr<Gdk::DeviceManager>{}));
		auto& sError = oPairMDM.second;
		if (! sError.empty()) {
			if (oInit.m_bVerbose) {
				std::cerr << "GtkDeviceManager initialization error: " << sError << '\n';
			}
			return std::make_pair(refRes, std::move(sError)); //----------------
		}
		auto& refMDM = oPairMDM.first;
		if (refMDM) {
			aManagers.push_back(std::move(refMDM));
		}
	}
	#ifndef STMI_OMIT_PLUGINS
	{
		PluginsDeviceManager::Init oPluginInit;
		oPluginInit.m_bEnableEventClasses = oInit.m_bEnableEventClasses;
		oPluginInit.m_aEnDisableEventClasses = oInit.m_aEnDisableEventClasses;
		oPluginInit.m_sAdditionalPluginPath = oInit.m_sAdditionalPluginPath;
		oPluginInit.m_bAdditionalPluginPathOnly = oInit.m_bAdditionalPluginPathOnly;
		oPluginInit.m_bEnablePlugins = oInit.m_bEnablePlugins;
		oPluginInit.m_aEnDisablePlugins = oInit.m_aEnDisablePlugins;
		oPluginInit.m_aGroups = oInit.m_aGroups;
		oPluginInit.m_sAppName = oInit.m_sAppName;
		oPluginInit.m_bVerbose = oInit.m_bVerbose;
		auto oPairPIDM = PluginsDeviceManager::create(std::move(oPluginInit));
		auto& sError = oPairPIDM.second;
		if (! sError.empty()) {
			if (oInit.m_bVerbose) {
				std::cerr << "GtkDeviceManager initialization error: " << sError << '\n';
			}
			return std::make_pair(refRes, std::move(sError)); //----------------
		}
		auto& refPIDM = oPairPIDM.first;
		if (refPIDM) {
			aManagers.push_back(std::move(refPIDM));
		}
	}
	#endif //NOT STMI_OMIT_PLUGINS
	{
		JsDeviceFiles oDeviceFiles;
		oDeviceFiles.addBaseNrFiles("/dev/input/js");
		auto oPairJDM = JsGtkDeviceManager::create(oInit.m_bEnableEventClasses, oInit.m_aEnDisableEventClasses, oDeviceFiles);
		auto& sError = oPairJDM.second;
		if (sError.empty()) {
			auto& refJDM = oPairJDM.first;
			if (refJDM) {
				aManagers.push_back(std::move(oPairJDM.first));
			}
		} else {
			std::cout << sError << '\n';
		}
	}
	refRes = shared_ptr<GtkDeviceManager>(new GtkDeviceManager());
	refRes->init(aManagers);
	return std::make_pair(refRes, "");
}

} // namespace stmi
