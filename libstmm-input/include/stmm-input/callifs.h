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
 * File:   callifs.h
 */

#ifndef STMI_CALLIFS_H
#define STMI_CALLIFS_H

#include "devicemanager.h"
#include "capability.h"
#include "device.h"
#include "event.h"

#include <cassert>
#include <string>
#include <functional>
#include <cstdint>
#include <memory>
#include <typeinfo>

namespace stmi { class Accessor; }

namespace stmi
{

////////////////////////////////////////////////////////////////////////////////
/** Always call back listener.
 */
class CallIfTrue final : public CallIf
{
public:
	//
	bool operator()(const shared_ptr<const Event>& /*refEvent*/) const noexcept override
	{
		return true;
	}
	/** The singleton CallIfTrue instance.
	 * @return The only instance. Cannot be null.
	 */
	static const shared_ptr<CallIfTrue>& getInstance() noexcept
	{
		static const shared_ptr<CallIfTrue> s_refCallIfTrue(new CallIfTrue());
		return s_refCallIfTrue;
	}
private:
	CallIfTrue() = default;
};
////////////////////////////////////////////////////////////////////////////////
/** Never call back listener.
 */
class CallIfFalse final : public CallIf
{
public:
	//
	bool operator()(const shared_ptr<const Event>& /*refEvent*/) const noexcept override
	{
		return false;
	}
	/** The singleton CallIfFalse instance.
	 * @return The only instance. Cannot be null.
	 */
	static const shared_ptr<CallIfFalse>& getInstance() noexcept
	{
		static shared_ptr<CallIfFalse> s_refCallIfFalse(new CallIfFalse());
		return s_refCallIfFalse;
	}
private:
	CallIfFalse() = default;
};

////////////////////////////////////////////////////////////////////////////////
/** Applies a "conditional AND" to two callif conditions.
 */
class CallIfAnd final : public CallIf
{
public:
	CallIfAnd(const shared_ptr<CallIf>& refCallIf1, const shared_ptr<CallIf>& refCallIf2) noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		return m_refCallIf1->operator()(refEvent) && m_refCallIf2->operator()(refEvent);
	}
	inline const shared_ptr<CallIf>& getCallIf1() const noexcept { return m_refCallIf1; }
	inline const shared_ptr<CallIf>& getCallIf2() const noexcept { return m_refCallIf2; }
private:
	const shared_ptr<CallIf> m_refCallIf1;
	const shared_ptr<CallIf> m_refCallIf2;
private:
	CallIfAnd() = delete;
};

////////////////////////////////////////////////////////////////////////////////
/** Applies a "conditional OR" to two callif conditions.
 */
class CallIfOr final : public CallIf
{
public:
	CallIfOr(const shared_ptr<CallIf>& refCallIf1, const shared_ptr<CallIf>& refCallIf2) noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		return m_refCallIf1->operator()(refEvent) || m_refCallIf2->operator()(refEvent);
	}
	inline const shared_ptr<CallIf>& getCallIf1() const noexcept { return m_refCallIf1; }
	inline const shared_ptr<CallIf>& getCallIf2() const noexcept { return m_refCallIf2; }
private:
	const shared_ptr<CallIf> m_refCallIf1;
	const shared_ptr<CallIf> m_refCallIf2;
private:
	CallIfOr() = delete;
};

////////////////////////////////////////////////////////////////////////////////
/** Applies a "NOT" to a callif condition.
 */
class CallIfNot final : public CallIf
{
public:
	/** Constructor.
	 * @param refCallIf The negated callif. Cannot be null.
	 */
	explicit CallIfNot(const shared_ptr<CallIf>& refCallIf) noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		return ! (m_refCallIf->operator()(refEvent));
	}
	/** The negated callif.
	 */
	inline const shared_ptr<CallIf>& getCallIf() const noexcept { return m_refCallIf; }
private:
	const shared_ptr<CallIf> m_refCallIf;
private:
	CallIfNot() = delete;
};

////////////////////////////////////////////////////////////////////////////////
/** Accessor callif.
 * Selects events with the given accessor.
 */
class CallIfAccessor : public CallIf
{
public:
	/** Empty constructor.
	 * Selects events that didn't need an accessor to be generated.
	 */
	CallIfAccessor() noexcept;
	/** Constructor.
	 * Selects events that needed the given accessor to be generated,
	 * if null same as empty constructor, otherwise instances of this class keep
	 * a strong reference to the accessor.
	 * @param refAccessor Accessor subclass instance or null.
	 */
	explicit CallIfAccessor(const shared_ptr<const Accessor>& refAccessor) noexcept;
	/** Event selection function.
	 * Note: if both this callif's accessor and the `refAccessor` are null, `true` is returned.
	 */
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		const bool bMemberNull = (!m_refAccessor);
		const shared_ptr<const Accessor>& refAccessor = refEvent->getAccessor();
		const bool bOtherNull = (!refAccessor);
		if (bOtherNull || bMemberNull) {
			// if both null, it's a match!
			return (bMemberNull == bOtherNull);
		}
		// compare objects
		return ((*m_refAccessor) == (*refAccessor));
	}
	/** The accessor to be selected.
	 * @return The accessor. Can be null.
	 */
	inline const shared_ptr<const Accessor>& getAccessor() const noexcept { return m_refAccessor; }
private:
	const shared_ptr<const Accessor> m_refAccessor;
};

////////////////////////////////////////////////////////////////////////////////
/** Device id callif.
 * Selects events generated by a specific device instance identified by its id.
 */
