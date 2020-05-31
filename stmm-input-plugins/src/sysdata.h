/*
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   sysdata.h
 */

#ifndef STMI_SYS_DATA_H
#define STMI_SYS_DATA_H

#include "dlpinfos.h"

#include <vector>
#include <string>
#include <memory>

#include <stdint.h>


namespace stmi
{

using std::unique_ptr;

class SysData
{
public:
	static SysData getSysData() noexcept;

	const std::string& getError() const noexcept { return m_sError; }

	static std::string getVersionString() noexcept;

	const DlpInfos& getDlpInfos() const noexcept;
	const DlpInfo* findPlugin(const std::string& sPluginName) const noexcept;

	/** The paths where disable-dlps.txt files can be found.
	 * If not empty the getLocalDataPath() is the last.
	 * @return The paths.
	 */
	const std::vector<std::string>& getDisablePaths() const noexcept;
	/** The local path.
	 * Ex. /home/john/.local/share/etc/stmm-input-dl
	 * @return Can be empty (root).
	 */
	const std::string& getLocalDataPath() const noexcept { return m_sLocalDataPath; }
	const std::vector<std::string>& getSysDataPaths() const noexcept { return m_aDataPaths; }
	bool isRoot() const noexcept { return m_bIsRoot; }

	int dropPrivileges() const noexcept;

private:
	std::vector<std::string> m_aDataPaths; // need root access, usually: {/usr/local/etc/stmm-input-dl, /etc/stmm-input-dl}
	std::string m_sLocalDataPath; // in user's $HOME, ex. /home/john/.local/etc/stmm-input-dl
	std::string m_sError;
	bool m_bIsRoot = false;
	int32_t m_nUID = -1; // The user. If isRoot() is true, the user that called sudo.
	int32_t m_nGID = -1; // The group. If isRoot() is true, the group that called sudo.
	unique_ptr<DlpInfos> m_refDlpInfos;
	std::vector<std::string> m_aDisablePaths; // m_aDataPaths + [m_sLocalDataPath]
};

#endif /* STMI_SYS_DATA_H */

} // namespace stmi

