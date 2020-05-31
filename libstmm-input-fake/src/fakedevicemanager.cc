/*
 * Copyright © 2018-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   fakedevicemanager.cc
 */

#include "fakedevicemanager.h"

#include <stmm-input-base/basicdevicemanager.h>
#include <stmm-input-ev/keycapability.h>
#include <stmm-input-ev/pointercapability.h>
#include <stmm-input-ev/touchcapability.h>
#include <stmm-input/capability.h>
#include <stmm-input/devicemanager.h>

namespace stmi { class Accessor; }
namespace stmi { class ChildDeviceManager; }
namespace stmi { class DeviceMgmtCapability; }

namespace stmi
{

namespace testing
{

FakeDeviceManager::FakeDeviceManager(bool bEnableEventClasses, const std::vector< Event::Class > &aEnDisableEventClasses) noexcept
: StdDeviceManager({KeyCapability::getClass(), PointerCapability::getClass()
						, TouchCapability::getClass(), JoystickCapability::getClass()}
					, {DeviceMgmtEvent::getClass(), KeyEvent::getClass(), PointerEvent::getClass(), PointerScrollEvent::getClass()
						, TouchEvent::getClass(), JoystickButtonEvent::getClass(), JoystickHatEvent::getClass(), JoystickAxisEvent::getClass()}
					, bEnableEventClasses, aEnDisableEventClasses)
{
}

FakeDeviceManager::FakeDeviceManager() noexcept
: FakeDeviceManager(false, {})
{
}
bool FakeDeviceManager::simulateRemoveDevice(int32_t nDeviceId) noexcept
{
	auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
	if (!refDevice) {
		return false;
	}
	#ifndef NDEBUG
	const bool bRemoved = 
	#endif
	StdDeviceManager::removeDevice(refDevice);
	assert(bRemoved);
	// save reference
	auto refRemovedDevice = refDevice;
	sendDeviceMgmtToListeners(stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED, refRemovedDevice);
	return true;
}
bool FakeDeviceManager::simulateChangedDevice(int32_t nDeviceId) noexcept
{
	auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
	if (!refDevice) {
		return false;
	}
	sendDeviceMgmtToListeners(stmi::DeviceMgmtEvent::DEVICE_MGMT_CHANGED, refDevice);
	return true;
}
int32_t FakeDeviceManager::simulateEvent(const shared_ptr<Event>& refEvent) noexcept
{
	if (!isEventClassEnabled(refEvent->getEventClass())) {
		return -1;
	}
	int32_t nTotSent = 0;
	auto refListeners = getListeners();
	for (ListenerData* p0ListenerData : *refListeners) {
		const bool bSent = p0ListenerData->handleEventCallIf(-1, refEvent);
		if (bSent) {
			++nTotSent;
		}
	}
	return nTotSent;
}
int32_t FakeDeviceManager::simulateKeyEvent(int32_t nDeviceId, KeyEvent::KEY_INPUT_TYPE eType, HARDWARE_KEY eKey) noexcept
{
	auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
	if (!refDevice) {
		return -1; //-----------------------------------------------------------
	}
	shared_ptr<KeyCapability> refKeyCapability;
	const bool bFound = refDevice->getCapability(refKeyCapability);
	if (!bFound) {
		return -1; //-----------------------------------------------------------
	}
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	shared_ptr<Event> refKeyEvent = std::make_shared<KeyEvent>(nTimeUsec, shared_ptr<Accessor>{}, refKeyCapability, eType, eKey);
	return simulateEvent(refKeyEvent);
}
int32_t FakeDeviceManager::simulatePointerEvent(int32_t nDeviceId, double fX, double fY, PointerEvent::POINTER_INPUT_TYPE eType
												, int32_t nButton, bool bAnyButtonPressed, bool bWasAnyButtonPressed) noexcept
{
	auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
	if (!refDevice) {
		return -1; //-----------------------------------------------------------
	}
	shared_ptr<PointerCapability> refPointerCapability;
	const bool bFound = refDevice->getCapability(refPointerCapability);
	if (!bFound) {
		return -1; //-----------------------------------------------------------
	}
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	shared_ptr<Event> refPointerEvent = std::make_shared<PointerEvent>(nTimeUsec, shared_ptr<Accessor>{}, refPointerCapability
															, fX, fY, eType, nButton, bAnyButtonPressed, bWasAnyButtonPressed);
	return simulateEvent(refPointerEvent);
}
int32_t FakeDeviceManager::simulatePointerScrollEvent(int32_t nDeviceId, PointerScrollEvent::POINTER_SCROLL_DIR eScrollDir
													, double fX, double fY, bool bAnyButtonPressed) noexcept
{
	auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
	if (!refDevice) {
		return -1; //-----------------------------------------------------------
	}
	shared_ptr<PointerCapability> refPointerCapability;
	const bool bFound = refDevice->getCapability(refPointerCapability);
	if (!bFound) {
		return -1; //-----------------------------------------------------------
	}
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	shared_ptr<Event> refPointerScrollEvent = std::make_shared<PointerScrollEvent>(nTimeUsec, shared_ptr<Accessor>{}
												, refPointerCapability, eScrollDir, fX, fY, bAnyButtonPressed);
	return simulateEvent(refPointerScrollEvent);
}
int32_t FakeDeviceManager::simulateTouchEvent(int32_t nDeviceId, TouchEvent::TOUCH_INPUT_TYPE eType
											, double fX, double fY, int64_t nFingerId) noexcept
{
	auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
	if (!refDevice) {
		return -1; //-----------------------------------------------------------
	}
	shared_ptr<TouchCapability> refTouchCapability;
	const bool bFound = refDevice->getCapability(refTouchCapability);
	if (!bFound) {
		return -1; //-----------------------------------------------------------
	}
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	shared_ptr<Event> refTouchEvent = std::make_shared<TouchEvent>(nTimeUsec, shared_ptr<Accessor>{}, refTouchCapability
												, eType, fX, fY, nFingerId);
	return simulateEvent(refTouchEvent);
}
int32_t FakeDeviceManager::simulateJoystickButtonEvent(int32_t nDeviceId, JoystickButtonEvent::BUTTON_INPUT_TYPE eType
														, JoystickCapability::BUTTON eButton) noexcept
{
	auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
	if (!refDevice) {
		return -1; //-----------------------------------------------------------
	}
	shared_ptr<JoystickCapability> refJoystickCapability;
	const bool bFound = refDevice->getCapability(refJoystickCapability);
	if (!bFound) {
		return -1; //-----------------------------------------------------------
	}
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	shared_ptr<Event> refJoystickButtonEvent = std::make_shared<JoystickButtonEvent>(
								nTimeUsec, shared_ptr<Accessor>{}, refJoystickCapability
								, eType, eButton);
	return simulateEvent(refJoystickButtonEvent);
}
int32_t FakeDeviceManager::simulateJoystickHatEvent(int32_t nDeviceId, int32_t nHat
													, JoystickCapability::HAT_VALUE eValue, JoystickCapability::HAT_VALUE ePreviousValue) noexcept
{
	auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
	if (!refDevice) {
		return -1; //-----------------------------------------------------------
	}
	shared_ptr<JoystickCapability> refJoystickCapability;
	const bool bFound = refDevice->getCapability(refJoystickCapability);
	if (!bFound) {
		return -1; //-----------------------------------------------------------
	}
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	shared_ptr<Event> refJoystickHatEvent = std::make_shared<JoystickHatEvent>(
								nTimeUsec, shared_ptr<Accessor>{}, refJoystickCapability
								, nHat, eValue, ePreviousValue);
	return simulateEvent(refJoystickHatEvent);
}
int32_t FakeDeviceManager::simulateJoystickAxisEvent(int32_t nDeviceId, JoystickCapability::AXIS eAxis, int32_t nValue) noexcept
{
	auto refDevice = BasicDeviceManager::getDevice(nDeviceId);
	if (!refDevice) {
		return -1; //-----------------------------------------------------------
	}
	shared_ptr<JoystickCapability> refJoystickCapability;
	const bool bFound = refDevice->getCapability(refJoystickCapability);
	if (!bFound) {
		return -1; //-----------------------------------------------------------
	}
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	shared_ptr<Event> refJoystickAxisEvent = std::make_shared<JoystickAxisEvent>(
								nTimeUsec, shared_ptr<Accessor>{}, refJoystickCapability
								, eAxis, nValue);
	return simulateEvent(refJoystickAxisEvent);
}
void FakeDeviceManager::finalizeListener(ListenerData& /*oListenerData*/) noexcept
{
}
void FakeDeviceManager::sendDeviceMgmtToListeners(const stmi::DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice) noexcept
{
	auto refCapa = getDeviceMgmtCapability();
	//
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	//
	shared_ptr<DeviceMgmtEvent> refEvent = std::make_shared<DeviceMgmtEvent>(nTimeUsec, refCapa, eMgmtType, refDevice);
	simulateEvent(refEvent);
}

} // namespace testing

} // namespace stmi

