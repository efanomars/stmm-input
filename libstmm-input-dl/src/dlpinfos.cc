/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   dlpinfos.cc
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_STMMINPUTPLUGINS_CMAKE@   */

#include "dlpinfos.h"

#include <iostream>
#include <cctype>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <cstdint>

#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>

namespace stmi
{

static const std::string s_sDlpExt = ".dlp";
static constexpr int32_t s_nDlpExtLen = 4;
static constexpr int32_t s_nRankLen = 2;

static const std::string s_sDlpBlockSeparator = "#=#+#=#";
static constexpr int32_t s_nDlpBlockSeparatorLen = 7;

const std::string DlpInfos::s_sDlpCommentLineStart = "#";
static constexpr int32_t s_nDlpCommentLineStartLen = 1;

const std::string DlpInfos::s_sDisabledDlpsFile = "disabled-dlps.txt";

DlpInfos::DlpInfos(bool bEnablePlugins, const std::vector<std::string>& aEnDisablePlugins
					, const std::vector<std::string>& aGroups
					, const std::vector<std::string>& aPluginPaths, const std::vector<std::string>& aDisablePaths
					, bool bVerbose) noexcept
: m_bVerbose(bVerbose)
{
	std::vector<DlpInfo> aDlps;
	for (const auto& sDirPath : aPluginPaths) {
		getDir(bEnablePlugins, aEnDisablePlugins, aDlps, sDirPath);
	}
	if (aDlps.empty()) {
		return; //--------------------------------------------------------------
	}
	// parse dlps and remove erroneous ones
	aDlps.erase(std::remove_if(aDlps.begin(), aDlps.end(), [](DlpInfo& oDlpInfo)
	{
		const bool bOk = parseDlpInfo(oDlpInfo);
		return ! bOk;
	}), aDlps.end());
	// order by dependency (if cycles are present the order is undefined).
	for (auto& oDlp : aDlps) {
		traverse(oDlp, aDlps, m_aOrderedDlpInfos);
	}
	// at this point all m_aOrderedDlpInfos[].m_bVisited are true
	// If no groups are requested by the app all plugins are selected
	if (! aGroups.empty()) {
		// Disable dlps not in the requested groups
		for (auto& oDlp : m_aOrderedDlpInfos) {
			if (oDlp.m_aGroups.empty()) {
				// a plugin with no groups listed is always selected
				continue; // for oDlp ----
			}
			bool bProvidesARequestedGroup = false;
			for (const auto& sMustGroup : aGroups) {
				const auto itFindGroup = std::find_if(oDlp.m_aGroups.begin(), oDlp.m_aGroups.end()
													, [&](const std::string& sGroup)
					{
						return (sGroup == sMustGroup);
					});
				if (itFindGroup != oDlp.m_aGroups.end()) {
					// this plugin satisfies at least one of the requested groups
					bProvidesARequestedGroup = true;
					break; // for(sMustGroup ----
				}
			}
			if (! bProvidesARequestedGroup) {
				// disable
				oDlp.m_bVisited = false;
			}
		}
	}
	// disable dlps listed in the disable files
	for (const auto& sDirPath : aPluginPaths) {
		parseDisabledNamesFile(sDirPath + "/" + s_sDisabledDlpsFile, m_aOrderedDlpInfos, true);
	}
	for (const auto& sDirPath : aDisablePaths) {
		parseDisabledNamesFile(sDirPath + "/" + s_sDisabledDlpsFile, m_aOrderedDlpInfos, false);
	}
}

void DlpInfos::getDir(bool bEnablePlugins, const std::vector<std::string>& aEnDisablePlugins
								, std::vector<DlpInfo>& aDlpInfos, const std::string& sDirPath) noexcept
{
	DIR* p0Dir = ::opendir(sDirPath.c_str());
	if (p0Dir == nullptr) {
		if (m_bVerbose) {
			std::cout << "DlpInfo - getDir: error (" << ::strerror(errno) << ") opening directory '" << sDirPath << "'" << '\n';
		}
		return; //--------------------------------------------------------------
	}

//std::cout << "DlpInfo - getDir() ----sDirPath=" << sDirPath << '\n';
	std::vector<std::string> aFileNames;
	do {
		errno = 0;
		struct dirent* p0DirEntry = ::readdir(p0Dir);
		if (p0DirEntry == nullptr) {
			if (errno == 0) {
				break; // do ------
			} else {
				if (m_bVerbose) {
					std::cout << "DlpInfo - getDir: error (" << ::strerror(errno) << ") reading directory '" << sDirPath << "'" << '\n';
				}
				::closedir(p0Dir);
				return; //------------------------------------------------------
			}
		}
		std::string sFile{p0DirEntry->d_name};
//std::cout << "DlpInfo - getDir() sFile: " << sFile << '\n';
		const auto nLen = sFile.size();
		if ((nLen > s_nRankLen + s_nDlpExtLen) && (sFile.substr(nLen - s_nDlpExtLen, s_nDlpExtLen) == s_sDlpExt)) {
			if (std::count_if(std::begin(sFile), std::begin(sFile) + s_nRankLen, [](char c)
					{
						return std::isdigit(c);
					}) == s_nRankLen) {
				// Only accept filenames nnNAME.dlp (where n is a digit)
//std::cout << "DlpInfo - getDir() is valid" << '\n';
				aFileNames.push_back(std::move(sFile));
			}
		}
	} while (true);
	//
	std::sort(aFileNames.begin(), aFileNames.end());
	//
	for (const auto& sFile : aFileNames) {
		const auto nLen = sFile.size();
		const std::string sRankedName = sFile.substr(0, nLen - s_nDlpExtLen);
		const std::string sName = sRankedName.substr(s_nRankLen);
//std::cout << "DlpInfo - getDir() ----sName=" << sName << "  sFile=" << sFile << '\n';
		auto itFindDuplicate = std::find_if(aDlpInfos.begin(), aDlpInfos.end(), [&](const DlpInfo& oInfo)
		{
			return sName == oInfo.m_sName;
		});
		if (itFindDuplicate != aDlpInfos.end()) {
			// skip
			if (m_bVerbose) {
				std::cout << "DlpInfo - getDir: '" << sDirPath << "/" << sFile << "' ignored (overridden)" << '\n';
			}
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
	//
	::closedir(p0Dir);
}

bool DlpInfos::parseDlpInfo(DlpInfo& oInfo) noexcept
{
	std::ifstream oInFile(oInfo.m_sFile);
	if (!oInFile.good()) {
		std::cout << "DlpInfo - Couldn't open file '" << oInfo.m_sFile << "'" << '\n';
		return false; //--------------------------------------------------------
	}
	oInfo.m_aDllPaths = parseBlock(oInFile, true);
	if (oInfo.m_aDllPaths.empty()) {
		std::cout << "DlpInfo - File '" << oInfo.m_sFile << "' doesn't contain dll paths" << '\n';
		return false; //--------------------------------------------------------
	}
	oInfo.m_aDepends = parseBlock(oInFile, true);
	oInfo.m_aDllDescriptionLines = parseBlock(oInFile, false);
	oInfo.m_aGroups = parseBlock(oInFile, true);
	for (int32_t nIdx = 0; nIdx < static_cast<int32_t>(oInfo.m_aGroups.size()); ++nIdx) {
		const auto& sGroup = oInfo.m_aGroups[nIdx];
		const auto nGroupNameSize = static_cast<int32_t>(sGroup.size());
		assert(nGroupNameSize > 0);
		const auto c0 = sGroup[0];
		if (! std::isalpha(c0)) {
			std::cout << "DlpInfo - parseDlpInfo: group name must start with alpha! '" << sGroup << "' doesn't" << '\n';
			return false; //----------------------------------------------------
		}
		for (int32_t nGroupIdx = 1; nGroupIdx < nGroupNameSize; ++nGroupIdx) {
			const auto c = sGroup[nGroupIdx];
			if (! std::isalnum(c)) {
				std::cout << "DlpInfo - parseDlpInfo: group name must contain only alphanums! '" << sGroup << "' doesn't" << '\n';
				return false; //------------------------------------------------
			}
		}
	}
	return true;
}
bool DlpInfos::parseDisabledNamesFile(const std::string& sFile, std::vector<DlpInfo>& aDlps, bool bDisabledInSys) noexcept
{
//std::cout << "DlpInfo - parseDisabledNamesFile " << sFile << '\n';
	std::ifstream oInFile(sFile);
	if (!oInFile.good()) {
		//std::cout << "DlpInfo - Couldn't open file '" << sFile << "'" << '\n';
		return false; //--------------------------------------------------------
	}
	auto aLines = parseBlock(oInFile, true);
	for (const auto& sPluginLine : aLines) {
		const std::string sPluginName = trimString(sPluginLine);
		auto itFind = std::find_if(aDlps.begin(), aDlps.end(), [&](const DlpInfo& oCur)
		{
			return oCur.m_sName == sPluginName;
		});
		if (itFind == aDlps.end()) {
			continue; // for ---
		}
		// mark as disabled
		(*itFind).m_bVisited = false;
		(*itFind).m_bDisabledInSys = bDisabledInSys;
	}
	return true;
}

std::vector<std::string> DlpInfos::parseBlock(std::ifstream& oInFile, bool bSkipComments) noexcept
{
	std::string sLatestLine;
	return parseBlock(oInFile, bSkipComments, sLatestLine);
}
std::vector<std::string> DlpInfos::parseBlock(std::ifstream& oInFile, bool bSkipComments, std::string& sLatestLine) noexcept
{
	sLatestLine = "";
	std::vector<std::string> aLines;
	for (std::string sLine; std::getline(oInFile, sLine); ) {
		if (sLine.substr(0, s_nDlpBlockSeparatorLen) == s_sDlpBlockSeparator) {
			// end of block
			sLatestLine = sLine;
			break;  //for ---
		}
		if (bSkipComments) {
			sLine = trimString(sLine);

			if (sLine.empty() || sLine.substr(0, s_nDlpCommentLineStartLen) == s_sDlpCommentLineStart) {
				continue; //for ---
			}
		}
		aLines.push_back(sLine);
	}
	return aLines;
}
void DlpInfos::traverse(DlpInfo& oInfo, std::vector<DlpInfo>& aDlps, std::vector<DlpInfo>& aOrderedDlps) noexcept
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
std::string DlpInfos::trimString(const std::string& sStr) noexcept
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
bool DlpInfos::dirExists(const std::string& sPath) noexcept
{
	struct ::stat oInfo;
	if (::stat(sPath.c_str(), &oInfo) != 0) {
		return false;
	}
	return ((oInfo.st_mode & S_IFDIR) != 0);
}

bool DlpInfos::makePath(const std::string& sPath) noexcept
{
	mode_t nMode = 0755;
	const auto nRet = ::mkdir(sPath.c_str(), nMode);
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
		return (0 == ::mkdir(sPath.c_str(), nMode)); //---------------------------

	case EEXIST:
		// done!
		return dirExists(sPath); //---------------------------------------------

	default:
		return false;
	}
}

} // namespace stmi

