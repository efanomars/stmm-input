/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   event.h
 */

#ifndef _STMI_EVENT_H_
#define _STMI_EVENT_H_

#include "hardwarekey.h"
#include "private-namedtypes.h"
#include "capability.h"
#include "accessor.h"

#include <typeinfo>
#include <type_traits>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cassert>

namespace stmi
{

using std::shared_ptr;

class XYEvent;

/** Base event class.
 * Usage: if an event listener's callback (stmi::EventListener)
 * is called by the device manager, you get the actual registered subclass of the event 
 * by statically casting as follows:
 *
 *     void handleEvent(const shared_ptr<stmi::Event>& refEvent)
 *     {
 *       const stmi::Event::Class& oC = refEvent->getEventClass();
 *       auto& oT = oC.getTypeInfo();
 *       if (oT == typeid(stmi::KeyEvent)) {
 *         handleKeyEvent(static_cast<stmi::KeyEvent*>(refEvent.get()));
 *       } else if (oT == typeid(stmi::PointerEvent)) {
 *         handlePointerEvent(static_cast<stmi::PointerEvent*>(refEvent.get()));
 *       } // and so on
 *     }
 *
 * with specific registered class implementations:
 *
 *     void handleKeyEvent(stmi::KeyEvent* pEvent) { ... }
 *     void handlePointerEvent(stmi::PointerEvent* pEvent) { ... }
 *     // and so on
 */
class Event
{
public:
	virtual ~Event() = default;

	/** Returns the time this event originated.
	 * @return The time is in microseconds from epoch or -1 if unknown.
	 */
	inline int64_t getTimeUsec() const { return m_nTimeUsec; }
	/** Returns the capability that generated this event.
	 * The Event subclass implementation keeps only a weak_ptr on the capability to avoid reference cycles.
	 * @return The capability or null if the capability was deleted.
	 */
	virtual shared_ptr<Capability> getCapability() const = 0;
	/** Returns the id of the capability that generated this event.
	 * The unique id isn't changed when the capability is removed and
	 * getCapability() returns null.
	 * @return The capability id.
	 */
	inline int32_t getCapabilityId() const { return m_nCapabilityId; }
	/** Returns the accessor that helped generate this event.
	 * @return The accessor. Can be null.
	 */
	inline const shared_ptr<Accessor>& getAccessor() const { return m_refAccessor; }

