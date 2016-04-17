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
 * File:   stddevicemanager.h
 */

#ifndef _STMI_STD_DEVICE_MANAGER_H_
#define _STMI_STD_DEVICE_MANAGER_H_

#include <typeinfo>
#include <unordered_map>
#include <vector>
#include <list>
#include <string>

#include "childdevicemanager.h"
//#include "devicemgmtevent.h"

namespace stmi
{

/** Helper class for implementing a device manager.
 */
class StdDeviceManager : public ChildDeviceManager
{
public:
	shared_ptr<Device> getDevice(int32_t nDeviceId) const override;

	std::vector<int32_t> getDevicesWithCapabilityClass(const Capability::Class& oCapabilityClass) const override;
	std::vector<int32_t> getDevices() const override;

	std::vector<Capability::Class> getCapabilityClasses() const override;
	std::vector<Event::Class> getEventClasses() const override;

	bool getEventClassEnabled(const Event::Class& oEventClass) const override;

	void enableEventClass(const Event::Class& oEventClass) override;

	/** Default empty implementation.
	 * @return `false`
	 */
	bool addAccessor(const shared_ptr<Accessor>& /*refAccessor*/) override { return false; }
	/** Default empty implementation.
	 * @return `false`
	 */
	bool removeAccessor(const shared_ptr<Accessor>& /*refAccessor*/) override { return false; }

	bool addEventListener(const shared_ptr<EventListener>& refEventListener, const shared_ptr<CallIf>& refCallIf) override;
	bool removeEventListener(const shared_ptr<EventListener>& refEventListener, bool bFinalize) override;

protected:
	/** Constructor.
	 * If bEnableEventClasses is `true` then all event classes in aEnDisableEventClass are enabled, all others disabled,
	 * if `false` then all event classes supported by this instance are enabled except those in aEnDisableEventClass.
	 * StdDeviceManager doesn't allow disabling event classes once constructed, only enabling.
	 * Example: To enable all the event classes supported by this instance pass
	 *
	 *     bEnableEventClasses = false,  aEnDisableEventClass = {}
	 *
	 * @param aCapabitityClass Vector of registered capability classes supported by this manager.
	 * @param aEventClass Vector of registered event classes supported by this manager.
	 * @param bEnableEventClasses Whether to enable or disable all but aEnDisableEventType.
	 * @param aEnDisableEventClass The event classes to be enabled or disabled according to bEnableEventClasses.
	 */
	StdDeviceManager(const std::vector<Capability::Class>& aCapabitityClass
					, const std::vector<Event::Class>& aEventClass
					, bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass);

	/** When adding a device, subclasses of StdDeviceManager should call this. */
	bool addDevice(const shared_ptr<Device>& refDevice);
	/** When removing a device, subclasses of StdDeviceManager should call this */
	bool removeDevice(const shared_ptr<Device>& refDevice);

	/** Tells whether the caller is within a listener callback. */
	// Function that might be useful to subclasses
	inline bool isWithinAListenerCallback() const { return (m_nListenerListRecursing > 0); }

	class ListenerData;

