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
 * File:   touchevent.h
 */

#ifndef _STMI_TOUCH_EVENT_H_
#define _STMI_TOUCH_EVENT_H_

#include <cassert>
#include <iostream>

#include <stmm-input/xyevent.h>

#include "touchcapability.h"

namespace stmi
{

using std::weak_ptr;

/** Event generated when the state of a touch is changed.
 * 
 * Note that the reference to the capability that generated this event is weak.
 */
class TouchEvent : public XYEvent
{
public:
	/** The touch event type. */
	enum TOUCH_INPUT_TYPE {
		TOUCH_BEGIN = 1
		, TOUCH_UPDATE = 2
		, TOUCH_END = 3
		, TOUCH_CANCEL = 4
	};
	/** Constructor.
	 * @param nTimeUsec Time from epoch in microseconds.
	 * @param refAccessor The accessor used to generate the event. Can be null.
	 * @param refTouchCapability The capability that generated this event.
	 * @param eType The touch event type.
	 * @param fX The X position of the touch.
	 * @param fY The Y position of the touch.
	 * @param nFingerId The finger id performing the touch.
	 */
	TouchEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
				, const shared_ptr<TouchCapability>& refTouchCapability, TOUCH_INPUT_TYPE eType
				, double fX, double fY, int64_t nFingerId);
	/** The touch event type. */
	inline TOUCH_INPUT_TYPE getType() const { return m_eType; }
	/**  The finger id performing the touch. */
	inline int64_t getFingerId() const { return m_nFingerId; }
	/** The touch capability that generated the event.
	 * @return The capability or null if the capability was deleted.
	 */
	inline shared_ptr<TouchCapability> getTouchCapability() const { return m_refTouchCapability.lock(); }
	//
	shared_ptr<Capability> getCapability() const override { return m_refTouchCapability.lock(); }
	//
	static const char* const s_sClassId;
	static const Event::Class& getClass()
	{
		static const Event::Class s_oTouchClass = s_oInstall.getEventClass();
		return s_oTouchClass;
	}
protected:
	inline void setType(TOUCH_INPUT_TYPE eType)
	{
		assert((eType >= TOUCH_BEGIN) && (eType <= TOUCH_CANCEL));
		m_eType = eType;
	}
	inline void setFingerId(int64_t nFingerId) { m_nFingerId = nFingerId; }
	inline void setTouchCapability(const shared_ptr<TouchCapability>& refTouchCapability)
	{
		assert(refTouchCapability);
		m_refTouchCapability = refTouchCapability;
	}
private:
	TOUCH_INPUT_TYPE m_eType;
	int64_t m_nFingerId;
	weak_ptr<TouchCapability> m_refTouchCapability;
	//
	static RegisterClass<TouchEvent> s_oInstall;
private:
	TouchEvent() = delete;
};

} // namespace stmi

#endif	/* _STMI_TOUCH_EVENT_H_ */
