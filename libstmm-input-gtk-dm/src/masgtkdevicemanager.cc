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
 * File:   masgtkdevicemanager.cc
 */

#include "masgtkdevicemanager.h"
#include "masgtkwindowdata.h"
#include "masgtkkeyboarddevice.h"
#include "masgtkpointerdevice.h"

#include "masgtkbackend.h"

#include <stmm-input-gtk/gdkkeyconverter.h>

#include <stmm-input-ev/devicemgmtevent.h>
#include <stmm-input-ev/keycapability.h>
#include <stmm-input-ev/keyevent.h>
#include <stmm-input-ev/pointercapability.h>
#include <stmm-input-ev/pointerevent.h>
#include <stmm-input-ev/touchcapability.h>
#include <stmm-input-ev/touchevent.h>
#include <stmm-input-gtk/gtkaccessor.h>
#include <stmm-input/capability.h>
#include <stmm-input/devicemanager.h>

#include <type_traits>
#include <cassert>

namespace stmi { class Accessor; }
namespace stmi { class ChildDeviceManager; }

namespace stmi
{

using Private::Mas::GtkBackend;
using Private::Mas::GtkWindowData;
using Private::Mas::GtkWindowDataFactory;
using Private::Mas::GtkKeyboardDevice;
using Private::Mas::GtkPointerDevice;
using Private::Mas::MasGtkListenerExtraData;

std::pair<shared_ptr<MasGtkDeviceManager>, std::string> MasGtkDeviceManager::create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses
																					, KeyRepeat::MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter
																					, const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager) noexcept
{
	shared_ptr<MasGtkDeviceManager> refInstance(new MasGtkDeviceManager(bEnableEventClasses, aEnDisableEventClasses
																		, eKeyRepeatMode, refGdkConverter));
	auto refBackend = std::make_unique<GtkBackend>(refInstance.operator->(), refGdkDeviceManager);
	auto refFactory = std::make_unique<GtkWindowDataFactory>();
	refInstance->init(refFactory, refBackend);
	return std::make_pair(refInstance, "");
}

MasGtkDeviceManager::MasGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses
										, KeyRepeat::MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter) noexcept
: StdDeviceManager({Capability::Class{typeid(KeyCapability)}, Capability::Class{typeid(PointerCapability)}
						, Capability::Class{typeid(TouchCapability)}}
					, {Event::Class{typeid(DeviceMgmtEvent)}, Event::Class{typeid(KeyEvent)}, Event::Class{typeid(PointerEvent)}
						, Event::Class{typeid(PointerScrollEvent)}, Event::Class{typeid(TouchEvent)}}
					, bEnableEventClasses, aEnDisableEventClasses)
