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
 * File:   touchevent.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMI_TOUCH_EVENT_H
#define STMI_TOUCH_EVENT_H

#include "touchcapability.h"

#include <stmm-input/xyevent.h>
#include <stmm-input/event.h>

#include <cassert>
#include <cstdint>
#include <memory>

namespace stmi { class Accessor; }
namespace stmi { class Capability; }

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
	 * @param refTouchCapability The capability that generated this event. Cannot be null.
	 * @param eType The touch event type.
	 * @param fX The X position of the touch.
	 * @param fY The Y position of the touch.
	 * @param nFingerId The id of the finger performing the touch.
	 */
	TouchEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
				, const shared_ptr<TouchCapability>& refTouchCapability, TOUCH_INPUT_TYPE eType
				, double fX, double fY, int64_t nFingerId) noexcept;
	/** The touch event type. */
	inline TOUCH_INPUT_TYPE getType() const noexcept { return m_eType; }
	/**  The finger id performing the touch. */
	inline int64_t getFingerId() const noexcept { return m_nFingerId; }
	/** The touch capability that generated the event.
	 * @return The capability or null if the capability was deleted.
	 */
	inline shared_ptr<TouchCapability> getTouchCapability() const noexcept { return m_refTouchCapability.lock(); }
	//
	shared_ptr<Capability> getCapability() const noexcept override { return m_refTouchCapability.lock(); }
	//
	static const char* const s_sClassId;
	static const Event::Class& getClass() noexcept
	{
		static const Event::Class s_oTouchClass = s_oInstall.getEventClass();
		return s_oTouchClass;
	}
protected:
	/** Sets touch and grab fields.
	 * This method also calls XYEvent::setXYGrab().
	 * @param eType The touch event type.
	 * @param nFingerId The finger id performing the touch.
	 */
	inline void setTypeAndFinger(TOUCH_INPUT_TYPE eType, int64_t nFingerId) noexcept
	{
		XY_GRAB_INPUT_TYPE eXYGrabType = XY_UNGRAB_CANCEL;
		switch (eType) {
		case TOUCH_BEGIN: eXYGrabType = XY_GRAB; break;
		case TOUCH_UPDATE: eXYGrabType = XY_MOVE; break;
		case TOUCH_END: eXYGrabType = XY_UNGRAB; break;
		case TOUCH_CANCEL: eXYGrabType = XY_UNGRAB_CANCEL; break;
		default: assert(false);
		}
		setXYGrab(eXYGrabType, nFingerId);
		m_nFingerId = nFingerId;
		m_eType = eType;
	}
	/** Sets the capability.
	 * @param refTouchCapability The capability that generated this event. Cannot be null.
	 */
	inline void setTouchCapability(const shared_ptr<TouchCapability>& refTouchCapability) noexcept
	{
		assert(refTouchCapability);
		setCapabilityId(refTouchCapability->getId());
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

#endif /* STMI_TOUCH_EVENT_H */
