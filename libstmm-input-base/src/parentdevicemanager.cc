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
 * File:   parentdevicemanager.cc
 */

#include "parentdevicemanager.h"

#include "utilbase.h"

#include <cassert>
//#include <iostream>

namespace stmi { class Accessor; }
namespace stmi { class Device; }

namespace stmi
{

ParentDeviceManager::ParentDeviceManager() noexcept
: ChildDeviceManager(true)
{
}
void ParentDeviceManager::init(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager) noexcept
{
	assert(!aChildDeviceManager.empty());
	// Reinitialization is not allowed because ChildDeviceManager caches the root
	if (!m_aChildDeviceManagers.empty()) {
		assert(false);
		return;
	}
	m_aChildDeviceManagers = aChildDeviceManager;
	auto refChildThis = ChildDeviceManager::shared_from_this();
	auto refParentThis = std::static_pointer_cast<ParentDeviceManager>(refChildThis);
	for (auto& refCDM : aChildDeviceManager) {
		assert(refCDM);
		refCDM->setParent(refParentThis);
	}
}

shared_ptr<Device> ParentDeviceManager::getDevice(int32_t nDeviceId) const noexcept
{
	shared_ptr<Device> refDevice;
	for (auto& refCDM : m_aChildDeviceManagers) {
		refDevice = refCDM->getDevice(nDeviceId);
		if (refDevice) {
			break; // for
		}
	}
	return refDevice;
}
shared_ptr<Capability> ParentDeviceManager::getCapability(const Capability::Class& oClass) const noexcept
{
	shared_ptr<Capability> refCapa;
	for (auto& refCDM : m_aChildDeviceManagers) {
		refCapa = refCDM->getCapability(oClass);
		if (refCapa) {
			break; //for
		}
	}
	return refCapa;
}
shared_ptr<Capability> ParentDeviceManager::getCapability(int32_t nCapabilityId) const noexcept
{
	ParentDeviceManager* p0This = const_cast<ParentDeviceManager*>(this);
	shared_ptr<Capability> refCapa;
	for (shared_ptr<ChildDeviceManager>& refCDM : p0This->m_aChildDeviceManagers) {
		refCapa = refCDM->getCapability(nCapabilityId);
		if (refCapa) {
			break; // for
		}
	}
	return refCapa;
}
std::vector<shared_ptr<Capability>> ParentDeviceManager::getDeviceManagerCapabilities(const Capability::Class& oClass) const noexcept
{
	ParentDeviceManager* p0This = const_cast<ParentDeviceManager*>(this);
	std::vector<shared_ptr<Capability>> aCapas;
	for (shared_ptr<ChildDeviceManager>& refCDM : p0This->m_aChildDeviceManagers) {
		if (refCDM->isParent()) {
			Util::addToVectorSet(aCapas, refCDM->getAsParent()->getDeviceManagerCapabilities(oClass));
		} else {
			auto refDMCapa = refCDM->getCapability(oClass);
			if (refDMCapa) {
				Util::addToVectorSet(aCapas, refDMCapa);
			}
		}
	}
	return aCapas;
}

std::vector<Capability::Class> ParentDeviceManager::getCapabilityClasses() const noexcept
{
	std::vector<Capability::Class> aSet;
	for (auto& refCDM : m_aChildDeviceManagers) {
		Util::addToVectorSet(aSet, refCDM->getCapabilityClasses());
	}
	return aSet;
}
std::vector<Capability::Class> ParentDeviceManager::getDeviceCapabilityClasses() const noexcept
{
	std::vector<Capability::Class> aSet;
	for (auto& refCDM : m_aChildDeviceManagers) {
		Util::addToVectorSet(aSet, refCDM->getDeviceCapabilityClasses());
	}
	return aSet;
}
std::vector<Event::Class> ParentDeviceManager::getEventClasses() const noexcept
{
	std::vector<Event::Class> aSet;
	for (auto& refCDM : m_aChildDeviceManagers) {
		Util::addToVectorSet(aSet, refCDM->getEventClasses());
	}
	return aSet;
}
std::vector<int32_t> ParentDeviceManager::getDevicesWithCapabilityClass(const Capability::Class& oCapabilityClass) const noexcept
{
	std::vector<int32_t> aSet;
	for (auto& refCDM : m_aChildDeviceManagers) {
		Util::addToVectorSet(aSet, refCDM->getDevicesWithCapabilityClass(oCapabilityClass));
	}
	return aSet;
}
std::vector<int32_t> ParentDeviceManager::getDevices() const noexcept
{
	std::vector<int32_t> aSet;
	for (auto& refCDM : m_aChildDeviceManagers) {
		Util::addToVectorSet(aSet, refCDM->getDevices());
	}
	return aSet;
}
bool ParentDeviceManager::isEventClassEnabled(const Event::Class& oEventClass) const noexcept
{
	for (auto& refCDM : m_aChildDeviceManagers) {
		const bool bChildEnabled = refCDM->isEventClassEnabled(oEventClass);
		if (bChildEnabled) {
			return true;
		}
	}
	return false;
}
void ParentDeviceManager::enableEventClass(const Event::Class& oEventClass) noexcept
{
	for (auto& refCDM : m_aChildDeviceManagers) {
		refCDM->enableEventClass(oEventClass);
	}
}
bool ParentDeviceManager::addAccessor(const shared_ptr<Accessor>& refAccessor) noexcept
{
	bool bAdded = false;
	for (auto& refCDM : m_aChildDeviceManagers) {
		const bool bChildAdded = refCDM->addAccessor(refAccessor);
		bAdded = bAdded || bChildAdded;
	}
	return bAdded;
}
bool ParentDeviceManager::removeAccessor(const shared_ptr<Accessor>& refAccessor) noexcept
{
	bool bRemoved = false;
	for (auto& refCDM : m_aChildDeviceManagers) {
		const bool bChildRemoved = refCDM->removeAccessor(refAccessor);
		bRemoved = bRemoved || bChildRemoved;
	}
	return bRemoved;
}
bool ParentDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor) noexcept
{
	for (auto& refCDM : m_aChildDeviceManagers) {
		const bool bChildHasIt = refCDM->hasAccessor(refAccessor);
		if (bChildHasIt) {
			return true;
		}
	}
	return false;
}
bool ParentDeviceManager::addEventListener(const shared_ptr<EventListener>& refEventListener, const shared_ptr<CallIf>& refCallIf) noexcept
{
	bool bAdded = false;
	for (auto& refCDM : m_aChildDeviceManagers) {
		const bool bChildAdded = refCDM->addEventListener(refEventListener, refCallIf);
		bAdded = bAdded || bChildAdded;
	}
	return bAdded;
}
bool ParentDeviceManager::addEventListener(const shared_ptr<EventListener>& refEventListener) noexcept
{
	bool bAdded = false;
	for (auto& refCDM : m_aChildDeviceManagers) {
		const bool bChildAdded = refCDM->addEventListener(refEventListener);
		bAdded = bAdded || bChildAdded;
	}
	return bAdded;
}
bool ParentDeviceManager::removeEventListener(const shared_ptr<EventListener>& refEventListener, bool bFinalize) noexcept
{
	bool bRemoved = false;
	for (auto& refCDM : m_aChildDeviceManagers) {
		const bool bChildRemoved = refCDM->removeEventListener(refEventListener, bFinalize);
		bRemoved = bRemoved || bChildRemoved;
	}
	return bRemoved;
}
bool ParentDeviceManager::removeEventListener(const shared_ptr<EventListener>& refEventListener) noexcept
{
	return removeEventListener(refEventListener, false);
}

} // namespace stmi