, m_nCancelingNestedDepth(0)
, m_eKeyRepeatMode((eKeyRepeatMode == KeyRepeat::MODE_NOT_SET) ? KeyRepeat::getMode() : eKeyRepeatMode)
, m_refGdkConverter(refGdkConverter ? refGdkConverter : GdkKeyConverter::getConverter())
, m_oConverter(*m_refGdkConverter)
, m_nClassIdxKeyEvent(getEventClassIndex(Event::Class{typeid(KeyEvent)}))
, m_nClassIdxPointerEvent(getEventClassIndex(Event::Class{typeid(PointerEvent)}))
, m_nClassIdxPointerScrollEvent(getEventClassIndex(Event::Class{typeid(PointerScrollEvent)}))
, m_nClassIdxTouchEvent(getEventClassIndex(Event::Class{typeid(TouchEvent)}))
{
	assert(m_refGdkConverter);
	assert((m_eKeyRepeatMode >= KeyRepeat::MODE_SUPPRESS) && (m_eKeyRepeatMode <= KeyRepeat::MODE_ADD_RELEASE_CANCEL));
	// The whole implementation of this class is based on this assumption
	static_assert(sizeof(int) <= sizeof(int32_t), "");
}
MasGtkDeviceManager::~MasGtkDeviceManager() noexcept
{
//std::cout << "MasGtkDeviceManager::~MasGtkDeviceManager()" << '\n';
}
void MasGtkDeviceManager::enableEventClass(const Event::Class& oEventClass) noexcept
{
	StdDeviceManager::enableEventClass(oEventClass);
	adjustConnectionsAfterEnablingClass();
}
void MasGtkDeviceManager::adjustConnectionsAfterEnablingClass() noexcept
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
void MasGtkDeviceManager::init(std::unique_ptr<GtkWindowDataFactory>& refFactory, std::unique_ptr<GtkBackend>& refBackend) noexcept
{
	assert(refFactory);
	assert(refBackend);
	m_refFactory.swap(refFactory);
	m_refBackend.swap(refBackend);
	addDevices();
}
void MasGtkDeviceManager::onDeviceChanged(bool bPointer) noexcept
{
	if (bPointer) {
		sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_CHANGED, m_refPointerDevice);
	} else {
		sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_CHANGED, m_refKeyboardDevice);
	}
}
void MasGtkDeviceManager::onDevicePairAdded() noexcept
{
	addDevices();
}
void MasGtkDeviceManager::onDevicePairRemoved() noexcept
{
	removeDevices();
}
void MasGtkDeviceManager::addDevices() noexcept
{
	if (!m_refBackend) {
		// init() wasn't called yet
		return;
	}
	assert(!m_refKeyboardDevice);
	assert(!m_refPointerDevice);

	shared_ptr<ChildDeviceManager> refChildThis = shared_from_this();
	assert(std::dynamic_pointer_cast<MasGtkDeviceManager>(refChildThis));
	auto refThis = std::static_pointer_cast<MasGtkDeviceManager>(refChildThis);

	m_refKeyboardDevice = std::make_shared<GtkKeyboardDevice>(m_refBackend->getKeyboardDeviceName(), refThis);
	#ifndef NDEBUG
	const bool bKeyboardAdded =
	#endif //NDEBUG
	StdDeviceManager::addDevice(m_refKeyboardDevice);
	assert(bKeyboardAdded);
	m_refPointerDevice = std::make_shared<GtkPointerDevice>(m_refBackend->getPointerDeviceName(), refThis);
	#ifndef NDEBUG
	const bool bPointerAdded =
	#endif //NDEBUG
	StdDeviceManager::addDevice(m_refPointerDevice);
	assert(bPointerAdded);
	//
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_ADDED, m_refKeyboardDevice);
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_ADDED, m_refPointerDevice);
}
void MasGtkDeviceManager::removeDevices() noexcept
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
void MasGtkDeviceManager::finalizeListener(ListenerData& oListenerData) noexcept
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
				, std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itFind) noexcept
{
	for (itFind = m_aGtkWindowData.begin(); itFind != m_aGtkWindowData.end(); ++itFind) {
		if (itFind->first == p0GtkmmWindow) {
			return true;
		}
	}
	return false;
}
bool MasGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor, bool& bValid
				, std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itFind) noexcept
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
bool MasGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor) noexcept
{
	bool bValid;
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	return hasAccessor(refAccessor, bValid, itFind);
}
bool MasGtkDeviceManager::addAccessor(const shared_ptr<Accessor>& refAccessor) noexcept
{
//std::cout << "MasGtkDeviceManager::addAccessor 0" << '\n';
	assert(m_refBackend);
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
	if (!m_refBackend->isCompatible(refGtkAccessor)) {
		// wrong display
		return false; //--------------------------------------------------------
	}

	Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
	m_aGtkWindowData.emplace_back(p0GtkmmWindow, m_refFactory->create());
	shared_ptr<GtkWindowData> refData = m_aGtkWindowData.back().second;
	GtkWindowData& oData = *refData;

	oData.enable(refGtkAccessor, this);

	oData.connect();

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
bool MasGtkDeviceManager::removeAccessor(const shared_ptr<Accessor>& refAccessor) noexcept
{
	assert(m_refBackend);
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	bool bValid;
	const bool bHasAccessor = hasAccessor(refAccessor, bValid, itFind);
	if (!bHasAccessor) {
		return false; //--------------------------------------------------------
	}
//std::cout << "MasGtkDeviceManager::removeAccessor &oWindowData=" << &(itFind->second) << '\n';
	removeAccessor(itFind);
	return true;
}
void MasGtkDeviceManager::removeAccessor(const std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itGtkData) noexcept
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
		// refData reference avoids recycling during cancel callbacks!
		deselectAccessor();
	}
}
void MasGtkDeviceManager::deselectAccessor() noexcept
{
//std::cout << "MasGtkDeviceManager::deselectAccessor  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << '\n';
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
void MasGtkDeviceManager::cancelDevices() noexcept
{
//std::cout << "MasGtkDeviceManager::cancelDeviceKeys  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << '\n';
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
void MasGtkDeviceManager::onIsActiveChanged(const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "MasGtkDeviceManager::onIsActiveChanged 0" << '\n';
	if (!refWindowData->isEnabled()) {
		return;
	}
	auto& refGtkAccessor = refWindowData->getAccessor();
	bool bIsActive = false;
	if (!refGtkAccessor->isDeleted()) {
		bIsActive = refWindowData->isWindowActive();
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
bool MasGtkDeviceManager::onKeyPress(GdkEventKey* p0KeyEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "MasGtkDeviceManager::onKeyPress 0 " << p0KeyEv->keyval << '\n';
	if (!m_refKeyboardDevice) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refKeyboardDevice->handleGdkEventKey(p0KeyEv, refWindowData);
	return false; // propagate
}
bool MasGtkDeviceManager::onKeyRelease(GdkEventKey* p0KeyEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "MasGtkDeviceManager::onKeyRelease 0 " << p0KeyEv->keyval << '\n';
	if (!m_refKeyboardDevice) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refKeyboardDevice->handleGdkEventKey(p0KeyEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onMotionNotify(GdkEventMotion* p0MotionEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "MasGtkDeviceManager::onMotionNotify" << '\n';
	if (!m_refPointerDevice) {
		return false;
	}
	if (! m_refBackend->isPointerDevice(p0MotionEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventMotion(p0MotionEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onButtonPress(GdkEventButton* p0ButtonEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "MasGtkDeviceManager::onButtonPress" << '\n';
	if (!m_refPointerDevice) {
		return false;
	}
	if (! m_refBackend->isPointerDevice(p0ButtonEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventButton(p0ButtonEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onButtonRelease(GdkEventButton* p0ButtonEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "MasGtkDeviceManager::on_button_release" << '\n';
	if (!m_refPointerDevice) {
		return false;
	}
	if (! m_refBackend->isPointerDevice(p0ButtonEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventButton(p0ButtonEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onScroll(GdkEventScroll* p0ScrollEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "MasGtkDeviceManager::on_scroll" << '\n';
	if (!m_refPointerDevice) {
		return false;
	}
	if (! m_refBackend->isPointerDevice(p0ScrollEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventScroll(p0ScrollEv, refWindowData);
	return false;
}
bool MasGtkDeviceManager::onTouch(GdkEventTouch* p0TouchEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "MasGtkDeviceManager::on_touch  type=" << p0TouchEv->type << "  seq=" << (int64_t)p0TouchEv->sequence << '\n';
	if (!m_refPointerDevice) {
		return false;
	}
	if (! m_refBackend->isPointerDevice(p0TouchEv->device)) {
		return false;
	}
	assert(refWindowData->getAccessor());
	m_refPointerDevice->handleGdkEventTouch(p0TouchEv, refWindowData);
//std::cout << "MasGtkDeviceManager::on_touch  AFTER type=" << p0TouchEv->type << "  seq=" << (int64_t)p0TouchEv->sequence << '\n';
	return false;
}

} // namespace stmi
