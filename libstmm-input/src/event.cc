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
 * File:   event.cc
 */

#include "event.h"

namespace stmi
{

using std::shared_ptr;

bool Event::isEventClassIdRegistered(const std::string& sEventClassId) noexcept
{
	return getNamedTypes().hasStringId(sEventClassId.c_str());
}
Event::Class Event::getEventClassIdClass(const std::string& sEventClassId) noexcept
{
	bool bFound;
	return Class(getNamedTypes().getTypeFromStringId(sEventClassId.c_str(), bFound));
}
bool Event::isEventTypeRegistered(const std::type_info& oEventType) noexcept
{
	return getNamedTypes().hasType(oEventType);
}
const std::type_info& Event::getEventClassIdType(const std::string& sEventClassId, bool& bRegistered) noexcept
{
	assert(!sEventClassId.empty());
	return getNamedTypes().getTypeFromStringId(sEventClassId.data(), bRegistered);
}
std::string Event::getEventTypeClassId(const std::type_info& oType) noexcept
{
	bool bIsXYEvent;
	const char* p0TypeId = getNamedTypes().getDataFromType(oType, bIsXYEvent);
	if (p0TypeId == nullptr) {
		return "";
	}
	return p0TypeId;
}

Event::Event(const Class& oClass, int64_t nTimeUsec, int32_t nCapabilityId, const shared_ptr<Accessor>& refAccessor) noexcept
: m_nTimeUsec(nTimeUsec)
, m_nCapabilityId(nCapabilityId)
, m_refAccessor(refAccessor)
, m_oClass(oClass)
{
	assert((nTimeUsec >= 0) || (nTimeUsec == -1));
	assert(nCapabilityId >= 0);
	assert(oClass); // Class has to be registered
}

} // namespace stmi

