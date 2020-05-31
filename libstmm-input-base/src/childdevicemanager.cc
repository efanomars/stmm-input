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
 * File:   childdevicemanager.cc
 */

#include "childdevicemanager.h"

#include "parentdevicemanager.h"

#include <cassert>

namespace stmi
{

ChildDeviceManager::ChildDeviceManager() noexcept
: m_bImRoot(true)
, m_bImParent(false)
{
}
ChildDeviceManager::ChildDeviceManager(bool bIsParent) noexcept
: m_bImRoot(true)
, m_bImParent(bIsParent)
{
}
shared_ptr<ParentDeviceManager> ChildDeviceManager::getAsParent() const noexcept
{
	if (!m_bImParent) {
		return shared_ptr<ParentDeviceManager>{};
	}
	ChildDeviceManager* p0This = const_cast<ChildDeviceManager*>(this);
	return p0This->getAsParent();
}
shared_ptr<ParentDeviceManager> ChildDeviceManager::getAsParent() noexcept
{
	if (!m_bImParent) {
		return shared_ptr<ParentDeviceManager>{};
	}
	return std::dynamic_pointer_cast<ParentDeviceManager>(shared_from_this());
}

shared_ptr<ParentDeviceManager> ChildDeviceManager::getParent() const noexcept
{
	ChildDeviceManager* p0This = const_cast<ChildDeviceManager*>(this);
	return p0This->getParent();
}
shared_ptr<ParentDeviceManager> ChildDeviceManager::getParent() noexcept
{
	auto refParent = m_refWeakParent.lock();
	if (!refParent) {
		if (!m_bImRoot) {
			// parent was removed
			m_bImRoot = true;
			m_refWeakRoot.reset();
		}
	}
	return refParent;
}
void ChildDeviceManager::setParent(const shared_ptr<ParentDeviceManager>& refParent) noexcept
{
	if (!m_bImRoot) {
		auto refCurParent = m_refWeakParent.lock();
		if (refCurParent) {
			// reparenting is not allowed
			assert(false);
			return;
		}
		// parent was removed, set allowed
	}
	m_refWeakParent = refParent;
	calcRoot();
}
shared_ptr<ParentDeviceManager> ChildDeviceManager::getRoot() const noexcept
{
	ChildDeviceManager* p0This = const_cast<ChildDeviceManager*>(this);
	return p0This->getRoot();
}
shared_ptr<ParentDeviceManager> ChildDeviceManager::getRoot() noexcept
{
	if (m_bImRoot) {
		assert(isParent());
		return getAsParent();
	}
	auto refRoot = m_refWeakRoot.lock();
	if (!refRoot) {
		// root was removed, recalculate
		refRoot = calcRoot();
		if (m_bImRoot) {
			return getAsParent();
		}
	}
	if (!refRoot->m_bImRoot) {
		// What this instance thought was the root no longer is,
		// it was added as a child to another parent. Recalculate.
		refRoot = calcRoot();
	}
	return refRoot;
}
shared_ptr<ParentDeviceManager> ChildDeviceManager::calcRoot() noexcept
{
	auto refParent = getParent();
	if (!refParent) {
		m_bImRoot = true;
		m_refWeakRoot.reset();
		return refParent;
	}
	m_bImRoot = false;
	shared_ptr<ParentDeviceManager> refTop;
	do {
		refTop = refParent;
		refParent = refParent->getParent();
	} while (refParent);
	m_refWeakRoot = refTop;
	return refTop;
}

} // namespace stmi

