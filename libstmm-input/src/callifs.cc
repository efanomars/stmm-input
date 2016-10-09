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
 * File:   callifs.cc
 */

#include "callifs.h"

namespace stmi
{

///////////////////////////////////////////////////////////////////////////////
CallIfAnd::CallIfAnd(const shared_ptr<CallIf>& refCallIf1, const shared_ptr<CallIf>& refCallIf2)
: m_refCallIf1(refCallIf1)
, m_refCallIf2(refCallIf2)
{
	assert(refCallIf1);
	assert(refCallIf2);
}

///////////////////////////////////////////////////////////////////////////////
CallIfOr::CallIfOr(const shared_ptr<CallIf>& refCallIf1, const shared_ptr<CallIf>& refCallIf2)
: m_refCallIf1(refCallIf1)
, m_refCallIf2(refCallIf2)
{
	assert(refCallIf1);
	assert(refCallIf2);
}

///////////////////////////////////////////////////////////////////////////////
CallIfNot::CallIfNot(const shared_ptr<CallIf>& refCallIf)
: m_refCallIf(refCallIf)
{
	// can be null
}

///////////////////////////////////////////////////////////////////////////////
CallIfAccessor::CallIfAccessor()
: m_refAccessor(Accessor::s_refEmptyAccessor)
{
}
CallIfAccessor::CallIfAccessor(const shared_ptr<const Accessor>& refAccessor)
: m_refAccessor(refAccessor)
{
}

////////////////////////////////////////////////////////////////////////////////
CallIfDeviceId::CallIfDeviceId()
: m_nDeviceId(-1)
{
}
CallIfDeviceId::CallIfDeviceId(int32_t nDeviceId)
: m_nDeviceId(nDeviceId)
{
	assert(nDeviceId >= 0);
}


////////////////////////////////////////////////////////////////////////////////
CallIfCapabilityId::CallIfCapabilityId(int32_t nCapabilityId)
: m_nCapabilityId(nCapabilityId)
{
	assert(nCapabilityId >= 0);
}

////////////////////////////////////////////////////////////////////////////////
CallIfCapabilityClass::CallIfCapabilityClass(const Capability::Class& oClass)
: m_oClass(oClass)
{
	assert(oClass);
}
CallIfCapabilityClass::CallIfCapabilityClass(const std::string& sCapabilityClassId)
: CallIfCapabilityClass(Capability::getCapabilityClassIdClass(sCapabilityClassId))
{
}

///////////////////////////////////////////////////////////////////////////////
CallIfDeviceManagerCapability::CallIfDeviceManagerCapability()
{
}

///////////////////////////////////////////////////////////////////////////////
CallIfEventClass::CallIfEventClass(const Event::Class& oClass)
: m_oClass(oClass)
{
	assert(m_oClass);
}
CallIfEventClass::CallIfEventClass(const std::string& sEventClassId)
: CallIfEventClass(Event::getEventClassIdClass(sEventClassId))
{
}

///////////////////////////////////////////////////////////////////////////////
CallIfXYEvent::CallIfXYEvent()
{
}

///////////////////////////////////////////////////////////////////////////////
CallIfFuction::CallIfFuction(CallIfFunction&& oCallIfFunction)
: m_oCallIfFunction(std::move(oCallIfFunction))
{
}
CallIfFuction::CallIfFuction(const CallIfFunction& oCallIfFunction)
: m_oCallIfFunction(oCallIfFunction)
{
}

} // namespace stmi

