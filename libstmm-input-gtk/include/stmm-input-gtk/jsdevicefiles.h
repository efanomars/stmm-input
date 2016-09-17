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
 * File:   jsdevicefiles.h
 */

#include <vector>
#include <string>

#ifndef _STMI_JS_DEVICE_FILES_H_
#define _STMI_JS_DEVICE_FILES_H_

namespace stmi
{

/** Joystick device files initialization class for JsGtkDeviceManager.
 */
class JsDeviceFiles
{
public:
	/** Adds a single device file.
	 * Example: "/dev/input/js7" or "/dev/special/joystick"
	 * @param sPathName Absolute path filename.
	 */
	void addFile(const std::string& sPathName)
	{
		if (sPathName.empty()) {
			return;
		}
		m_aPathName.push_back(sPathName);

	}
	/** Adds numbered device files.
	 * This includes all the files with the given base filename followed by
	 * a positive integer number.
	 *
	 * Example: "/dev/input/js" adds {"/dev/input/js0", "/dev/input/js1", ..., "/dev/input/js43", ... }
	 * @param sPathBaseName Absolute path base of filename.
	 */
	void addBaseNrFiles(const std::string& sPathBaseName)
	{
		if (sPathBaseName.empty()) {
			return;
		}
		m_aPathBaseName.push_back(sPathBaseName);
	}
	inline const std::vector<std::string>& getFiles() const { return m_aPathName; }
	inline const std::vector<std::string>& getBaseNrFiles() const { return m_aPathBaseName; }
private:
	//friend class stmi::Private::Js::GtkBackend;
	std::vector<std::string> m_aPathName;
	std::vector<std::string> m_aPathBaseName;
};

} // namespace stmi

#endif	/* _STMI_JS_DEVICE_FILES_H_ */
