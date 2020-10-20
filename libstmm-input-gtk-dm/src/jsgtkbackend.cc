/*
 * Copyright © 2016-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   jsgtkbackend.cc
 */

#include "jsgtkbackend.h"

#include "joysticksources.h"
#include "jsgtkjoystickdevice.h"
#include "jsdevicefiles.h"

#include <cassert>
#include <set>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <tuple>
#include <type_traits>

#include <linux/joystick.h>
#include <linux/input-event-codes.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>


namespace stmi
{

namespace Private
{
namespace Js
{

const char* const GtkBackend::s_sDefaultPathBase = "/dev/input/js";

std::pair<unique_ptr<GtkBackend>, std::string> GtkBackend::create(JsGtkDeviceManager* p0Owner, const JsDeviceFiles& oDeviceFiles, bool bCreateDefault) noexcept
{
	auto refInstance = unique_ptr<GtkBackend>(new GtkBackend(p0Owner));
	std::string sError = refInstance->init(oDeviceFiles, bCreateDefault);
	if (! sError.empty()) {
		return std::make_pair(unique_ptr<GtkBackend>{}, std::move(sError));
	}
	return std::make_pair(std::move(refInstance), "");
}

GtkBackend::GtkBackend(JsGtkDeviceManager* p0Owner) noexcept
: m_p0Owner(p0Owner)
{
	assert(p0Owner != nullptr);
}
std::string GtkBackend::init(const JsDeviceFiles& oDeviceFiles, bool bCreateDefault) noexcept
{
	const auto& aPathName = oDeviceFiles.getFiles();
	const auto& aPathBaseName = oDeviceFiles.getBaseNrFiles();
	// create a set (automatically avoids duplicates)
	std::set< std::tuple< std::string, std::string, bool > > oDevices; //TODO NOT NEEDED!
	for (auto& sPathName : aPathName) {
		std::string sPath;
		std::string sName;
		const bool bOk = splitPathName(sPathName, sPath, sName);
		if (bOk) {
			oDevices.emplace(sPath, sName, false);
		} else {
			std::string sError = "JsGtkDeviceManager::init() malformed sPathName=" + sPathName;
			return sError; //---------------------------------------------------
		}
	}
	const bool bNoDevices = (aPathName.empty() && aPathBaseName.empty());
	//
	bCreateDefault = bCreateDefault && bNoDevices;
	auto aDefaultPathName = std::vector<std::string>({s_sDefaultPathBase});
	const auto& aAllPaths = (bCreateDefault ? aDefaultPathName : aPathBaseName);
	for (auto& sPathBaseName : aAllPaths) {
		std::string sPath;
		std::string sBaseName;
		const bool bOk = splitPathName(sPathBaseName, sPath, sBaseName);
		if (bOk) {
			oDevices.emplace(sPath, sBaseName, true);
		} else {
			std::string sError = "JsGtkDeviceManager::init() malformed sPathBaseName=" + sPathBaseName;
			return sError; //---------------------------------------------------
		}
	}
	//
	// Create an INotify life source for each directory
	for (auto itTuple = oDevices.cbegin(); itTuple != oDevices.cend(); ++itTuple) {
		std::string sPath;
		std::string sBaseName;
		bool bNrs;
		std::tie(sPath, sBaseName, bNrs) = *itTuple;
//std::cout << "JsGtkDeviceManager::init() sPath=" << sPath << " sBaseName=" << sBaseName << " bNrs=" << bNrs << '\n';
		const bool bOk = cleanPath(sPath);
		if (!bOk) {
			// malformed path
			std::string sError = "JsGtkDeviceManager::init() malformed sPath=" + sPath;
			return sError; //---------------------------------------------------
		}
		auto itFindPath = m_oPathSources.find(sPath);
		const bool bCreatePathSource = (itFindPath == m_oPathSources.end());
		if (bCreatePathSource) {
//std::cout << "JsGtkDeviceManager::init() create PathSources for " << sPath << '\n';
			// The source is created but the connection is established later.
			auto oPair = m_oPathSources.emplace(sPath, std::make_shared<JoystickLifeSource>(sPath));
			itFindPath = oPair.first;
			assert(oPair.second);
		}
		shared_ptr<JoystickLifeSource>& refPathSource = itFindPath->second;
		if (bNrs) {
			refPathSource->addNumberedFilesBaseName(sBaseName);
		} else {
			refPathSource->addSingleFileName(sBaseName);
		}
	}
	return addDevices();
}
std::string GtkBackend::addDevices() noexcept
{
	for (auto& oPair : m_oPathSources) {
		// Create a joystick for each file in the directory that matches the allowed names.
		shared_ptr<JoystickLifeSource>& refPathSource = oPair.second;
		refPathSource->connect(sigc::mem_fun(this, &GtkBackend::doINotifyEventCallback));
		refPathSource->attach();
		//
		const std::string& sPath = refPathSource->getPath();
		try {
			Glib::Dir oDirectory(sPath);
			auto itFile = oDirectory.begin();
			//
			while (itFile != oDirectory.end()) {
				const std::string& sName = *itFile;
				if (refPathSource->isFileNameMine(sName)) {
					std::string sPathName;
					if (! composePathName(sPath, sName, sPathName)) {
						std::string sError = "JsGtkDeviceManager::init() malformed sPath=" + sPath + "  sName=" + sName;
						return sError; //---------------------------------------
					}
					const bool bNewJoystick = addIfJoystick(sPathName);
					if (bNewJoystick) {
//std::cout << "JsGtkDeviceManager::init() joystick added: " << sPathName << '\n';
					}
				}
				++itFile;
			}
		} catch (Glib::FileError& eError) {
			// sPath probably doesn't exist, skip
			std::cout << "JsGtkDeviceManager::init() file error: " << eError.what() << '\n';
		}
	}
	return "";
}
bool GtkBackend::doINotifyEventCallback(const std::string& sPath, const std::string& sName, bool bAdd) noexcept
{
	const bool bContinue = true;
	//
	std::string sPathName;
	if (!composePathName(sPath, sName, sPathName)) {
		return bContinue;
	}
	if (bAdd) {
		const Glib::RefPtr<DevInitTimeoutSource> refTimeOut(new DevInitTimeoutSource(s_nTimeoutRetryMsec, sPathName));
		refTimeOut->connectSlot(sigc::mem_fun(this, &GtkBackend::doTimeoutSourceCallback));
		refTimeOut->attach();
	} else { // Remove
		auto itFind = std::find_if(m_aInputSources.begin(), m_aInputSources.end()
				, [&](const Glib::RefPtr<JoystickInputSource>& refSource)
					{
						return (sPathName == refSource->getJoystickPathName());
					});
		if (itFind == m_aInputSources.end()) {
			return bContinue; // -----------------------------------------------
		}
		const Glib::RefPtr<JoystickInputSource>& refSource = *itFind;
		//
		onDeviceRemoved(refSource->getJoystickId());
		//
		m_aInputSources.erase(itFind);
		//
	}
	return bContinue;
}
bool GtkBackend::doTimeoutSourceCallback(const std::string& sPathName, int32_t nElapsedMillisec) noexcept
{
	const bool bContinue = true;
	//
	auto itFind = std::find_if(m_aInputSources.begin(), m_aInputSources.end()
			, [&](const Glib::RefPtr<JoystickInputSource>& refSource)
				{
					return (sPathName == refSource->getJoystickPathName());
				});
	if (itFind != m_aInputSources.end()) {
		// already created, destroy timeout source
		return !bContinue; //---------------------------------------------------
	}
	const bool bNewJoystick = addIfJoystick(sPathName);
	if (!bNewJoystick) {
		if (nElapsedMillisec <= s_nTimeoutMaxRetryMsec) {
			// try for 4 seconds (each second)
			return bContinue; // -----------------------------------------------
		}
		// Failed to create, destroy timeout source
		return !bContinue; // --------------------------------------------------
	}
	// It's a joystick
	return bContinue;
}
bool GtkBackend::splitPathName(const std::string& sPathName, std::string& sPath, std::string& sName) noexcept
{
	try {
		const Glib::ustring sUtf8PathName = Glib::filename_to_utf8(sPathName);
		const std::size_t nSlashPos = sUtf8PathName.rfind("/");
		if (nSlashPos != Glib::ustring::npos) {
			sPath = Glib::filename_from_utf8(sUtf8PathName.substr(0, nSlashPos)); // don't include the found slash
			sName = Glib::filename_from_utf8(sUtf8PathName.substr(nSlashPos + 1));
		} else {
			sPath.clear();
			sName = sPathName;
		}
	} catch (Glib::ConvertError&) {
		return false;
	}
	return true;
}
bool GtkBackend::composePathName(const std::string& sPathParam, const std::string& sName, std::string& sPathName) noexcept
{
	std::string sPath = sPathParam;
	if (!cleanPath(sPath)) {
		return false;
	}
	try {
		const Glib::ustring sUtf8Path = Glib::filename_to_utf8(sPath);
		const Glib::ustring sUtf8Name = Glib::filename_to_utf8(sName);
		Glib::ustring sUtf8PathName = sUtf8Path;
		if (sUtf8Path != "/") {
			sUtf8PathName.append("/");
		}
		sUtf8PathName.append(sUtf8Name);
		sPathName = Glib::filename_from_utf8(sUtf8PathName);
	} catch (Glib::ConvertError&) {
		return false;
	}
	return true;
}
bool GtkBackend::cleanPath(std::string& sPath) noexcept
{
	if (sPath.empty()) {
		return true;
	}
	Glib::ustring sUtf8Path;
	try {
		sUtf8Path = Glib::filename_to_utf8(sPath);
	} catch (Glib::ConvertError&) {
		return false;
	}
	if (sUtf8Path.compare("/") == 0) {
		// Root
		return true;
	}
	auto nLastNotSlash = sUtf8Path.find_last_not_of("/");
	if (nLastNotSlash == Glib::ustring::npos) {
		// All slashes, malformed
		return false;
	}
	auto nLen = sUtf8Path.size();
	// Ex. nLen = 5
	// 0123456789
	// path/
	//    ^ nLastNotSlash = 3
	if (nLastNotSlash < nLen - 2) {
		// more than one trailing slash, error
		return false;
	}
	sUtf8Path = sUtf8Path.substr(0, nLastNotSlash + 1);  // eliminate trailing slash, if present
//std::cout << "cleanPath 5 sUtf8Path=" << sUtf8Path << '\n';
	try {
		sPath = Glib::filename_from_utf8(sUtf8Path);
	} catch (Glib::ConvertError&) {
		return false;
	}
	return true;
}

struct RAII_Fd
{
	explicit RAII_Fd(int32_t nFD) noexcept : m_nFD(nFD) {}
	~RAII_Fd() noexcept { if (m_nFD >= 0) { ::close(m_nFD); } }
	void dontClose() noexcept { m_nFD = -1; }
	int32_t m_nFD;
};
bool GtkBackend::addIfJoystick(const std::string& sPathName) noexcept
{
	//bool bReadOnly = false;
	// Try Read+Write for feedback
	int32_t nFD = open(sPathName.c_str(), O_RDWR | O_NONBLOCK, 0);
	if (nFD < 0) {
		// Some kind of error: try readonly
		nFD = open(sPathName.c_str(), O_RDONLY | O_NONBLOCK, 0);
		if (nFD < 0) {
			// Some kind of error
			return false; //----------------------------------------------------
		}
		// No force feedback??
		//bReadOnly = true;
	}

	RAII_Fd oRAII(nFD);

	static_assert(sizeof(unsigned long) <= sizeof(int64_t), ""); // st_rdev
	struct stat oSB;
	if (stat(sPathName.c_str(), &oSB) == -1) {
		return false; //--------------------------------------------------------
	}
	// Check for uniqueness
	const int64_t nFileSysDeviceId  = oSB.st_rdev;
	auto itFind = std::find_if(m_aInputSources.begin(), m_aInputSources.end()
			, [&](const Glib::RefPtr<JoystickInputSource>& refSource)
				{
					return (nFileSysDeviceId == refSource->getJoystickFileSysDeviceId());
				});
	if (itFind != m_aInputSources.end()) {
		// already added
		return false;
	}
	//
	char aDeviceName[1024] = {0};
	if (ioctl(nFD, JSIOCGNAME(sizeof aDeviceName), aDeviceName) < 0) {
		return false; //--------------------------------------------------------
	}
	const std::string sDeviceName(aDeviceName);

	uint8_t nTotAxis = 0;
	uint8_t nTotButtons = 0;
	ioctl(nFD, JSIOCGAXES,	&nTotAxis);
	ioctl(nFD, JSIOCGBUTTONS, &nTotButtons);
	if ((nTotAxis == 0) || (nTotButtons == 0)) {
		return false; //--------------------------------------------------------
	}

	std::vector<int32_t> aButtonCode;
	if (!getButtonMapping(nFD, nTotButtons, aButtonCode)) {
		return false; //--------------------------------------------------------
	}
	std::vector<int32_t> aAxisCode;
	if (!getAxisMapping(nFD, nTotAxis, aAxisCode)) {
		return false; //--------------------------------------------------------
	}
	std::vector<int32_t> aAxisCodeHats;
	std::copy_if(aAxisCode.begin(), aAxisCode.end(), std::back_inserter(aAxisCodeHats)
					, [&](int32_t nAxis) { return JoystickDevice::isHatAxis(nAxis); }
				);
	std::sort(aAxisCodeHats.begin(), aAxisCodeHats.end());
	int32_t nHatAxisExpected = ABS_HAT0X;
	for (auto nHatAxisCode : aAxisCodeHats) {
		if (nHatAxisExpected != nHatAxisCode) {
			// Doubt there's a device that implements hat 0 and hat 2 but not hat 1.
			return false; //----------------------------------------------------
		}
		++nHatAxisExpected;
	}
	const int32_t nTotHatAxes = nHatAxisExpected - ABS_HAT0X;
	if ((nTotHatAxes % 2) != 0) {
		// X without Y, probably a microsoft keyboard
		return false; //--------------------------------------------------------
	}
	const int32_t nTotHats = nTotHatAxes / 2;

	const shared_ptr<Private::Js::JoystickDevice>& refNewJoystick = onDeviceAdded(sDeviceName, aButtonCode, nTotHats, aAxisCode);
	if (!refNewJoystick) {
		return false; //--------------------------------------------------------
	}
	const int32_t nDeviceId = refNewJoystick->getDeviceId();

	oRAII.dontClose(); // open file descriptor passed to JoystickInputSource

	Glib::RefPtr<JoystickInputSource> refGlibSource(new JoystickInputSource(nFD, sPathName, nFileSysDeviceId, nDeviceId));

	m_aInputSources.push_back(refGlibSource);

	refGlibSource->connect(sigc::mem_fun(*refNewJoystick, &JoystickDevice::doInputJoystickEventCallback));
	refGlibSource->attach();

	return true;
}
bool GtkBackend::getButtonMapping(int32_t nFD, int32_t nTotButtons, std::vector<int32_t>& aButtonCode) noexcept
{
	uint16_t aBtnMap[KEY_MAX - BTN_MISC + 1];
	if (ioctl(nFD, JSIOCGBTNMAP, aBtnMap) < 0) {
		return false;
	}
	std::copy(aBtnMap, aBtnMap + nTotButtons, std::back_inserter(aButtonCode));
	return true;
}
bool GtkBackend::getAxisMapping(int32_t nFD, int32_t nTotAxes, std::vector<int32_t>& aAxisCode) noexcept
{
	uint8_t aAxisMap[ABS_MAX + 1];
	if (ioctl(nFD, JSIOCGAXMAP, aAxisMap) < 0) {
		return false;
	}
	std::copy(aAxisMap, aAxisMap + nTotAxes, std::back_inserter(aAxisCode));
	return true;
}

} // namespace Js
} // namespace Private

} // namespace stmi
