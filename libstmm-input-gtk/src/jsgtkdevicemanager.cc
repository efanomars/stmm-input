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
 * File:   jsgtkdevicemanager.cc
 */

#include "jsgtkdevicemanager.h"

#include "joysticksources.h"
#include "jsgtkjoystickdevice.h"
#include "jsgtklistenerextradata.h"
#include "jsgtkwindowdata.h"

#include <stmm-input-ev/joystickevent.h>
#include <stmm-input/device.h>

#include <giomm/file.h>
#include <glibmm/fileutils.h>

#include <sstream>
#include <set>

#include <linux/joystick.h>

#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

namespace stmi
{

////////////////////////////////////////////////////////////////////////////////
using Private::Js::JoystickLifeSource;
using Private::Js::DevInitTimeoutSource;
using Private::Js::JoystickInputSource;
using Private::Js::JoystickDevice;
using Private::Js::GtkWindowData;
using Private::Js::JsGtkListenerExtraData;

const char* const JsGtkDeviceManager::s_sDefaultPathBase = "/dev/input/js";

void JsGtkDeviceManager::DeviceFiles::addFile(const std::string& sPathName)
{
	if (sPathName.empty()) {
		return;
	}
	m_aPathName.push_back(sPathName);

}
void JsGtkDeviceManager::DeviceFiles::addBaseNrFiles(const std::string& sPathBaseName)
{
	if (sPathBaseName.empty()) {
		return;
	}
	m_aPathBaseName.push_back(sPathBaseName);
}

shared_ptr<JsGtkDeviceManager> JsGtkDeviceManager::create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
														, const DeviceFiles& oDeviceFiles)
{
	shared_ptr<JsGtkDeviceManager> refInstance(new JsGtkDeviceManager(bEnableEventClasses, aEnDisableEventClass));
	refInstance->init(oDeviceFiles);
	return refInstance;
}

JsGtkDeviceManager::JsGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass)
: StdDeviceManager({typeid(DeviceMgmtCapability), typeid(JoystickCapability)}
					, {typeid(DeviceMgmtEvent), typeid(JoystickButtonEvent), typeid(JoystickHatEvent), typeid(JoystickAxisEvent)}
					, bEnableEventClasses, aEnDisableEventClass)
