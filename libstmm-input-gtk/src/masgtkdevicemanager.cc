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
 * File:   masgtkdevicemanager.cc
 */

#include <limits>

#include <gdk/gdk.h>
#include <sigc++/connection.h>
#include <gdkmm/devicemanager.h>

#include "masgtkdevicemanager.h"
#include "gdkkeyconverter.h"
#include "masgtkwindowdata.h"
#include "masgtkkeyboarddevice.h"
#include "masgtkpointerdevice.h"
#include "masgtklistenerextradata.h"


namespace stmi
{

namespace Private
{
namespace Mas
{
void gdkDeviceManagerCallbackAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data)
{
	auto p0MasGtkDeviceManager = static_cast<MasGtkDeviceManager*>(p0Data);
	p0MasGtkDeviceManager->gdkDeviceAdded(p0DeviceManager, p0Device);
}
void gdkDeviceManagerCallbackChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data)
{
	auto p0MasGtkDeviceManager = static_cast<MasGtkDeviceManager*>(p0Data);
	p0MasGtkDeviceManager->gdkDeviceChanged(p0DeviceManager, p0Device);
}
void gdkDeviceManagerCallbackRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data)
{
	auto p0MasGtkDeviceManager = static_cast<MasGtkDeviceManager*>(p0Data);
	p0MasGtkDeviceManager->gdkDeviceRemoved(p0DeviceManager, p0Device);
}
} // namespace Mas
} // namespace Private

////////////////////////////////////////////////////////////////////////////////
using Private::Mas::GtkWindowData;
using Private::Mas::GtkKeyboardDevice;
using Private::Mas::GtkPointerDevice;
using Private::Mas::MasGtkListenerExtraData;

shared_ptr<MasGtkDeviceManager> MasGtkDeviceManager::create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
															, KEY_REPEAT_MODE eKeyRepeatMode, const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager)
{
	shared_ptr<MasGtkDeviceManager> refInstance(new MasGtkDeviceManager(bEnableEventClasses, aEnDisableEventClass, eKeyRepeatMode));
	refInstance->init(refGdkDeviceManager);
	return refInstance;
}

MasGtkDeviceManager::MasGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
										, KEY_REPEAT_MODE eKeyRepeatMode)
: StdDeviceManager({typeid(DeviceMgmtCapability), typeid(KeyCapability), typeid(PointerCapability), typeid(TouchCapability)}
					, {typeid(DeviceMgmtEvent), typeid(KeyEvent), typeid(PointerEvent), typeid(PointerScrollEvent), typeid(TouchEvent)}
					, bEnableEventClasses, aEnDisableEventClass)
