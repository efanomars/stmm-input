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
 * File:   basicdevicemanager.cc
 */

#include "basicdevicemanager.h"

#include "callifsimplifier.h"
#include "utilbase.h"

#include <stmm-input/callifs.h>
#include <stmm-input/device.h>

#ifndef NDEBUG
//#include <iostream>
#endif //NDEBUG
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <typeinfo>
#include <utility>

namespace stmi
{

uint64_t BasicDeviceManager::s_nUniqueTimeStamp = 0;

BasicDeviceManager::BasicDeviceManager(const std::vector<Capability::Class>& aCapabitityClasses
									 , const std::vector<Capability::Class>& aDeviceCapabitityClasses
									 , const std::vector<Event::Class>& aEventClasses
									 , bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses) noexcept
: ChildDeviceManager()
, m_aCapabitityClasses(aCapabitityClasses)
, m_aDeviceCapabitityClasses(aDeviceCapabitityClasses)
, m_aEventClasses(aEventClasses)
, m_aEventClassEnabled(aEventClasses.size(), !bEnableEventClasses)
, m_refListeners(std::make_shared< std::list< ListenerData* > >())
, m_nListenerListRecursing(0)
, m_bListenerListDirty(false)
{
//std::cout << "BasicDeviceManager::BasicDeviceManager() " << reinterpret_cast<int64_t>(this) << '\n';
	// Make sure no class duplicates are passed
	for (size_t nIdx = 0; nIdx < m_aCapabitityClasses.size(); ++nIdx) {
		for (size_t nIdx2 = 0; nIdx2 < m_aCapabitityClasses.size(); ++nIdx2) {
			if (nIdx != nIdx2) {
				assert(m_aCapabitityClasses[nIdx] != m_aCapabitityClasses[nIdx2]);
			}
		}
	}
	// Make sure no class duplicates are passed
	for (size_t nIdx = 0; nIdx < m_aDeviceCapabitityClasses.size(); ++nIdx) {
		for (size_t nIdx2 = 0; nIdx2 < m_aDeviceCapabitityClasses.size(); ++nIdx2) {
			if (nIdx != nIdx2) {
				assert(m_aDeviceCapabitityClasses[nIdx] != m_aDeviceCapabitityClasses[nIdx2]);
			}
		}
	}
	// Make sure no class duplicates are passed
	for (size_t nIdx = 0; nIdx < m_aEventClasses.size(); ++nIdx) {
		for (size_t nIdx2 = 0; nIdx2 < m_aEventClasses.size(); ++nIdx2) {
			if (nIdx != nIdx2) {
				assert(m_aEventClasses[nIdx] != m_aEventClasses[nIdx2]);
			}
		}
	}
	for (auto& oEventClass : aEnDisableEventClasses) {
		for (size_t nIdx = 0; nIdx < m_aEventClasses.size(); ++nIdx) {
			if (m_aEventClasses[nIdx] == oEventClass) {
				m_aEventClassEnabled[nIdx] = bEnableEventClasses;
				break; // for(nIdx)
			}
		}
	}
	#ifndef NDEBUG
	for (const auto& oDMCapaClass : m_aCapabitityClasses) {
		assert(oDMCapaClass.isDeviceManagerCapability());
	}
	for (const auto& oDevCapaClass : aDeviceCapabitityClasses) {
		assert(! oDevCapaClass.isDeviceManagerCapability());
	}
	#endif //NDEBUG
}

shared_ptr<Device> BasicDeviceManager::getDevice(int32_t nDeviceId) const noexcept
{
	auto itFind = m_oDevices.find(nDeviceId);
	if (itFind == m_oDevices.end()) {
		return shared_ptr<Device>();
	}
	return itFind->second;
}
std::vector<int32_t> BasicDeviceManager::getDevicesWithCapabilityClass(const Capability::Class& oCapabilityClass) const noexcept
{
	std::vector<int32_t> aSet;
	//
	const bool bRegistered = oCapabilityClass.operator bool();
	if (!bRegistered) {
		// not registered
		return aSet; //---------------------------------------------------------
	}
	for (auto& oPair : m_oDevices) {
		const int32_t nDeviceId = oPair.first;
		const shared_ptr<Device>& refDevice = oPair.second;
		shared_ptr<Capability> refCapability = refDevice->getCapability(oCapabilityClass);
		if (refCapability) {
			Util::addToVectorSet(aSet, nDeviceId);
		}
	}
	return aSet;
}
std::vector<int32_t> BasicDeviceManager::getDevices() const noexcept
{
	std::vector<int32_t> aSet;
	for (auto& oPair : m_oDevices) {
		Util::addToVectorSet(aSet, oPair.first);
	}
	return aSet;
}
shared_ptr< const std::list< BasicDeviceManager::ListenerData* > > BasicDeviceManager::getListeners() noexcept
{
	return m_refListeners;
}
bool BasicDeviceManager::addEventListener(const shared_ptr<EventListener>& refEventListener, const shared_ptr<CallIf>& refCallIf) noexcept
{
	assert(refEventListener);
	auto p0EventListener = refEventListener.get();
	auto itFind = std::find_if(m_oListenersData.begin(), m_oListenersData.end()
			, [p0EventListener](const ListenerData& oListenerData) {
				return ((oListenerData.m_p0EventListener == p0EventListener)
					&& !oListenerData.m_bListenerWasRemoved);
			});
	if (itFind != m_oListenersData.end()) {
		// the listener was already added (and not removed)
		return false; //--------------------------------------------------------
	}
	maybeRemoveDataOfRemovedListeners();

	auto itListenerData = m_oListenersData.emplace(m_oListenersData.end());
	ListenerData& oListenerData = *itListenerData;
	auto itListeners = m_refListeners->emplace(m_refListeners->end(), &oListenerData);

	oListenerData.m_p1Owner = this;
	oListenerData.m_refEventListener = refEventListener;
	oListenerData.m_nAddedTimeStamp = BasicDeviceManager::getUniqueTimeStamp();
	oListenerData.m_refCallIf = refCallIf;
	oListenerData.m_p0EventListener = p0EventListener;
	oListenerData.m_itListeners = itListeners;
	const int32_t nTotEventClasses = static_cast<int32_t>(m_aEventClasses.size());
	assert(oListenerData.m_aCallIfEventClass.empty());
	oListenerData.m_aCallIfEventClass.resize(nTotEventClasses);
	if (refCallIf) {
		for (int32_t nClassIdx = 0; nClassIdx < nTotEventClasses; ++nClassIdx) {
			auto& refClassCallIf = oListenerData.m_aCallIfEventClass[nClassIdx];
			refClassCallIf = CallIfSimplifier::simplify(refCallIf, m_aEventClasses[nClassIdx]);
			const CallIf& oClassCallIf = *refClassCallIf;
			const std::type_info& oCallIfType = typeid(oClassCallIf);
			if (oCallIfType == typeid(CallIfTrue)) {
				// True is the same as no callif
				// False is the same as no listener, but we keep it anyway
				refClassCallIf.reset();
			}
		}
	}
	return true;
}
bool BasicDeviceManager::addEventListener(const shared_ptr<EventListener>& refEventListener) noexcept
{
	return addEventListener(refEventListener, shared_ptr<CallIf>{});
}
bool BasicDeviceManager::removeEventListener(const shared_ptr<EventListener>& refEventListener, bool bFinalize) noexcept
{
	assert(refEventListener);
	maybeRemoveDataOfRemovedListeners();
	auto p0EventListener = refEventListener.get();
	auto itFind = std::find_if(m_oListenersData.begin(), m_oListenersData.end()
			, [p0EventListener](const ListenerData& oListenerData) {
				return ((oListenerData.m_p0EventListener == p0EventListener)
							&& !oListenerData.m_bListenerWasRemoved);
			});
	if (itFind == m_oListenersData.end()) {
		return false; //--------------------------------------------------------
	}
	ListenerData& oListenerData = *itFind;
	// mark as removed, so that re-adding of the listener is allowed in a 
	// finalize callback
	oListenerData.m_bListenerWasRemoved = true;
	m_bListenerListDirty = true;
	// finalize if requested
	if (bFinalize) {
		// despite the listener being removed, it still must be able to send
		// finalize events (signaled by m_bListenerRemoving)
		oListenerData.m_bListenerRemoving = true;
		++m_nListenerListRecursing;
		finalizeListener(oListenerData);
		--m_nListenerListRecursing;
		oListenerData.m_bListenerRemoving = false;
	}
	maybeRemoveDataOfRemovedListeners();
	return true;
}
bool BasicDeviceManager::removeEventListener(const shared_ptr<EventListener>& refEventListener) noexcept
{
	return removeEventListener(refEventListener, false);
}
void BasicDeviceManager::maybeRemoveDataOfRemovedListeners() noexcept
{
	if ((!m_bListenerListDirty) || (m_nListenerListRecursing > 0)) {
		// No removed listener to garbage collect
		// or still working on the list
		return; //--------------------------------------------------------------
	}
	if (!(m_refListeners.use_count() == 1)) {
		// Someone in the callback stack is iterating over the list
		// do not remove!
		return; //--------------------------------------------------------------
	}
	auto itListenerData = m_oListenersData.begin();
	while (itListenerData != m_oListenersData.end()) {
		if (itListenerData->m_bListenerWasRemoved) {
			assert(!itListenerData->m_bListenerRemoving);
			m_refListeners->erase(itListenerData->m_itListeners);
			itListenerData = m_oListenersData.erase(itListenerData);
		} else {
			++itListenerData;
		}
	}
	m_bListenerListDirty = false;
}
void BasicDeviceManager::resetExtraDataOfAllListeners() noexcept
{
//std::cout << "BasicDeviceManager::resetExtraDataOfAllListeners" << '\n';
	for (auto& oListenerData : m_oListenersData) {
		auto& refExtraData = oListenerData.m_refExtraData;
		if (refExtraData) {
			refExtraData->reset();
		}
	}
}
bool BasicDeviceManager::ListenerData::handleEventCommon(int32_t nClassTypeIdx, const shared_ptr<Event>& refEvent) const noexcept
{
	if (m_bListenerWasRemoved && !m_bListenerRemoving) {
		return false; //--------------------------------------------------------
	}
	auto refListener = m_refEventListener.lock();
	if (refListener) {
		++(m_p1Owner->m_nListenerListRecursing);
//std::cout << "BasicDeviceManager::ListenerData::handleEventCommon   " << m_p1Owner->m_nListenerListRecursing << '\n';
		if (nClassTypeIdx >= 0) {
			auto& refCallIf = m_aCallIfEventClass[nClassTypeIdx];
			if (refCallIf && !(*refCallIf)(refEvent)) {
				// CallIf disallows sending of event to listener
//std::cout << "BasicDeviceManager::ListenerData::handleEventCommon exit A  " << m_p1Owner->m_nListenerListRecursing << '\n';
				--(m_p1Owner->m_nListenerListRecursing);
				return false; //------------------------------------------------
			}
		}
		// callback
		(*refListener)(refEvent);
//std::cout << "BasicDeviceManager::ListenerData::handleEventCommon exit B  " << m_p1Owner->m_nListenerListRecursing << '\n';
		--(m_p1Owner->m_nListenerListRecursing);
	} else {
		// Can't remove this element right now, because the caller is 
		// probably iterating over the list.
		// Should be done next time m_oListenersData list is accessed (getEventListeners, etc.).
		m_p1Owner->m_bListenerListDirty = true;
		auto p0This = const_cast<ListenerData*>(this);
		p0This->m_bListenerWasRemoved = true;
	}
	return true;
}
bool BasicDeviceManager::ListenerData::handleEventCallIf(int32_t nClassTypeIdx, const shared_ptr<Event>& refEvent) const noexcept
{
	if (nClassTypeIdx < 0) {
		nClassTypeIdx = m_p1Owner->getEventClassIndex(refEvent->getEventClass());
	}
	assert((nClassTypeIdx >= 0) && (nClassTypeIdx < static_cast<int32_t>(m_aCallIfEventClass.size())));
	return handleEventCommon(nClassTypeIdx, refEvent);
}
std::vector<Capability::Class> BasicDeviceManager::getCapabilityClasses() const noexcept
{
	return m_aCapabitityClasses;
}
std::vector<Capability::Class> BasicDeviceManager::getDeviceCapabilityClasses() const noexcept
{
	return m_aDeviceCapabitityClasses;
}
std::vector<Event::Class> BasicDeviceManager::getEventClasses() const noexcept
{
	return m_aEventClasses;
}
int32_t BasicDeviceManager::getEventClassIndex(const Event::Class& oEventClass) const noexcept
{
	const int32_t nTotEventClasses = static_cast<int32_t>(m_aEventClasses.size());
	for (int32_t nIdx = 0; nIdx < nTotEventClasses; ++nIdx) {
		if (m_aEventClasses[nIdx] == oEventClass) {
			return nIdx;
		}
	}
	return -1;
}
bool BasicDeviceManager::isEventClassEnabled(const Event::Class& oEventClass) const noexcept
{
	const int32_t nIdx = getEventClassIndex(oEventClass);
//std::cout << "isEventClassEnabled=" << oEventClass.getId() << " nIdx=" << nIdx << " m_aEventClasses.size()=" << m_aEventClasses.size() << '\n';
	if (nIdx < 0) {
		return false;
	}
//std::cout << "            enabled=" << m_aEventClassEnabled[nIdx] << '\n';
	return m_aEventClassEnabled[nIdx];
}
void BasicDeviceManager::enableEventClass(const Event::Class& oEventClass) noexcept
{
	const int32_t nIdx = getEventClassIndex(oEventClass);
	if (nIdx < 0) {
		return;
	}
	m_aEventClassEnabled[nIdx] = true;
}
bool BasicDeviceManager::addDevice(const shared_ptr<Device>& refDevice) noexcept
{
	assert(refDevice);
	const int32_t nDeviceId = refDevice->getId();
	auto itFind = m_oDevices.find(nDeviceId);
	if (! (itFind == m_oDevices.end())) {
		// already present, couldn't add
		return false;
	}
	m_oDevices.insert(std::make_pair(nDeviceId, refDevice));
	return true;
}
bool BasicDeviceManager::removeDevice(const shared_ptr<Device>& refDevice) noexcept
{
	assert(refDevice);
	const int32_t nDeviceId = refDevice->getId();
	auto itFind = m_oDevices.find(nDeviceId);
	if (itFind == m_oDevices.end()) {
		return false;
	}
	m_oDevices.erase(itFind);
	return true;
}

} // namespace stmi
