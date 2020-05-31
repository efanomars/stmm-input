/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   masgtkkeyboarddevice.cc
 */

#include "masgtkkeyboarddevice.h"

#include "masgtkwindowdata.h"
#include "masgtklistenerextradata.h"

#include <stmm-input-base/basicdevicemanager.h>
#include <stmm-input-gtk/gdkkeyconverter.h>
#include <stmm-input-gtk/keyrepeatmode.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/event.h>

#include <cstdint>
#include <limits>
#include <utility>
#include <cassert>

namespace stmi { class Device; }
namespace stmi { class GtkAccessor; }

namespace stmi
{

namespace Private
{
namespace Mas
{

GtkKeyboardDevice::~GtkKeyboardDevice() noexcept
{
//std::cout << "GtkKeyboardDevice::~GtkKeyboardDevice()  " << getId() << std::endl;
}
shared_ptr<Device> GtkKeyboardDevice::getDevice() const noexcept
{
	shared_ptr<const GtkKeyboardDevice> refConstThis = shared_from_this();
	shared_ptr<GtkKeyboardDevice> refThis = std::const_pointer_cast<GtkKeyboardDevice>(refConstThis);
	return refThis;
}
shared_ptr<Capability> GtkKeyboardDevice::getCapability(const Capability::Class& oClass) const noexcept
{
	shared_ptr<Capability> refCapa;
	if (oClass == typeid(KeyCapability)) {
		shared_ptr<const GtkKeyboardDevice> refConstThis = shared_from_this();
		shared_ptr<GtkKeyboardDevice> refThis = std::const_pointer_cast<GtkKeyboardDevice>(refConstThis);
		refCapa = refThis;
	}
	return refCapa;
}
shared_ptr<Capability> GtkKeyboardDevice::getCapability(int32_t nCapabilityId) const noexcept
{
	const auto nKeyCapaId = KeyCapability::getId();
	if (nCapabilityId != nKeyCapaId) {
		return shared_ptr<Capability>{};
	}
	shared_ptr<const GtkKeyboardDevice> refConstThis = shared_from_this();
	shared_ptr<GtkKeyboardDevice> refThis = std::const_pointer_cast<GtkKeyboardDevice>(refConstThis);
	return refThis;
}
std::vector<int32_t> GtkKeyboardDevice::getCapabilities() const noexcept
{
	return {KeyCapability::getId()};
}
std::vector<Capability::Class> GtkKeyboardDevice::getCapabilityClasses() const noexcept
{
	return {KeyCapability::getClass()};
}
bool GtkKeyboardDevice::handleGdkEventKey(GdkEventKey* p0KeyEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept
{
//std::cout << "GtkKeyboardDevice::handleGdkEventKey()  nHardwareKey=" << p0KeyEv->hardware_keycode << '\n';
	const bool bContinue = true;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return !bContinue;
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	if (p0Owner->m_refSelected != refWindowData) {
		// This device manager suppresses events sent to a not active window
		// (ex. the parent of a modal dialog)
		return bContinue; //----------------------------------------------------
	}

	HARDWARE_KEY eHardwareKey;
	if (!p0Owner->m_oConverter.convertEventKeyToHardwareKey(p0KeyEv, eHardwareKey)) {
		return bContinue; //----------------------------------------------------
	}
	auto refListeners = p0Owner->getListeners();

	const GdkEventType eGdkType = p0KeyEv->type;
	uint64_t nTimePressedStamp = std::numeric_limits<uint64_t>::max();
	auto itFind = m_oPressedKeys.find(eHardwareKey);
	const bool bHardwareKeyPressed = (itFind != m_oPressedKeys.end());
//std::cout << "GtkKeyboardDevice::handleGdkEventKey()  nHardwareKey=" << nHardwareKey << "  bHardwareKeyPressed=" << bHardwareKeyPressed << std::endl;
	auto refWindowAccessor = refWindowData->getAccessor();
	assert(refWindowAccessor);
	KeyEvent::KEY_INPUT_TYPE eInputType; // = ((eGdkType == GDK_KEY_PRESS) ? Event::KEY_PRESS : Event::KEY_RELEASE);
	if (eGdkType == GDK_KEY_PRESS) {
		if (bHardwareKeyPressed) {
			// Key repeat
			if (p0Owner->m_eKeyRepeatMode == KeyRepeat::MODE_SUPPRESS) {
				return bContinue; //--------------------------------------------
			}
			KeyEvent::KEY_INPUT_TYPE eAddInputType;
			if (p0Owner->m_eKeyRepeatMode == KeyRepeat::MODE_ADD_RELEASE) {
				eAddInputType = KeyEvent::KEY_RELEASE;
			} else {
				eAddInputType = KeyEvent::KEY_RELEASE_CANCEL;
			}
			KeyData& oKeyData = itFind->second;
			auto nOldTimePressedStamp = oKeyData.m_nPressedTimeStamp;
			m_oPressedKeys.erase(itFind);
			shared_ptr<Event> refEvent;
			const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
			for (auto& p0ListenerData : *refListeners) {
				sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nOldTimePressedStamp, eAddInputType, eHardwareKey
										, refWindowAccessor, p0Owner, refEvent);
			}
			if (!refWindowData->isEnabled()) {
				// the accessor was removed during the callbacks!
				return bContinue; //--------------------------------------------
			}
//std::cout << "  handleGdkEventKey erase 1 nHardwareKey=" << nHardwareKey << std::endl;
		}
		nTimePressedStamp = MasGtkDeviceManager::getUniqueTimeStamp();
		KeyData oKeyData;
		oKeyData.m_nPressedTimeStamp = nTimePressedStamp;
		m_oPressedKeys.emplace(eHardwareKey, oKeyData);
//std::cout << "  handleGdkEventKey add 1 nHardwareKey=" << nHardwareKey << std::endl;
		eInputType = KeyEvent::KEY_PRESS;
	} else {
		assert(eGdkType == GDK_KEY_RELEASE);
		if (!bHardwareKeyPressed) {
			// orphan release, ignore
//std::cout << "  handleGdkEventKey orphan release nHardwareKey=" << nHardwareKey << std::endl;
			return bContinue; //------------------------------------------------
		}
		KeyData& oKeyData = itFind->second;
		nTimePressedStamp = oKeyData.m_nPressedTimeStamp;
		eInputType = KeyEvent::KEY_RELEASE;
//std::cout << "  handleGdkEventKey release nHardwareKey=" << nHardwareKey << std::endl;
		m_oPressedKeys.erase(itFind);
	}
	shared_ptr<Event> refEvent;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& p0ListenerData : *refListeners) {
		sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nTimePressedStamp
								, eInputType, eHardwareKey, refWindowAccessor, p0Owner, refEvent);
		if ((eInputType == KeyEvent::KEY_PRESS) && (m_oPressedKeys.find(eHardwareKey) == m_oPressedKeys.end())) {
			// The key was canceled by the callback (ex. through removeAccessor)
			break; // for -------
		}
	}
	return bContinue;
}
void GtkKeyboardDevice::finalizeListener(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec) noexcept
{
//std::cout << "GtkKeyboardDevice::finalizeListener m_oPressedKeys.size()=" << m_oPressedKeys.size() << std::endl;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	if (!p0Owner->isEventClassEnabled(Event::Class{typeid(KeyEvent)})) {
		return; //--------------------------------------------------------------
	}
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; //--------------------------------------------------------------
	}
	auto refSelectedAccessor = refSelected->getAccessor();

	MasGtkListenerExtraData* p0ExtraData = nullptr;
	oListenerData.getExtraData(p0ExtraData);

	// Send KEY_RELEASE_CANCEL for the currently pressed keys to the listener
	// work on copy
	auto oPressedKeys = m_oPressedKeys;
	for (auto& oPair : oPressedKeys) {
		const int32_t nHardwareKey = oPair.first;
		const KeyData& oKeyData = oPair.second;
		//
		const auto nKeyPressedTimeStamp = oKeyData.m_nPressedTimeStamp;
		const HARDWARE_KEY eHardwareKey = static_cast<HARDWARE_KEY>(nHardwareKey);
		//
		if (p0ExtraData->isKeyCanceled(nHardwareKey)) {
			continue; // for ------------
		}
		p0ExtraData->setKeyCanceled(nHardwareKey);
		//
		shared_ptr<Event> refEvent;
		sendKeyEventToListener(oListenerData, nEventTimeUsec, nKeyPressedTimeStamp, KeyEvent::KEY_RELEASE_CANCEL
								, eHardwareKey, refSelectedAccessor, p0Owner, refEvent);
		if (!refSelected) {
			// There can't be pressed keys without an active window
			break; // for -----------
		}
	}
}
void GtkKeyboardDevice::removingDevice() noexcept
{
	resetOwnerDeviceManager();
}
void GtkKeyboardDevice::cancelSelectedAccessorKeys() noexcept
{
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	if (!p0Owner->isEventClassEnabled(Event::Class{typeid(KeyEvent)})) {
		return; // -------------------------------------------------------------
	}
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; // -------------------------------------------------------------
	}
	auto refSelectedAccessor = p0Owner->m_refSelected->getAccessor();
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();

	auto refListeners = p0Owner->getListeners();
	// Cancel all keys generated by the selected accessor (window)
	// work on copy
	auto oPressedKeys = m_oPressedKeys;
	for (auto& oPair : oPressedKeys) {
		const int32_t nHardwareKey = oPair.first;
		const KeyData& oKeyData = oPair.second;
		const HARDWARE_KEY eHardwareKey = static_cast<HARDWARE_KEY>(nHardwareKey);
		const auto nKeyPressedTimeStamp = oKeyData.m_nPressedTimeStamp;
		//
		shared_ptr<Event> refEvent;
		for (auto& p0ListenerData : *refListeners) {
			MasGtkListenerExtraData* p0ExtraData = nullptr;
			p0ListenerData->getExtraData(p0ExtraData);
			if (p0ExtraData->isKeyCanceled(nHardwareKey)) {
				continue; // for itListenerData ------------
			}
			p0ExtraData->setKeyCanceled(nHardwareKey);
			sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nKeyPressedTimeStamp, KeyEvent::KEY_RELEASE_CANCEL
									, eHardwareKey, refSelectedAccessor, p0Owner, refEvent);
		}
	}
	// Remove the keys
	m_oPressedKeys.clear();
}
void GtkKeyboardDevice::sendKeyEventToListener(const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
												, uint64_t nTimePressedStamp
												, KeyEvent::KEY_INPUT_TYPE eInputType, HARDWARE_KEY eHardwareKey
												, const shared_ptr<GtkAccessor>& refAccessor
												, MasGtkDeviceManager* p0Owner
												, shared_ptr<Event>& refEvent) noexcept
{
	const uint64_t nAddTimeStamp = oListenerData.getAddedTimeStamp();
//std::cout << "GtkKeyboardDevice::sendKeyEventToListener nAddTimeUsec=" << nAddTimeUsec;
//std::cout << "  nTimePressedUsec=" << nTimePressedUsec << "  nEventTimeUsec=" << nEventTimeUsec;
//std::cout << "  &GtkmmWindow=" << (int64_t)refAccessor->getGtkmmWindow() << "  &accessor=" << (int64_t)refAccessor.operator->();
	if (nTimePressedStamp < nAddTimeStamp) {
		// The listener was added after the key was pressed
//std::cout << " listener LATE" << std::endl;
		return;
	}
//std::cout << std::endl;
	if (!refEvent) {
		m_oKeyEventRecycler.create(refEvent, nEventTimeUsec, refAccessor, p0Owner->m_refKeyboardDevice, eInputType, eHardwareKey);
	}
	oListenerData.handleEventCallIf(p0Owner->m_nClassIdxKeyEvent, refEvent);
		// no need to reset because KeyEvent cannot be modified.
}

} // namespace Mas
} // namespace Private

} // namespace stmi