, m_nCancelingNestedDepth(0)
, m_nConnectHandlerDeviceAdded(0)
, m_nConnectHandlerDeviceChanged(0)
, m_nConnectHandlerDeviceRemoved(0)
, m_eKeyRepeatMode(eKeyRepeatMode)
, m_oConverter(*GdkKeyConverter::getConverter())
, m_nClassIdxKeyEvent(getEventClassIndex(typeid(KeyEvent)))
, m_nClassIdxPointerEvent(getEventClassIndex(typeid(PointerEvent)))
, m_nClassIdxPointerScrollEvent(getEventClassIndex(typeid(PointerScrollEvent)))
, m_nClassIdxTouchEvent(getEventClassIndex(typeid(TouchEvent)))
, m_nClassIdxDeviceMgmtEvent(getEventClassIndex(typeid(DeviceMgmtEvent)))
{
}
MasGtkDeviceManager::~MasGtkDeviceManager()
{
	deinitDeviceManager();
//std::cout << "MasGtkDeviceManager::~MasGtkDeviceManager()" << std::endl;
}
shared_ptr<DeviceManager> MasGtkDeviceManager::getDeviceManager() const
{
	return getRoot();
}
shared_ptr<DeviceManager> MasGtkDeviceManager::getDeviceManager()
{
	return getRoot();
}
void MasGtkDeviceManager::initDeviceManager()
{
	assert(m_refGdkDeviceManager);
	assert(m_nConnectHandlerDeviceAdded == 0);
	GdkDeviceManager* p0GdkDeviceManager = m_refGdkDeviceManager->gobj();
	m_nConnectHandlerDeviceAdded = g_signal_connect(p0GdkDeviceManager, "device-added", G_CALLBACK(Private::Mas::gdkDeviceManagerCallbackAdded), this);
	assert(m_nConnectHandlerDeviceAdded > 0);
	m_nConnectHandlerDeviceChanged = g_signal_connect(p0GdkDeviceManager, "device-changed", G_CALLBACK(Private::Mas::gdkDeviceManagerCallbackChanged), this);
	assert(m_nConnectHandlerDeviceChanged > 0);
	m_nConnectHandlerDeviceRemoved = g_signal_connect(p0GdkDeviceManager, "device-removed", G_CALLBACK(Private::Mas::gdkDeviceManagerCallbackRemoved), this);
	assert(m_nConnectHandlerDeviceRemoved > 0);
}
void MasGtkDeviceManager::deinitDeviceManager()
{
	if (m_refGdkDeviceManager && (m_nConnectHandlerDeviceAdded > 0)) {
		GdkDeviceManager* p0GdkDeviceManager = m_refGdkDeviceManager->gobj();
		if (g_signal_handler_is_connected(p0GdkDeviceManager, m_nConnectHandlerDeviceAdded)) {
			g_signal_handler_disconnect(p0GdkDeviceManager, m_nConnectHandlerDeviceAdded);
		}
		if (g_signal_handler_is_connected(p0GdkDeviceManager, m_nConnectHandlerDeviceChanged)) {
			g_signal_handler_disconnect(p0GdkDeviceManager, m_nConnectHandlerDeviceChanged);
		}
		if (g_signal_handler_is_connected(p0GdkDeviceManager, m_nConnectHandlerDeviceRemoved)) {
			g_signal_handler_disconnect(p0GdkDeviceManager, m_nConnectHandlerDeviceRemoved);
		}
	}
}
void MasGtkDeviceManager::enableEventClass(const Event::Class& oEventClass)
{
	StdDeviceManager::enableEventClass(oEventClass);
	adjustConnectionsAfterEnablingClass();
}
void MasGtkDeviceManager::adjustConnectionsAfterEnablingClass()
{
	for (auto& oPair : m_aGtkWindowData) {
		shared_ptr<GtkWindowData>& refWindowData = oPair.second;
		auto& refAccessor = refWindowData->getAccessor();
		assert(refWindowData->isEnabled());
		if (!refAccessor->isDeleted()) {
			refWindowData->connect();
		}
	}
}
bool MasGtkDeviceManager::init(const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager)
{
	if (m_refGdkDeviceManager) {
		// Already initialized
		return false;
	}
	if (!refGdkDeviceManager) {
		// Take the default one
		Glib::RefPtr<Gdk::Display> refDefaultDisplay = Gdk::Display::get_default();
		assert(refDefaultDisplay);
		m_refGdkDeviceManager = refDefaultDisplay->get_device_manager();
	} else {
		m_refGdkDeviceManager = refGdkDeviceManager;
	}
	initDeviceManager();
	addDevices(nullptr);
	return true;
}
void MasGtkDeviceManager::addDevices(GdkDevice* p0NotThisOne)
{
	assert(m_refGdkDeviceManager);
	assert(!m_refKeyboardDevice);
	assert(!m_refPointerDevice);

	shared_ptr<ChildDeviceManager> refChildThis = shared_from_this();
	assert(std::dynamic_pointer_cast<MasGtkDeviceManager>(refChildThis));
	auto refThis = std::static_pointer_cast<MasGtkDeviceManager>(refChildThis);

	const std::vector< Glib::RefPtr< Gdk::Device > > aDevice = m_refGdkDeviceManager->list_devices(Gdk::DEVICE_TYPE_MASTER);
	for (auto& refMasterDevice : aDevice) {
		const Gdk::InputSource eSource = refMasterDevice->get_source();
		if (eSource == Gdk::SOURCE_KEYBOARD) {
			// add the master keyboard and its associated master pointer
			const Glib::RefPtr<Gdk::Device>& refKeyboard = refMasterDevice;
			const Glib::RefPtr<Gdk::Device> refPointer = refMasterDevice->get_associated_device();
			if (!((refKeyboard->gobj() == p0NotThisOne) || (refPointer->gobj() == p0NotThisOne))) {
				// We don't want to read a master device that is about to be removed
				if (!m_refKeyboardDevice) {
					m_refKeyboardDevice = std::make_shared<GtkKeyboardDevice>(refKeyboard->get_name(), refKeyboard, refThis);
					#ifndef NDEBUG
					const bool bAdded = 
					#endif //NDEBUG
					StdDeviceManager::addDevice(m_refKeyboardDevice);	
					assert(bAdded);
//std::cout << "MasGtkDeviceManager::addDevices  Master keyboard added " << m_refKeyboardDevice->getName() << std::endl;
				} else {
					// m_aKeyboardGdkDevice.push_back(p0Keyboard);
				}
				if (!m_refPointerDevice) {
					m_refPointerDevice = std::make_shared<GtkPointerDevice>(refPointer->get_name(), refPointer, refThis);
					#ifndef NDEBUG
					const bool bAdded = 
					#endif //NDEBUG
					StdDeviceManager::addDevice(m_refPointerDevice);	
					assert(bAdded);
//std::cout << "MasGtkDeviceManager::addDevices  Master pointer added " << m_refPointerDevice->getName() << std::endl;
				} else {
					// m_aPointerGdkDevice.push_back(p0Pointer);
				}
			}
		}
	}
	assert(m_refKeyboardDevice);
	assert(m_refPointerDevice);
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_ADDED, m_refKeyboardDevice);
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_ADDED, m_refPointerDevice);
}
void MasGtkDeviceManager::removeDevices()
{
	assert(m_refKeyboardDevice);
	assert(m_refPointerDevice);
	//
	cancelDevices();
	m_refKeyboardDevice->removingDevice();
	m_refPointerDevice->removingDevice();

	auto refRemovedKeyboardDevice = m_refKeyboardDevice;
	auto refRemovedPointerDevice = m_refPointerDevice;
	#ifndef NDEBUG
	const bool bRemovedK =
	#endif //NDEBUG
	StdDeviceManager::removeDevice(m_refKeyboardDevice);
	assert(bRemovedK);
	m_refKeyboardDevice.reset();

	#ifndef NDEBUG
	const bool bRemovedP =
	#endif //NDEBUG
	StdDeviceManager::removeDevice(m_refPointerDevice);
	assert(bRemovedP);
	m_refPointerDevice.reset();
	//
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_REMOVED, refRemovedKeyboardDevice);
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_REMOVED, refRemovedPointerDevice);
}
void MasGtkDeviceManager::finalizeListener(ListenerData& oListenerData)
{
	++m_nCancelingNestedDepth;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	if (m_refKeyboardDevice) {
		m_refKeyboardDevice->finalizeListener(oListenerData, nEventTimeUsec);
	}
	if (m_refPointerDevice) {
		m_refPointerDevice->finalizeListener(oListenerData, nEventTimeUsec);
	}
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
bool MasGtkDeviceManager::findWindow(Gtk::Window* p0GtkmmWindow
				, std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itFind)
{
	for (itFind = m_aGtkWindowData.begin(); itFind != m_aGtkWindowData.end(); ++itFind) {
		if (itFind->first == p0GtkmmWindow) {
			return true;
		}
	}
	return false;
}
bool MasGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor, bool& bValid
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
bool MasGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor)
{
	bool bValid;
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	return hasAccessor(refAccessor, bValid, itFind);
}
bool MasGtkDeviceManager::addAccessor(const shared_ptr<Accessor>& refAccessor)
{
	assert(m_refGdkDeviceManager);
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
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
	assert(p0GtkmmWindow != nullptr);
	Glib::RefPtr<Gdk::Display> refDisplay = p0GtkmmWindow->get_display();
	assert(refDisplay);
	Glib::RefPtr<Gdk::DeviceManager> refGdkDeviceManager = refDisplay->get_device_manager();
	assert(refGdkDeviceManager);
	if (!(m_refGdkDeviceManager == refGdkDeviceManager)) {
		// can only have one underlying gdk device manager per instance
		return false; //--------------------------------------------------------
	}
	m_aGtkWindowData.emplace_back(p0GtkmmWindow, getGtkWindowData());
	shared_ptr<GtkWindowData> refData = m_aGtkWindowData.back().second;
	GtkWindowData& oData = *refData;

	oData.enable(refGtkAccessor, this);

	oData.connect();

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
bool MasGtkDeviceManager::removeAccessor(const shared_ptr<Accessor>& refAccessor)
{
	assert(m_refGdkDeviceManager);
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	bool bValid;
	const bool bHasAccessor = hasAccessor(refAccessor, bValid, itFind);
	if (!bHasAccessor) {
		return false; //--------------------------------------------------------
	}
//std::cout << "MasGtkDeviceManager::removeAccessor &oWindowData=" << &(itFind->second) << std::endl;
	removeAccessor(itFind);
	return true;
}
void MasGtkDeviceManager::removeAccessor(const std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itGtkData)
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
		// refData reference avoids recycling during cancel callbacks!
		deselectAccessor();
	}
}
void MasGtkDeviceManager::deselectAccessor()
{
//std::cout << "MasGtkDeviceManager::deselectAccessor  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << std::endl;
	++m_nCancelingNestedDepth;
	//
	if (m_refKeyboardDevice) {
		// cancel all keys that are pressed for the selected accessor
		m_refKeyboardDevice->cancelSelectedAccessorKeys();
	}
	if (m_refPointerDevice) {
		// cancel all buttons, touches that are pressed for the selected accessor
		m_refPointerDevice->cancelSelectedAccessorButtonsAndSequences();
	}
	m_refSelected.reset();
	//
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
void MasGtkDeviceManager::cancelDevices()
{
//std::cout << "MasGtkDeviceManager::cancelDeviceKeys  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << std::endl;
	++m_nCancelingNestedDepth;
	//
	m_refKeyboardDevice->cancelSelectedAccessorKeys();
	m_refPointerDevice->cancelSelectedAccessorButtonsAndSequences();
	//
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
void MasGtkDeviceManager::onIsActiveChanged(const shared_ptr<GtkWindowData>& refWindowData)
{
	if (!refWindowData->isEnabled()) {
		return;
	}
	auto& refGtkAccessor = refWindowData->getAccessor();
	bool bIsActive = false;
	if (!refGtkAccessor->isDeleted()) {
		auto p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
		bIsActive = p0GtkmmWindow->get_realized() && p0GtkmmWindow->get_visible() && p0GtkmmWindow->is_active();
	}
	if (bIsActive) {
		if (refWindowData == m_refSelected) {
			// Activating the already active window ... shouldn't happen.
			return; //----------------------------------------------------------
		}
		if (m_refSelected) {
			deselectAccessor();
		}
		m_refSelected = refWindowData;
	} else {
		if (refWindowData == m_refSelected) {
			// Send cancels for open keys, buttons and sequences
			deselectAccessor();
			assert(!m_refSelected);
		}
	}
}
std::shared_ptr<GtkWindowData> MasGtkDeviceManager::getGtkWindowData()
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
shared_ptr<Device> MasGtkDeviceManager::findGdkMasterDevice(GdkDevice* p0MasterDevice) const
{
	shared_ptr<Device> refFoundDevice;
	if ((m_refKeyboardDevice) && (m_refKeyboardDevice->getGdkDevice()->gobj() == p0MasterDevice)) {
		refFoundDevice = m_refKeyboardDevice;
	} else if ((m_refPointerDevice) && (m_refPointerDevice->getGdkDevice()->gobj() == p0MasterDevice)) {
		refFoundDevice = m_refPointerDevice;
	}
	return refFoundDevice;
}
void MasGtkDeviceManager::sendDeviceMgmtToListeners(const DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice)
{
	if (!getEventClassEnabled(typeid(DeviceMgmtEvent))) {
		return;
	}
	shared_ptr<ChildDeviceManager> refChildThis = shared_from_this();
	assert(std::dynamic_pointer_cast<MasGtkDeviceManager>(refChildThis));
	auto refThis = std::static_pointer_cast<MasGtkDeviceManager>(refChildThis);
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
void MasGtkDeviceManager::gdkDeviceAdded(
GdkDeviceManager*
#ifndef NDEBUG
p0GdkDeviceManager
#endif //NDEBUG
, GdkDevice* p0Device)
{
	assert(p0GdkDeviceManager == m_refGdkDeviceManager->gobj());
	GdkDeviceType eDeviceType = gdk_device_get_device_type(p0Device);
	if (eDeviceType != GDK_DEVICE_TYPE_SLAVE) {
		// This manager doesn't care about floating or other master devices
		return; //--------------------------------------------------------------
	}
	// get the master
	GdkDevice* p0MasterDevice = gdk_device_get_associated_device(p0Device);
	// find it
	shared_ptr<Device> refAffectedDevice = findGdkMasterDevice(p0MasterDevice);
	if (!refAffectedDevice) {
		// The slave is being added to another master
		return; //----------------------------------------------------------
	}
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_CHANGED, refAffectedDevice);
}
void MasGtkDeviceManager::gdkDeviceChanged(
GdkDeviceManager*
#ifndef NDEBUG
p0GdkDeviceManager
#endif //NDEBUG
, GdkDevice* p0Device)
{
	assert(p0GdkDeviceManager == m_refGdkDeviceManager->gobj());
	GdkDeviceType eDeviceType = gdk_device_get_device_type(p0Device);
	if (eDeviceType != GDK_DEVICE_TYPE_MASTER) {
		// This manager has only master devices
		return; //--------------------------------------------------------------
	}
	shared_ptr<Device> refAffectedDevice = findGdkMasterDevice(p0Device);
	if (!refAffectedDevice) {
		// It's not the "main" (our) master
		return; //--------------------------------------------------------------
	}
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_CHANGED, refAffectedDevice);
}
void MasGtkDeviceManager::gdkDeviceRemoved(
GdkDeviceManager *
#ifndef NDEBUG
p0GdkDeviceManager
#endif //NDEBUG
, GdkDevice* p0Device)
{
	assert(p0GdkDeviceManager == m_refGdkDeviceManager->gobj());
	GdkDeviceType eDeviceType = gdk_device_get_device_type(p0Device);
	if (eDeviceType == GDK_DEVICE_TYPE_FLOATING) {
		// This manager has only master devices, doesn't care about floating
		return; //--------------------------------------------------------------
	}
	shared_ptr<Device> refAffectedDevice;
	if (eDeviceType == GDK_DEVICE_TYPE_SLAVE) {
//std::cout << "MasGtkDeviceManager::gdkDeviceRemoved  SLAVE REMOVED" << std::endl;
		// get the master
		GdkDevice* p0MasterDevice = gdk_device_get_associated_device(p0Device);
		// find it
		refAffectedDevice = findGdkMasterDevice(p0MasterDevice);
		if (!refAffectedDevice) {
			// It isn't a slave of "our" master
			return; //----------------------------------------------------------
		}
		// Removing one of its slaves has changed the master
		sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_CHANGED, refAffectedDevice);
	} else {
		// removing a master device
		assert(eDeviceType == GDK_DEVICE_TYPE_MASTER);
		refAffectedDevice = findGdkMasterDevice(p0Device);
		if (!refAffectedDevice) {
			// It's another master, we don't care
			return; //----------------------------------------------------------
		}
		removeDevices();
		// One of "our" masters was removed, add the alternative master pair (keyboard+pointer)
		// there always should be one
		addDevices(p0Device);
	}
}

