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
 * File:   fakedevicemanager.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef _ZIMTRIS_TESTING_FAKE_DEVICE_MANAGER_H_
#define _ZIMTRIS_TESTING_FAKE_DEVICE_MANAGER_H_

#include <stmm-input-ev/stddevicemanager.h>
#include <stmm-input-ev/devicemgmtevent.h>
#include <stmm-input-ev/joystickcapability.h>
#include <stmm-input-ev/joystickevent.h>
#include <stmm-input-ev/keyevent.h>
#include <stmm-input-ev/pointerevent.h>
#include <stmm-input-ev/touchevent.h>
#include <stmm-input/device.h>
#include <stmm-input/event.h>
#include <stmm-input/hardwarekey.h>

#include <memory>
#include <vector>
#include <type_traits>
#include <cassert>

#include <stdint.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

/** Class for testing code that interacts with device managers.
 * It allows to add and remove listeners, create and remove devices (which trigger
 * a DeviceMgmtEvent to be sent to listeners), send events to the listeners but
 * without providing a consistent state of the fake devices. This means that
 * when a device or a listener is removed pressed keys aren't canceled and that
 * the user has to make sure for example that the press of a key isn't followed
 * by another press of the same key but rather by a release or a cancel.
 * This also means that the JoystickCapability's methods shouldn't be called by
 * the to be tested code because they return bogus values.
 *
 * This class is supposed to be instantiated with std::make_shared().
 */
class FakeDeviceManager : public StdDeviceManager
{
public:
	/** Constructor.
	 * Only the given event classes are enabled.
	 * @param bEnableEventClasses Whether to enable or disable all but aEnDisableEventClasses.
	 * @param aEnDisableEventClasses The event classes to be enabled or disabled according to bEnableEventClasses.
	 */
	FakeDeviceManager(bool bEnableEventClasses, const std::vector< Event::Class > &aEnDisableEventClasses) noexcept;
	/** Default constructor.
	 * All event classes are enabled.
	 */
	FakeDeviceManager() noexcept;

