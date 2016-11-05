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
 * File:   jsgtkjoystickdevice.cc
 */

#include "jsgtkjoystickdevice.h"

#include "joysticksources.h"
#include "jsgtkwindowdata.h"
#include "jsgtklistenerextradata.h"

namespace stmi
{

namespace Private
{
namespace Js
{

JoystickDevice::JoystickDevice(std::string sName, const shared_ptr<JsGtkDeviceManager>& refDeviceManager
								, const std::vector<int32_t>& aButtonCode, int32_t nTotHats, const std::vector<int32_t>& aAxisCode)
: BasicDevice<JsGtkDeviceManager>(sName, refDeviceManager)
, m_aButtonCode(aButtonCode)
, m_aButtonPressed(aButtonCode.size(), ButtonData{false, std::numeric_limits<int64_t>::max()})
, m_nTotHats(nTotHats)
, m_aAxisCode(aAxisCode)
, m_aAxisValue(aAxisCode.size(), 0)
{
	assert((nTotHats >= 0) && (nTotHats <= s_nMaxHats));
	m_aHatStatus.resize(m_nTotHats, HatData{std::numeric_limits<int64_t>::max(), 0, 0});
}
JoystickDevice::~JoystickDevice()
{
}
shared_ptr<Device> JoystickDevice::getDevice() const
{
	shared_ptr<const JoystickDevice> refConstThis = shared_from_this();
	shared_ptr<JoystickDevice> refThis = std::const_pointer_cast<JoystickDevice>(refConstThis);
	return refThis;
}
shared_ptr<Device> JoystickDevice::getDevice()
{
	shared_ptr<JoystickDevice> refThis = shared_from_this();
	return refThis;
}
shared_ptr<Capability> JoystickDevice::getCapability(const Capability::Class& oClass) const
{
	shared_ptr<Capability> refCapa;
	if (oClass == typeid(JoystickCapability)) {
		shared_ptr<const JoystickDevice> refConstThis = shared_from_this();
		shared_ptr<JoystickDevice> refThis = std::const_pointer_cast<JoystickDevice>(refConstThis);
		refCapa = refThis;
	}
	return refCapa;
}
shared_ptr<Capability> JoystickDevice::getCapability(int32_t nCapabilityId) const
{
	const auto nJoystickCapaId = JoystickCapability::getId();
	if (nCapabilityId != nJoystickCapaId) {
		return shared_ptr<Capability>{};
	}
	shared_ptr<const JoystickDevice> refConstThis = shared_from_this();
	shared_ptr<JoystickDevice> refThis = std::const_pointer_cast<JoystickDevice>(refConstThis);
	return refThis;
}
std::vector<int32_t> JoystickDevice::getCapabilities() const
{
	return {JoystickCapability::getId()};
}
std::vector<Capability::Class> JoystickDevice::getCapabilityClasses() const
{
	return {JoystickCapability::getClass()};
}
size_t JoystickDevice::getButtonNr(JoystickCapability::BUTTON eButton) const
{
	auto itFind = std::find(m_aButtonCode.begin(), m_aButtonCode.end(), static_cast<int32_t>(eButton));
	if (itFind == m_aButtonCode.end()) {
		return std::numeric_limits<size_t>::max();
	}
	return std::distance(m_aButtonCode.begin(), itFind);
}
size_t JoystickDevice::getAxisNr(JoystickCapability::AXIS eAxis) const
{
	if (isHatAxis(static_cast<int32_t>(eAxis))) {
		return std::numeric_limits<size_t>::max();
	}
	auto itFind = std::find(m_aAxisCode.begin(), m_aAxisCode.end(), static_cast<int32_t>(eAxis));
	if (itFind == m_aAxisCode.end()) {
		return std::numeric_limits<size_t>::max();
	}
	return std::distance(m_aAxisCode.begin(), itFind);
}
bool JoystickDevice::getHasButton(JoystickCapability::BUTTON eButton) const
{
	const size_t nNr = getButtonNr(eButton);
	return (nNr < std::numeric_limits<size_t>::max());
}
int32_t JoystickDevice::getTotHats() const
{
	return m_nTotHats;
}
bool JoystickDevice::getHasAxis(JoystickCapability::AXIS eAxis) const
{
	const size_t nNr = getAxisNr(eAxis);
	return (nNr < std::numeric_limits<size_t>::max());
}
bool JoystickDevice::isButtonPressed(JoystickCapability::BUTTON eButton) const
{
	const size_t nNr = getButtonNr(eButton);
	if (nNr == std::numeric_limits<size_t>::max()) {
		return false;
	}
	return m_aButtonPressed[nNr].m_bPressed;
}
JoystickCapability::HAT_VALUE JoystickDevice::getHatValue(int32_t nHat) const
{
	assert((nHat >= 0) && (nHat < m_nTotHats));
	auto& oHatStatus = m_aHatStatus[nHat];
	return static_cast<JoystickCapability::HAT_VALUE>(oHatStatus.m_nAxisY * 4 + oHatStatus.m_nAxisX);
}
int32_t JoystickDevice::getAxisValue(JoystickCapability::AXIS eAxis) const
{
	const size_t nNr = getAxisNr(eAxis);
	if (nNr == std::numeric_limits<size_t>::max()) {
		return false;
	}
	return m_aAxisValue[nNr];
}

bool JoystickDevice::doInputJoystickEventCallback(const struct js_event* p0JoyEvent)
{
//std::cout << "JoystickDevice::doInputJoystickEventCallback" << '\n';
	const bool bContinue = true;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return !bContinue;
	}
	JsGtkDeviceManager* p0Owner = refOwner.get();
	if (!p0Owner->m_refSelected) {
		return bContinue; //----------------------------------------------------
	}
	auto refSelectedAccessor = p0Owner->m_refSelected->getAccessor();

