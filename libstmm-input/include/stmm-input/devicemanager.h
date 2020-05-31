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
 * File:   devicemanager.h
 */

#ifndef STMI_DEVICE_MANAGER_H
#define STMI_DEVICE_MANAGER_H

#include "event.h"
#include "capability.h"

#include <vector>
#include <chrono>
#include <memory>
#include <functional>
#include <type_traits>
#include <cassert>
#include <cstdint>

namespace stmi { class Accessor; }
namespace stmi { class Device; }

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;


/** The callback function type of the event listener.
 * Should not throw exceptions.
 * @see DeviceManager::addEventListener().
 * @param refEvent The event. Can't be null.
 */
using EventListener = std::function<void(const shared_ptr<Event>&) noexcept>;

/** Base class used to determine whether to send an event to a listener.
 * @see DeviceManager::addEventListener(const shared_ptr<EventListener>&, const shared_ptr<CallIf>&)
 */
class CallIf
{
public:
	virtual ~CallIf() noexcept = default;
	/** Function that determines whether an event should be sent to a listener.
	 * @param refEvent The event. Cannot be null.
	 * @return `true` if the event is to be sent to the listener, `false` otherwise.
	 */
	virtual bool operator()(const shared_ptr<const Event>& refEvent) const noexcept = 0;
protected:
	CallIf() noexcept {}
};

/** This class manages devices that generate events that listeners will receive.
 */
class DeviceManager
{
public:
	virtual ~DeviceManager() noexcept = default;

	/** Returns the device with the given id, or null if not found.
	 */
	virtual shared_ptr<Device> getDevice(int32_t nDeviceId) const noexcept = 0;

	/** The registered capability classes of the device manager.
	 * Example: the vector {DeviceMgmtCapability::getClass()} is returned.
	 * @return The set of registered device manager capability classes.
	 */
	virtual std::vector<Capability::Class> getCapabilityClasses() const noexcept = 0;
	/** The registered capability classes of the device manager's devices.
	 * The capability classes include those that managed devices potentially have,
	 * independently from the currently available devices.
	 *
	 * Example: a device manager that handles mice and touch pads will return
	 * {PointerCapability::getClass(), TouchCapability::getClass()}
	 * even though no touch device is connected.
	 * @return The set of registered device capability classes.
	 */
	virtual std::vector<Capability::Class> getDeviceCapabilityClasses() const noexcept = 0;
	/** Request a registered device manager capability.
	 * @param refCapa [out] The registered DeviceManagerCapability subclass or null if not supported by device manager.
	 * @return Whether the device manager has the requested capability.
	 */
	template < typename TCapa >
	bool getCapability(shared_ptr<TCapa>& refCapa) const noexcept
	{
		static_assert(std::is_base_of<DeviceManagerCapability, TCapa>::value && !std::is_base_of<TCapa, DeviceManagerCapability>::value
						, "TCapa must be subclass of DeviceManagerCapability");
		shared_ptr<Capability> refSubCapa = getCapability(Capability::Class{typeid(TCapa)});
		if (!refSubCapa) {
			refCapa.reset();
			return false; //----------------------------------------------------
		}
		assert(refSubCapa->getCapabilityClass().isDeviceManagerCapability());
		refCapa = std::static_pointer_cast<TCapa>(refSubCapa);
		return true;
	}
	/** Requests the instance of a capability class.
	 * If the device manager doesn't have the capability null is returned.
	 * @param oClass The requested registered device manager capability class.
	 * @return The capability or null.
	 */
	virtual shared_ptr<Capability> getCapability(const Capability::Class& oClass) const noexcept = 0;
	/** Returns the device manager's capability with the given id, or null if not found.
	 */
	virtual shared_ptr<Capability> getCapability(int32_t nCapabilityId) const noexcept = 0;
	/** The registered event classes the device manager and all its devices can send.
	 * The event classes include those that managed devices potentially can send,
	 * independently from the currently available devices.
	 * @return The set of registered event classes.
	 */
	virtual std::vector<Event::Class> getEventClasses() const noexcept = 0;

	/** Returns the ids of the devices that have the given capability class.
	 * @param oCapabilityClass The registered capability class.
	 * @return The set of device ids.
	 */
	virtual std::vector<int32_t> getDevicesWithCapabilityClass(const Capability::Class& oCapabilityClass) const noexcept = 0;
	/** Returns the ids of all the device manager's devices.
	 * @return The set of device ids.
	 */
	virtual std::vector<int32_t> getDevices() const noexcept = 0;

	/** Tells whether an event class is enabled.
	 * If an event class is disabled the device manager (with all its devices) won't
	 * send events of that class to listeners.
	 * @param oEventClass The registered event class.
	 * @return Tells whether the event class is enabled.
	 */
	virtual bool isEventClassEnabled(const Event::Class& oEventClass) const noexcept = 0;
	/** Enables an event class.
	 * @see isEventClassEnabled(const Event::Class&)
	 * @param oEventClass The event class to enable.
	 */
	virtual void enableEventClass(const Event::Class& oEventClass) noexcept = 0;

