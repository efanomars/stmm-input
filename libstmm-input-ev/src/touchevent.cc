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
 * File:   touchevent.cc
 */

#include "touchevent.h"

namespace stmi
{

TouchEvent::TouchEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
						, const shared_ptr<TouchCapability>& refTouchCapability, TOUCH_INPUT_TYPE eType
						, double fX, double fY, int64_t nFingerId)
: XYEvent(s_oInstall.getEventClass(), nTimeUsec, refAccessor, fX, fY, XY_HOVER, nFingerId)
, m_eType(eType)
, m_nFingerId(nFingerId)
, m_refTouchCapability(refTouchCapability)
{
	assert((eType >= TOUCH_BEGIN) && (eType <= TOUCH_CANCEL));
	XY_GRAB_INPUT_TYPE eXYGrabType = XY_UNGRAB_CANCEL;
	switch (eType) {
	case TOUCH_BEGIN: eXYGrabType = XY_GRAB; break;
	case TOUCH_UPDATE: eXYGrabType = XY_MOVE; break;
	case TOUCH_END: eXYGrabType = XY_UNGRAB; break;
	case TOUCH_CANCEL: eXYGrabType = XY_UNGRAB_CANCEL; break;
	default: assert(false);
	}
	setXYGrab(eXYGrabType, nFingerId);
}
const char* const TouchEvent::s_sClassId = "Touch:TouchEvent";
Event::RegisterClass<TouchEvent> TouchEvent::s_oInstall(s_sClassId);

} // namespace stmi
