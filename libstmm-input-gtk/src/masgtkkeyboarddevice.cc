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
 * File:   masgtkkeyboarddevice.cc
 */

#include "masgtkkeyboarddevice.h"
#include "masgtkwindowdata.h"
#include "masgtklistenerextradata.h"

namespace stmi
{

namespace Private
{
namespace Mas
{

GtkKeyboardDevice::~GtkKeyboardDevice()
{
//std::cout << "GtkKeyboardDevice::~GtkKeyboardDevice()  " << getId() << std::endl;
}
shared_ptr<Device> GtkKeyboardDevice::getDevice() const
{
	shared_ptr<const GtkKeyboardDevice> refConstThis = shared_from_this();
	shared_ptr<GtkKeyboardDevice> refThis = std::const_pointer_cast<GtkKeyboardDevice>(refConstThis);
	return refThis;
}
shared_ptr<Device> GtkKeyboardDevice::getDevice()
{
	shared_ptr<GtkKeyboardDevice> refThis = shared_from_this();
	return refThis;
}
shared_ptr<Capability> GtkKeyboardDevice::getCapability(const Capability::Class& oClass) const
{
	shared_ptr<Capability> refCapa;
	if (oClass == typeid(KeyCapability)) {
		shared_ptr<const GtkKeyboardDevice> refConstThis = shared_from_this();
		shared_ptr<GtkKeyboardDevice> refThis = std::const_pointer_cast<GtkKeyboardDevice>(refConstThis);
		refCapa = refThis;
	}
	return refCapa;
}
std::vector<Capability::Class> GtkKeyboardDevice::getCapabilityClasses() const
{
	return {KeyCapability::getClass()};
}
bool GtkKeyboardDevice::handleGdkEventKey(GdkEventKey* p0KeyEv, const shared_ptr<GtkWindowData>& refWindowData)
{
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

	const int32_t nHardwareKey = static_cast<int32_t>(eHardwareKey);
	const GdkEventType eGdkType = p0KeyEv->type;
	int64_t nTimePressedUsec = std::numeric_limits<int64_t>::max();
	auto itFind = m_oPressedKeys.find(nHardwareKey);
	const bool bHardwareKeyPressed = (itFind != m_oPressedKeys.end());
//std::cout << "GtkKeyboardDevice::handleGdkEventKey()  nHardwareKey=" << nHardwareKey << "  bHardwareKeyPressed=" << bHardwareKeyPressed << std::endl;
	auto refWindowAccessor = refWindowData->getAccessor();
	assert(refWindowAccessor);
	KeyEvent::KEY_INPUT_TYPE eInputType; // = ((eGdkType == GDK_KEY_PRESS) ? Event::KEY_PRESS : Event::KEY_RELEASE);
	if (eGdkType == GDK_KEY_PRESS) {
		if (bHardwareKeyPressed) {
			// Key repeat
			if (p0Owner->m_eKeyRepeatMode == KEY_REPEAT_MODE_SUPPRESS) {
				return bContinue; //--------------------------------------------
			}
			KeyEvent::KEY_INPUT_TYPE eAddInputType;
			if (p0Owner->m_eKeyRepeatMode == KEY_REPEAT_MODE_ADD_RELEASE) {
				eAddInputType = KeyEvent::KEY_RELEASE;
			} else {
				eAddInputType = KeyEvent::KEY_RELEASE_CANCEL;
			}
			KeyData& oKeyData = itFind->second;
			auto nOldTimePressedUsec = oKeyData.m_nPressedTimeUsec;
			m_oPressedKeys.erase(itFind);
			shared_ptr<KeyEvent> refEvent;
			const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
			for (auto& p0ListenerData : *refListeners) {
				sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nOldTimePressedUsec, eAddInputType, eHardwareKey
										, refWindowAccessor, p0Owner, refEvent);
			}
			if (!refWindowData->isEnabled()) {
				// the accessor was removed during the callbacks!
				return bContinue; //--------------------------------------------
			}
//std::cout << "  handleGdkEventKey erase 1 nHardwareKey=" << nHardwareKey << std::endl;
		}
		nTimePressedUsec = DeviceManager::getNowTimeMicroseconds();
		KeyData oKeyData;
		oKeyData.m_nPressedTimeUsec = nTimePressedUsec;
		m_oPressedKeys.emplace(nHardwareKey, oKeyData);
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
		nTimePressedUsec = oKeyData.m_nPressedTimeUsec;
		eInputType = KeyEvent::KEY_RELEASE;
//std::cout << "  handleGdkEventKey release nHardwareKey=" << nHardwareKey << std::endl;
		m_oPressedKeys.erase(itFind);
	}
	shared_ptr<KeyEvent> refEvent;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& p0ListenerData : *refListeners) {
		sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nTimePressedUsec
								, eInputType, eHardwareKey, refWindowAccessor, p0Owner, refEvent);
		if ((eInputType == KeyEvent::KEY_PRESS) && (m_oPressedKeys.find(nHardwareKey) == m_oPressedKeys.end())) {
			// The key was canceled by the callback (ex. through removeAccessor)
			break; // for -------
		}
	}
	return bContinue;
}
void GtkKeyboardDevice::finalizeListener(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec)
{
//std::cout << "GtkKeyboardDevice::finalizeListener m_oPressedKeys.size()=" << m_oPressedKeys.size() << std::endl;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	if (!p0Owner->getEventClassEnabled(typeid(KeyEvent))) {
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
		const auto nKeyPressedTime = oKeyData.m_nPressedTimeUsec;
		const HARDWARE_KEY eHardwareKey = static_cast<HARDWARE_KEY>(nHardwareKey);
		//
		if (p0ExtraData->isKeyCanceled(nHardwareKey)) {
			continue; // for ------------
		}
		p0ExtraData->setKeyCanceled(nHardwareKey);
		//
		shared_ptr<KeyEvent> refEvent;
		sendKeyEventToListener(oListenerData, nEventTimeUsec, nKeyPressedTime, KeyEvent::KEY_RELEASE_CANCEL
								, eHardwareKey, refSelectedAccessor, p0Owner, refEvent);
		if (!refSelected) {
			// There can't be pressed keys without an active window
			break; // for -----------
		}
	}
}
void GtkKeyboardDevice::removingDevice()
{
	resetOwnerDeviceManager();
}
void GtkKeyboardDevice::cancelSelectedAccessorKeys()
{
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	if (!p0Owner->getEventClassEnabled(typeid(KeyEvent))) {
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
		const auto nKeyPressedTime = oKeyData.m_nPressedTimeUsec;
		//
		shared_ptr<KeyEvent> refEvent;
		for (auto& p0ListenerData : *refListeners) {
			MasGtkListenerExtraData* p0ExtraData = nullptr;
			p0ListenerData->getExtraData(p0ExtraData);
			if (p0ExtraData->isKeyCanceled(nHardwareKey)) {
				continue; // for itListenerData ------------
			}
			p0ExtraData->setKeyCanceled(nHardwareKey);
			sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nKeyPressedTime, KeyEvent::KEY_RELEASE_CANCEL
									, eHardwareKey, refSelectedAccessor, p0Owner, refEvent);
		}
	}
	// Remove the keys
	m_oPressedKeys.clear();
}
void GtkKeyboardDevice::sendKeyEventToListener(const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
												, int64_t nTimePressedUsec
												, KeyEvent::KEY_INPUT_TYPE eInputType, HARDWARE_KEY eHardwareKey
												, const shared_ptr<GtkAccessor>& refAccessor
												, MasGtkDeviceManager* p0Owner
												, shared_ptr<KeyEvent>& refEvent)
{
	const int64_t nAddTimeUsec = oListenerData.getAddedTime();
//std::cout << "GtkKeyboardDevice::sendKeyEventToListener nAddTimeUsec=" << nAddTimeUsec;
//std::cout << "  nTimePressedUsec=" << nTimePressedUsec << "  nEventTimeUsec=" << nEventTimeUsec;
//std::cout << "  &GtkmmWindow=" << (int64_t)refAccessor->getGtkmmWindow() << "  &accessor=" << (int64_t)refAccessor.operator->();
	if (nTimePressedUsec < nAddTimeUsec) {
		// The listener was added after the key was pressed
//std::cout << " listener LATE" << std::endl;
		return;
	}
//std::cout << std::endl;
	if (!refEvent) {
		refEvent = std::make_shared<KeyEvent>(nEventTimeUsec, refAccessor
									, p0Owner->m_refKeyboardDevice, eInputType, eHardwareKey);
	}
	oListenerData.handleEventCallIf(p0Owner->m_nClassIdxKeyEvent, refEvent);
		// no need to reset because KeyEvent cannot be modified.
		//TODO Recycling  refEvent->set   you need to subclass KeyEvent and expose setXXX!
}

} // namespace Mas
} // namespace Private

} // namespace stmi