	/** Adds an accessor to the device manager.
	 * If the device manager recognizes the accessor and the accessor wasn't
	 * already added `true` is returned, `false` otherwise.
	 * 
	 * It's possible to add a null accessor, it's meaning is implementation
	 * dependent.
	 * @param refAccessor The accessor to add. Can be null.
	 * @return Whether the accessor was added.
	 */
	virtual bool addAccessor(const shared_ptr<Accessor>& refAccessor) noexcept = 0;
	/** Removes an accessor from the device manager.
	 * If the accessor wasn't previously (successfully) added with
	 * addAccessor(const shared_ptr<Accessor>&), `false` is returned.
	 * @param refAccessor The accessor to remove. Can be null.
	 * @return Whether the accessor was removed.
	 */
	virtual bool removeAccessor(const shared_ptr<Accessor>& refAccessor) noexcept = 0;
	/** Tells whether the device manager has a given accessor.
	 * @param refAccessor The accessor. Can be null.
	 * @return Whether the accessor was added (and not yet removed) to the device manager.
	 */
	virtual bool hasAccessor(const shared_ptr<Accessor>& refAccessor) noexcept = 0;

	/** Adds an event listener with its callif condition to the device manager.
	 * The EventListener callback function (actually its address) identifies 
	 * the listener being added and is therefore used to remove it with removeEventListener().
	 * The callback function is called for all events originating from this 
	 * device manager's devices that satisfy the callif condition.
	 * 
	 * If the listener was already added, even with a different callif,
	 * the function has no effect and `false` is returned.
	 *
	 * The device manager keeps a weak reference (std::weak_ptr) to the listener.
	 * This means that the caller has to keep a strong reference to the listener,
	 * otherwise it will be removed automatically.
	 *
	 * A null callback condition `refCallIf` means that the callback function is
	 * called for all events. The device manager keeps a strong reference to the
	 * callif object as long as the listener remains in the device manager.
	 *
	 * The event parameter passed to the callback function can be queued
	 * (outlive the callback) or modified if its interface allows it.
	 * To get the registered class of the event use Event::getEventClass() and statically cast to it.
	 * Note: don't use `typeid(*refEvent)` since it isn't necessarily equal to 
	 * `refEvent->getEventClass().getTypeInfo()`.
	 * @see class Event.
	 *
	 * Example:
	 * 
	 *     void createListeners()
	 *         ...
	 *         // The listener has to be stored in a member variable otherwise it will
	 *         // automatically be removed from the device manager.
	 *         m_refKeyEventListener = std::make_shared<stmi::EventListener>(
	 *             [this](const shared_ptr<stmi::Event>& refEvent)
	 *             {
	 *                 // Because of the callif condition below we know that refEvent
	 *                 // is a stmi::KeyEvent
	 *                 auto pKeyEvent = static_cast<stmi::KeyEvent*>(refEvent.get());
	 *                 ...
	 *             });
	 *         m_refDeviceManager->addEventListener(m_refKeyEventListener
	 *                           , std::make_shared<stmi::CallIfEventClass>(typeid(stmi::KeyEvent)));
	 *         ...
	 *     }
	 * 
	 * @param refEventListener The listener to be added. Cannot be null.
	 * @param refCallIf The callif condition to call back the listener. Can be null.
	 * @return Whether the listener was added.
	 */
	virtual bool addEventListener(const shared_ptr<EventListener>& refEventListener
									, const shared_ptr<CallIf>& refCallIf) noexcept = 0;
	/**  Adds an event listener to the device manager.
	 * Shortcut of addEventListener(refEventListener, shared_ptr<CallIf>{}).
	 * @param refEventListener The listener to be added. Cannot be null.
	 * @return Whether the listener was added.
	 */
	virtual bool addEventListener(const shared_ptr<EventListener>& refEventListener) noexcept = 0;
	/** Removes an event listener from the device manager.
	 * If the listener wasn't found, `false` is returned, `true` otherwise.
	 * To be found the event listener must have been added with addEventListener()
	 * and not deleted since then. Beware! A copy of the event listener can't be
	 * used to remove it.
	 *
	 * Optionally finalization events are sent to the listener before its removal.
	 * Example: if a device's key is pressed when this function is called,
	 * a RELEASE_CANCEL event is sent to the listener being removed.
	 *
	 * @param refEventListener The listener to remove. Cannot be null.
	 * @param bFinalize Whether to send closing events to the listener.
	 * @return Whether listener was removed.
	 */
	virtual bool removeEventListener(const shared_ptr<EventListener>& refEventListener, bool bFinalize) noexcept = 0;
	/** Removes an event listener from the device manager.
	 * Shortcut of removeEventListener(refEventListener, false).
	 * @param refEventListener The listener to remove. Cannot be null.
	 */
	virtual bool removeEventListener(const shared_ptr<EventListener>& refEventListener) noexcept = 0;

	/** Current time from epoch in microseconds.
	 * Helper function used to set timestamp of events.
	 * @return The time in microseconds.
	 * @see Event::getTimeUsec()
	 */
	static int64_t getNowTimeMicroseconds() noexcept
	{
		const int64_t nTimeUsec = std::chrono::duration_cast<std::chrono::microseconds>(
									std::chrono::steady_clock::now().time_since_epoch()).count();
		return nTimeUsec;
	}
protected:
	DeviceManager() noexcept;

private:
	DeviceManager(const DeviceManager& oSource) = delete;
	DeviceManager& operator=(const DeviceManager& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_DEVICE_MANAGER_H */

