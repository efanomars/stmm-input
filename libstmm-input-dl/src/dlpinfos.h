/*
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   dlpinfos.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_STMMINPUTPLUGINS_CMAKE@   */

#ifndef STMI_DLP_INFOS_H
#define STMI_DLP_INFOS_H

#include <fstream>
#include <vector>
#include <string>
#include <memory>


namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;


struct DlpInfo
{
	std::string m_sName; // The name of the plugin. Ex. "stmm-input-bt"
	std::string m_sFile; // The full path of the .dlp file. Ex. "/etc/stmm-input-dl/39stmm-input-bt.dlp"
	std::vector<std::string> m_aDllPaths; // The possible library paths. The first for which dlopen works is chosen.
	std::vector<std::string> m_aDepends; // The .dlp names that should be loaded before this.
	std::vector<std::string> m_aDllDescriptionLines; // The description.
	std::vector<std::string> m_aGroups; // The groups.
	bool isEnabled() const noexcept { return m_bVisited; }
	bool isDisabledInSysPath() const noexcept { return m_bDisabledInSys; }
private:
	friend class DlpInfos;
	bool m_bVisited = false;
	bool m_bDisabledInSys = false;
};

class DlpInfos
{
public:
	/** Constructor.
	 * @param bEnablePlugins Whether to enable only plugins in aEnDisablePlugins or enable all but those in aEnDisablePlugins.
	 * @param aEnDisablePlugins The plugin names enabled or disabled according to bEnablePlugins.
	 * @param aGroups If not empty, the groups the plugins must define to be enabled.
	 * @param aPluginPaths The paths where plugins can be installed and disable-dlps.txt files defined.
	 * @param aDisablePaths Additional paths where to find disable-dlps.txt files.
	 * @param bVerbose
	 */
	DlpInfos(bool bEnablePlugins, const std::vector<std::string>& aEnDisablePlugins, const std::vector<std::string>& aGroups
			, const std::vector<std::string>& aPluginPaths, const std::vector<std::string>& aDisablePaths, bool bVerbose) noexcept;

	const std::vector<DlpInfo>& getOrderedDlps() const noexcept { return m_aOrderedDlpInfos; }

	/** Parses lines until next block separator or EOF.
	 * @param oInFile The input stream.
	 * @param bSkipComments Whether to skip comments.If true returned lines are trimmed.
	 * @return  The read lines.
	 */
	static std::vector<std::string> parseBlock(std::ifstream& oInFile, bool bSkipComments) noexcept;
	/** Parses lines until next block separator or EOF.
	 * @param oInFile The input stream.
	 * @param bSkipComments Whether to skip comments.If true returned lines are trimmed.
	 * @param sLatestLine [output] The block separator line. Is empty if EOF reached.
	 * @return  The read lines.
	 */
	static std::vector<std::string> parseBlock(std::ifstream& oInFile, bool bSkipComments, std::string& sLatestLine) noexcept;
	/** Remove leading and trailing spaces from string.
	 * @param sStr The string to trim.
	 * @return The trimmed string.
	 */
	static std::string trimString(const std::string& sStr) noexcept;
	/** Creates a directory and its ancestors if necessary.
	 * @param sPath The path to create.
	 * @return Whether the creation was successful.
	 */
	static bool makePath(const std::string& sPath) noexcept;

	static const std::string s_sDisabledDlpsFile;
	static const std::string s_sDlpCommentLineStart;
private:
	void getDir(bool bEnablePlugins, const std::vector<std::string>& aEnDisablePlugins
						, std::vector<DlpInfo>& aNameFiles, const std::string& sDirPath) noexcept;
	static bool parseDlpInfo(DlpInfo& oInfo) noexcept;
	static bool parseDisabledNamesFile(const std::string& sFile, std::vector<DlpInfo>& aDlps, bool bDisabledInSys) noexcept;
	static void traverse(DlpInfo& oInfo, std::vector<DlpInfo>& aDlps, std::vector<DlpInfo>& aOrderedDlps) noexcept;
	static bool dirExists(const std::string& sPath) noexcept;
private:
	std::vector<DlpInfo> m_aOrderedDlpInfos;
	bool m_bVerbose;
};

} // namespace stmi

#endif /* STMI_DLP_INFOS_H */

