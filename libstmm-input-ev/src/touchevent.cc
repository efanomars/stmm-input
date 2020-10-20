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
 * File:   touchevent.cc
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#include "stmm-input-ev/touchevent.h"

namespace stmi { class Accessor; }

namespace stmi
{

const char* const TouchEvent::s_sClassId = "stmi::Touch:TouchEvent";
Event::RegisterClass<TouchEvent> TouchEvent::s_oInstall(s_sClassId);

TouchEvent::TouchEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
						, const shared_ptr<TouchCapability>& refTouchCapability, TOUCH_INPUT_TYPE eType
						, double fX, double fY, int64_t nFingerId) noexcept
: XYEvent(s_oInstall.getEventClass(), nTimeUsec, (refTouchCapability ? refTouchCapability->getId() : -1)
		, refAccessor, fX, fY, XY_HOVER, nFingerId)
, m_eType(eType)
, m_nFingerId(nFingerId)
, m_refTouchCapability(refTouchCapability)
{
	setTypeAndFinger(eType, nFingerId);
}

} // namespace stmi
