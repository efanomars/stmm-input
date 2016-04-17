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
 * File:   childdevicemanager.cc
 */

#include <algorithm>

#include "childdevicemanager.h"
#include "util.h"

namespace stmi
{

ChildDeviceManager::ChildDeviceManager()
: m_bImRoot(true)
{
}
shared_ptr<ParentDeviceManager> ChildDeviceManager::getParent() const
{
	ChildDeviceManager* p0This = const_cast<ChildDeviceManager*>(this);
	return p0This->getParent();
}
shared_ptr<ParentDeviceManager> ChildDeviceManager::getParent()
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
void ChildDeviceManager::setParent(const shared_ptr<ParentDeviceManager>& refParent)
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
shared_ptr<DeviceManager> ChildDeviceManager::getRoot() const
{
	ChildDeviceManager* p0This = const_cast<ChildDeviceManager*>(this);
	return p0This->getRoot();
}
shared_ptr<DeviceManager> ChildDeviceManager::getRoot()
{
	if (m_bImRoot) {
		return shared_from_this();
	}
	auto refRoot = m_refWeakRoot.lock();
	if (!refRoot) {
		// root was removed, recalculate
		refRoot = calcRoot();
		if (m_bImRoot) {
			return shared_from_this();
		}
	}
	if (!refRoot->m_bImRoot) {
		// What this instance thought was the root no longer is,
		// it was added as a child to another parent. Recalculate.
		refRoot = calcRoot();
	}
	return refRoot;
}
shared_ptr<ParentDeviceManager> ChildDeviceManager::calcRoot()
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


////////////////////////////////////////////////////////////////////////////////
shared_ptr<ParentDeviceManager> ParentDeviceManager::create(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager)
{
	shared_ptr<ParentDeviceManager> refInstance(new ParentDeviceManager());
	refInstance->init(aChildDeviceManager);
	return refInstance;
}

ParentDeviceManager::ParentDeviceManager()
{
}
ParentDeviceManager::~ParentDeviceManager()
{
//std::cout << "ParentDeviceManager::~ParentDeviceManager()" << std::endl;
}
void ParentDeviceManager::init(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager)
{
	assert(!aChildDeviceManager.empty());
	// Reinitialization is not allowed because ChildDeviceManager caches the root
	if (!m_aChildDeviceManager.empty()) {
		assert(false);
		return;
	}
	m_aChildDeviceManager = aChildDeviceManager;
	auto refChildThis = ChildDeviceManager::shared_from_this();
	auto refParentThis = std::static_pointer_cast<ParentDeviceManager>(refChildThis);
	for (auto& refCDM : aChildDeviceManager) {
		assert(refCDM);
		refCDM->setParent(refParentThis);
	}
}

shared_ptr<Device> ParentDeviceManager::getDevice(int32_t nDeviceId) const
{
	shared_ptr<Device> refDevice;
	for (auto& refCDM : m_aChildDeviceManager) {
		refDevice = refCDM->getDevice(nDeviceId);
		if (refDevice) {
			break; // for
		}
	}
	return refDevice;
}
std::vector<Capability::Class> ParentDeviceManager::getCapabilityClasses() const
{
	std::vector<Capability::Class> aSet;
	for (auto& refCDM : m_aChildDeviceManager) {
		addToVectorSet(aSet, refCDM->getCapabilityClasses());
	}
	return aSet;
}
std::vector<Event::Class> ParentDeviceManager::getEventClasses() const
{
	std::vector<Event::Class> aSet;
	for (auto& refCDM : m_aChildDeviceManager) {
		addToVectorSet(aSet, refCDM->getEventClasses());
	}
	return aSet;
}
std::vector<int32_t> ParentDeviceManager::getDevicesWithCapabilityClass(const Capability::Class& oCapabilityClass) const
{
	std::vector<int32_t> aSet;
	for (auto& refCDM : m_aChildDeviceManager) {
		addToVectorSet(aSet, refCDM->getDevicesWithCapabilityClass(oCapabilityClass));
	}
	return aSet;
}
std::vector<int32_t> ParentDeviceManager::getDevices() const
{
	std::vector<int32_t> aSet;
	for (auto& refCDM : m_aChildDeviceManager) {
		addToVectorSet(aSet, refCDM->getDevices());
	}
	return aSet;
}
bool ParentDeviceManager::getEventClassEnabled(const Event::Class& oEventClass) const
{
	for (auto& refCDM : m_aChildDeviceManager) {
		const bool bChildEnabled = refCDM->getEventClassEnabled(oEventClass);
		if (bChildEnabled) {
			return true;
		}
	}
	return false;
}
void ParentDeviceManager::enableEventClass(const Event::Class& oEventClass)
{
	for (auto& refCDM : m_aChildDeviceManager) {
		refCDM->enableEventClass(oEventClass);
	}
}
bool ParentDeviceManager::addAccessor(const shared_ptr<Accessor>& refAccessor)
{
	bool bAdded = false;
	for (auto& refCDM : m_aChildDeviceManager) {
		const bool bChildAdded = refCDM->addAccessor(refAccessor);
		bAdded = bAdded || bChildAdded;
	}
	return bAdded;
}
bool ParentDeviceManager::removeAccessor(const shared_ptr<Accessor>& refAccessor)
{
	bool bRemoved = false;
	for (auto& refCDM : m_aChildDeviceManager) {
		const bool bChildRemoved = refCDM->removeAccessor(refAccessor);
		bRemoved = bRemoved || bChildRemoved;
	}
	return bRemoved;
}
bool ParentDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor)
{
	for (auto& refCDM : m_aChildDeviceManager) {
		const bool bChildHasIt = refCDM->hasAccessor(refAccessor);
		if (bChildHasIt) {
			return true;
		}
	}
	return false;
}
bool ParentDeviceManager::addEventListener(const shared_ptr<EventListener>& refEventListener, const shared_ptr<CallIf>& refCallIf)
{
	bool bAdded = false;
	for (auto& refCDM : m_aChildDeviceManager) {
		const bool bChildAdded = refCDM->addEventListener(refEventListener, refCallIf);
		bAdded = bAdded || bChildAdded;
	}
	return bAdded;
}
bool ParentDeviceManager::removeEventListener(const shared_ptr<EventListener>& refEventListener, bool bFinalize)
{
	bool bRemoved = false;
	for (auto& refCDM : m_aChildDeviceManager) {
		const bool bChildRemoved = refCDM->removeEventListener(refEventListener, bFinalize);
		bRemoved = bRemoved || bChildRemoved;
	}
	return bRemoved;
}

} // namespace stmi