	const int32_t nType = p0JoyEvent->type;
	const int32_t nNr = p0JoyEvent->number;
	const int32_t nValue = p0JoyEvent->value;
	const bool bInit = ((nType & JS_EVENT_INIT) != 0);
	shared_ptr<JoystickDevice> refThis = shared_from_this();
	//
//std::cout << "---> doInputJoystickEventCallback() Device:" << refThis->getName() << "     type=" << nType << " number=" << nNr << " value=" << nValue << '\n';
	if ((nType & JS_EVENT_BUTTON) != 0) {
		assert((nNr >= 0) && (nNr < static_cast<int32_t>(m_aButtonCode.size())));
		const int32_t nLinuxButton = m_aButtonCode[nNr];
		const JoystickCapability::BUTTON eButton = static_cast<JoystickCapability::BUTTON>(nLinuxButton);
//std::cout << "                                    nLinuxButton=0x" << std::hex << nLinuxButton << std::dec << " value=" << nValue << std::endl;
		if (JoystickCapability::isValidButton(eButton)) {
			if (bInit) {
//std::cout << "  INIT: Device: " << refThis->getName() << "  nType=" << (nType & ~JS_EVENT_INIT) << " number=" << nNr << " value=" << nValue << std::endl;
				ButtonData& oButtonStatus = m_aButtonPressed[nNr];
				oButtonStatus.m_bPressed = false; //(nValue != 0);
				oButtonStatus.m_nPressedTimeStamp = std::numeric_limits<uint64_t>::max();
			} else {
				handleButton(p0Owner, refSelectedAccessor, refThis, nNr, eButton, nValue);
			}
		} else {
			//assert(false);
		}
	} else if ((nType & JS_EVENT_AXIS) != 0) {
		assert((nNr >= 0) && (nNr < static_cast<int32_t>(m_aAxisCode.size())));
		const int32_t nLinuxAxis = m_aAxisCode[nNr];
//std::cout << "                                    m_aAxisCode=0x" << std::hex << nLinuxAxis << std::dec << " value=" << nValue << std::endl;
		if (isHatAxis(nLinuxAxis)) {
			// It's a hat
			const int32_t nHatBase = nLinuxAxis - ABS_HAT0X;
			const int32_t nHat = nHatBase / 2;
			const int32_t bY = (nHatBase & 1) != 0;
			assert((nHat >= 0) && (nHat < m_nTotHats));
			if (bInit) {
				auto& oHatData = m_aHatStatus[nHat];
				if (bY) {
					oHatData.m_nAxisY = 0; //nValue;
				} else {
					oHatData.m_nAxisX = 0; //nValue;
				}
				oHatData.m_nPressedTimeStamp = std::numeric_limits<uint64_t>::max();
			} else {
				handleHat(p0Owner, refSelectedAccessor, refThis, nHat, bY, nValue);
			}
		} else {
			const JoystickCapability::AXIS eAxis = static_cast<JoystickCapability::AXIS>(nLinuxAxis);
			if (JoystickCapability::isValidAxis(eAxis)) {
				// normal axis
				int32_t& nAxisValue = m_aAxisValue[nNr];
				if (nAxisValue != nValue) {
					nAxisValue = nValue;
					if (!bInit) {
						handleAxis(p0Owner, refSelectedAccessor, refThis, eAxis, nValue);
					}
				}
			} else {
				//assert(false);
			}
		}
	} else {
		//assert(false);
	}
	return bContinue;
}
void JoystickDevice::handleButton(JsGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
									, const shared_ptr<JoystickCapability>& refThis
									, int32_t nNr, JoystickCapability::BUTTON eButton, int32_t nValue)
{
	if (!p0Owner->isEventClassEnabled(typeid(JoystickButtonEvent))) {
		return;
	}
	auto refListeners = p0Owner->getListeners();
	//
	ButtonData& oButtonStatus = m_aButtonPressed[nNr];
	const bool bButtonWasPressed = oButtonStatus.m_bPressed;
	const auto nWasPressedTimeStamp = oButtonStatus.m_nPressedTimeStamp;
	const bool bButtonIsPressed = (nValue != 0);
	uint64_t nPressedTimeStamp = std::numeric_limits<uint64_t>::max();
	JoystickButtonEvent::BUTTON_INPUT_TYPE eInputType;
	if (bButtonIsPressed) {
		if (bButtonWasPressed) {
			// Send a cancel
			oButtonStatus.m_bPressed = false;
			shared_ptr<Event> refEvent;
			const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
			for (auto& p0ListenerData : *refListeners) {
				sendButtonEventToListener(*p0ListenerData, nEventTimeUsec, refSelectedAccessor, nWasPressedTimeStamp, JoystickButtonEvent::BUTTON_RELEASE_CANCEL
											, eButton, refThis, p0Owner->m_nClassIdxJoystickButtonEvent, refEvent);
			}
		}
		nPressedTimeStamp = JsGtkDeviceManager::getUniqueTimeStamp();
		oButtonStatus.m_bPressed = true;
		oButtonStatus.m_nPressedTimeStamp = nPressedTimeStamp;
		eInputType = JoystickButtonEvent::BUTTON_PRESS;
	} else {
		if (!bButtonWasPressed) {
			// orphan release probably due to event type enabled while button pressed, ignore.
			return; //----------------------------------------------------------
		}
		oButtonStatus.m_bPressed = false;
		nPressedTimeStamp = oButtonStatus.m_nPressedTimeStamp;
		eInputType = JoystickButtonEvent::BUTTON_RELEASE;
	}
	//
	shared_ptr<Event> refEvent;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& p0ListenerData : *refListeners) {
		sendButtonEventToListener(*p0ListenerData, nEventTimeUsec, refSelectedAccessor, nPressedTimeStamp, eInputType, eButton
									, refThis, p0Owner->m_nClassIdxJoystickButtonEvent, refEvent);
	}
}
void JoystickDevice::handleHat(JsGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
								, const shared_ptr<JoystickCapability>& refThis
								, int32_t nHat, bool bY, int32_t nValue)
{
//std::cout << "JoystickDevice::handleHat" << '\n';
	auto& oHatData = m_aHatStatus[nHat];
	int32_t nAxisX = oHatData.m_nAxisX;
	int32_t nAxisY = oHatData.m_nAxisY;
	const auto eOldValue = calcHatValue(nAxisX, nAxisY);
	if (bY) {
		nAxisY = ((nValue == 0) ? 0 : ((nValue < 0) ? 1 : 2));
	} else {
		nAxisX = ((nValue == 0) ? 0 : ((nValue < 0) ? 1 : 2));
	}
	// Y\X| 1 | 0 | 2
	// ---|-----------
	//  1 | 5   4   6
	// ---|
	//  0 | 1   0   2
	// ---|
	//  2 | 9   8  10
	//
	const auto eValue = calcHatValue(nAxisX, nAxisY);
	if (eOldValue == eValue) {
		// no change
		return;
	}
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	const bool bWasPressed = (eOldValue != JoystickCapability::HAT_CENTER);
	const bool bIsPressed = (eValue != JoystickCapability::HAT_CENTER);
	if (bIsPressed) {
		if (!bWasPressed) {
			oHatData.m_nPressedTimeStamp = JsGtkDeviceManager::getUniqueTimeStamp();
		}
	}
	oHatData.m_nAxisX = nAxisX;
	oHatData.m_nAxisY = nAxisY;
	//
	if (!p0Owner->isEventClassEnabled(typeid(JoystickHatEvent))) {
		// Unlike handleButton and handleAxis this type keeps track of the state
		// of the hats despite the JoystickHatEvent type not being enabled.
		// This is done because otherwise key simulation would be inconsistent.
		// That's also why JsGtkDeviceManager has to provide m_nHatEventTypeEnabledTimeUsec
		return; //--------------------------------------------------------------
	}
	if (oHatData.m_nPressedTimeStamp < p0Owner->m_nHatEventTypeEnabledTimeStamp) {
		// Hat hasn't come back to center position since hat JoystickHatEvent was enabled.
		// Do not send to listeners for key simulation consistency reasons.
		return; //--------------------------------------------------------------
	}
	//
	auto refListeners = p0Owner->getListeners();
	shared_ptr<Event> refEvent;
	for (auto& p0ListenerData : *refListeners) {
		sendHatEventToListener(*p0ListenerData, nEventTimeUsec, refSelectedAccessor, oHatData.m_nPressedTimeStamp
									, nHat, eValue, eOldValue
									, refThis, p0Owner->m_nClassIdxJoystickHatEvent, refEvent);
	}
	if (!bIsPressed) {
		oHatData.m_nPressedTimeStamp = std::numeric_limits<uint64_t>::max();
	}
}
void JoystickDevice::handleAxis(JsGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
								, const shared_ptr<JoystickCapability>& refThis
								, JoystickCapability::AXIS eAxis, int32_t nValue)
{
	if (!p0Owner->isEventClassEnabled(typeid(JoystickAxisEvent))) {
		return; //--------------------------------------------------------------
	}
	auto refListeners = p0Owner->getListeners();
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	shared_ptr<Event> refEvent;
	for (auto& p0ListenerData : *refListeners) {
		if (!refEvent) {
			m_oJoystickAxisEventRecycler.create(refEvent, nEventTimeUsec, refSelectedAccessor, refThis, eAxis, nValue);
		}
		p0ListenerData->handleEventCallIf(p0Owner->m_nClassIdxJoystickAxisEvent, refEvent);
			// no need to reset because JoystickAxisEvent cannot be modified.
	}
}
void JoystickDevice::finalizeListener(JsGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec)
{
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	JsGtkDeviceManager* p0Owner = refOwner.get();
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; //--------------------------------------------------------------
	}
	auto refSelectedAccessor = refSelected->getAccessor();
	shared_ptr<JoystickDevice> refThis = shared_from_this();
	if (p0Owner->isEventClassEnabled(typeid(JoystickButtonEvent))) {
		finalizeListenerButton(oListenerData, nEventTimeUsec, refThis, p0Owner->m_nClassIdxJoystickButtonEvent, refSelectedAccessor);
	}
	if (p0Owner->isEventClassEnabled(typeid(JoystickHatEvent))) {
		finalizeListenerHat(oListenerData, nEventTimeUsec, refThis, p0Owner->m_nClassIdxJoystickHatEvent, refSelectedAccessor);
	}
}
void JoystickDevice::finalizeListenerButton(JsGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
											, const shared_ptr<JoystickCapability>& refCapability, int32_t nClassIdxJoystickButtonEvent
											, const shared_ptr<GtkAccessor>& refSelectedAccessor)
{
//std::cout << "JoystickDevice::finalizeListenerButton" << std::endl;
	// work on copy
	JsGtkListenerExtraData* p0ExtraData = nullptr;
	oListenerData.getExtraData(p0ExtraData);

	// work on copy
	auto aButtonPressed = m_aButtonPressed;

	const size_t nTotButtons = aButtonPressed.size();
	for (size_t nButtonIdx = 0; nButtonIdx < nTotButtons; ++nButtonIdx) {
		auto& oButtonData = aButtonPressed[nButtonIdx];
		if (!oButtonData.m_bPressed) {
			continue; // for -------------
		}
		if (p0ExtraData->isButtonCanceled(nButtonIdx)) {
			continue; // for ----------
		}
		p0ExtraData->setButtonCanceled(nButtonIdx);

		const auto nButtonPressedTimeStamp = oButtonData.m_nPressedTimeStamp;
		const int32_t nLinuxButton = m_aButtonCode[nButtonIdx];
		const JoystickCapability::BUTTON eButton = static_cast<JoystickCapability::BUTTON>(nLinuxButton);
//std::cout << "   finalizing  nButton=" << eButton << std::endl;
		shared_ptr<Event> refEvent;
		sendButtonEventToListener(oListenerData, nEventTimeUsec, refSelectedAccessor, nButtonPressedTimeStamp
									, JoystickButtonEvent::BUTTON_RELEASE_CANCEL, eButton, refCapability
									, nClassIdxJoystickButtonEvent, refEvent);
	}
}
void JoystickDevice::finalizeListenerHat(JsGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
										, const shared_ptr<JoystickCapability>& refCapability, int32_t nClassIdxJoystickHatEvent
										, const shared_ptr<GtkAccessor>& refSelectedAccessor)
{
//std::cout << "JoystickDevice::finalizeListenerHat" << std::endl;
	//
	// work on copy
	JsGtkListenerExtraData* p0ExtraData = nullptr;
	oListenerData.getExtraData(p0ExtraData);

	// work on copy
	auto aHatStatus = m_aHatStatus;

	for (int32_t nHat = 0; nHat < m_nTotHats; ++nHat) {
		const auto& oHatData = aHatStatus[nHat];
		const int32_t nAxisX = oHatData.m_nAxisX;
		const int32_t nAxisY = oHatData.m_nAxisY;
		const auto eOldValue = calcHatValue(nAxisX, nAxisY);
		if (eOldValue == JoystickCapability::HAT_CENTER) {
			// hat "not pressed"
			continue; // for ------------
		}
		if (p0ExtraData->isHatCanceled(nHat)) {
			continue; // for ----------
		}
		p0ExtraData->setHatCanceled(nHat);
//std::cout << "   finalizing  nHat=" << nHat << std::endl;
		const auto nAnyPressedTimeStamp = oHatData.m_nPressedTimeStamp;
		shared_ptr<Event> refEvent;
		sendHatEventToListener(oListenerData, nEventTimeUsec, refSelectedAccessor
											, nAnyPressedTimeStamp, nHat
											, JoystickCapability::HAT_CENTER_CANCEL, eOldValue
											, refCapability
											, nClassIdxJoystickHatEvent
											, refEvent);
	}
}
void JoystickDevice::removingDevice()
{
	resetOwnerDeviceManager();
}
void JoystickDevice::cancelSelectedAccessorButtonsAndHats()
{
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	JsGtkDeviceManager* p0Owner = refOwner.get();
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; // -------------------------------------------------------------
	}
	auto refSelectedAccessor = refSelected->getAccessor();
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();

	auto refListeners = p0Owner->getListeners();
	shared_ptr<JoystickDevice> refThis = shared_from_this();

	if (p0Owner->isEventClassEnabled(typeid(JoystickButtonEvent))) {
		cancelSelectedAccessorButtons(refThis, refListeners, nEventTimeUsec, refSelectedAccessor, p0Owner);
	}
	if (p0Owner->isEventClassEnabled(typeid(JoystickHatEvent))) {
		cancelSelectedAccessorHats(refThis, refListeners, nEventTimeUsec, refSelectedAccessor, p0Owner);
	}
}
void JoystickDevice::cancelSelectedAccessorButtons(const shared_ptr<JoystickCapability>& refCapability
												, const shared_ptr< const std::list< JsGtkDeviceManager::ListenerData* > >& refListeners
												, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
												, JsGtkDeviceManager* p0Owner)
{
	const auto nClassIdxJoystickButtonEvent = p0Owner->m_nClassIdxJoystickButtonEvent;
	// work on copy
	auto aButtonPressed = m_aButtonPressed;

	const size_t nTotButtons = aButtonPressed.size();
	for (size_t nButtonIdx = 0; nButtonIdx < nTotButtons; ++nButtonIdx) {
		auto& oButtonData = aButtonPressed[nButtonIdx];
		if (!oButtonData.m_bPressed) {
			continue; // for
		}
		const auto nButtonPressedTimeStamp = oButtonData.m_nPressedTimeStamp;
		const int32_t nLinuxButton = m_aButtonCode[nButtonIdx];
		const JoystickCapability::BUTTON eButton = static_cast<JoystickCapability::BUTTON>(nLinuxButton);
//std::cout << "   finalizing  nButton=" << eButton << std::endl;
		shared_ptr<Event> refEvent;
		for (auto& p0ListenerData : *refListeners) {
			JsGtkListenerExtraData* p0ExtraData = nullptr;
			p0ListenerData->getExtraData(p0ExtraData);
			if (p0ExtraData->isButtonCanceled(nButtonIdx)) {
				continue; // for p0ListenerData ------------
			}
			p0ExtraData->setButtonCanceled(nButtonIdx);
			//shared_ptr<JoystickButtonEvent> refEvent;
			sendButtonEventToListener(*p0ListenerData, nEventTimeUsec, refSelectedAccessor, nButtonPressedTimeStamp
										, JoystickButtonEvent::BUTTON_RELEASE_CANCEL, eButton, refCapability
										, nClassIdxJoystickButtonEvent, refEvent);
		}
	}
	std::fill(m_aButtonPressed.begin(), m_aButtonPressed.end(), ButtonData{false, std::numeric_limits<int64_t>::max()});
}
void JoystickDevice::cancelSelectedAccessorHats(const shared_ptr<JoystickCapability>& refCapability
												, const shared_ptr< const std::list< JsGtkDeviceManager::ListenerData* > >& refListeners
												, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
												, JsGtkDeviceManager* p0Owner)
{
	const auto nClassIdxJoystickHatEvent = p0Owner->m_nClassIdxJoystickHatEvent;
	// work on copy
	auto aHatStatus = m_aHatStatus;

	for (int32_t nHat = 0; nHat < m_nTotHats; ++nHat) {
		const auto& oHatData = aHatStatus[nHat];
		const int32_t nAxisX = oHatData.m_nAxisX;
		const int32_t nAxisY = oHatData.m_nAxisY;
		const auto eOldValue = calcHatValue(nAxisX, nAxisY);
		if (eOldValue == JoystickCapability::HAT_CENTER) {
			// hat "not pressed"
			continue; // for ------------
		}
		const auto nAnyPressedTimeStamp = oHatData.m_nPressedTimeStamp;
		shared_ptr<Event> refEvent;
		for (auto& p0ListenerData : *refListeners) {
			JsGtkListenerExtraData* p0ExtraData = nullptr;
			p0ListenerData->getExtraData(p0ExtraData);
			if (p0ExtraData->isHatCanceled(nHat)) {
				continue; // for p0ListenerData ------------
			}
			p0ExtraData->setHatCanceled(nHat);
			sendHatEventToListener(*p0ListenerData, nEventTimeUsec, refSelectedAccessor
												, nAnyPressedTimeStamp, nHat
												, JoystickCapability::HAT_CENTER_CANCEL, eOldValue
												, refCapability
												, nClassIdxJoystickHatEvent
												, refEvent);
		}
	}
	std::fill(m_aHatStatus.begin(), m_aHatStatus.end(), HatData{std::numeric_limits<int64_t>::max(), 0, 0});
}
void JoystickDevice::sendButtonEventToListener(const JsGtkDeviceManager::ListenerData& oListenerData
												, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refAccessor
												, uint64_t nPressedTimeStamp
												, JoystickButtonEvent::BUTTON_INPUT_TYPE eInputType
												, JoystickCapability::BUTTON eButtonId
												, const shared_ptr<JoystickCapability>& refCapability
												, int32_t nClassIdxJoystickButtonEvent
												, shared_ptr<Event>& refEvent)
{
	const auto nAddTimeStamp = oListenerData.getAddedTimeStamp();
	if (nPressedTimeStamp < nAddTimeStamp) {
//std::cout << "nAddTimeUsec = " << nAddTimeUsec << "  nTimePressedUsec=" << nTimePressedUsec << '\n';
		// The listener was added after the button was pressed, so it doesn't need a release
		return;
	}
	if (!refEvent) {
		m_oJoystickButtonEventRecycler.create(refEvent, nEventTimeUsec, refAccessor, refCapability, eInputType, eButtonId);
	}
	oListenerData.handleEventCallIf(nClassIdxJoystickButtonEvent, refEvent);
		// no need to reset because JoystickButtonEvent cannot be modified.
}
void JoystickDevice::sendHatEventToListener(const JsGtkDeviceManager::ListenerData& oListenerData
											, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refAccessor
											, uint64_t nPressedTimeStamp, int32_t nHat
											, JoystickCapability::HAT_VALUE eValue
											, JoystickCapability::HAT_VALUE ePreviousValue
											, const shared_ptr<JoystickCapability>& refCapability
											, int32_t nClassIdxJoystickHatEvent
											, shared_ptr<Event>& refEvent)
{
	const auto nAddTimeStamp = oListenerData.getAddedTimeStamp();
	if (nPressedTimeStamp < nAddTimeStamp) {
//std::cout << "JoystickDevice::sendHatEventToListener nTimeAnyPressedUsec<nAddTimeUsec" << '\n';
		// Hat hasn't come back to center position since listener addition.
		return;
	}
	if (!refEvent) {
		m_oJoystickHatEventRecycler.create(refEvent, nEventTimeUsec, refAccessor, refCapability, nHat, eValue, ePreviousValue);
	}
	oListenerData.handleEventCallIf(nClassIdxJoystickHatEvent, refEvent);
		// no need to reset because JoystickButtonEvent cannot be modified.
}


} // namespace Js
} // namespace Private

} // namespace stmi
