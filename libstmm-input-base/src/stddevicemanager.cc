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
 * File:   stddevicemanager.cc
 */

#include "stddevicemanager.h"

#include "callifs.h"
#include "callifsimplifier.h"
#include "util.h"

#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG
#include <algorithm>

namespace stmi
{

StdDeviceManager::StdDeviceManager(const std::vector<Capability::Class>& aCapabitityClass
									, const std::vector<Event::Class>& aEventClass
									, bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass)
: ChildDeviceManager()
, m_aCapabitityClass(aCapabitityClass)
, m_aEventClass(aEventClass)
, m_aEventClassEnabled(aEventClass.size(), !bEnableEventClasses)
, m_refListeners(std::make_shared< std::list< ListenerData* > >())
, m_nListenerListRecursing(0)
, m_bListenerListDirty(false)
{
	// Make sure no CapabilityType duplicates are passed
	for (size_t nIdx = 0; nIdx < aCapabitityClass.size(); ++nIdx) {
		for (size_t nIdx2 = 0; nIdx2 < aCapabitityClass.size(); ++nIdx2) {
			if (nIdx != nIdx2) {
				assert(aCapabitityClass[nIdx] != aCapabitityClass[nIdx2]);
			}
		}
	}
	// Make sure no EventType duplicates are passed
	for (size_t nIdx = 0; nIdx < aEventClass.size(); ++nIdx) {
		for (size_t nIdx2 = 0; nIdx2 < aEventClass.size(); ++nIdx2) {
			if (nIdx != nIdx2) {
				assert(aEventClass[nIdx] != aEventClass[nIdx2]);
			}
		}
	}
	for (auto& oEventClass : aEnDisableEventClass) {
		for (size_t nIdx = 0; nIdx < aEventClass.size(); ++nIdx) {
			if (aEventClass[nIdx] == oEventClass) {
				m_aEventClassEnabled[nIdx] = bEnableEventClasses;
				break; // for(nIdx)
			}
		}
	}
}
shared_ptr<Device> StdDeviceManager::getDevice(int32_t nDeviceId) const
{
	auto itFind = m_oDevices.find(nDeviceId);
	if (itFind == m_oDevices.end()) {
		return shared_ptr<Device>();
	}
	return itFind->second;
}
std::vector<int32_t> StdDeviceManager::getDevicesWithCapabilityClass(const Capability::Class& oCapabilityClass) const
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
			addToVectorSet(aSet, nDeviceId);
		}
	}
	return aSet;
}
std::vector<int32_t> StdDeviceManager::getDevices() const
{
	std::vector<int32_t> aSet;
	for (auto& oPair : m_oDevices) {
		addToVectorSet(aSet, oPair.first);
	}
	return aSet;
}
shared_ptr< const std::list< StdDeviceManager::ListenerData* > > StdDeviceManager::getListeners()
{
	return m_refListeners;
}
bool StdDeviceManager::addEventListener(const shared_ptr<EventListener>& refEventListener, const shared_ptr<CallIf>& refCallIf)
{
//std::cout << "StdDeviceManager::addEventListener()" << std::endl;
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
	oListenerData.m_nAddedTimeUsec = DeviceManager::getNowTimeMicroseconds();
	oListenerData.m_refCallIf = refCallIf;
	oListenerData.m_p0EventListener = p0EventListener;
	oListenerData.m_itListeners = itListeners;
	const int32_t nTotEventClasses = m_aEventClass.size();
	assert(oListenerData.m_aCallIfEventClass.empty());
	oListenerData.m_aCallIfEventClass.resize(nTotEventClasses);
	if (refCallIf) {
		for (int32_t nClassIdx = 0; nClassIdx < nTotEventClasses; ++nClassIdx) {
			auto& refClassCallIf = oListenerData.m_aCallIfEventClass[nClassIdx];
			refClassCallIf = CallIfSimplifier::simplify(refCallIf, m_aEventClass[nClassIdx]);
			const std::type_info& oCallIfType = typeid(*refClassCallIf);
			if (oCallIfType == typeid(CallIfTrue)) {
				// True is the same as no callif
				// False is the same as no listener, but we keep it anyway
				refClassCallIf.reset();
			}
		}
	}
	return true;
}
bool StdDeviceManager::removeEventListener(const shared_ptr<EventListener>& refEventListener, bool bFinalize)
{
//std::cout << "StdDeviceManager::removeEventListener()" << std::endl;
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
void StdDeviceManager::maybeRemoveDataOfRemovedListeners()
{
	if ((!m_bListenerListDirty) || (m_nListenerListRecursing > 0)) {
		// No removed listener to garbage collect
		// or still working on the list
		return; //--------------------------------------------------------------
	}
	if (!m_refListeners.unique()) {
		// Someone in the callback stack is iterating over the list
		// do not remove!
		return; //--------------------------------------------------------------
	}
	auto itListenerData = m_oListenersData.begin();
	while (itListenerData != m_oListenersData.end()) {
		if (itListenerData->m_bListenerWasRemoved) {
//std::cout << "StdDeviceManager::maybeRemoveDataOfRemovedListeners" << std::endl;
			assert(!itListenerData->m_bListenerRemoving);
			m_refListeners->erase(itListenerData->m_itListeners);
			itListenerData = m_oListenersData.erase(itListenerData);
		} else {
			++itListenerData;
		}
	}
	m_bListenerListDirty = false;
}
void StdDeviceManager::resetExtraDataOfAllListeners()
{
//std::cout << "StdDeviceManager::resetExtraDataOfAllListeners" << std::endl;
	for (auto& oListenerData : m_oListenersData) {
		auto& refExtraData = oListenerData.m_refExtraData;
		if (refExtraData) {
			refExtraData->reset();
		}
	}
}
bool StdDeviceManager::ListenerData::handleEventCommon(int32_t nClassTypeIdx, const shared_ptr<Event>& refEvent) const
{
	if (m_bListenerWasRemoved && !m_bListenerRemoving) {
		return false; //--------------------------------------------------------
	}
	auto refListener = m_refEventListener.lock();
	if (refListener) {
		++(m_p1Owner->m_nListenerListRecursing);
//std::cout << "StdDeviceManager::ListenerData::handleEventCommon   " << m_p1Owner->m_nListenerListRecursing << std::endl;
		if (nClassTypeIdx >= 0) {
			auto& refCallIf = m_aCallIfEventClass[nClassTypeIdx];
			if (refCallIf && !(*refCallIf)(refEvent)) {
				// CallIf disallows sending of event to listener
//std::cout << "StdDeviceManager::ListenerData::handleEventCommon exit A  " << m_p1Owner->m_nListenerListRecursing << std::endl;
				--(m_p1Owner->m_nListenerListRecursing);
				return false; //------------------------------------------------
			}
		}
		// callback
		(*refListener)(refEvent);
//std::cout << "StdDeviceManager::ListenerData::handleEventCommon exit B  " << m_p1Owner->m_nListenerListRecursing << std::endl;
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
bool StdDeviceManager::ListenerData::handleEvent(const shared_ptr<Event>& refEvent) const
{
	return handleEventCommon(-1, refEvent);
}
bool StdDeviceManager::ListenerData::handleEventCallIf(int32_t nClassTypeIdx, const shared_ptr<Event>& refEvent) const
{
	if (nClassTypeIdx < 0) {
		nClassTypeIdx = m_p1Owner->getEventClassIndex(refEvent->getEventClass());
	}
	assert((nClassTypeIdx >= 0) && (nClassTypeIdx < static_cast<int32_t>(m_aCallIfEventClass.size())));
	return handleEventCommon(nClassTypeIdx, refEvent);
}
std::vector<Capability::Class> StdDeviceManager::getCapabilityClasses() const
{
	return m_aCapabitityClass;
}
std::vector<Event::Class> StdDeviceManager::getEventClasses() const
{
	return m_aEventClass;
}
int32_t StdDeviceManager::getEventClassIndex(const Event::Class& oEventClass) const
{
	for (size_t nIdx = 0; nIdx < m_aEventClass.size(); ++nIdx) {
		if (m_aEventClass[nIdx] == oEventClass) {
			return static_cast<int32_t>(nIdx);
		}
	}
	return -1;
}
bool StdDeviceManager::getEventClassEnabled(const Event::Class& oEventClass) const
{
//std::cout << "getEventClassEnabled=" << oEventClass.name() << " m_aEventClass.size()=" << m_aEventClass.size() << std::endl;
	const int32_t nIdx = getEventClassIndex(oEventClass);
	if (nIdx < 0) {
		return false;
	}
	return m_aEventClassEnabled[nIdx];
}
void StdDeviceManager::enableEventClass(const Event::Class& oEventClass)
{
	const int32_t nIdx = getEventClassIndex(oEventClass);
	if (nIdx < 0) {
		return;
	}
	m_aEventClassEnabled[nIdx] = true;
}
bool StdDeviceManager::addDevice(const shared_ptr<Device>& refDevice)
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
bool StdDeviceManager::removeDevice(const shared_ptr<Device>& refDevice)
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
