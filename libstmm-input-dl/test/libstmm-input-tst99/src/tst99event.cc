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
 * File:   tst99event.cc
 */

#include "tst99event.h"

namespace stmi
{

const char* const Tst99Event::s_sClassId = "stmi::Tst99:Tst99Event";
Event::RegisterClass<Tst99Event> Tst99Event::s_oInstall(s_sClassId);

Tst99Event::Tst99Event(int64_t nTimeUsec, const shared_ptr<Tst99Capability>& refTst99Capability, int32_t nButton)
: XYEvent(s_oInstall.getEventClass(), nTimeUsec, (refTst99Capability ? refTst99Capability->getId() : -1)
			, shared_ptr<stmi::Accessor>{}, 222, 333, XY_HOVER, -1)
, m_nButton(nButton)
, m_refTst99Capability(refTst99Capability)
{
}

} // namespace stmi

