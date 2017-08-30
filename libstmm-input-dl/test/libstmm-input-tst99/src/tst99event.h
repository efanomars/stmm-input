/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   tst99event.h
 */

#ifndef _STMI_TST_99_EVENT_H_
#define _STMI_TST_99_EVENT_H_

#include "tst99capability.h"

#include <stmm-input/xyevent.h>

#include <cassert>
#include <utility>
#include <vector>

namespace stmi
{

using std::weak_ptr;

class Tst99Event : public XYEvent
{
public:
	Tst99Event(int64_t nTimeUsec, const shared_ptr<Tst99Capability>& refTst99Capability, int32_t nButton);

	inline int32_t getButton() const { return m_nButton; }

	inline shared_ptr<Tst99Capability> getTst99Capability() const { return m_refTst99Capability.lock(); }
	//
	shared_ptr<Capability> getCapability() const override { return m_refTst99Capability.lock(); }
	//
	static const char* const s_sClassId;
	static const Event::Class& getClass()
	{
		static const Event::Class s_oTst99Class = s_oInstall.getEventClass();
		return s_oTst99Class;
	}
private:
	int32_t m_nButton;
	weak_ptr<Tst99Capability> m_refTst99Capability;
	//
	static RegisterClass<Tst99Event> s_oInstall;
private:
	Tst99Event() = delete;
};


} // namespace stmi

#endif /* _STMI_TST_99_EVENT_H_ */