	/** Key simulation type.
	 */
	enum AS_KEY_INPUT_TYPE {
		AS_KEY_PRESS = 1
		, AS_KEY_RELEASE = 2
		, AS_KEY_RELEASE_CANCEL = 3
	};
	/** Tells whether the event can simulate hardware keys.
	 * If it can the parameters nKey and eType will
	 * contain the first of the simulated keys, bMoreThanOne whether there are more.
	 * If it's the case (`bMoreThanOne == true`) then getAsKeys() should be called, 
	 * which returns all the simulated keys (the first included).
	 * If the function returns `false` the parameters are left unchanged. The default implementation
	 * returns `false`.
	 * 
	 * Ex.: a subclass JoystickHatEvent might return, for transition from HAT_CENTER to HAT_RIGHTUP,
	 *  the first key (nKey=KEY_RIGHT, eType=AS_KEY_PRESS) and bMoreThanOne=true
	 *  while getAsKeys() returns all keys {(KEY_RIGHT,AS_KEY_PRESS), (KEY_UP,AS_KEY_PRESS)}.
	 * @param eKey The first simulated hardware key (or unchanged if no keys).
	 * @param eType The type of the first simulated key (or unchanged if no keys).
	 * @param bMoreThanOne If more than one key is simulated (or unchanged if no keys).
	 * @return Whether keys are simulated.
	 */
	virtual bool getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const
	{
		// To avoid warnings about unused parameters.
		return false && (0 != (bMoreThanOne ? 0 * static_cast<int32_t>(eKey) + 0 * static_cast<int32_t>(eType) : 0));
	}
	/** All the keys this event simulates.
	 * The default implementation calls getAsKey() and returns an empty set if no keys are simulated,
	 * otherwise a set containing the first (nKey, eType) pair.
	 * Therefore subclass implementations have to override this function only if 
	 * more than one key is simulated.
	 * 
	 * Ex.: a subclass PointerScrollEvent may return {(KEY_SCROLLUP,AS_KEY_PRESS), (KEY_SCROLLUP,AS_KEY_RELEASE)},
	 *  if mouse`s wheel was scrolled up.
	 * @return Vector of (HARDWARE_KEY,AS_KEY_INPUT_TYPE) pairs that simulate the event. Empty if no key simulated.
	 */
	virtual std::vector< std::pair<HARDWARE_KEY, AS_KEY_INPUT_TYPE> > getAsKeys() const
	{
		HARDWARE_KEY eKey;
		AS_KEY_INPUT_TYPE eType;
		bool bMoreThanOne = false;
		const bool bHasKeys = getAsKey(eKey, eType, bMoreThanOne);
		//assert(bMoreThanOne == false);
		if (bHasKeys) {
			return { std::make_pair(eKey, eType) };
		} else  {
			return {};
		}
	}
	/** The representation of a registered event class.
	 */
	class Class final
	{
	public:
		/** Constructs an empty event class instance.
		 */
		Class()
		: m_p0EventType(nullptr)
		, m_bIsXYEvent(false)
		{
		}
		/** Constructs an event class instance.
		 * If the passed c++ typeid is not of a registered class an empty
		 * class is constructed.
		 * @param oEventType underlying c++ typeid of the class.
		 */
		Class(const std::type_info& oEventType)
		: Class(oEventType, false)
		{
		}
		/** Compares a registered event class instance with a c++ typeid.
		 * If the class instance is empty `false` is returned.
		 * @param oOtherEventType The c++ typeid.
		 * @return The result of the comparison.
		 */
		inline bool operator==(const std::type_info& oOtherEventType) const
		{
			if (m_p0EventType == nullptr) {
				return false;
			}
			return ((*m_p0EventType) == oOtherEventType);
		}
		/**
		 * @see operator==(const std::type_info&)
		 */
		inline bool operator!=(const std::type_info& oOtherEventType) const
		{
			return !operator==(oOtherEventType);
		}
		/** Compares a registered event class to another.
		 * If both are empty `true` is returned.
		 * @param oOther The other class instance. Might be empty.
		 * @return  The result of the comparison.
		 */
		inline bool operator==(const Class& oOther) const
		{
			if (m_p0EventType == nullptr) {
				return (oOther.m_p0EventType == nullptr);
			}
			return ((*m_p0EventType) == (*oOther.m_p0EventType));
		}
		/**
		 * @see operator==(const Class&)
		 */
		inline bool operator!=(const Class& oOther) const
		{
			return !operator==(oOther);
		}
		/** Tells whether class is registered.
		 * @return Whether class is not empty.
		 */
		inline operator bool() const
		{
			return (m_p0EventType != nullptr);
		}
		/** The underlying c++ typeid of the class.
		 * @return The c++ type or if class is empty an invalid value.
		 * @see operator bool() const
		 */
		inline const std::type_info& getTypeInfo() const
		{
			return *m_p0EventType;
		}
		/** The class id.
		 * If the class is empty returns empty string.
		 * @return The class id.
		 */
		inline std::string getId() const
		{
			if (m_p0EventType == nullptr) {
				return "";
			}
			return getEventTypeClassId(*m_p0EventType);
		}
		/** Tells whether you can statically cast an instance of this Class to XYEvent.
		 * @see class XYEvent
		 * @return Whether this registered class is subclass of XYEvent.
		 */
		inline bool isXYEvent() const
		{
			return m_bIsXYEvent;
		}
	private:
		Class(const std::type_info& oEventType, bool bIsXYEvent)
		: m_p0EventType((nullptr != getNamedTypes().getDataFromType(oEventType, bIsXYEvent)) ? &oEventType : nullptr)
		, m_bIsXYEvent(bIsXYEvent)
		{
			if (m_p0EventType == nullptr) {
				std::cout << "Event::Class(oEventType) oEventType '" << oEventType.name() << "' not registered!" << std::endl;
			}
		}
	private:
		const std::type_info* m_p0EventType;
		bool m_bIsXYEvent;
	};
	/** Get the registered class of the event instance.
	 * @return The registered class.
	 */
	inline const Class& getEventClass() const
	{
		return m_oClass;
	}
	/** Tells whether the given event class id is registered.
	 * @param sEventClassId The class id.
	 * @return Whether registered.
	 */
	static bool isEventClassIdRegistered(const std::string& sEventClassId);
	/** Gets the event class with given class id.
	 * @param sEventClassId The registered class id.
	 * @return The class. Might be empty if sEventClassId is not registered.
	 */
	static Class getEventClassIdClass(const std::string& sEventClassId);

protected:
	/** Constructor to be called from subclasses.
	 * The registered class has to be the actual class of the instance being constructed
	 * or a superclass of it.
	 * @param oClass The registered class of the event.
	 * @param nTimeUsec Time from epoch in microseconds.
	 * @param nCapabilityId The id of the capability that generated this event. Must be >= 0.
	 * @param refAccessor The accessor used to generate the event. Can be null.
	 */
	Event(const Class& oClass, int64_t nTimeUsec, int32_t nCapabilityId, const shared_ptr<Accessor>& refAccessor);
	/** Constructor to be called from subclasses (with empty accessor).
	 * The registered class has to be the actual class of the instance being constructed
	 * or a superclass of it.
	 * @param oClass The registered class of the event.
	 * @param nTimeUsec Time from epoch in microseconds.
	 * @param nCapabilityId The id of the capability that generated this event. Must be >= 0.
	 */
	Event(const Class& oClass, int64_t nTimeUsec, int32_t nCapabilityId)
	: Event(oClass, nTimeUsec, nCapabilityId, Accessor::s_refEmptyAccessor)
	{
	}
	/** Set the event time.
	 * @param nTimeUsec Time from epoch in microseconds.
	 */
	inline void setTimeUsec(int64_t nTimeUsec) { m_nTimeUsec = nTimeUsec; }
	/** Set the capability id.
	 * @param nCapabilityId The id of the capability that generated this event. Must be >= 0.
	 */
	inline void setCapabilityId(int32_t nCapabilityId) { assert(nCapabilityId >= 0); m_nCapabilityId = nCapabilityId; }
	/** Set the accessor.
	 * @param refAccessor Can be null.
	 */
	inline void setAccessor(const shared_ptr<Accessor>& refAccessor) { m_refAccessor = refAccessor; }
	/** Template used to register event classes.
	 * In a subclass of Event you want to register define (ex. KeyEvent):
	 *
	 *     class KeyEvent : public Event {
	 *     public:
	 *       ...
	 *       static const char* const s_sClassId;
	 *       static const Event::Class& getClass()
	 *       {
	 *         static const Event::Class s_oKeyClass = s_oInstall.getEventClass();
	 *         return s_oKeyClass;
	 *       }
	 *     private:
	 *       static RegisterClass<KeyEvent> s_oInstall;
	 *       ...
	 *     }
	 *
	 * with the instantiation
	 *
	 *     const char* const KeyEvent::s_sClassId = "KeyEvent";
	 *     // this instance registers the class!
	 *     Event::RegisterClass<KeyEvent> KeyEvent::s_oInstall(s_sClassId);
	 *
	 * Beware! No registered class should be subclass of another registered class!
	 */
	template<class T>
	class RegisterClass final
	{
	public:
		/** Class registration constructor.
		 * The class id must be a statically defined unique ascii string with length > 0.
		 * @param p0EventClassId The class id. Can't be nullptr.
		 */
		RegisterClass(const char* const &p0EventClassId)
		{
			static_assert(std::is_base_of<Event,T>::value, "Wrong type.");
			static_assert(!std::is_same<Event,T>::value, "Wrong type.");
			static_assert(!std::is_same<XYEvent,T>::value, "Wrong type.");
			assert(p0EventClassId != nullptr);
			assert((*p0EventClassId) != static_cast<char>(0));
			assert(!getNamedTypes().hasType(typeid(T)));
			getNamedTypes().addType(typeid(T), p0EventClassId, std::is_base_of<XYEvent,T>::value);
			m_oFirstInstanceClass = Class(typeid(T));
		}
		~RegisterClass()
		{
			// The following would be good in a run-time loaded shared object situation
			// but is bad because at least g++ doesn't unload (destroy static objects)
			// in the reverse order of the loading so that it can occur that
			// stmm-input is unloaded before stmm-input-ev !!!
			//if (m_oFirstInstanceClass) {
			//	const std::type_info& oTypeInfo = m_oFirstInstanceClass.getTypeInfo();
			//	if (getNamedTypes().hasType(oTypeInfo)) {
			//		getNamedTypes().removeType(oTypeInfo);
			//	}
			//}
		}
		/** Get the class of the registered type.
		 * @return A non-empty class.
		 */
		inline const Class& getEventClass()
		{
			return m_oFirstInstanceClass;
		}
	private:
		Event::Class m_oFirstInstanceClass;
	private:
		RegisterClass() = delete;
	};
	/** Tells whether an event type was registered.
	 * @param oEventType The c++ typeid of a class.
	 * @return Whether the class was registered.
	 */
	static bool isEventTypeRegistered(const std::type_info& oEventType);
	/** Gets the type of the registered event subclass associated with class id.
	 * @param sEventClassId The type string.
	 * @param bRegistered Whether the return value is valid.
	 * @return The registered type. If bRegistered is `false` the result is undefined.
	 */
	static const std::type_info& getEventClassIdType(const std::string& sEventClassId, bool& bRegistered);
	/** Gets the class id of the given type.
	 * @param oType The type to be tested.
	 * @return The class id or an empty string if the type is not registered.
	 */
	static std::string getEventTypeClassId(const std::type_info& oType);

private:
	// Deferred unique instance creation (Scott Meyers singleton)
	static Private::NamedTypes<bool>& getNamedTypes()
	{
		static Private::NamedTypes<bool> s_oNamedTypes;
		return s_oNamedTypes;
	}
private:
	int64_t m_nTimeUsec;
	int32_t m_nCapabilityId;
	shared_ptr<Accessor> m_refAccessor;
	//
	const Event::Class m_oClass;

private:
	Event() = delete;
};

} // namespace stmi

namespace std {
	template <>
	struct hash<stmi::Event::Class>
	{
		std::size_t operator()(const stmi::Event::Class& oClass) const
		{
			if (!oClass) {
				return 0;
			}
			return oClass.getTypeInfo().hash_code();
		}
	};
} // namespace std

#endif	/* _STMI_EVENT_H_ */
