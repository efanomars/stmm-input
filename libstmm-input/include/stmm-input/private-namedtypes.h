/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   private-namedtypes.h
 */

#ifndef _STMI_PRIVATE_NAMED_TYPES_H_
#define _STMI_PRIVATE_NAMED_TYPES_H_

#include <unordered_map>
#include <typeinfo>
#include <typeindex>
#include <cstdlib>
#include <cassert>
#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG

#include <stdint.h>
#include <string.h>

namespace stmi
{
namespace Private
{

/* * Helper class to store and access unique types with their associated data.
 * Each type has a unique string id.
 */
template<class T>
class NamedTypes
{
public:
	/* * Construct empty instance. */
	NamedTypes() {}
	/* *
	 * @param oType The type
	 * @param oData [out] The data associated with the type.
	 * @return The StringId for the type. nullptr if type not found.
	 */
	const char* getDataFromType(const std::type_info& oType, T& oData) const
	{
		auto itFind = m_oTypeData.find(std::type_index(oType));
		if (itFind == m_oTypeData.end()) {
			return nullptr;
		}
		const TypeData& oTypeData = itFind->second;
		oData = oTypeData.m_oData;
		return oTypeData.m_p0StringId;
	}
	/* *
	 * @param p0StringId type id
	 * @param bFound Tells whether the type was found
	 * @return The found type. Undefined if not found.
	 */
	const std::type_info& getTypeFromStringId(const char* const &p0StringId, bool& bFound) const
	{
		auto itFind = m_oStringIdType.find(p0StringId);
		if (itFind == m_oStringIdType.end()) {
			bFound = false;
			return typeid(NamedTypes);
		} else {
			bFound = true;
			return itFind->second;
		}
	}
	bool hasType(const std::type_info& oType) const
	{
		auto itFind = m_oTypeData.find(std::type_index(oType));
		const bool bFound = (itFind != m_oTypeData.end());
//std::cout << "NamedTypes::hasType()  type_info=" << oType.name() << " addr=" << (int64_t)&oType << "  ->" << bFound << std::endl;
		return bFound;
	}
	bool hasStringId(const char* const &oStringId) const
	{
		auto itFind = m_oStringIdType.find(oStringId);
		return (itFind != m_oStringIdType.end());
	}
	/* * Adds a type, its string representation and associated data.
	 * Duplicates are not allowed.
	 * @param oType The C++ type.
	 * @param p0StringId Must be PERSISTENT (static) for the lifetime of this class! Can't be null.
	 * @param oData The data associated with the type.
	 */
	void addType(const std::type_info& oType, const char* const &p0StringId, const T& oData)
	{
//std::cout << "NamedTypes::addStringIdType()  stringId=" << p0StringId << "  type_info=" << oType.name() << " addr=" << (int64_t)&oType << std::endl;
		assert(p0StringId != nullptr);
		#ifndef NDEBUG
		auto oPair1 = 
		#endif //NDEBUG
		m_oStringIdType.emplace(p0StringId, oType);
		assert(oPair1.second); // string already exists!
		#ifndef NDEBUG
		auto oPair2 = 
		#endif //NDEBUG
		m_oTypeData.emplace(std::type_index(oType), TypeData{p0StringId, oData});
		assert(oPair2.second); // C++ type already exists!
		//assert(m_oTypeData.find(&oType) != m_oTypeData.end());
	}
	/* * Remove a type.
	 * The type must have been added.
	 * @param oType The type to remove.
	 */
	void removeType(const std::type_info& oType)
	{
		auto itFind = m_oTypeData.find(std::type_index(oType));
		if (itFind == m_oTypeData.end()) {
			return;
		}
		const TypeData& oTypeData = itFind->second;
		m_oStringIdType.erase(oTypeData.m_p0StringId);
		m_oTypeData.erase(itFind);
	}
#ifndef NDEBUG
	void dump() const
	{
		std::cout << "NamedTypes::dump()  size=" << m_oStringIdType.size() << std::endl;
		for (auto& oPair : m_oStringIdType) {
			std::cout << "  stringId=" << oPair.first << " typeid().name=" << oPair.second.name() << std::endl;
		}
		for (auto& oPair : m_oTypeData) {
			std::cout << "  typeid().name=" << (*oPair.first).name() << " addr=" << reinterpret_cast<int64_t>(oPair.first)
						<< "   stringId=" << oPair.second.m_p0StringId << "   size(m_oData)=" << sizeof(oPair.second.m_oData) << std::endl;
		}
	}
#endif
private:
	struct TypeData final
	{
		const char* const m_p0StringId;
		T m_oData;
	};
	struct HasherStr
	{
		std::size_t operator()(const char* const p0Str) const
		{
//std::cout << "  HasherStr of " << p0Str << std::endl;
			const int32_t nA = 54059; // prime
			const int32_t nB = 76963; // prime
			//const int32_t nC = 86969; // prime
			size_t nH = 31; // prime
			char* s = const_cast<char*>(p0Str);
			while (*s) {
				nH = (nH * nA) ^ (s[0] * nB);
				++s;
			}
			return nH;
		}
	};
	struct EqualStr
	{
		bool operator()(const char* p0Str1, const char* p0Str2) const
		{
			const bool bEqual = (strcmp(p0Str1, p0Str2) == 0);
			return bEqual;
		}
	};
	std::unordered_map<std::type_index, TypeData> m_oTypeData;
	std::unordered_map<const char*, const std::type_info&, HasherStr, EqualStr > m_oStringIdType;
};

} // namespace Private

} // namespace stmi

#endif	/* _STMI_PRIVATE_NAMED_TYPES_H_ */
