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
 * File:   device.cc
 */

#include "device.h"

namespace stmi
{

std::atomic<int32_t> Device::s_nNewIdCounter(0);

int32_t Device::getNewDeviceId()
{
	const int32_t nNewId = ++s_nNewIdCounter;
	return nNewId;
}
Device::Device()
: m_nDeviceId(getNewDeviceId())
{
}

} // namespace stmi

