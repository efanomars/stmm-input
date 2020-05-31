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
 * File:   keyevent.cc
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#include "keyevent.h"

namespace stmi { class Accessor; }

namespace stmi
{

const char* const KeyEvent::s_sClassId = "stmi::Keys:KeyEvent";
Event::RegisterClass<KeyEvent> KeyEvent::s_oInstall(s_sClassId);


KeyEvent::KeyEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<KeyCapability>& refKeyCapability, KEY_INPUT_TYPE eType, HARDWARE_KEY eKey) noexcept
: Event(s_oInstall.getEventClass(), nTimeUsec, (refKeyCapability ? refKeyCapability->getId() : -1), refAccessor)
, m_eType(eType)
, m_eKey(eKey)
, m_refKeyCapability(refKeyCapability)
{
	assert((eType >= KEY_PRESS) || (eType <= KEY_RELEASE_CANCEL));
	assert(eKey >= 0);
	assert(refKeyCapability);
}
bool KeyEvent::getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const noexcept
{
	eKey = m_eKey;
	eType = static_cast<AS_KEY_INPUT_TYPE>(m_eType);
	bMoreThanOne = false;
	return true;
}

} // namespace stmi

