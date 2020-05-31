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
 * File:   main.cc
 */

#include "sysdata.h"
#include "dlpinfos.h"

#include <iterator>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

#include <stdlib.h>

namespace stmi
{

void printVersion() noexcept
{
	std::cout << SysData::getVersionString() << '\n';
}
void printUsage() noexcept
{
	std::cout << "Usage: stmm-input-plugins [OPTION]" << '\n';
	std::cout << "Test keyboard client that connects to a server over bluetooth" << '\n';
	std::cout << "with the btkeys protocol." << '\n';
	std::cout << "Option:" << '\n';
	std::cout << "  -l --list           List installed plugins." << '\n';
	std::cout << "  -s --status         With --list also shows enabled status of the plugin." << '\n';
	std::cout << "  -f --file           With --list also shows full path of the plugin." << '\n';
	std::cout << "  -a --all            Same as -s -f." << '\n';
	std::cout << "  -e --enable  NAME   Enables a plugin." << '\n';
	std::cout << "  -d --disable NAME   Disables a plugin." << '\n';
	std::cout << "  -i --info    NAME   Show information about a plugin." << '\n';
	std::cout << "     --paths          Prints plugins paths." << '\n';
	std::cout << "  -h --help           Prints this message." << '\n';
	std::cout << "  -v --version        Prints version." << '\n';
}

void evalNoArg(int& nArgC, char**& aArgV, const std::string& sOption1, const std::string& sOption2
				, std::string& sMatch, bool& bVar) noexcept
{
	sMatch.clear();
	if (aArgV[1] == nullptr) {
		return;
	}
	const bool bIsOption1 = (sOption1 == std::string(aArgV[1]));
	if (bIsOption1 || ((!sOption2.empty()) && (sOption2 == std::string(aArgV[1])))) {
		sMatch = (bIsOption1 ? sOption1 : sOption2);
		bVar = true;
		--nArgC;
		++aArgV;
	}
}
bool evalStringArg(int& nArgC, char**& aArgV, const std::string& sOption1, const std::string& sOption2
					, std::string& sMatch, std::string& sName) noexcept
{
	sMatch.clear();
	if (aArgV[1] == nullptr) {
		return true;
	}
	const std::string sArg = aArgV[1];
	const bool bIsOption1 = (sOption1 == sArg);
	if (bIsOption1 || ((!sOption2.empty()) && (sOption2 == sArg))) {
		sMatch = (bIsOption1 ? sOption1 : sOption2);
		--nArgC;
		++aArgV;
		if (nArgC == 1) {
			std::cerr << "Error: " << sMatch << " missing argument" << '\n';
			return false; //----------------------------------------------------
		} else {
			sName = aArgV[1];
			--nArgC;
			++aArgV;
		}
	}
	return true;
}
bool writeLinesToStream(std::ofstream& oOutFile, const std::vector<std::string>& aLines) noexcept
{
	bool bErrorWriting = false;
	for (const std::string& sLine : aLines) {
		oOutFile.write(sLine.c_str(), sLine.size());
		if (! oOutFile.good()) {
			bErrorWriting = true;
			break; //----
		}
		oOutFile.put('\n');
		if (! oOutFile.good()) {
			bErrorWriting = true;
			break; //----
		}
	}
	return bErrorWriting;
}

int doList(bool bListPath, bool bListStatus) noexcept
{
	SysData oSysData = SysData::getSysData();
	if (! oSysData.getError().empty()) {
		std::cerr << oSysData.getError() << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	const DlpInfos& oDlpInfos = oSysData.getDlpInfos();
	const std::vector<DlpInfo>& aOrderedInfos = oDlpInfos.getOrderedDlps();
	bool bFoundOne = false;
	for (const DlpInfo& oDlpInfo : aOrderedInfos) {
		std::cout << oDlpInfo.m_sName;
		if (bListStatus) {
			std::cout << "  " << (oDlpInfo.isEnabled() ? "E" : "-");
		}
		if (bListPath) {
			std::cout << "  " << oDlpInfo.m_sFile;
		}
		std::cout << '\n';
		bFoundOne = true;
	}
	if (! bFoundOne) {
		std::cout << "No plugins found" << '\n';
	}
	return EXIT_SUCCESS;
}
int doInfoPlugin(const std::string& sPluginName) noexcept
{
	SysData oSysData = SysData::getSysData();
	if (! oSysData.getError().empty()) {
		std::cerr << oSysData.getError() << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	const DlpInfo* p0DlpInfo = oSysData.findPlugin(sPluginName);
	if (p0DlpInfo == nullptr) {
		std::cout << "Unknown plugin " << sPluginName << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	const DlpInfo& oDlpInfo = *p0DlpInfo;
	std::cout << "Plugin  name: " << oDlpInfo.m_sName << '\n';
	std::cout << "      status: " << (oDlpInfo.isEnabled() ? "Enabled" : "Disabled") << '\n';
	std::cout << "        file: " << oDlpInfo.m_sFile << '\n';
	std::cout << " description: " << '\n';
	for (const std::string& sLine : oDlpInfo.m_aDllDescriptionLines) {
		std::cout << "              " << sLine << '\n';
	}
	if (! oDlpInfo.m_aDllPaths.empty()) {
		std::cout << "   dll paths: " << '\n';
		for (const std::string& sDllPath : oDlpInfo.m_aDllPaths) {
			std::cout << "              " << sDllPath << '\n';
		}
	}
	if (! oDlpInfo.m_aGroups.empty()) {
		std::cout << "      groups: " << '\n';
		for (const std::string& sGroup : oDlpInfo.m_aGroups) {
			std::cout << "              " << sGroup << '\n';
		}
	}
	if (! oDlpInfo.m_aDepends.empty()) {
		std::cout << "     depends: " << '\n';
		for (const std::string& sDepend : oDlpInfo.m_aDepends) {
			std::cout << "              " << sDepend << '\n';
		}
	}
	return EXIT_SUCCESS;
}
int doPluginPaths() noexcept
{
	SysData oSysData = SysData::getSysData();
	if (! oSysData.getError().empty()) {
		std::cerr << oSysData.getError() << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	std::cout << "Plugin paths:" << '\n';
	const auto& aDataPaths = oSysData.getSysDataPaths();
	for (const std::string& sDataPath : aDataPaths) {
		std::cout << "  '" << sDataPath << "'" << '\n';
	}
	std::cout << "Disable paths (" << DlpInfos::s_sDisabledDlpsFile << ")" << '\n';
	for (const std::string& sDataPath : aDataPaths) {
		std::cout << "  '" << sDataPath << "'" << '\n';
	}
	const std::string& sLocalPath = oSysData.getLocalDataPath();
	if (! sLocalPath.empty()) {
		std::cout << "  '" << sLocalPath << "'" << '\n';
	}
	return EXIT_SUCCESS;
}

/* Pseudo code:
 * ENABLE:
 * (if already enabled exit)
 * // Might need to modify all disabled-dlps.txt
 * if euid is root (0)
 *   if SUDO_USER is not empty (means prg called with sudo)
 *     remove plugin for all data dirs
 *     drop privileges and remove from local dir
 *   else // its really root
 *     remove plugin for all data dirs
 *   endif
 * else
 *   if Config::getUserDataDirRelPath() not empty
 *     modify disabled-dlps.txt 
 *     of $HOME/Config::getUserDataDirRelPath()
 *     if disabled in data dirs
 *       error: sorry sudo is needed to enable
 *     endif
 *   else
 *     if disabled in data dirs
 *       error: sorry sudo is needed to enable
 *     endif
 *     error: no local directory found
 *   endif
 * endif
 */
int doEnablePlugin(const std::string& sPluginName) noexcept
{
	SysData oSysData = SysData::getSysData();
	if (! oSysData.getError().empty()) {
		std::cerr << oSysData.getError() << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	const DlpInfo* p0DlpInfo = oSysData.findPlugin(sPluginName);
	if (p0DlpInfo == nullptr) {
		std::cerr << "Unknown plugin '" << sPluginName << "'" << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	const DlpInfo& oDlpInfo = *p0DlpInfo;
	if (oDlpInfo.isEnabled()) {
		std::cout << "Plugin '" << oDlpInfo.m_sName << "' is already enabled!" << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	// try to modify all that have line
	//     pluginname
	// to
	//     #pluginname
	std::vector<std::string> aRest;
	const auto& aDisablePaths = oSysData.getDisablePaths();
	for (const std::string& sDisablePath : aDisablePaths) {
		if (sDisablePath == oSysData.getLocalDataPath()) {
			// it's the last path in aDisablePaths
			if (oSysData.isRoot()) {
				// drop privileges, so that the ownership of the file isn't root
				int nRet = oSysData.dropPrivileges();
				if (nRet != EXIT_SUCCESS) {
					return nRet; //---------------------------------------------
				}
			}
		}
		const std::string sDisableFile = sDisablePath + "/" + DlpInfos::s_sDisabledDlpsFile;
		std::ifstream oInFile(sDisableFile);
		if (!oInFile.good()) {
			// doesn't exist or no permissions to read
			continue; //---
		}
		std::string sLatestLine;
		std::vector<std::string> aPluginNameLines = DlpInfos::parseBlock(oInFile, false, sLatestLine);
		bool bModified = false;
		for (std::string& sLine : aPluginNameLines) {
			if (DlpInfos::trimString(sLine) == oDlpInfo.m_sName) {
				// disable, whitespace is removed
				sLine = DlpInfos::s_sDlpCommentLineStart + oDlpInfo.m_sName;
				bModified = true;
			}
		}
		if (bModified) {
			// the rest of the file is preserved
			aRest.clear();
			while (! sLatestLine.empty()) {
				aRest.push_back(sLatestLine);
				std::vector<std::string> aRestLines = DlpInfos::parseBlock(oInFile, false, sLatestLine);
				std::move(aRestLines.begin(), aRestLines.end(), std::back_inserter(aRest));
			}
			oInFile.close();
			//TODO file lock
			bool bErrorWriting = false;
			std::ofstream oOutFile(sDisableFile, std::ios::out);
			if (! oOutFile.good()) {
				if (oSysData.isRoot()) {
					bErrorWriting = true;
				} else if (oDlpInfo.isDisabledInSysPath()) {
					// The enabling cannot work
					std::cout << "Plugin '" << oDlpInfo.m_sName << "' can only be enabled with sudo!" << '\n';
					return EXIT_FAILURE; //-------------------------------------
				}
			} else {
				bErrorWriting = writeLinesToStream(oOutFile, aPluginNameLines);
				if (! bErrorWriting) {
					bErrorWriting = writeLinesToStream(oOutFile, aRest);
				}
			}
			if (bErrorWriting) {
				std::cerr << "Error cannot write to file '" << sDisableFile << "'" << '\n';
				return EXIT_FAILURE; //-----------------------------------------
			}
		}
	}
	return EXIT_SUCCESS;
}
/* Pseudo code:
 * DISABLE:
 * (if already disabled exit)
 * if euid is root (0)
 *   if SUDO_USER is not empty (means prg called with sudo)
 *     if Config::getDataDirPaths() is not empty
 *       create or modify disabled-dlps.txt of last in Config::getDataDirPaths()
 *       adding the plugin
 *     else if Config::getUserDataDirRelPath() not empty
 *       drop privileges and create or modify disabled-dlps.txt
 *       of $HOME/Config::getUserDataDirRelPath()
 *     else
 *       error: sys directories not found
 *     endif
 *   else // its really root
 *     // assume root can't use plugins
 *     if Config::getDataDirPaths() is not empty
 *       create or modify disabled-dlps.txt of last in Config::getDataDirPaths()
 *       adding the plugin
 *     else
 *       error: sys directories not found
 *     endif
 *   endif
 * else
 *   if Config::getUserDataDirRelPath() not empty
 *     create or modify disabled-dlps.txt 
 *     of $HOME/Config::getUserDataDirRelPath()
 *   else
 *     error: no local directory found
 *   endif
 * endif
 */
int doDisablePlugin(const std::string& sPluginName) noexcept
{
	SysData oSysData = SysData::getSysData();
	if (! oSysData.getError().empty()) {
		std::cerr << oSysData.getError() << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	const DlpInfo* p0DlpInfo = oSysData.findPlugin(sPluginName);
	if (p0DlpInfo == nullptr) {
		std::cerr << "Unknown plugin '" << sPluginName << "'" << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	const DlpInfo& oDlpInfo = *p0DlpInfo;
	if (! oDlpInfo.isEnabled()) {
		std::cout << "Plugin '" << oDlpInfo.m_sName << "' is already disabled!" << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	// try to modify one that has a line
	//     #pluginname
	// to
	//     pluginname
	std::string sDisablePath;
	if (oSysData.isRoot()) {
		const auto& oSysDataPaths = oSysData.getSysDataPaths();
		const auto nTotDataPaths = oSysDataPaths.size();
		if (nTotDataPaths > 0) {
			sDisablePath = oSysDataPaths[nTotDataPaths - 1];
		} else {
			sDisablePath = oSysData.getLocalDataPath();
			if (sDisablePath.empty()) {
				std::cerr << "No path defined to disable plugin" << '\n';
				return EXIT_FAILURE; //-----------------------------------------
			}
			oSysData.dropPrivileges();
		}
	} else {
		sDisablePath = oSysData.getLocalDataPath();
		if (sDisablePath.empty()) {
			std::cerr << "No local path defined to disable plugin" << '\n';
			return EXIT_FAILURE; //---------------------------------------------
		}
	}
	const std::string sDisableFile = sDisablePath + "/" + DlpInfos::s_sDisabledDlpsFile;
	std::ifstream oInFile(sDisableFile);
	if (!oInFile.good()) {
		const bool bOk = DlpInfos::makePath(sDisablePath);
		if (! bOk) {
			std::cout << "Error: cannot create dir '" << sDisablePath << "'" << '\n';
			return EXIT_FAILURE; //---------------------------------------------
		}
		// create empty sDisableFile
		//TODO file lock, open with exclusive write
		std::ofstream oOutFile(sDisableFile, std::ios::out);
		if (! oOutFile.good()) {
			std::cout << "Error: cannot create '" << sDisableFile << "'" << '\n';
			return EXIT_FAILURE; //---------------------------------------------
		}
	}
	// disabled plugins are in the first block
	// successive blocks aren't used yet
	// but are left unmodified
	std::string sLatestLine;
	std::vector<std::string> aPluginNameLines = DlpInfos::parseBlock(oInFile, false, sLatestLine);
	bool bModified = false;
	for (std::string& sLine : aPluginNameLines) {
		if (DlpInfos::trimString(sLine) == DlpInfos::s_sDlpCommentLineStart + oDlpInfo.m_sName) {
			// disable by removing leading #
			sLine = oDlpInfo.m_sName;
			bModified = true;
		}
	}
	if (! bModified) {
		// add the disabled plugin
		aPluginNameLines.push_back(oDlpInfo.m_sName);
	}
	std::vector<std::string> aRest;
	while (! sLatestLine.empty()) {
		aRest.push_back(sLatestLine);
		std::vector<std::string> aRestLines = DlpInfos::parseBlock(oInFile, false, sLatestLine);
		std::move(aRestLines.begin(), aRestLines.end(), std::back_inserter(aRest));
	}
	oInFile.close();
	//TODO file lock
	bool bErrorWriting = false;
	std::ofstream oOutFile(sDisableFile, std::ios::out);
	if (! oOutFile.good()) {
		bErrorWriting = true;
	} else {
		bErrorWriting = writeLinesToStream(oOutFile, aPluginNameLines);
		if (! bErrorWriting) {
			bErrorWriting = writeLinesToStream(oOutFile, aRest);
		}
	}
	if (bErrorWriting) {
		std::cout << "Error cannot write to file '" << sDisableFile << "'" << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	return EXIT_SUCCESS;
}

int pluginsMain(int nArgC, char** aArgV) noexcept
{
	bool bHelp = false;
	bool bVersion = false;
	bool bList = false;
	bool bListStatus = false;
	bool bListFile = false;
	bool bListAll = false;
	bool bEnablePlugin = false;
	bool bDisablePlugin = false;
	bool bInfoPlugin = false;
	bool bPluginPaths = false;
	std::string sPluginName;
	//
	std::string sMatch;
	std::string sListMatch;
	std::string sCommandMatch;
	//
	char* p0ArgVZeroSave = ((nArgC >= 1) ? aArgV[0] : nullptr);
	while (nArgC >= 2) {
		auto nOldArgC = nArgC;
		evalNoArg(nArgC, aArgV, "--help", "-h", sMatch, bHelp);
		if (bHelp) {
			printUsage();
			return EXIT_SUCCESS; //---------------------------------------------
		}
		evalNoArg(nArgC, aArgV, "--version", "-v", sMatch, bVersion);
		if (bVersion) {
			printVersion();
			return EXIT_SUCCESS; //---------------------------------------------
		}
		evalNoArg(nArgC, aArgV, "--paths", "", sMatch, bPluginPaths);
		evalNoArg(nArgC, aArgV, "--list", "-l", sListMatch, bList);
		evalNoArg(nArgC, aArgV, "--status", "-s", sMatch, bListStatus);
		evalNoArg(nArgC, aArgV, "--file", "-f", sMatch, bListFile);
		evalNoArg(nArgC, aArgV, "--all", "-a", sMatch, bListAll);
		//
		bool bOk = evalStringArg(nArgC, aArgV, "--enable", "-e", sMatch, sPluginName);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		if (! sMatch.empty()) {
			if (bDisablePlugin || bInfoPlugin) {
				std::cerr << "Error: too many options: try removing " << sMatch << '\n';
				return EXIT_FAILURE; //-----------------------------------------
			}
			sCommandMatch = sMatch;
			bEnablePlugin = true;
		}
		bOk = evalStringArg(nArgC, aArgV, "--disable", "-d", sMatch, sPluginName);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		if (! sMatch.empty()) {
			if (bEnablePlugin || bInfoPlugin) {
				std::cerr << "Error: too many options: try removing " << sMatch << '\n';
				return EXIT_FAILURE; //-----------------------------------------
			}
			sCommandMatch = sMatch;
			bDisablePlugin = true;
		}
		bOk = evalStringArg(nArgC, aArgV, "--info", "-i", sMatch, sPluginName);
		if (!bOk) {
			return EXIT_FAILURE; //---------------------------------------------
		}
		if (! sMatch.empty()) {
			if (bEnablePlugin || bDisablePlugin) {
				std::cerr << "Error: too many options: try removing " << sMatch << '\n';
				return EXIT_FAILURE; //-----------------------------------------
			}
			sCommandMatch = sMatch;
			bInfoPlugin = true;
		}
		if (nOldArgC == nArgC) {
			std::cerr << "Unknown argument: " << ((aArgV[1] == nullptr) ? "(null)" : std::string(aArgV[1])) << '\n';
			return EXIT_FAILURE; //---------------------------------------------
		}
		aArgV[0] = p0ArgVZeroSave;
	}

	if (bList) {
		if (bEnablePlugin || bDisablePlugin || bInfoPlugin) {
			std::cerr << "Error: too many options: try removing " << sCommandMatch << '\n';
			return EXIT_FAILURE; //---------------------------------------------
		}
	} else if (bListStatus || bListFile || bListAll) {
		std::cerr << "Error: --list argument missing" << '\n';
		return EXIT_FAILURE; //-------------------------------------------------
	}
	//

	int nRet = EXIT_SUCCESS;
	if (bInfoPlugin) {
		nRet = doInfoPlugin(sPluginName);
	} else if (bEnablePlugin) {
		nRet = doEnablePlugin(sPluginName);
	} else if (bDisablePlugin) {
		nRet = doDisablePlugin(sPluginName);
	} else if (bPluginPaths) {
		nRet = doPluginPaths();
	} else {
		if (bListAll || !bList) {
			bListStatus = true;
			bListFile = true;
		}
		nRet = doList(bListFile, bListStatus);
	}

	return nRet;
}

} // namespace stmi

int main(int nArgC, char** aArgV)
{
	return stmi::pluginsMain(nArgC, aArgV);
}

