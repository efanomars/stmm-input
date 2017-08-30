/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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

#include "jsgtkjoystickdevice.h"
#include "jsgtklistenerextradata.h"
#include "jsgtkwindowdata.h"
#include "jsgtkbackend.h"

#include <stmm-input-ev/joystickevent.h>
#include <stmm-input/device.h>

#include <giomm/file.h>
#include <glibmm/fileutils.h>

#include <sstream>

namespace stmi
{

////////////////////////////////////////////////////////////////////////////////
using Private::Js::GtkBackend;
using Private::Js::JoystickDevice;
using Private::Js::GtkWindowData;
using Private::Js::GtkWindowDataFactory;
using Private::Js::JsGtkListenerExtraData;

shared_ptr<JsGtkDeviceManager> JsGtkDeviceManager::create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses
														, const JsDeviceFiles& oDeviceFiles)
{
	shared_ptr<JsGtkDeviceManager> refInstance(new JsGtkDeviceManager(bEnableEventClasses, aEnDisableEventClasses));
	auto refBackend = std::make_unique<GtkBackend>(refInstance.operator->(), oDeviceFiles);
	auto refFactory = std::make_unique<GtkWindowDataFactory>();
	refInstance->init(refFactory, refBackend);
	return refInstance;
}

JsGtkDeviceManager::JsGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses)
: StdDeviceManager({typeid(JoystickCapability)}
					, {typeid(DeviceMgmtEvent), typeid(JoystickButtonEvent), typeid(JoystickHatEvent), typeid(JoystickAxisEvent)}
					, bEnableEventClasses, aEnDisableEventClasses)
, m_nCancelingNestedDepth(0)
, m_nHatEventTypeEnabledTimeStamp(std::numeric_limits<uint64_t>::max())
, m_nClassIdxJoystickButtonEvent(getEventClassIndex(typeid(JoystickButtonEvent)))
, m_nClassIdxJoystickHatEvent(getEventClassIndex(typeid(JoystickHatEvent)))
, m_nClassIdxJoystickAxisEvent(getEventClassIndex(typeid(JoystickAxisEvent)))
{
}
JsGtkDeviceManager::~JsGtkDeviceManager()
{
//std::cout << "JsGtkDeviceManager::~JsGtkDeviceManager()" << std::endl;
}
void JsGtkDeviceManager::init(std::unique_ptr<Private::Js::GtkWindowDataFactory>& refFactory
								, std::unique_ptr<Private::Js::GtkBackend>& refBackend)
{
	assert(refFactory);
	assert(refBackend);
	m_refFactory.swap(refFactory);
	m_refBackend.swap(refBackend);
	if (isEventClassEnabled(typeid(JoystickHatEvent))) {
		m_nHatEventTypeEnabledTimeStamp = 0;
	}
}
void JsGtkDeviceManager::enableEventClass(const Event::Class& oEventClass)
{
	const bool bIsHatEventClass = (oEventClass == typeid(JoystickHatEvent));
	const bool bWasHatEventTypeEnabled = bIsHatEventClass && isEventClassEnabled(JoystickHatEvent::getClass());
	StdDeviceManager::enableEventClass(oEventClass);
	if (bIsHatEventClass && (!bWasHatEventTypeEnabled)) {
		m_nHatEventTypeEnabledTimeStamp = StdDeviceManager::getUniqueTimeStamp();
	}
}
const shared_ptr<JoystickDevice>& JsGtkDeviceManager::onDeviceAdded(const std::string& sName, const std::vector<int32_t>& aButtonCode
												, int32_t nTotHats, const std::vector<int32_t>& aAxisCode)
{
	shared_ptr<ChildDeviceManager> refChildThis = shared_from_this();
	assert(std::dynamic_pointer_cast<JsGtkDeviceManager>(refChildThis));
	auto refThis = std::static_pointer_cast<JsGtkDeviceManager>(refChildThis);
	//
	auto refNewJoystick = std::make_shared<JoystickDevice>(sName, refThis, aButtonCode, nTotHats, aAxisCode);
	#ifndef NDEBUG
	auto itFind = std::find_if(m_aJoysticks.begin(), m_aJoysticks.end()
					, [&](const shared_ptr<Private::Js::JoystickDevice>& refJoystick)
					{
						return refJoystick->getDeviceId() == refNewJoystick->getDeviceId();
					});
	#endif
	assert(itFind == m_aJoysticks.end());
	//
	m_aJoysticks.push_back(refNewJoystick);
	//
	#ifndef NDEBUG
	const bool bAdded = 
	#endif
	StdDeviceManager::addDevice(refNewJoystick);
	assert(bAdded);
	auto& refBack = m_aJoysticks.back();
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_ADDED, refNewJoystick);
	return refBack;
}
void JsGtkDeviceManager::onDeviceRemoved(int32_t nJoystickId)
{
	auto itFind = std::find_if(m_aJoysticks.begin(), m_aJoysticks.end()
					, [&](const shared_ptr<Private::Js::JoystickDevice>& refJoystick)
					{
						return refJoystick->getDeviceId() == nJoystickId;
					});
	assert(itFind != m_aJoysticks.end());
	shared_ptr<JoystickDevice>& refJoystickDevice = *itFind;
	//
	cancelDeviceButtonsAndHats(refJoystickDevice);
	refJoystickDevice->removingDevice();

	auto refRemovedJoystickDevice = refJoystickDevice;
	#ifndef NDEBUG
	const bool bRemoved =
	#endif //NDEBUG
	StdDeviceManager::removeDevice(refJoystickDevice);
	assert(bRemoved);
	m_aJoysticks.erase(itFind);
	//
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_REMOVED, refRemovedJoystickDevice);
}
void JsGtkDeviceManager::finalizeListener(ListenerData& oListenerData)
{
	++m_nCancelingNestedDepth;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& refJoystickDevice : m_aJoysticks) {
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
	m_aGtkWindowData.emplace_back(p0GtkmmWindow, m_refFactory->create()); //getGtkWindowData()
	shared_ptr<GtkWindowData> refData = m_aGtkWindowData.back().second;
	GtkWindowData& oData = *refData;
	oData.enable(refGtkAccessor, this);

	const bool bIsActive = oData.isWindowActive();
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
	// Note: an additional shared_ptr to the object is created to avoid it
	//       being recycled during deselectAccessor()
	auto refData = itGtkData->second;

	const bool bIsSelected = (m_refSelected == refData);

	refData->disable(); // doesn't clear accessor!
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
	for (auto& refJoystick : m_aJoysticks) {
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
		bIsActive = refWindowData->isWindowActive();
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

} // namespace stmi