class CallIfDeviceId : public CallIf
{
public:
	/** Empty constructor.
	 * Selects an event if device of its capability is not defined.
	 */
	CallIfDeviceId() noexcept;
	/** Constructor that selects events with a specific device id.
	 * @param nDeviceId The device id. Must be `>=0`.
	 */
	explicit CallIfDeviceId(int32_t nDeviceId) noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		assert(refEvent);
		const shared_ptr<Capability> refCapability = refEvent->getCapability();
		if (!refCapability) {
			// capability was deleted
			return false; //----------------------------------------------------
		}
		const shared_ptr<Device> refDevice = refCapability->getDevice();
		const bool bDeviceIsNull = !refDevice;
		if (m_nDeviceId < 0) {
			// select event if device is not defined, that is
			// it's a device manager capability or the device was deleted
			return bDeviceIsNull; //--------------------------------------------
		}
		if (bDeviceIsNull) {
			return false; //----------------------------------------------------
		}
		return (refDevice->getId() == m_nDeviceId);
	}
	/** The device id to be selected.
	 * @return The device id or -1.
	 */
	inline int32_t getDeviceId() const noexcept { return m_nDeviceId; }
private:
	const int32_t m_nDeviceId;
};

////////////////////////////////////////////////////////////////////////////////
/** Capability id callif.
 * Selects events generated by a specific capability instance identified by its id.
 */
class CallIfCapabilityId : public CallIf
{
public:
	/** Constructor.
	 * Selects events with a specific capability id.
	 * @param nCapabilityId The capability id. Must be `>=0`.
	 */
	explicit CallIfCapabilityId(int32_t nCapabilityId) noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		assert(refEvent);
		return (m_nCapabilityId == refEvent->getCapabilityId());
	}
	/** The capability id to be selected.
	 * @return The capability id. Always >= 0.
	 */
	inline int32_t getCapabilityId() const noexcept { return m_nCapabilityId; }
private:
	const int32_t m_nCapabilityId;
private:
	CallIfCapabilityId() = delete;
};

////////////////////////////////////////////////////////////////////////////////
/** Capability class callif.
 * Selects events generated by the instances of a specific registered capability class.
 */
class CallIfCapabilityClass : public CallIf
{
public:
	/** Constructor.
	 * @param oClass The capability class. Must be registered.
	 */
	explicit CallIfCapabilityClass(const Capability::Class& oClass) noexcept;
	/** Type id constructor.
	 * @param oCapabilityType The capability class by typeid. Must be registered.
	 */
	explicit CallIfCapabilityClass(const std::type_info& oCapabilityType) noexcept;
	/** String id Constructor.
	 * @param sCapabilityClassId The registered capability class string id. Cannot be null.
	 */
	explicit CallIfCapabilityClass(const std::string& sCapabilityClassId) noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		assert(refEvent);
		if (! (refEvent->getCapability()) ) {
			return false;
		}
		return (m_oClass == refEvent->getCapability()->getCapabilityClass());
	}
	/** The registered capability class to be selected.
	 */
	inline const Capability::Class& getCapabilityClass() const noexcept { return m_oClass; }
private:
	const Capability::Class m_oClass;
private:
	CallIfCapabilityClass() = delete;
};

////////////////////////////////////////////////////////////////////////////////
/** Device manager capability callif.
 * Selects events generated by capabilities that are subclasses of stmi::DeviceManagerCapability.
 */
class CallIfDeviceManagerCapability : public CallIf
{
public:
	CallIfDeviceManagerCapability() noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		assert(refEvent);
		if (! (refEvent->getCapability())) {
			return false;
		}
		return refEvent->getCapability()->getCapabilityClass().isDeviceManagerCapability();
	}
};

////////////////////////////////////////////////////////////////////////////////
/** Event class callif.
 * Selects events that are (sub)classes of a given registered event class.
 */
class CallIfEventClass final : public CallIf
{
public:
	/** Constructor.
	 * @param oClass The event class. Must be registered.
	 */
	explicit CallIfEventClass(const Event::Class& oClass) noexcept;
	/** Type id constructor.
	 * @param oEventType The event class by typeid. Must be registered.
	 */
	explicit CallIfEventClass(const std::type_info& oEventType) noexcept;
	/** String id Constructor.
	 * @param sEventClassId The registered event class string id. Cannot be null.
	 */
	explicit CallIfEventClass(const std::string& sEventClassId) noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		assert(refEvent);
		return (m_oClass == refEvent->getEventClass());
	}
	/** The registered event class to be selected.
	 */
	inline const Event::Class& getClass() const noexcept { return m_oClass; }
private:
	const Event::Class m_oClass;
private:
	CallIfEventClass() = delete;
};

////////////////////////////////////////////////////////////////////////////////
/** XYEvent class callif.
 * Selects events that are subclasses of XYEvent.
 */
class CallIfXYEvent final : public CallIf
{
public:
	CallIfXYEvent() noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		assert(refEvent);
		return refEvent->getEventClass().isXYEvent();
	}
};

////////////////////////////////////////////////////////////////////////////////
/** Function callif.
 * Selects an event according to the return value of a custom function.
 *
 * Beware! The function should not cause reference cycles that can lead
 * to memory leaks.
 *
 * The function should not throw exceptions (be noexcept).
 */
class CallIfFuction final : public CallIf
{
public:
	using CallIfFunction = std::function<bool(const shared_ptr<const Event>&)>;
	//
	explicit CallIfFuction(CallIfFunction&& oCallIfFunction) noexcept;
	explicit CallIfFuction(const CallIfFunction& oCallIfFunction) noexcept;
	//
	bool operator()(const shared_ptr<const Event>& refEvent) const noexcept override
	{
		return m_oCallIfFunction(refEvent);
	}
private:
	const CallIfFunction m_oCallIfFunction;
private:
	CallIfFuction() = delete;
};

} // namespace stmi

#endif /* STMI_CALLIFS_H */