, m_nCancelingNestedDepth(0)
, m_nHatEventTypeEnabledTimeUsec(std::numeric_limits<int64_t>::max())
, m_nClassIdxJoystickButtonEvent(getEventClassIndex(typeid(JoystickButtonEvent)))
, m_nClassIdxJoystickHatEvent(getEventClassIndex(typeid(JoystickHatEvent)))
, m_nClassIdxJoystickAxisEvent(getEventClassIndex(typeid(JoystickAxisEvent)))
, m_nClassIdxDeviceMgmtEvent(getEventClassIndex(typeid(DeviceMgmtEvent)))
{
}
JsGtkDeviceManager::~JsGtkDeviceManager()
{
//std::cout << "JsGtkDeviceManager::~JsGtkDeviceManager()" << std::endl;
}
shared_ptr<DeviceManager> JsGtkDeviceManager::getDeviceManager() const
{
	return getRoot();
}
shared_ptr<DeviceManager> JsGtkDeviceManager::getDeviceManager()
{
	return getRoot();
}
void JsGtkDeviceManager::init(const DeviceFiles& oDeviceFiles)
{
	if (getEventClassEnabled(typeid(JoystickHatEvent))) {
		m_nHatEventTypeEnabledTimeUsec = 0;
	}
	assert(m_refPathSources.empty());
	// create a set (automatically avoids duplicates)
	std::set< std::tuple< std::string, std::string, bool > > oDevices; //TODO NOT NEEDED!
	for (auto& sPathName : oDeviceFiles.m_aPathName) {
		std::string sPath;
		std::string sName;
		const bool bOk = splitPathName(sPathName, sPath, sName);
		if (bOk) {
			oDevices.emplace(sPath, sName, false);
		} else {
			assert(false);
			std::cout << "JsGtkDeviceManager::init() malformed sPathName=" << sPathName << std::endl;
		}
	}
	const bool bNoDevices = (oDeviceFiles.m_aPathName.empty() && oDeviceFiles.m_aPathBaseName.empty());
	for (auto& sPathBaseName : (bNoDevices ? std::vector<std::string>({s_sDefaultPathBase}) : oDeviceFiles.m_aPathBaseName)) {
		std::string sPath;
		std::string sBaseName;
		const bool bOk = splitPathName(sPathBaseName, sPath, sBaseName);
		if (bOk) {
			oDevices.emplace(sPath, sBaseName, true);
		} else {
			assert(false);
			std::cout << "JsGtkDeviceManager::init() malformed sPathBaseName=" << sPathBaseName << std::endl;
		}
	}
	//
	// Create an INotify life source for each directory
	for (auto itTuple = oDevices.cbegin(); itTuple != oDevices.cend(); ++itTuple) {
		std::string sPath;
		std::string sBaseName;
		bool bNrs;
		std::tie(sPath, sBaseName, bNrs) = *itTuple;
//std::cout << "JsGtkDeviceManager::init() sPath=" << sPath << " sBaseName=" << sBaseName << " bNrs=" << bNrs << std::endl;
		const bool bOk = cleanPath(sPath);
		if (!bOk) {
			// malformed path, instead of throwing we skip it
			assert(false);
			std::cout << "JsGtkDeviceManager::init() malformed sPath=" << sPath << std::endl;
			continue; // for ----------
		}
		auto itFindPath = m_refPathSources.find(sPath);
		const bool bCreatePathSource = (itFindPath == m_refPathSources.end());
		if (bCreatePathSource) {
//std::cout << "JsGtkDeviceManager::init() create PathSources for " << sPath << std::endl;
			// The source is created but the connection is established later.
			auto oPair = m_refPathSources.emplace(sPath, std::make_shared<JoystickLifeSource>(sPath));
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
	for (auto& oPair : m_refPathSources) {
		// Create a joystick for each file in the directory that matches the allowed names.
		shared_ptr<JoystickLifeSource>& refPathSource = oPair.second;
		refPathSource->connect(sigc::mem_fun(this, &JsGtkDeviceManager::doINotifyEventCallback));
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
						assert(false);
						std::cout << "JsGtkDeviceManager::init() malformed sPath=" << sPath << "  sName=" << sName << std::endl;
						continue; // while--------------------------------------
					}
					shared_ptr<JoystickDevice> refJoy = addIfJoystick(sPathName);
					if (refJoy) {
//std::cout << "JsGtkDeviceManager::init() joystick added: " << sPathName << std::endl;
					}
				}
				++itFile;
			}
		} catch (Glib::FileError& eError) {
			// sPath is probably malformed or doesn't exist, just skip
			std::cout << "JsGtkDeviceManager::init() file error: " << eError.what() << std::endl;
		}
	}
}
void JsGtkDeviceManager::enableEventClass(const Event::Class& oEventClass)
{
	const bool bIsHatEventTypeEnabled = getEventClassEnabled(JoystickHatEvent::getClass());
	StdDeviceManager::enableEventClass(oEventClass);
	if ((!bIsHatEventTypeEnabled) && (oEventClass == typeid(JoystickHatEvent))) {
		m_nHatEventTypeEnabledTimeUsec = DeviceManager::getNowTimeMicroseconds();
	}
}
bool JsGtkDeviceManager::doINotifyEventCallback(const std::string& sPath, const std::string& sName, bool bAdd)
{
	const bool bContinue = true;
	//
	std::string sPathName;
	if (!composePathName(sPath, sName, sPathName)) {
		return bContinue;
	}
	if (bAdd) {
		const Glib::RefPtr<DevInitTimeoutSource> refTimeOut(new DevInitTimeoutSource(s_nTimeoutRetryMsec, sPathName));
		refTimeOut->connectSlot(sigc::mem_fun(this, &JsGtkDeviceManager::doTimeoutSourceCallback));
		refTimeOut->attach();
	} else { // Remove
		auto itFind = m_oJoystickByPathName.find(sPathName);
		if (itFind == m_oJoystickByPathName.end()) {
			return bContinue; // -----------------------------------------------
		}
		shared_ptr<JoystickDevice>& refJoystickDevice = itFind->second;
		cancelDeviceButtonsAndHats(refJoystickDevice);
		refJoystickDevice->removingDevice();

		auto refRemovedJoystickDevice = refJoystickDevice;
		#ifndef NDEBUG
		const bool bRemoved =
		#endif //NDEBUG
		StdDeviceManager::removeDevice(refJoystickDevice);
		assert(bRemoved);
		m_oJoystickByPathName.erase(itFind);
		m_oJoysticksById.erase(refJoystickDevice->getId());
		//
		sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_REMOVED, refRemovedJoystickDevice);
	}
	return bContinue;
}
bool JsGtkDeviceManager::doTimeoutSourceCallback(const std::string& sPathName, int32_t nElapsedMillisec)
{
	const bool bContinue = true;
	//
	auto itFind = m_oJoystickByPathName.find(sPathName);
	if (itFind != m_oJoystickByPathName.end()) {
		// already created, destroy timeout source
		return !bContinue;
	}
	std::shared_ptr<JoystickDevice> refNewJoystick = addIfJoystick(sPathName);
	if (!refNewJoystick) {
		;
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
void JsGtkDeviceManager::sendDeviceMgmtToListeners(const DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice)
{
	if (!getEventClassEnabled(typeid(DeviceMgmtEvent))) {
		return;
	}
	shared_ptr<ChildDeviceManager> refChildThis = shared_from_this();
	assert(std::dynamic_pointer_cast<JsGtkDeviceManager>(refChildThis));
	auto refThis = std::static_pointer_cast<JsGtkDeviceManager>(refChildThis);
	auto refCapa = std::static_pointer_cast<DeviceMgmtCapability>(refThis);
	//
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	//
	shared_ptr<DeviceMgmtEvent> refEvent = std::make_shared<DeviceMgmtEvent>(nTimeUsec, refCapa, eMgmtType, refDevice);
	auto refListeners = getListeners();
	for (auto& p0ListenerData : *refListeners) {
		p0ListenerData->handleEventCallIf(m_nClassIdxDeviceMgmtEvent, refEvent);
	}
}
struct RAII_Fd
{
	RAII_Fd(int32_t nFD) : m_nFD(nFD) {}
	~RAII_Fd() { if (m_nFD >= 0) { ::close(m_nFD); } }
	inline void dontClose() { m_nFD = -1; }
	int32_t m_nFD;
};
shared_ptr<JoystickDevice> JsGtkDeviceManager::addIfJoystick(const std::string& sPathName)
{
	shared_ptr<JoystickDevice> refNewJoystick;

	//bool bReadOnly = false;
	// Try Read+Write for feedback
	int32_t nFD = open(sPathName.c_str(), O_RDWR | O_NONBLOCK, 0);
	if (nFD < 0) {
		// Some kind of error: try readonly
		nFD = open(sPathName.c_str(), O_RDONLY | O_NONBLOCK, 0);
		if (nFD < 0) {
			// Some kind of error
			return refNewJoystick; // ------------------------------------------
		}
		// No force feedback??
		//bReadOnly = true;
	}

	RAII_Fd oRAII(nFD);

	static_assert(sizeof(unsigned long) <= sizeof(int64_t), ""); // st_rdev
	struct stat oSB;
	if (stat(sPathName.c_str(), &oSB) == -1) {
		return refNewJoystick; // ----------------------------------------------
	}
	// Check for uniqueness
	const int64_t nFileSysDeviceId  = oSB.st_rdev;
	for (auto& oPair : m_oJoysticksById) {
		const shared_ptr<JoystickDevice>& refJoystick = oPair.second;
		if (nFileSysDeviceId == refJoystick->m_nFileSysDeviceId) {
			return refNewJoystick; // ------------------------------------------
		}
	}
	//
	char aDeviceName[1024] = {0};
	if (ioctl(nFD, JSIOCGNAME(sizeof aDeviceName), aDeviceName) < 0) {
		return refNewJoystick; // ----------------------------------------------
	}
	const std::string sDeviceName(aDeviceName);

	uint8_t nTotAxis = 0;
	uint8_t nTotButtons = 0;
	ioctl(nFD, JSIOCGAXES,	&nTotAxis);
	ioctl(nFD, JSIOCGBUTTONS, &nTotButtons);
	if ((nTotAxis <= 0) || (nTotButtons <= 0)) {
		return refNewJoystick; // ----------------------------------------------
	}

	std::vector<int32_t> aButtonCode;
	if (!getButtonMapping(nFD, nTotButtons, aButtonCode)) {
		return refNewJoystick; // ----------------------------------------------
	}
	std::vector<int32_t> aAxisCode;
	if (!getAxisMapping(nFD, nTotAxis, aAxisCode)) {
		return refNewJoystick; // ----------------------------------------------
	}
	std::vector<int32_t> aAxisCodeHats;
	std::copy_if(aAxisCode.begin(), aAxisCode.end(), std::back_inserter(aAxisCodeHats)
					, [&](int32_t nAxis) { return Private::Js::isHatAxis(static_cast<JoystickCapability::AXIS>(nAxis)); }
				);
	std::sort(aAxisCodeHats.begin(), aAxisCodeHats.end());
	int32_t nHatAxisExpected = ABS_HAT0X;
	for (auto nHatAxisCode : aAxisCodeHats) {
		if (nHatAxisExpected != nHatAxisCode) {
			// Doubt there's a device that implements hat 0 and hat 2 but not hat 1.
			return refNewJoystick; // ------------------------------------------
		}
		++nHatAxisExpected;
	}
	const int32_t nTotHatAxes = nHatAxisExpected - ABS_HAT0X;
	if ((nTotHatAxes % 2) != 0) {
		// X without Y, probably a microsoft keyboard
		return refNewJoystick; // ----------------------------------------------
	}
	const int32_t nTotHats = nTotHatAxes / 2;

	//
	shared_ptr<ChildDeviceManager> refChildThis = shared_from_this();
	assert(std::dynamic_pointer_cast<JsGtkDeviceManager>(refChildThis));
	auto refThis = std::static_pointer_cast<JsGtkDeviceManager>(refChildThis);
	//
	oRAII.dontClose(); // open file descriptor passed to joystick
	//
	refNewJoystick = std::make_shared<JoystickDevice>(sDeviceName, refThis, nFD, nFileSysDeviceId, aButtonCode, nTotHats, aAxisCode);
	{
		#ifndef NDEBUG
		auto oPairRes = 
		#endif //NDEBUG
		m_oJoysticksById.emplace(refNewJoystick->getId(), refNewJoystick);
		#ifndef NDEBUG
		const bool bInserted = oPairRes.second;
		#endif //NDEBUG
		assert(bInserted);
	}
	{
		auto oPairRes = m_oJoystickByPathName.emplace(sPathName, refNewJoystick);
		const bool bInserted = oPairRes.second;
		if (!bInserted) {
			m_oJoysticksById.erase(refNewJoystick->getId());
			refNewJoystick.reset();
			return refNewJoystick; // ------------------------------------------
		}
	}
	const bool bAdded = StdDeviceManager::addDevice(refNewJoystick);
	if (!bAdded) {
		assert(false);
		refNewJoystick.reset();
		return refNewJoystick; // ----------------------------------------------
	}

	Glib::RefPtr<JoystickInputSource> refGlibSource(new JoystickInputSource(nFD, sPathName));

	refNewJoystick->m_refSource = refGlibSource;

	refGlibSource->connect(sigc::mem_fun(*refNewJoystick, &JoystickDevice::doInputJoystickEventCallback));
	refGlibSource->attach();

	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_ADDED, refNewJoystick);

	return refNewJoystick;
}
bool JsGtkDeviceManager::getButtonMapping(int32_t nFD, int32_t nTotButtons, std::vector<int32_t>& aButtonCode)
{
	uint16_t aBtnMap[KEY_MAX - BTN_MISC + 1];
	if (ioctl(nFD, JSIOCGBTNMAP, aBtnMap) < 0) {
		return false;
	}
	std::copy(aBtnMap, aBtnMap + nTotButtons, std::back_inserter(aButtonCode));
	return true;
}
bool JsGtkDeviceManager::getAxisMapping(int32_t nFD, int32_t nTotAxes, std::vector<int32_t>& aAxisCode)
{
	uint8_t aAxisMap[ABS_MAX + 1];
	if (ioctl(nFD, JSIOCGAXMAP, aAxisMap) < 0) {
		return false;
	}
	std::copy(aAxisMap, aAxisMap + nTotAxes, std::back_inserter(aAxisCode));
	return true;
}
bool JsGtkDeviceManager::splitPathName(const std::string& sPathName, std::string& sPath, std::string& sName)
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
	} catch (Glib::ConvertError) {
		return false;
	}
	return true;
}
bool JsGtkDeviceManager::composePathName(const std::string& sPathParam, const std::string& sName, std::string& sPathName)
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
	} catch (Glib::ConvertError) {
		return false;
	}
	return true;
}
bool JsGtkDeviceManager::cleanPath(std::string& sPath)
{
	if (sPath.empty()) {
		return true;
	}
	Glib::ustring sUtf8Path;
	try {
		sUtf8Path = Glib::filename_to_utf8(sPath);
	} catch (Glib::ConvertError) {
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
//std::cout << "cleanPath 5 sUtf8Path=" << sUtf8Path << std::endl;
	try {
		sPath = Glib::filename_from_utf8(sUtf8Path);
	} catch (Glib::ConvertError) {
		return false;
	}
	return true;
}
void JsGtkDeviceManager::finalizeListener(ListenerData& oListenerData)
{
	++m_nCancelingNestedDepth;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& oPair : m_oJoysticksById) {
		shared_ptr<JoystickDevice>& refJoystickDevice = oPair.second;
		refJoystickDevice->finalizeListener(oListenerData, nEventTimeUsec);
	}
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
bool JsGtkDeviceManager::findWindow(Gtk::Window* p0GtkmmWindow
				, std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itFind)
{
	for (itFind = m_aGtkWindowData.begin(); itFind != m_aGtkWindowData.end(); ++itFind) {
		if (itFind->first == p0GtkmmWindow) {
			return true;
		}
	}
	return false;
}
bool JsGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor, bool& bValid
				, std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itFind)
{
	bValid = false;
	if (!refAccessor) {
		return false; //--------------------------------------------------------
	}
	shared_ptr<GtkAccessor> refGtkAccessor = std::dynamic_pointer_cast<GtkAccessor>(refAccessor);
	if (!refGtkAccessor) {
		return false; //--------------------------------------------------------
	}
	bValid = true;
	// Note: might be that refGtkAccessor->isDeleted()
	Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
	const bool bFoundWindow = findWindow(p0GtkmmWindow, itFind);
	return bFoundWindow;
}
bool JsGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor)
{
	bool bValid;
	std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	return hasAccessor(refAccessor, bValid, itFind);
}
bool JsGtkDeviceManager::addAccessor(const shared_ptr<Accessor>& refAccessor)
{
	std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	bool bValid;
	const bool bHasAccessor = hasAccessor(refAccessor, bValid, itFind);
	if (!bValid) {
		return false; //--------------------------------------------------------
	}
	shared_ptr<GtkAccessor> refGtkAccessor = std::static_pointer_cast<GtkAccessor>(refAccessor);
	if (refGtkAccessor->isDeleted()) {
		// the window was already destroyed, unusable
		// but if a zombie GtkAccessorData is still around, remove it
		if (bHasAccessor) {
			removeAccessor(itFind);
		}
		return false; //--------------------------------------------------------
	}
	if (bHasAccessor) {
		// Accessor is already present
		return false; //--------------------------------------------------------
	}
	Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
	m_aGtkWindowData.emplace_back(p0GtkmmWindow, getGtkWindowData());
	shared_ptr<GtkWindowData> refData = m_aGtkWindowData.back().second;
	GtkWindowData& oData = *refData;
	oData.enable(refGtkAccessor, this);

	const bool bIsActive = p0GtkmmWindow->get_realized() && p0GtkmmWindow->get_visible() && p0GtkmmWindow->is_active();
	if (!m_refSelected) {
		if (bIsActive) {
			m_refSelected = refData;
		}
	} else {
		if (bIsActive) {
			auto refSelectedAccessor = m_refSelected->getAccessor();
			if (refSelectedAccessor->isDeleted()) {
				Gtk::Window* p0SelectedGtkmmWindow = refSelectedAccessor->getGtkmmWindow();
				std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itSelectedFind;
				#ifndef NDEBUG
				const bool bFoundWindow = 
				#endif //NDEBUG
				findWindow(p0SelectedGtkmmWindow, itSelectedFind);
				assert(bFoundWindow);
				removeAccessor(itSelectedFind);
			} else {
				// This shouldn't happen: the added window is active while another is still selected.
				deselectAccessor();
			}
			m_refSelected = refData;
		} else {
			deselectAccessor();
		}
	}
	return true;
}
bool JsGtkDeviceManager::removeAccessor(const shared_ptr<Accessor>& refAccessor)
{
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	bool bValid;
	const bool bHasAccessor = hasAccessor(refAccessor, bValid, itFind);
	if (!bHasAccessor) {
		return false; //--------------------------------------------------------
	}
//std::cout << "JsGtkDeviceManager::removeAccessor &oWindowData=" << &(itFind->second) << std::endl;
	removeAccessor(itFind);
	return true;
}
void JsGtkDeviceManager::removeAccessor(const std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itGtkData)
{
	assert(itGtkData->second);
	// Keep a reference so that it doesn't get recycled
	auto refData = itGtkData->second;
	const bool bIsSelected = (m_refSelected == refData);

	refData->disable(); // doesn't clear accessor!

	// move from m_oWindows to m_aFreePool
	m_aFreePool.emplace_back();
	m_aFreePool.back().swap(itGtkData->second);
	assert(!itGtkData->second);
	assert(m_aFreePool.back());
	//
	m_aGtkWindowData.erase(itGtkData);

	if (bIsSelected) {
		deselectAccessor();
	}
}
void JsGtkDeviceManager::selectAccessor(const shared_ptr<GtkWindowData>& refData)
{
//std::cout << "JsGtkDeviceManager::selectAccessor  accessor=" << (int64_t)&(*(refData->getAccessor())) << std::endl;
	m_refSelected = refData;
}
void JsGtkDeviceManager::deselectAccessor()
{
//std::cout << "JsGtkDeviceManager::deselectAccessor  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << std::endl;
	++m_nCancelingNestedDepth;
	//
	for (auto& oPair : m_oJoysticksById) {
		shared_ptr<JoystickDevice>& refJoystick = oPair.second;
		// cancel all buttons and hats that are pressed for the currently selected accessor
		refJoystick->cancelSelectedAccessorButtonsAndHats();
	}
	m_refSelected.reset();
	//
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
void JsGtkDeviceManager::cancelDeviceButtonsAndHats(const shared_ptr<Private::Js::JoystickDevice>& refJoystickDevice)
{
//std::cout << "JsGtkDeviceManager::cancelDeviceButtonsAndHats  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << std::endl;
	++m_nCancelingNestedDepth;
	//
	refJoystickDevice->cancelSelectedAccessorButtonsAndHats();
	//
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
void JsGtkDeviceManager::onIsActiveChanged(const shared_ptr<GtkWindowData>& refWindowData)
{
//std::cout << "JsGtkDeviceManager::onIsActiveChanged  accessor=" << (int64_t)&(*(refWindowData->getAccessor()));
	if (!refWindowData->isEnabled()) {
//std::cout << std::endl;
		return;
	}
	auto& refGtkAccessor = refWindowData->getAccessor();
	bool bIsActive = false;
	if (!refGtkAccessor->isDeleted()) {
		auto p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
		bIsActive = p0GtkmmWindow->get_realized() && p0GtkmmWindow->get_visible() && p0GtkmmWindow->is_active();
	}
//std::cout << " active=" << bIsActive << std::endl;
	const bool bIsSelected = (refWindowData == m_refSelected);
	if (bIsActive) {
		if (bIsSelected) {
			// Activating the already active window ... shouldn't happen.
			return; //----------------------------------------------------------
		}
		if (m_refSelected) {
			deselectAccessor();
		}
		selectAccessor(refWindowData);
	} else {
		if (bIsSelected) {
			// Send cancels for open keys, buttons and sequences
			deselectAccessor();
		}
	}
}
std::shared_ptr<GtkWindowData> JsGtkDeviceManager::getGtkWindowData()
{
	const size_t nSize = m_aFreePool.size();
	size_t nIdx = 0;
	for (auto itWindowData = m_aFreePool.begin(); itWindowData != m_aFreePool.end(); ++itWindowData) {
		shared_ptr<GtkWindowData>& refGtkWindowData = *itWindowData;
		if (refGtkWindowData.unique()) {
			shared_ptr<GtkWindowData> refReuse = refGtkWindowData;
			if (nSize > 1) {
				m_aFreePool[nIdx] = m_aFreePool[nSize - 1];
			}
			m_aFreePool.pop_back();
			return refReuse; //-------------------------------------------------
		}
		++nIdx;
	}
	return std::make_shared<GtkWindowData>();
}

} // namespace stmi