	/** Finalize listener.
	 * This function is optionally called by this class when a listener is about to be removed.
	 * Subclasses should implement it by sending (if the listener`s callif allows it)
	 * "cancels" for pressed keys, pressed mouse buttons, unclosed touch sequences, etc.
	 * @param oListenerData Interface to call the listener's callback.
	 */
	virtual void finalizeListener(ListenerData& oListenerData) = 0;
	/** Returns the listeners data pointers.
	 * The returned value prevents the data pointed to by the elements
	 * of the list to be deleted while the std::shared_ptr is held.
	 * This also means that StdDeviceManager subclasses shouldn't store the shared_ptr
	 * because it would prevent deletion of unused ListenerData objects.
	 * @return The reference counted pointer to the list.
	 */
	shared_ptr< const std::list< ListenerData* > > getListeners();
	/** Returns the index of the event class in the array passed to the constructor.
	 * The index can be used by 
	 * ListenerData::handleEventCallIf(int32_t, const shared_ptr<Event>&, const shared_ptr<const Accessor>&) const
	 * for faster access to the simplified callif expression for the event class.
	 * This function can be called  from the constructor of a subclass to initialize
	 * variables but has to be called after the call of this class' constructor.
	 * @param oEventClass The class.
	 * @return Index or -1 if not supported event class.
	 */
	int32_t getEventClassIndex(const Event::Class& oEventClass) const;
	/** Calls overloaded ListenerExtraData::reset() for each listener. */
	void resetExtraDataOfAllListeners();
	/** Base class to store extra listener data.
	 * Subclass this class to attach additional data to the listener.
	 * The subclass is constructed within StdDeviceManager and must have a
	 * public constructor with no parameters.
	 * The lifetime of an instance is the same as the ListenerData object
	 * it is associated with.
	 */
	class ListenerExtraData
	{
	public:
		/** Empty default constructor. */
		ListenerExtraData() {}
		virtual ~ListenerExtraData() {}
		/** Resets the extra data.
		 * Called by StdDeviceManager::resetExtraDataOfAllListeners().
		 * Override this function to clear the custom data in a subclass of
		 * ListenerExtraData.
		 */
		virtual void reset() {}
	};
	/** Helper class that represents a listener.
	 */
	class ListenerData final
	{
	public:
		ListenerData()
		: m_p1Owner(nullptr)
		, m_bListenerWasRemoved(false)
		, m_bListenerRemoving(false)
		, m_nAddedTimeUsec(-1)
		, m_p0EventListener(nullptr)
		{
		}
		/** Send event to the listener without callif evaluation.
		 * @param refEvent Event to send.
		 * @return Whether the event was sent.
		 */
		bool handleEvent(const shared_ptr<Event>& refEvent) const;
		/** Send event to the listener with callif evaluation.
		 * The event is sent only if callif allows it.
		 * @param nClassTypeIdx The index of the class. If you pass -1 it calls getEventClassIndex(const Event::Class&) const.
		 * @param refEvent The event to send.
		 * @return Whether the event was sent.
		 */
		bool handleEventCallIf(int32_t nClassTypeIdx, const shared_ptr<Event>& refEvent) const;
		/** The time the listener was added to the device manager.
		 * @return In nanoseconds from epoch.
		 */
		inline int64_t getAddedTime() const { return m_nAddedTimeUsec; }
		/** The callif associated with the listener.
		 * @return If null means always send event to listener (@see class CallIfTrue).
		 */
		inline shared_ptr<CallIf> getCallIf() const { return m_refCallIf; }
		/** The simplified callif for one of the supported event classes.
		 * @param nClassTypeIdx The index of the class. @see getEventClassIndex(const Event::Class&) const
		 * @return If null means always send event to listener (@see class CallIfTrue).
		 */
		inline shared_ptr<CallIf> getCallIf(int32_t nClassTypeIdx) const
		{
			assert((nClassTypeIdx >= 0) && (nClassTypeIdx < static_cast<int32_t>(m_aCallIfEventClass.size())));
			return m_aCallIfEventClass[nClassTypeIdx];
		}
		/** Get listener's extra data.
		 * The first time this function is called an instance of type ED is created
		 * with its default constructor. Further calls must have the same type ED
		 * and will return the same instance.
		 * 
		 * ED must be a subclass of ListenerExtraData and should override method
		 * ListenerExtraData::reset() to initialize the data.
		 * 
		 * @param p0ExtraData The pointer to the data.
		 */
		template<class ED>
		void getExtraData(ED*& p0ExtraData)
		{
			static_assert(std::is_base_of<ListenerExtraData,ED>::value, "");
			if (!m_refExtraData) {
				m_refExtraData = std::unique_ptr<ED>(new ED());
				m_refExtraData->reset();
			}
			assert(dynamic_cast<ED*>(&(*m_refExtraData)) != nullptr);
			p0ExtraData = static_cast<ED*>(&(*m_refExtraData));
		}
	private:
		bool handleEventCommon(int32_t nClassTypeIdx, const shared_ptr<Event>& refEvent) const;
	private:
		friend class StdDeviceManager;
		StdDeviceManager* m_p1Owner;
		//TODO: instead of two bools use an enum ACTIVE, REMOVING, REMOVED
		bool m_bListenerWasRemoved;
		bool m_bListenerRemoving; // Invariant: if m_bListenerRemoving == true then m_bListenerWasRemoved = true
		std::unique_ptr<ListenerExtraData> m_refExtraData; // StdDeviceManager subclass defined data
		int64_t m_nAddedTimeUsec;
		shared_ptr<CallIf> m_refCallIf; // The callif without simplification
		// One callif for each registered event class type supported by this device manager
		std::vector< shared_ptr<CallIf> > m_aCallIfEventClass; // Size: m_p1Owner->m_aEventClass.size(), Value: refSimplifiedCallIf
		weak_ptr<EventListener> m_refEventListener;
		EventListener* m_p0EventListener; // Useful in case the weak ptr becomes null.
		std::list< ListenerData* >::iterator m_itListeners; // "pointer" to element within list *m_refListeners
	private:
		ListenerData& operator=(const ListenerData& oSource) = delete;
		ListenerData(const ListenerData& oSource) = delete;
	};

private:
	void maybeRemoveDataOfRemovedListeners();

private:
	const std::vector<Capability::Class> m_aCapabitityClass;
	const std::vector<Event::Class> m_aEventClass;

	std::vector<bool> m_aEventClassEnabled; // Size: m_aEventClass.size()

	// All added devices
	std::unordered_map<int32_t, shared_ptr<Device> > m_oDevices; // Key: device id, Value: Device

	std::list< ListenerData > m_oListenersData;
	// This shared_ptr that prevents the elements in m_oListenersData to be removed
	// while in use, that is when the reference count of m_refListeners is > 1
	// The list is actually the same as m_oListenersData, but its elements are pointers
	// into m_oListenersData elements.
	// Motivation for two lists: getListeners() can return the const list of pointers without
	// the ListenerData objects themselves being constant.
	// TODO Maybe just keep one list of std::unique<ListenerData>, it would allow
	// getListeners() to return a const std::list< const std::unique< ListenerData > >
	shared_ptr< std::list< ListenerData* > > m_refListeners;
	// Invariants:
	// - If more ListenersData objects have the same m_p0EventListener, only one or none
	//   can have (m_bListenerWasRemoved == false)

	// Incremented while finalize is called:
	// is not really needed except for debugging StdDeviceManager subclasses which shouldn't
	// call themselves removeListener() while not having a m_refListeners reference
	int32_t m_nListenerListRecursing;
	// Tells whether one or more ListenerData were marked as removed
	bool m_bListenerListDirty; 

private:
	StdDeviceManager(const StdDeviceManager& oSource) = delete;
	StdDeviceManager& operator=(const StdDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif	/* _STMI_STD_DEVICE_MANAGER_H_ */
