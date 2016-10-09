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
 * File:   spinn.cc
 */

#include "spinn.h"

#include <stmm-input-ev/stmm-input-ev.h>

namespace example
{
namespace spinn
{

void Spinn::handleAllEvents(const shared_ptr<stmi::Event>& refEvent)
{
	auto p0Event = refEvent.get();
	auto nOldValue = m_nValue;
	auto aKeys = p0Event->getAsKeys();
	for (auto& oPair : aKeys) {
		const stmi::Event::AS_KEY_INPUT_TYPE eAsType = oPair.second;
		if (eAsType == stmi::Event::AS_KEY_PRESS) {
			const stmi::HARDWARE_KEY eAsKey = oPair.first;
			switch (eAsKey) {
			case stmi::HK_UP: ++m_nValue; break;
			case stmi::HK_DOWN: --m_nValue; break;
			case stmi::HK_BTN_LEFT: m_nValue += 10; break;
			case stmi::HK_BTN_RIGHT: m_nValue -= 10; break;
			case stmi::HK_BTN_A: m_nValue += 100; break;
			case stmi::HK_BTN_B: m_nValue -= 100; break;
			default: break;
			}
		}
	}
	if (nOldValue != m_nValue) {
		m_oVauleChangedSignal.emit();
	}
}

} // namespace spinn

} // namespace example
