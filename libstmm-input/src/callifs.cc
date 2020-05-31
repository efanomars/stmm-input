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
 * File:   callifs.cc
 */

#include "callifs.h"
#include "accessor.h"

#include <utility>

namespace stmi
{

///////////////////////////////////////////////////////////////////////////////
CallIfAnd::CallIfAnd(const shared_ptr<CallIf>& refCallIf1, const shared_ptr<CallIf>& refCallIf2) noexcept
: m_refCallIf1(refCallIf1)
, m_refCallIf2(refCallIf2)
{
	assert(refCallIf1);
	assert(refCallIf2);
}

///////////////////////////////////////////////////////////////////////////////
CallIfOr::CallIfOr(const shared_ptr<CallIf>& refCallIf1, const shared_ptr<CallIf>& refCallIf2) noexcept
: m_refCallIf1(refCallIf1)
, m_refCallIf2(refCallIf2)
{
	assert(refCallIf1);
	assert(refCallIf2);
}

///////////////////////////////////////////////////////////////////////////////
CallIfNot::CallIfNot(const shared_ptr<CallIf>& refCallIf) noexcept
: m_refCallIf(refCallIf)
{
	// can be null
}

///////////////////////////////////////////////////////////////////////////////
CallIfAccessor::CallIfAccessor() noexcept
: m_refAccessor(Accessor::s_refEmptyAccessor)
{
}
CallIfAccessor::CallIfAccessor(const shared_ptr<const Accessor>& refAccessor) noexcept
: m_refAccessor(refAccessor)
{
}

////////////////////////////////////////////////////////////////////////////////
CallIfDeviceId::CallIfDeviceId() noexcept
: m_nDeviceId(-1)
{
}
CallIfDeviceId::CallIfDeviceId(int32_t nDeviceId) noexcept
: m_nDeviceId(nDeviceId)
{
	assert(nDeviceId >= 0);
}


////////////////////////////////////////////////////////////////////////////////
CallIfCapabilityId::CallIfCapabilityId(int32_t nCapabilityId) noexcept
: m_nCapabilityId(nCapabilityId)
{
	assert(nCapabilityId >= 0);
}

////////////////////////////////////////////////////////////////////////////////
CallIfCapabilityClass::CallIfCapabilityClass(const Capability::Class& oClass) noexcept
: m_oClass(oClass)
{
	assert(oClass);
}
CallIfCapabilityClass::CallIfCapabilityClass(const std::type_info& oCapabilityType) noexcept
: CallIfCapabilityClass(Capability::Class{oCapabilityType})
{
}
CallIfCapabilityClass::CallIfCapabilityClass(const std::string& sCapabilityClassId) noexcept
: CallIfCapabilityClass(Capability::getCapabilityClassIdClass(sCapabilityClassId))
{
}

///////////////////////////////////////////////////////////////////////////////
CallIfDeviceManagerCapability::CallIfDeviceManagerCapability() noexcept
{
}

///////////////////////////////////////////////////////////////////////////////
CallIfEventClass::CallIfEventClass(const Event::Class& oClass) noexcept
: m_oClass(oClass)
{
	assert(m_oClass);
}
CallIfEventClass::CallIfEventClass(const std::type_info& oEventType) noexcept
: CallIfEventClass(Event::Class{oEventType})
{
}
CallIfEventClass::CallIfEventClass(const std::string& sEventClassId) noexcept
: CallIfEventClass(Event::getEventClassIdClass(sEventClassId))
{
}

///////////////////////////////////////////////////////////////////////////////
CallIfXYEvent::CallIfXYEvent() noexcept
{
}

///////////////////////////////////////////////////////////////////////////////
CallIfFuction::CallIfFuction(CallIfFunction&& oCallIfFunction) noexcept
: m_oCallIfFunction(std::move(oCallIfFunction))
{
}
CallIfFuction::CallIfFuction(const CallIfFunction& oCallIfFunction) noexcept
: m_oCallIfFunction(oCallIfFunction)
{
}

} // namespace stmi

