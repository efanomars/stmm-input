/*
 * Copyright © 2016-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   parentdevicemanager.h
 */

#ifndef STMI_PARENT_DEVICE_MANAGER_H
#define STMI_PARENT_DEVICE_MANAGER_H

#include "childdevicemanager.h"

#include <stmm-input/capability.h>
#include <stmm-input/devicemanager.h>
#include <stmm-input/event.h>

#include <cstdint>
#include <memory>
#include <vector>

namespace stmi { class Accessor; }
namespace stmi { class Device; }


namespace stmi
{

////////////////////////////////////////////////////////////////////////////////
/** Device manager container class.
 * The contained device managers (the children) must be subclasses of ChildDeviceManager.
 */
class ParentDeviceManager : public ChildDeviceManager
{
public:
	/** Return the device with the given id.
	 * @param nDeviceId The device id.
	 * @return The device or null if not found.
	 */
	shared_ptr<Device> getDevice(int32_t nDeviceId) const noexcept override;

	/** Get a device manager capability of a certain class.
	 * Queries its children. The returned capability isn't necessarily the only one for the given class.
	 * @param oClass The class. Must be valid.
	 * @return The device manager capability or null if not found.
	 */
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const noexcept override;
	/** Get a device manager capability by id.
	 * Queries its children.
	 * @param nCapabilityId The id.
	 * @return The device manager capability or null if not found.
	 */
	shared_ptr<Capability> getCapability(int32_t nCapabilityId) const noexcept override;
	/** The union of the device manager capability classes of all the children. */
	std::vector<Capability::Class> getCapabilityClasses() const noexcept override;
	/** The union of the device capability classes of all the children's devices. */
	std::vector<Capability::Class> getDeviceCapabilityClasses() const noexcept override;
	/** The union of the event types of all the children. */
	std::vector<Event::Class> getEventClasses() const noexcept override;

	/** The union of the devices of all the children with a given capability class. */
	std::vector<int32_t> getDevicesWithCapabilityClass(const Capability::Class& oCapabilityClass) const noexcept override;

	/** The union of the devices of all the children. */
	std::vector<int32_t> getDevices() const noexcept override;

	/** Tells whether an event class is enabled in at least one of the children. */
	bool isEventClassEnabled(const Event::Class& oEventClass) const noexcept override;
	/** Calls the function for all the children. */
	void enableEventClass(const Event::Class& oEventClass) noexcept override;

	/** Calls addAccessor() of children.
	 * @return `true` if at least one of the children returned `true`, `false` otherwise.
	 */
	bool addAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override;
	/** Calls removeAccessor() of children.
	 * @return `true` if at least one of the children returned `true`, `false` otherwise.
	 */
	bool removeAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override;
	/** Tells whether an accessor is at least in one of the children. */
	bool hasAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override;

	/** Calls addEventListener of children.
	 */
	bool addEventListener(const shared_ptr<EventListener>& refEventListener, const shared_ptr<CallIf>& refCallIf) noexcept override;
	/** Calls addEventListener of children.
	 */
	bool addEventListener(const shared_ptr<EventListener>& refEventListener) noexcept override;
	/** Calls removeEventListener of children.
	 */
	bool removeEventListener(const shared_ptr<EventListener>& refEventListener, bool bFinalize) noexcept override;
	/** Calls removeEventListener of children.
	 */
	bool removeEventListener(const shared_ptr<EventListener>& refEventListener) noexcept override;
	/** The child device managers.
	 * @return The child device managers.
	 */
	inline std::vector< shared_ptr<ChildDeviceManager> > getChildren() const noexcept
	{
		return m_aChildDeviceManagers;
	}
	/** Get the device manager capabilities of a given class of all the children.
	 * @param oClass The device manager capability class. Must be subclass of DeviceManagerCapability.
	 * @return The available device manager capabilities.
	 */
	virtual std::vector<shared_ptr<Capability>> getDeviceManagerCapabilities(const Capability::Class& oClass) const noexcept;

protected:
	ParentDeviceManager() noexcept;
	/** Initialization.
	 * Cannot be called twice.
	 * @param aChildDeviceManager The child device managers. Cannot be empty.
	 */
	void init(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager) noexcept;

	/** Removes all children.
	 */
	void removeChildren() noexcept
	{
		m_aChildDeviceManagers.clear();
	}
private:
	std::vector< shared_ptr<ChildDeviceManager> > m_aChildDeviceManagers;
private:
	ParentDeviceManager(const ParentDeviceManager& oSource) = delete;
	ParentDeviceManager& operator=(const ParentDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_PARENT_DEVICE_MANAGER_H */

