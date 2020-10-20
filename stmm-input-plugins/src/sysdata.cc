/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   sysdata.cc
 */

#include "sysdata.h"

#include "config.h"
#include "dlutil.h"

//#include <cassert>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <string>

#include <unistd.h>
#include <pwd.h>
#include <string.h>

#include <errno.h>

namespace stmi
{

SysData SysData::getSysData() noexcept
{
	SysData oSysData{};
	oSysData.m_aDataPaths = Config::getDataDirPaths();
	const std::string sUserDataDir = Config::getUserDataDirRelPath();

	const auto nEffectiveUID = ::geteuid();
	oSysData.m_nUID = nEffectiveUID;
	oSysData.m_nGID = ::getegid();
	if (nEffectiveUID == 0) {
		oSysData.m_bIsRoot = true;
		char* p0SudoUserId = ::getenv("SUDO_USER");
		errno = 0;
		::passwd* p0PwdData = ::getpwnam((p0SudoUserId != nullptr) ? p0SudoUserId : "root");
		if (p0PwdData == nullptr) {
			oSysData.m_sError = ::strerror(errno);
		} else {
			oSysData.m_nUID = p0PwdData->pw_uid;
			oSysData.m_nGID = p0PwdData->pw_gid;
			const char* p0HomePath = p0PwdData->pw_dir;
			oSysData.m_sLocalDataPath = std::string(p0HomePath) + "/" + sUserDataDir;
			if (p0SudoUserId == nullptr) {
				std::cerr << "Root user is not supposed to have a home directory." << '\n';
				std::cerr << "If you really want to create or modify file" << '\n';
				std::cerr << "\'" << oSysData.m_sLocalDataPath << "/" << DlpInfos::s_sDisabledDlpsFile << "'" << '\n';
				std::cerr << "do it manually!" << '\n';
				oSysData.m_sLocalDataPath.clear();
			}
		}
	} else {
		// normal user
		std::string sHome;
		#ifdef STMM_SNAP_PACKAGING
		// ex. /home/myusername/snap/mysnapname/x1
		sHome = DlUtil::getEnvString("SNAP_USER_DATA");
		#endif //STMM_SNAP_PACKAGING
		if (sHome.empty()) {
			// ex. /home/myusername
			sHome = DlUtil::getEnvString("HOME");
			if (sHome.empty()) {
				oSysData.m_sError = "Couldn't find home directory of user " + std::to_string(nEffectiveUID);
			}
		}
		if (oSysData.m_sError.empty()) {
			oSysData.m_sLocalDataPath = sHome + "/" + sUserDataDir;
		}
	}
	oSysData.m_aDisablePaths = oSysData.m_aDataPaths;
	std::vector<std::string> aExtraDisablePaths;
	if (! oSysData.m_sLocalDataPath.empty()) {
		oSysData.m_aDisablePaths.push_back(oSysData.m_sLocalDataPath);
		aExtraDisablePaths.push_back(oSysData.m_sLocalDataPath);
	}

	oSysData.m_refDlpInfos = std::make_unique<DlpInfos>(false, std::vector<std::string>{}, std::vector<std::string>{}
														, oSysData.m_aDataPaths, aExtraDisablePaths, false);
	return oSysData;
}
const std::vector<std::string>& SysData::getDisablePaths() const noexcept
{
	return m_aDisablePaths;
}
const DlpInfos& SysData::getDlpInfos() const noexcept
{
	return *m_refDlpInfos;
}
const DlpInfo* SysData::findPlugin(const std::string& sPluginName) const noexcept
{
	const std::vector<DlpInfo>& aDlps = getDlpInfos().getOrderedDlps();
	const auto itFind = std::find_if(aDlps.begin(), aDlps.end(), [&](const DlpInfo& oDlpInfo)
	{
		return (oDlpInfo.m_sName == sPluginName);
	});
	const bool bFoundExact = (itFind != aDlps.end());
	if (bFoundExact) {
		return &(*itFind); //---------------------------------------------------
	}
	const auto nPartialSize = sPluginName.size();
	if (nPartialSize < 2) {
		// at least two letters must partially match
		return nullptr; //------------------------------------------------------
	}
	const int32_t nTotDlpInfos = static_cast<int32_t>(aDlps.size());
	int32_t nFoundIdx = -1;
	for (int32_t nIdx = 0; nIdx < nTotDlpInfos; ++nIdx) {
		const DlpInfo& oDlpInfo = aDlps[nIdx];
		const auto nPluginNameSize = oDlpInfo.m_sName.size();
		if (nPartialSize >= nPluginNameSize) {
			continue;
		}
		if (oDlpInfo.m_sName.substr(nPluginNameSize - nPartialSize, nPartialSize) == sPluginName) {
			if (nFoundIdx < 0) {
				nFoundIdx = nIdx;
			} else {
				// more than one match: ambiguous
				return nullptr; //----------------------------------------------
			}
		}
	}

	return ((nFoundIdx < 0) ? nullptr : &(aDlps[nFoundIdx]));
}

int SysData::dropPrivileges() const noexcept
{
	// drop privileges, so that the ownership of the file isn't root
	auto nRet = ::setgid(m_nGID);
	if (nRet == -1) {
		std::cerr << "Error: " << ::strerror(errno) << ". Couldn't set group id to " << m_nGID << "." << '\n';
		return EXIT_FAILURE; //-------------------------------------
	}
	nRet = ::setuid(m_nUID);
	if (nRet == -1) {
		std::cerr << "Error: " << ::strerror(errno) << ". Couldn't set user id to " << m_nUID << "." << '\n';
		return EXIT_FAILURE; //-------------------------------------
	}
	return EXIT_SUCCESS;
}

std::string SysData::getVersionString() noexcept
{
	return Config::getVersionString();
}

} // namespace stmi


