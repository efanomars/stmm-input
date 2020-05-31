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
 * File:   xyevent.cc
 */

#include "xyevent.h"

namespace stmi { class Accessor; }

namespace stmi
{

XYEvent::XYEvent(const Class& oClass, int64_t nTimeUsec, int32_t nCapabilityId
				, const shared_ptr<Accessor>& refAccessor
				, double fX, double fY, XY_GRAB_INPUT_TYPE eXYGrabType, int64_t nGrabId) noexcept
: Event(oClass, nTimeUsec, nCapabilityId, refAccessor)
, m_fX(fX)
, m_fY(fY)
, m_bIsModified(false)
, m_eXYGrabType(eXYGrabType)
, m_nGrabId(nGrabId)
{
	assert((eXYGrabType >= XY_HOVER) && (eXYGrabType <= XY_UNGRAB_CANCEL));
}

XYEvent::XYEvent(const Class& oClass, int64_t nTimeUsec, int32_t nCapabilityId
				, const shared_ptr<Accessor>& refAccessor, double fX, double fY) noexcept
: XYEvent(oClass, nTimeUsec, nCapabilityId, refAccessor, fX, fY, XY_HOVER, -1)
{
}

void XYEvent::translateXY(double fX, double fY) noexcept
{
	const double fSaveX = m_fX;
	const double fSaveY = m_fY;
	m_fX += fX;
	m_fY += fY;
	if ((fSaveX != m_fX) || (fSaveY != m_fY)) {
		m_bIsModified = true;
	}
}
void XYEvent::scaleXY(double fSX, double fSY) noexcept
{
	const double fSaveX = m_fX;
	const double fSaveY = m_fY;
	m_fX *= fSX;
	m_fY *= fSY;
	if ((fSaveX != m_fX) || (fSaveY != m_fY)) {
		m_bIsModified = true;
	}
}

} // namespace stmi

