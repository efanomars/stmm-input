/*
 * Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   pluginsdevicemanager.cc
 */

#include "pluginsdevicemanager.h"

#include "plugininterface.h"

#include "dlpinfos.h"

#include "stmm-input-dl-config.h"

#include "dlutil.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <type_traits>

#include <dlfcn.h>

namespace stmi { class ChildDeviceManager; }


namespace stmi
{

static std::string getPrefix() noexcept
{
	#ifdef STMM_SNAP_PACKAGING
	const std::string sPrefix = DlUtil::getEnvString("SNAP");
	#else
	const std::string sPrefix;
	#endif //STMM_SNAP_PACKAGING
	return sPrefix;
}

std::pair<shared_ptr<PluginsDeviceManager>, std::string> PluginsDeviceManager::create(Init&& oInit) noexcept
{
//std::cout << "PluginsDeviceManager::create " << oInit.m_sAdditionalPluginPath << " only:" << oInit.m_bAdditionalPluginPathOnly << '\n';
	std::vector<std::string> aPluginPaths;
	std::vector<std::string> aDisablePaths;
	if (! oInit.m_sAdditionalPluginPath.empty()) {
		aPluginPaths.push_back(std::move(oInit.m_sAdditionalPluginPath));
	}
	if (! oInit.m_bAdditionalPluginPathOnly) {
		std::string sUserDataDir = libconfig::dl::getUserDataDirPath();
		if (! sUserDataDir.empty()) {
			DlpInfos::makePath(sUserDataDir);
			aDisablePaths.push_back(std::move(sUserDataDir));
		}
		for (std::string& sPath : libconfig::dl::getDataDirPaths()) {
			assert(!sPath.empty());
			aPluginPaths.push_back(std::move(sPath));
		}
	}
	DlpInfos oDlpInfos(oInit.m_bEnablePlugins, oInit.m_aEnDisablePlugins, oInit.m_aGroups
						, aPluginPaths, aDisablePaths, oInit.m_bVerbose);

	const auto& aOrderedDlps = oDlpInfos.getOrderedDlps();
	if (aOrderedDlps.empty()) {
		return std::make_pair(shared_ptr<PluginsDeviceManager>{}, ""); //-------
	}

	const std::string sPrefix{getPrefix()};

	std::vector<shared_ptr<ChildDeviceManager>> aChildDMs;
	std::vector<void *> aPluginHandles;
	for (const auto& oDlp : aOrderedDlps) {
		if (! oDlp.isEnabled()) {
			continue;
		}
		void* p0Handle = nullptr;
		std::string sLoadLibrary;
		std::string sError;
		for (const auto& sLibrary : oDlp.m_aDllPaths) {
			const std::string sPrefixedLibrary = sPrefix + sLibrary; // for snapcraft
			p0Handle = ::dlopen(sPrefixedLibrary.c_str(), RTLD_NOW | RTLD_GLOBAL);
			if (p0Handle != nullptr) {
				sLoadLibrary = sLibrary;
				break; // for sLibrary ----
			}
			if (sError.empty()) {
				sError = "PluginsDeviceManager::create error: could not dlopen library " + sLibrary + "\n -> " + std::string(::dlerror());
			}
		}
		if (p0Handle == nullptr) {
			if (!sError.empty()) {
				std::cout << sError << '\n';
			}
			continue; // for oDlp ----
		}
		createPlugin_t* p0CreatePluginFunc = reinterpret_cast<createPlugin_t*>(::dlsym(p0Handle, "createPlugin"));
		if (p0CreatePluginFunc == nullptr) {
			std::cout << "PluginsDeviceManager::create error: could not dlsym library " << sLoadLibrary << " " << ::dlerror() << '\n';
			::dlclose(p0Handle);
			continue; // for oDlp ----
		}
		auto refChildDM = (*p0CreatePluginFunc)(oInit.m_sAppName, oInit.m_bEnableEventClasses, oInit.m_aEnDisableEventClasses);
		if (!refChildDM) {
			std::cout << "PluginsDeviceManager::create error: library " << sLoadLibrary << " couldn't create a device manager" << '\n';
			continue; // for oDlp ----
		}
		aPluginHandles.push_back(p0Handle);
		aChildDMs.push_back(refChildDM);
	}
	if (aPluginHandles.empty()) {
		return std::make_pair(shared_ptr<PluginsDeviceManager>{}, ""); //-------
	}

	auto refPDM = shared_ptr<PluginsDeviceManager>(new PluginsDeviceManager(
													oInit.m_bEnableEventClasses, oInit.m_aEnDisableEventClasses, std::move(aPluginHandles)));
	refPDM->init(aChildDMs);
	return std::make_pair(refPDM, "");
}

PluginsDeviceManager::PluginsDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses
											, std::vector<void*>&& aPluginHandles) noexcept
: ParentDeviceManager()
, m_bEnableEventClasses(bEnableEventClasses)
, m_aEnDisableEventClass(aEnDisableEventClasses)
, m_aPluginHandles(aPluginHandles)
{
//std::cout << "PluginsDeviceManager::PluginsDeviceManager " << reinterpret_cast<int64_t>(this) << '\n';

}
PluginsDeviceManager::~PluginsDeviceManager() noexcept
{
//std::cout << "PluginsDeviceManager::~PluginsDeviceManager " << reinterpret_cast<int64_t>(this) << '\n';
	ParentDeviceManager::removeChildren();
	for (void* p0Handle : m_aPluginHandles) {
		::dlclose(p0Handle);
	}
}

} // namespace stmi

