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
 * File:   inputstrings.h
 */

#ifndef _EXAMPLE_SHOW_EVS_INPUT_STRINGS_
#define _EXAMPLE_SHOW_EVS_INPUT_STRINGS_

#include <string>
#include <unordered_map>

#include <stmm-input/hardwarekey.h>
#include <stmm-input-ev/joystickcapability.h>

namespace example
{

namespace showevs
{

class InputStrings
{
public:
	InputStrings()
	{
		initAxisMap();
		initKeyMap();
	}
	inline std::string getAxisString(stmi::JoystickCapability::AXIS eAxis) const
	{
		auto itFind = m_oAxisStringMap.find((int32_t)eAxis);
		if (itFind == m_oAxisStringMap.end()) {
			return "";
		}
		return itFind->second;
	}
	inline std::string getKeyString(stmi::HARDWARE_KEY eKey) const
	{
		auto itFind = m_oKeyStringMap.find((int32_t)eKey);
		if (itFind == m_oKeyStringMap.end()) {
			return "";
		}
		return itFind->second;
	}
private:
	inline void addAxis(stmi::JoystickCapability::AXIS eAxis, const std::string& sStr)
	{
		m_oAxisStringMap.emplace((int32_t)eAxis, sStr);
	}
	inline void addKey(stmi::HARDWARE_KEY eKey, const std::string& sStr)
	{
		m_oKeyStringMap.emplace((int32_t)eKey, sStr);
	}

	void initAxisMap();
	void initKeyMap();
private:
	std::unordered_map<int32_t, std::string> m_oKeyStringMap;
	std::unordered_map<int32_t, std::string> m_oAxisStringMap;
};

} // namespace showevs

} // namespace example

#endif	/* _EXAMPLE_SHOW_EVS_INPUT_STRINGS_ */
