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
 * File:   capability.cc
 */

#include "capability.h"

namespace stmi
{

bool Capability::isCapabilityClassIdRegistered(const std::string& sCapabilityClassId)
{
	return getNamedTypes().hasStringId(sCapabilityClassId.c_str());
}
Capability::Class Capability::getCapabilityClassIdClass(const std::string& sCapabilityClassId)
{
	bool bFound;
	return Class(getNamedTypes().getTypeFromStringId(sCapabilityClassId.c_str(), bFound));
}
bool Capability::isCapabilityTypeRegistered(const std::type_info& oCapabilityType)
{
	return getNamedTypes().hasType(oCapabilityType);
}

const std::type_info& Capability::getCapabilityClassIdType(const std::string& sCapabilityClassId, bool& bRegistered)
{
	assert(!sCapabilityClassId.empty());
	return getNamedTypes().getTypeFromStringId(sCapabilityClassId.data(), bRegistered);
}
std::string Capability::getCapabilityTypeClassId(const std::type_info& oType)
{
	bool bSpecial;
	const char* p0TypeId = getNamedTypes().getDataFromType(oType, bSpecial);
	if (p0TypeId == nullptr) {
		return "";
	}
	return p0TypeId;
}

Capability::Capability(const Class& oClass)
: m_oClass(oClass)
{
	assert(oClass);
}


////////////////////////////////////////////////////////////////////////////////
DeviceManagerCapability::DeviceManagerCapability(const Class& oClass)
: Capability(oClass)
{
}

} // namespace stmi

