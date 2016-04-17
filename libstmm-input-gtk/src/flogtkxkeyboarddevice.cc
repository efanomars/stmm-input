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
 * File:   flogtkxkeyboarddevice.cc
 */

#include "flogtkxkeyboarddevice.h"
#include "flogtkwindowdata.h"
#include "flogtklistenerextradata.h"

namespace stmi
{

namespace Private
{
namespace Flo
{

GtkXKeyboardDevice::~GtkXKeyboardDevice()
{
//std::cout << "GtkXKeyboardDevice::~GtkXKeyboardDevice() " << getId() << std::endl;
}
shared_ptr<Device> GtkXKeyboardDevice::getDevice() const
{
	shared_ptr<const GtkXKeyboardDevice> refConstThis = shared_from_this();
	shared_ptr<GtkXKeyboardDevice> refThis = std::const_pointer_cast<GtkXKeyboardDevice>(refConstThis);
	return refThis;
}
shared_ptr<Device> GtkXKeyboardDevice::getDevice()
{
	shared_ptr<GtkXKeyboardDevice> refThis = shared_from_this();
	return refThis;
}
shared_ptr<Capability> GtkXKeyboardDevice::getCapability(const Capability::Class& oClass) const
{
	shared_ptr<Capability> refCapa;
	if (oClass == typeid(KeyCapability)) {
		shared_ptr<const GtkXKeyboardDevice> refConstThis = shared_from_this();
		shared_ptr<GtkXKeyboardDevice> refThis = std::const_pointer_cast<GtkXKeyboardDevice>(refConstThis);
		refCapa = refThis;
	}
	return refCapa;
}
std::vector<Capability::Class> GtkXKeyboardDevice::getCapabilityClasses() const
{
	return {KeyCapability::getClass()};
}
bool GtkXKeyboardDevice::isKeyboard() const
{
	return true;
}
bool GtkXKeyboardDevice::handleXIDeviceEvent(XIDeviceEvent* p0XIDeviceEvent, const shared_ptr<GtkWindowData>& refWindowData)
{
//std::cout << "GtkXKeyboardDevice::handleXIDeviceEvent()" << std::endl;
	const bool bContinue = true;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return !bContinue; //---------------------------------------------------
	}
	//
	FloGtkDeviceManager* p0Owner = refOwner.get();
	if (p0Owner->m_refSelected != refWindowData) {
		//TODO this shouldn't happen
		return bContinue; //----------------------------------------------------
	}
	HARDWARE_KEY eHardwareKey;
	if (!p0Owner->m_oConverter.convertGdkKeyCodeToHardwareKey(static_cast<guint16>(p0XIDeviceEvent->detail), eHardwareKey)) {
		return bContinue; //----------------------------------------------------
	}
	auto refListeners = p0Owner->getListeners();
	const int32_t nHardwareKey = static_cast<int32_t>(eHardwareKey);
	const int32_t nEvType = p0XIDeviceEvent->evtype;
	int64_t nTimePressedUsec = std::numeric_limits<int64_t>::max();
	auto itFind = m_oPressedKeys.find(nHardwareKey);
	const bool bHardwareKeyPressed = (itFind != m_oPressedKeys.end());
	KeyEvent::KEY_INPUT_TYPE eInputType;
	shared_ptr<GtkXKeyboardDevice> refGtkXKeyboardDevice = shared_from_this();
	shared_ptr<KeyCapability> refCapability = refGtkXKeyboardDevice;
	auto refWindowAccessor = refWindowData->getAccessor();
	assert(refWindowAccessor);
	if (nEvType == XI_KeyPress) {
		if (bHardwareKeyPressed) {
			// Key repeat
			KeyData& oKeyData = itFind->second;
			if (refOwner->m_eKeyRepeatMode == KEY_REPEAT_MODE_SUPPRESS) {
				return bContinue; //--------------------------------------------
			}
			KeyEvent::KEY_INPUT_TYPE eAddInputType;
			if (refOwner->m_eKeyRepeatMode == KEY_REPEAT_MODE_ADD_RELEASE) {
				eAddInputType = KeyEvent::KEY_RELEASE;
			} else {
				eAddInputType = KeyEvent::KEY_RELEASE_CANCEL;
			}
			auto nTimePressed = oKeyData.m_nPressedTimeUsec;
			m_oPressedKeys.erase(itFind);
			shared_ptr<KeyEvent> refEvent;
			const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
			for (auto& p0ListenerData : *refListeners) {
				sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nTimePressed, eAddInputType, eHardwareKey
										, refWindowAccessor, refCapability, p0Owner->m_nClassIdxKeyEvent, refEvent);
			}
			if (!refWindowData->isEnabled()) {
				// the accessor was removed during the callbacks!
				return bContinue; //--------------------------------------------
			}
		}
		nTimePressedUsec = DeviceManager::getNowTimeMicroseconds();
		KeyData oKeyData;
		oKeyData.m_nPressedTimeUsec = nTimePressedUsec;
		m_oPressedKeys.emplace(nHardwareKey, oKeyData);
		eInputType = KeyEvent::KEY_PRESS;
	} else {
		assert(nEvType == XI_KeyRelease);
		if (!bHardwareKeyPressed) {
			// orphan release, ignore
//std::cout << "GtkXKeyboardDevice::handleXIDeviceEvent orphan release" << std::endl;
			return bContinue; //------------------------------------------------
		}
		KeyData& oKeyData = itFind->second;
		nTimePressedUsec = oKeyData.m_nPressedTimeUsec;
		eInputType = KeyEvent::KEY_RELEASE;
		m_oPressedKeys.erase(itFind);
	}
	shared_ptr<KeyEvent> refEvent;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& p0ListenerData : *refListeners) {
		sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nTimePressedUsec, eInputType, eHardwareKey
								, refWindowAccessor, refCapability, p0Owner->m_nClassIdxKeyEvent, refEvent);
		if ((eInputType == KeyEvent::KEY_PRESS) && (m_oPressedKeys.find(nHardwareKey) == m_oPressedKeys.end())) {
			// The key was canceled by the callback
			break; // for -------
		}
	}
	return bContinue;

}
void GtkXKeyboardDevice::finalizeListener(FloGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec)
{
//std::cout << "GtkXKeyboardDevice::finalizeListener m_oPressedKeys.size()=" << m_oPressedKeys.size() << std::endl;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	FloGtkDeviceManager* p0Owner = refOwner.get();
	if (!p0Owner->getEventClassEnabled(typeid(KeyEvent))) {
		return; //--------------------------------------------------------------
	}
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; //--------------------------------------------------------------
	}
	auto refSelectedAccessor = refSelected->getAccessor();

	shared_ptr<GtkXKeyboardDevice> refGtkXKeyboardDevice = shared_from_this();
	shared_ptr<KeyCapability> refCapability = refGtkXKeyboardDevice;

	FloGtkListenerExtraData* p0ExtraData = nullptr;
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
								, eHardwareKey, refSelectedAccessor, refCapability, p0Owner->m_nClassIdxKeyEvent, refEvent);
		if (!refSelected) {
			// There can't be pressed keys without an active window
			break; // for -----------
		}
	}
}
void GtkXKeyboardDevice::removingDevice()
{
	resetOwnerDeviceManager();
}
void GtkXKeyboardDevice::cancelSelectedAccessorKeys()
{
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	FloGtkDeviceManager* p0Owner = refOwner.get();
	if (!p0Owner->getEventClassEnabled(typeid(KeyEvent))) {
		return; // -------------------------------------------------------------
	}
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; // -------------------------------------------------------------
	}
	auto refSelectedAccessor = refSelected->getAccessor();

	auto refListeners = p0Owner->getListeners();
	//
	shared_ptr<GtkXKeyboardDevice> refGtkXKeyboardDevice = shared_from_this();
	shared_ptr<KeyCapability> refCapability = refGtkXKeyboardDevice;
	// Remove all keys generated by the accessor (widget)
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	// work on copy
	auto oPressedKeys = m_oPressedKeys;
	for (auto& oPair : oPressedKeys) {
		const int32_t nHardwareKey = oPair.first;
		const KeyData& oKeyData = oPair.second;
		//
		const HARDWARE_KEY eHardwareKey = static_cast<HARDWARE_KEY>(nHardwareKey);
		const auto nKeyPressedTime = oKeyData.m_nPressedTimeUsec;
		//
		shared_ptr<KeyEvent> refEvent;
		for (auto& p0ListenerData : *refListeners) {
			FloGtkListenerExtraData* p0ExtraData = nullptr;
			p0ListenerData->getExtraData(p0ExtraData);
			if (p0ExtraData->isKeyCanceled(nHardwareKey)) {
				continue; // for itListenerData ------------
			}
			p0ExtraData->setKeyCanceled(nHardwareKey);
			//
			sendKeyEventToListener(*p0ListenerData, nEventTimeUsec, nKeyPressedTime, KeyEvent::KEY_RELEASE_CANCEL
									, eHardwareKey, refSelectedAccessor, refCapability, p0Owner->m_nClassIdxKeyEvent, refEvent);
		}
	}
	m_oPressedKeys.clear();
}
void GtkXKeyboardDevice::sendKeyEventToListener(const StdDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
												, int64_t nTimePressedUsec
												, KeyEvent::KEY_INPUT_TYPE eInputType, HARDWARE_KEY eHardwareKey
												, const shared_ptr<GtkAccessor>& refAccessor
												, const shared_ptr<KeyCapability>& refCapability
												, int32_t nClassIdxKeyEvent
												, shared_ptr<KeyEvent>& refEvent)
{
	const int64_t nAddTimeUsec = oListenerData.getAddedTime();
//std::cout << "GtkXKeyboardDevice::sendKeyEventToListener nAddTimeUsec=" << nAddTimeUsec;
//std::cout << "  nTimePressedUsec=" << nTimePressedUsec << "  nEventTimeUsec=" << nEventTimeUsec << std::endl;
	if (nTimePressedUsec < nAddTimeUsec) {
		// The listener was added after the key was pressed
		return;
	}
	if (!refEvent) {
		refEvent = std::make_shared<KeyEvent>(nEventTimeUsec, refAccessor, refCapability, eInputType, eHardwareKey);
	}
	oListenerData.handleEventCallIf(nClassIdxKeyEvent, refEvent);
		// no need to reset because KeyEvent cannot be modified. refEvent.reset();
		//TODO Recycling
}

} // namespace Flo
} // namespace Private

} // namespace stmi
