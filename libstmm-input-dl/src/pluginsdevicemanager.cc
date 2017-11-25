/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
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

#include "stmm-input-dl-config.h"

#include <streambuf>

#include <vector>
#include <string>
#include <iostream>
#include <cctype>
#include <algorithm>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <ctype.h>
#include <ctype.h> // stat

namespace stmi
{

static const std::string& s_sDlpExt = ".dlp";
static const int32_t s_nDlpExtLen = 4;

static const std::string& s_sDlpBlockSeparator = "#=#+#=#";
static const int32_t s_nDlpBlockSeparatorLen = 7;

static const std::string& s_sDlpCommentLine = "#";
static const int32_t s_nDlpCommentLineLen = 1;

static const std::string& s_sDisabledDlpsFile = "disabled-dlps.txt";

shared_ptr<PluginsDeviceManager> PluginsDeviceManager::create(const Init& oInit)
{
//std::cout << "PluginsDeviceManager::create " << sAdditionalPluginPath << " only:" << bAdditionalPluginPathOnly << '\n';
	std::vector<std::string> aPluginPaths;
	if (! oInit.m_sAdditionalPluginPath.empty()) {
		aPluginPaths.push_back(oInit.m_sAdditionalPluginPath);
	}
	if (!oInit.m_bAdditionalPluginPathOnly) {
		makePath(libconfig::dl::getUserDataDir());
		aPluginPaths.push_back(libconfig::dl::getUserDataDir());
		if (aPluginPaths.back().empty()) {
			aPluginPaths.pop_back();
		}
		aPluginPaths.push_back(libconfig::dl::getDataDir());
		if (aPluginPaths.back().empty()) {
			aPluginPaths.pop_back();
		}
	}
	std::vector<DlpInfo> aDlps;
	for (const auto& sDirPath : aPluginPaths) {
		getDir(oInit.m_bEnablePlugins, oInit.m_aEnDisablePlugins, aDlps, sDirPath);
	}

	if (aDlps.empty()) {
		return shared_ptr<PluginsDeviceManager>{}; //---------------------------
	}

	for (auto& oDlp : aDlps) {
		const bool bOk = parseDlpInfo(oDlp);
		// if failed mark as visited so that won't be included when traversing
		// dependencies
		oDlp.m_bVisited = !bOk;
		if (bOk) {
			for (const auto& sMustGroup : oInit.m_aGroups) {
				const auto itFindGroup = std::find_if(oDlp.m_aGroups.begin(), oDlp.m_aGroups.end()
													, [&](const std::string& sGroup)
					{
						return (sGroup == sMustGroup);
					});
				if (itFindGroup == oDlp.m_aGroups.end()) {
					// this plugin doesn't belong to all the required groups
					oDlp.m_bVisited = true;
					break; // for(sMustGroup ----
				}
			}
		}
	}
	for (const auto& sDirPath : aPluginPaths) {
		parseDisabledNamesFile(sDirPath + "/" + s_sDisabledDlpsFile, aDlps);
	}

	std::vector<DlpInfo> aOrderedDlps;
	for (auto& oDlp : aDlps) {
		traverse(oDlp, aDlps, aOrderedDlps);
	}

	std::vector<shared_ptr<ChildDeviceManager>> aChildDMs;
	std::vector<void *> aPluginHandles;
	for (const auto& oDlp : aOrderedDlps) {
		void* p0Handle = nullptr;
		std::string sLoadLibrary;
		std::string sError;
		for (const auto& sLibrary : oDlp.m_aDllPaths) {
			p0Handle = dlopen(sLibrary.c_str(), RTLD_NOW | RTLD_GLOBAL);
			if (p0Handle != nullptr) {
				sLoadLibrary = sLibrary;
				break; // for sLibrary ----
			}
			if (sError.empty()) {
				sError = "PluginsDeviceManager::create error: could not dlopen library " + sLibrary + "\n -> " + std::string(dlerror());
			}
		}
		if (p0Handle == nullptr) {
			if (!sError.empty()) {
				std::cout << sError << '\n';
			}
			continue; // for oDlp ----
		}
		createPlugin_t* p0CreatePluginFunc = reinterpret_cast<createPlugin_t*>(dlsym(p0Handle, "createPlugin"));
		if (p0CreatePluginFunc == nullptr) {
			std::cout << "PluginsDeviceManager::create error: could not dlsym library " << sLoadLibrary << " " << dlerror() << '\n';
			dlclose(p0Handle);
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
		return shared_ptr<PluginsDeviceManager>{}; //---------------------------
	}

	auto refPDM = shared_ptr<PluginsDeviceManager>(new PluginsDeviceManager(
													oInit.m_bEnableEventClasses, oInit.m_aEnDisableEventClasses, std::move(aPluginHandles)));
	refPDM->init(aChildDMs);
	return refPDM;
}

PluginsDeviceManager::PluginsDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses
											, std::vector<void*>&& aPluginHandles)
: ParentDeviceManager()
, m_bEnableEventClasses(bEnableEventClasses)
, m_aEnDisableEventClass(aEnDisableEventClasses)
, m_aPluginHandles(aPluginHandles)
{
}
PluginsDeviceManager::~PluginsDeviceManager()
{
	ParentDeviceManager::removeChildren();
	for (void* p0Handle : m_aPluginHandles) {
		dlclose(p0Handle);
	}
}
shared_ptr<Capability> PluginsDeviceManager::getCapability(const Capability::Class& /*oClass*/) const
{
	return shared_ptr<Capability>{};
}
shared_ptr<Capability> PluginsDeviceManager::getCapability(int32_t /*nCapabilityId*/) const
{
	return shared_ptr<Capability>{};
}

void PluginsDeviceManager::getDir(bool bEnablePlugins, const std::vector<std::string>& aEnDisablePlugins
								, std::vector<DlpInfo>& aDlpInfos, const std::string& sDirPath)
{
    DIR* p0Dir = opendir(sDirPath.c_str());
    if (p0Dir == nullptr) {
		std::cout << "PluginsDeviceManager::getDir: error (" << strerror(errno) << ") opening directory '" << sDirPath << "'" << '\n';
		return; //--------------------------------------------------------------
    }

	do {
		errno = 0;
		struct dirent* p0DirEntry = readdir(p0Dir);
		if (p0DirEntry == nullptr) {
			if (errno == 0) {
				break; // do ------
			} else {
				std::cout << "PluginsDeviceManager::getDir: error (" << strerror(errno) << ") reading directory '" << sDirPath << "'" << '\n';
			    closedir(p0Dir);
		        return; //------------------------------------------------------
			}
		}
		const std::string sFile = std::string(p0DirEntry->d_name);
		const auto nLen = sFile.size();
		if ((nLen > s_nDlpExtLen) && (sFile.substr(nLen - s_nDlpExtLen, s_nDlpExtLen) == s_sDlpExt)) {
			const std::string sName = sFile.substr(0, sFile.size() - s_nDlpExtLen);
//std::cout << "----sName=" << sName << '\n';
			auto itFindDuplicate = std::find_if(aDlpInfos.begin(), aDlpInfos.end(), [&](const DlpInfo& oInfo)
			{
				return sName == oInfo.m_sName;
			});
			if (itFindDuplicate != aDlpInfos.end()) {
				// skip
				std::cout << "PluginsDeviceManager::getDir: '" << sDirPath << "' ignored (overridden)" << '\n';
				continue; // do -----
			}
			auto itFind = std::find(aEnDisablePlugins.begin(), aEnDisablePlugins.end(), sName);
			const bool bFound = (itFind != aEnDisablePlugins.end());
			if (bEnablePlugins) {
				if (!bFound) {
					// skip
					continue; // do -----
				}
			} else {
				if (bFound) {
					// skip
					continue; // do -----
				}
			}
			aDlpInfos.emplace_back();
			DlpInfo& oInfo = aDlpInfos.back();
			oInfo.m_sName = sName;
			oInfo.m_sFile = sDirPath + "/" + sFile;
//std::cout << "getDir() " << sName << '\n';
		}
	} while(true);
	//
    closedir(p0Dir);
}

bool PluginsDeviceManager::parseDlpInfo(DlpInfo& oInfo)
{
	std::ifstream oInFile(oInfo.m_sFile);
	if (!oInFile.good()) {
		std::cout << " Couldn't open file '" << oInfo.m_sFile << "'" << '\n';
		return false; //--------------------------------------------------------
	}
	oInfo.m_aDllPaths = parseBlock(oInFile, true);
	oInfo.m_aDepends = parseBlock(oInFile, true);
	oInfo.m_aDllDescriptionLines = parseBlock(oInFile, false);
	oInfo.m_aGroups = parseBlock(oInFile, true);
	for (int32_t nIdx = 0; nIdx < static_cast<int32_t>(oInfo.m_aGroups.size()); ++nIdx) {
		const auto& sGroup = oInfo.m_aGroups[nIdx];
		const auto nGroupNameSize = static_cast<int32_t>(sGroup.size());
		assert(nGroupNameSize > 0);
		const auto c0 = sGroup[0];
		if (! std::isalpha(c0)) {
			std::cout << " PluginsDeviceManager::parseDlpInfo: group name must start with alpha! '" << sGroup << "' doesn't" << '\n';
			return false; //----------------------------------------------------
		}
		for (int32_t nGroupIdx = 1; nGroupIdx < nGroupNameSize; ++nGroupIdx) {
			const auto c = sGroup[nGroupIdx];
			if (! std::isalnum(c)) {
				std::cout << " PluginsDeviceManager::parseDlpInfo: group name must contain alphanums! '" << sGroup << "' doesn't" << '\n';
				return false; //------------------------------------------------
			}
		}
	}
	return true;
}
bool PluginsDeviceManager::parseDisabledNamesFile(const std::string& sFile, std::vector<DlpInfo>& aDlps)
{
//std::cout << "PluginsDeviceManager::parseDisabledNamesFile " << sFile << '\n';
	std::ifstream oInFile(sFile);
	if (!oInFile.good()) {
		//std::cout << " Couldn't open file '" << sFile << "'" << '\n';
		return false; //--------------------------------------------------------
	}
	auto aNames = parseBlock(oInFile, true);
	for (const auto& sPluginName : aNames) {
		auto itFind = std::find_if(aDlps.begin(), aDlps.end(), [&](const DlpInfo& oCur)
		{
			return oCur.m_sName == sPluginName;
		});
		if (itFind == aDlps.end()) {
			continue; // for ---
		}
		(*itFind).m_bVisited = true;
	}
	return true;
}

std::vector<std::string> PluginsDeviceManager::parseBlock(std::ifstream& oInFile, bool bSkipComments)
{
	std::vector<std::string> aLines;
	for (std::string sLine; std::getline(oInFile, sLine); ) {
		if (sLine.substr(0, s_nDlpBlockSeparatorLen) == s_sDlpBlockSeparator) {
			// end of block
			break;  //for ---
		}
		if (bSkipComments) {
			sLine = trimString(sLine);

			if (sLine.empty() || sLine.substr(0, s_nDlpCommentLineLen) == s_sDlpCommentLine) {
				continue; //for ---
			}
		}
		aLines.push_back(sLine);
	}
	return aLines;
}
void PluginsDeviceManager::traverse(DlpInfo& oInfo, std::vector<DlpInfo>& aDlps, std::vector<DlpInfo>& aOrderedDlps)
{
	if (oInfo.m_bVisited) {
		return;
	}
	oInfo.m_bVisited = true;
	for (const auto& sDependsOn : oInfo.m_aDepends) {
		auto itFind = std::find_if(aDlps.begin(), aDlps.end(), [&](const DlpInfo& oCur)
		{
			return oCur.m_sName == sDependsOn;
		});
		if (itFind == aDlps.end()) {
			continue; // for ---
		}
		traverse(*itFind, aDlps, aOrderedDlps);
	}
	// depth (dependencies) first traversal
	aOrderedDlps.push_back(oInfo);
}
std::string PluginsDeviceManager::trimString(const std::string& sStr)
{
	// from stackoverflow.com/questions/25829143/c-trim-whitespace-from-a-string
	const auto nFirst = sStr.find_first_not_of(' ');
	if (nFirst == std::string::npos) {
		return sStr;
	}
	const auto nLast = sStr.find_last_not_of(' ');
	return sStr.substr(nFirst, (nLast - nFirst + 1));
}
// https://stackoverflow.com/questions/675039/how-can-i-create-directory-tree-in-c-linux
bool PluginsDeviceManager::dirExists(const std::string& sPath)
{
	struct ::stat oInfo;
	if (stat(sPath.c_str(), &oInfo) != 0) {
		return false;
	}
	return ((oInfo.st_mode & S_IFDIR) != 0);
}

bool PluginsDeviceManager::makePath(const std::string& sPath)
{
	mode_t nMode = 0755;
	const auto nRet = mkdir(sPath.c_str(), nMode);
	if (nRet == 0) {
		return true; //---------------------------------------------------------
	}

	switch (errno)
	{
	case ENOENT:
		// parent didn't exist, try to create it
		{
			const auto nPos = sPath.find_last_of('/');
			if (nPos == std::string::npos) {
				return false; //------------------------------------------------
			}
			if (! makePath( sPath.substr(0, nPos) )) {
				return false; //------------------------------------------------
			}
		}
		// now, try to create again
		return (0 == mkdir(sPath.c_str(), nMode)); //---------------------------

	case EEXIST:
		// done!
		return dirExists(sPath); //---------------------------------------------

	default:
		return false;
	}
}

} // namespace stmi