	/** Removes a device.
	 * @param nDeviceId The device id as returned by simulateNewXXXDevice().
	 * @return Whether the device was removed.
	 */
	bool simulateRemoveDevice(int32_t nDeviceId) noexcept;
	/** Sends a changed event to the listeners.
	 * @return Whether could send the event.
	 */
	bool simulateChangedDevice(int32_t nDeviceId) noexcept;
	/** Adds a device with single capability class.
	 * Example:
	 *       const int32_t nKeyDevId = refFakeDM->simulateNewDevice&lt;FakeKeyDevice&gt;();
	 *
	 * @return The id of the added device.
	 */
	template<class DEV>
	int32_t simulateNewDevice() noexcept
	{
		static_assert(std::is_base_of<Device, DEV>::value, "");
		auto refChildThis = shared_from_this();
		auto refDevice = std::make_shared<DEV>(refChildThis, "");
		#ifndef NDEBUG
		const bool bAdded =
		#endif
		StdDeviceManager::addDevice(refDevice);
		assert(bAdded);
		sendDeviceMgmtToListeners(stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED, refDevice);
		Device* p0Device = refDevice.get();
		return p0Device->getId();
	}
	/** Sends an event to the listeners.
	 * @param refEvent The event. Cannot be null.
	 * @return The number of listeners to which the event was actually sent or `-1` if not enabled.
	 */
	int32_t simulateEvent(const shared_ptr<Event>& refEvent) noexcept;
	/** Generates a key event and sends it to listeners.
	 * If the device doesn't exists or it has not a key capability `-1` is returned.
	 * @param nDeviceId The id of the device with the key capability.
	 * @param eType The key input type.
	 * @param eKey The key.
	 * @return The number of listeners to which the event was actually sent or `-1` if some error.
	 */
	int32_t simulateKeyEvent(int32_t nDeviceId, KeyEvent::KEY_INPUT_TYPE eType, HARDWARE_KEY eKey) noexcept;
	/** Generates a pointer event and sends it to listeners.
	 * If the device doesn't exists or it has not a pointer capability `-1` is returned.
	 * Note: the caller has to set the consistent values for the
	 * `bAnyButtonPressed` and `bWasAnyButtonPressed` parameters to reflect
	 * the current (and preceding) state of the pointer buttons.
	 * @param nDeviceId The id of the device with the pointer capability.
	 * @param fX The X position of the pointer.
	 * @param fY The Y position of the pointer.
	 * @param eType The key input type.
	 * @param nButton The button involved or `-1` if hover.
	 * @param bAnyButtonPressed Whether any button is pressed (after this event).
	 * @param bWasAnyButtonPressed Whether any button was pressed before this event.
	 * @return The number of listeners to which the event was actually sent or `-1` if some error.
	 */
	int32_t simulatePointerEvent(int32_t nDeviceId, double fX, double fY, PointerEvent::POINTER_INPUT_TYPE eType
									, int32_t nButton, bool bAnyButtonPressed, bool bWasAnyButtonPressed) noexcept;
	/** Generates a pointer scroll event and sends it to listeners.
	 * If the device doesn't exists or it has not a pointer capability `-1` is returned.
	 * Note: the caller has to set the consistent value for the `bAnyButtonPressed`
	 * parameter to reflect the current state of the pointer buttons.
	 * @param nDeviceId The id of the device with the pointer capability.
	 * @param eScrollDir The direction of the scroll.
	 * @param fX The X position of the pointer.
	 * @param fY The Y position of the pointer.
	 * @param bAnyButtonPressed Whether any button is pressed.
	 * @return The number of listeners to which the event was actually sent or `-1` if some error.
	 */
	int32_t simulatePointerScrollEvent(int32_t nDeviceId, PointerScrollEvent::POINTER_SCROLL_DIR eScrollDir
											, double fX, double fY, bool bAnyButtonPressed) noexcept;
	/** Generates a touch event and sends it to listeners.
	 * @param nDeviceId The id of the device with the touch capability.
	 * @param eType The touch event type.
	 * @param fX The X position of the touch.
	 * @param fY The Y position of the touch.
	 * @param nFingerId The id of the finger performing the touch.
	 * @return The number of listeners to which the event was actually sent or `-1` if some error.
	 */
	int32_t simulateTouchEvent(int32_t nDeviceId, TouchEvent::TOUCH_INPUT_TYPE eType
									, double fX, double fY, int64_t nFingerId) noexcept;
	/** Generates a joystick button event and sends it to listeners.
	 * @param nDeviceId The id of the device with the joystick capability.
	 * @param eType What happened to the button.
	 * @param eButton The button.
	 * @return The number of listeners to which the event was actually sent or `-1` if some error.
	 */
	int32_t simulateJoystickButtonEvent(int32_t nDeviceId, JoystickButtonEvent::BUTTON_INPUT_TYPE eType
										, JoystickCapability::BUTTON eButton) noexcept;
	/** Generates a joystick hat event and sends it to listeners.
	 * Note: the caller has to set the consistent value for the `ePreviousValue`
	 * parameter to reflect the current state of the hat.
	 * If ePreviousValue is set to JoystickCapability::HAT_VALUE_NOT_SET
	 * key simulation is disabled (Event::getAsKey() returns `false`).
	 * @param nDeviceId The id of the device with the joystick capability.
	 * @param nHat The hat that changed its state. Must be >= 0.
	 * @param eValue The new value for the hat. Can't be JoystickCapability::HAT_VALUE_NOT_SET.
	 * @param ePreviousValue The previous value for the hat. Can be JoystickCapability::HAT_VALUE_NOT_SET. Can't be HAT_CENTER_CANCEL.
	 * @return The number of listeners to which the event was actually sent or `-1` if some error.
	 */
	int32_t simulateJoystickHatEvent(int32_t nDeviceId, int32_t nHat
								, JoystickCapability::HAT_VALUE eValue, JoystickCapability::HAT_VALUE ePreviousValue) noexcept;
	/** Generates a joystick axis event with key simulation and sends it to listeners.
	 * @param nDeviceId The id of the device with the joystick capability.
	 * @param eAxis The axis.
	 * @param nValue Value normalized to [-32767, 32767].
	 * @return The number of listeners to which the event was actually sent or `-1` if some error.
	 */
	int32_t simulateJoystickAxisEvent(int32_t nDeviceId, JoystickCapability::AXIS eAxis, int32_t nValue) noexcept;
protected:
	/** Does nothing. */
	void finalizeListener(ListenerData& oListenerData) noexcept override;

private:
	void sendDeviceMgmtToListeners(const stmi::DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice) noexcept;

private:
	FakeDeviceManager(const FakeDeviceManager& oSource) = delete;
	FakeDeviceManager& operator=(const FakeDeviceManager& oSource) = delete;
};

} // namespace testing

} // namespace stmi

#endif /* _ZIMTRIS_TESTING_FAKE_DEVICE_MANAGER_H_ */