bool MasGtkDeviceManager::onKeyPress(GdkEventKey* p0KeyEv, const shared_ptr<GtkWindowData>& refWindowData)
{
//std::cout << "MasGtkDeviceManager::on_key_press " << p0KeyEv->keyval << std::endl;
	if (!m_refKeyboardDevice) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refKeyboardDevice->handleGdkEventKey(p0KeyEv, refWindowData);
	return false; // propagate
}
bool MasGtkDeviceManager::onKeyRelease(GdkEventKey* p0KeyEv, const shared_ptr<GtkWindowData>& refWindowData)
{
//std::cout << "MasGtkDeviceManager::on_key_release " << p0KeyEv->keyval << std::endl;
	if (!m_refKeyboardDevice) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refKeyboardDevice->handleGdkEventKey(p0KeyEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onMotionNotify(GdkEventMotion* p0MotionEv, const shared_ptr<GtkWindowData>& refWindowData)
{
	if (!m_refPointerDevice) {
		return false;
	}
	if (! (m_refPointerDevice->m_refPointer->gobj() == p0MotionEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventMotion(p0MotionEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onButtonPress(GdkEventButton* p0ButtonEv, const shared_ptr<GtkWindowData>& refWindowData)
{
//std::cout << "MasGtkDeviceManager::on_button_press" << std::endl;
	if (!m_refPointerDevice) {
		return false;
	}
	if (! (m_refPointerDevice->m_refPointer->gobj() == p0ButtonEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventButton(p0ButtonEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onButtonRelease(GdkEventButton* p0ButtonEv, const shared_ptr<GtkWindowData>& refWindowData)
{
//std::cout << "MasGtkDeviceManager::on_button_release" << std::endl;
	if (!m_refPointerDevice) {
		return false;
	}
	if (! (m_refPointerDevice->m_refPointer->gobj() == p0ButtonEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventButton(p0ButtonEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onScroll(GdkEventScroll* p0ScrollEv, const shared_ptr<GtkWindowData>& refWindowData)
{
//std::cout << "MasGtkDeviceManager::on_scroll" << std::endl;
	if (!m_refPointerDevice) {
		return false;
	}
	if (! (m_refPointerDevice->m_refPointer->gobj() == p0ScrollEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventScroll(p0ScrollEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onTouch(GdkEventTouch* p0TouchEv, const shared_ptr<GtkWindowData>& refWindowData)
{
//std::cout << "MasGtkDeviceManager::on_touch  type=" << p0TouchEv->type << "  seq=" << (int64_t)p0TouchEv->sequence << std::endl;
	if (!m_refPointerDevice) {
		return false;
	}
	if (! (m_refPointerDevice->m_refPointer->gobj() == p0TouchEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventTouch(p0TouchEv, refWindowData);
//std::cout << "MasGtkDeviceManager::on_touch  AFTER type=" << p0TouchEv->type << "  seq=" << (int64_t)p0TouchEv->sequence << std::endl;
	return false;
}

} // namespace stmi
