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
 * File:   recycler.h
 */

#ifndef _STMI_RECYCLER_H_
#define _STMI_RECYCLER_H_

#include <cassert>
#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include <memory>


namespace stmi
{

namespace Private
{

////////////////////////////////////////////////////////////////////////////////
/** Recycling factory for shared_ptr wrapped classes.
 */
template <class T>
class Recycler final
{
public:
	Recycler() = default;
	//~Recycler() = default;

	/** Construct or recycle the shared_ptr wrapped instance.
	 * T must have a constructor T(const P& ... oParam)
	 * and public member function reInit(const P& ... oParam) with the same params.
	 */
	template <typename ...P>
	std::shared_ptr<T> create(const P& ... oParam)
	{
		for (auto& refT : m_oAll) {
			if (refT.unique()) {
//#ifndef NDEBUG
//static int32_t nCount = 0;
//std::cout << "recycled " << nCount << '\n';
//++nCount;
//#endif //NDEBUG
				refT->reInit(oParam...);
				return refT;
			}
		}
		// not found: create new instance
		m_oAll.emplace_back(std::make_shared<T>(oParam...));
		return m_oAll.back();
	}
private:
	std::vector< std::shared_ptr<T> > m_oAll;
private:
	Recycler(const Recycler& oSource) = delete;
	Recycler& operator=(const Recycler& oSource) = delete;
};

} // namespace Private

} // namespace stmi

#endif	/* _STMI_RECYCLER_H_ */

