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
 * File:   util.h
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
void addToVectorSet(std::vector<T>& aSet, const std::vector<T>& aChildSet)
{
	std::copy_if(aChildSet.begin(), aChildSet.end(), std::back_inserter(aSet)
		, [&aSet](const T& oValue)
		{
			return std::find(aSet.begin(), aSet.end(), oValue) == aSet.end();
		});
}
/* * Add a value to a set "represented" by a vector.
 */
template <class T>
void addToVectorSet(std::vector<T>& aSet, const T& oValue)
{
	if (std::find(aSet.begin(), aSet.end(), oValue) == aSet.end()) {
		aSet.push_back(oValue);
	}
}

} // namespace Util

} // namespace stmi

#endif /* STMI_UTIL_H */

