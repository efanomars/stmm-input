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
 * File:   keyevent.h
 */

#ifndef _STMI_KEY_EVENT_H_
#define _STMI_KEY_EVENT_H_

#include "keycapability.h"

#include <stmm-input/event.h>

#include <cassert>

namespace stmi
{

using std::weak_ptr;

/** Event generated when the state of a key is changed.
 * Note that the reference to the capability that generated this event is weak.
 */
class KeyEvent : public Event
{
public:
	/** @see Same as Event::AS_KEY_INPUT_TYPE
	 */
	enum KEY_INPUT_TYPE {
		KEY_PRESS = AS_KEY_PRESS
		, KEY_RELEASE = AS_KEY_RELEASE
		, KEY_RELEASE_CANCEL = AS_KEY_RELEASE_CANCEL
	};
	/** Constructor.
	 * @param nTimeUsec Time from epoch in microseconds.
	 * @param refAccessor The accessor used to generate the event. Can be null.
	 * @param refKeyCapability The capability that generated this event. Cannot be null.
	 * @param eType The action type.
	 * @param eKey The key involved.
	 */
	KeyEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
			, const shared_ptr<KeyCapability>& refKeyCapability, KEY_INPUT_TYPE eType, HARDWARE_KEY eKey);
	//
	inline KEY_INPUT_TYPE getType() const { return m_eType; }
	inline HARDWARE_KEY getKey() const { return m_eKey; }
	/** The key capability that generated the event.
	 * @return The capability or null if the capability was deleted.
	 */
	inline shared_ptr<KeyCapability> getKeyCapability() const { return m_refKeyCapability.lock(); }
	//
	shared_ptr<Capability> getCapability() const override { return m_refKeyCapability.lock(); }
	/** Returns the key.
	 * @param eKey Same as getKey().
	 * @param eType Analogous to getType().
	 * @param bMoreThanOne Always `false`.
	 * @return Always `true`.
	 */
	bool getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const override;
	//
	static const char* const s_sClassId;
	static const Event::Class& getClass()
	{
		static const Event::Class s_oKeyClass = s_oInstall.getEventClass();
		return s_oKeyClass;
	}
protected:
	/** Sets the type.
	 * @param eType The action type.
	 */
	inline void setType(KEY_INPUT_TYPE eType)
	{
		assert((eType >= KEY_PRESS) || (eType <= KEY_RELEASE_CANCEL));
		m_eType = eType;
	}
	/** Sets the key.
	 * @param eKey The key involved.
	 */
	inline void setKey(HARDWARE_KEY eKey)
	{
		assert(eKey >= 0);
		m_eKey = eKey;
	}
	/** Sets the capability.
	 * @param refKeyCapability The capability that generated this event. Cannot be null.
	 */
	inline void setKeyCapability(const shared_ptr<KeyCapability>& refKeyCapability)
	{
		assert(refKeyCapability);
		setCapabilityId(refKeyCapability->getId());
		m_refKeyCapability = refKeyCapability;
	}
private:
	KEY_INPUT_TYPE m_eType;
	HARDWARE_KEY m_eKey;
	weak_ptr<KeyCapability> m_refKeyCapability;
	//
	static RegisterClass<KeyEvent> s_oInstall;
private:
	KeyEvent() = delete;
};

} // namespace stmi

#endif	/* _STMI_KEY_EVENT_H_ */
