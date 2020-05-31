/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   utilev.h
 */

#ifndef STMI_UTIL_H
#define STMI_UTIL_H

#include <vector>
#include <algorithm>

namespace stmi
{

namespace Util
{

/* * Add an array of values to a set "represented" by a vector.
 */
template <class T>
static void addToVectorSet(std::vector<T>& aSet, const std::vector<T>& aChildSet) noexcept
{
	std::copy_if(aChildSet.begin(), aChildSet.end(), std::back_inserter(aSet)
		, [&aSet](const T& oValue)
		{
			return std::find(aSet.begin(), aSet.end(), oValue) == aSet.end();
		});
}
/* * Add a value to a set "represented" by a vector.
 * This means that if the value is already present it won't be added to the vector.
 */
template <class T>
static void addToVectorSet(std::vector<T>& aSet, const T& oValue) noexcept
{
	if (std::find(aSet.begin(), aSet.end(), oValue) == aSet.end()) {
		aSet.push_back(oValue);
	}
}

} // namespace Util

} // namespace stmi

#endif /* STMI_UTIL_H */

