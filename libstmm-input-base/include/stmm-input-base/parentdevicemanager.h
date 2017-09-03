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
 * File:   parentdevicemanager.h
 */

#ifndef STMI_PARENT_DEVICE_MANAGER_H
#define STMI_PARENT_DEVICE_MANAGER_H

#include "childdevicemanager.h"

#include <vector>


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
	shared_ptr<Device> getDevice(int32_t nDeviceId) const override;

	/** The union of the device manager capability classes of all the children. */
	std::vector<Capability::Class> getCapabilityClasses() const override;
	/** The union of the device capability classes of all the children's devices. */
	std::vector<Capability::Class> getDeviceCapabilityClasses() const override;
	/** The union of the event types of all the children. */
	std::vector<Event::Class> getEventClasses() const override;

	/** The union of the devices of all the children with a given capability class. */
	std::vector<int32_t> getDevicesWithCapabilityClass(const Capability::Class& oCapabilityClass) const override;

	/** The union of the devices of all the children. */
	std::vector<int32_t> getDevices() const override;

	/** Tells whether an event class is enabled in at least one of the children. */
	bool isEventClassEnabled(const Event::Class& oEventClass) const override;
	/** Calls the function for all the children. */
	void enableEventClass(const Event::Class& oEventClass) override;

	/** Calls addAccessor() of children. 
	 * @return `true` if at least one of the children returned `true`, `false` otherwise.
	 */
	bool addAccessor(const shared_ptr<Accessor>& refAccessor) override;
	/** Calls removeAccessor() of children.
	 * @return `true` if at least one of the children returned `true`, `false` otherwise.
	 */
	bool removeAccessor(const shared_ptr<Accessor>& refAccessor) override;
	/** Tells whether an accessor ia at least in one of the children. */
	bool hasAccessor(const shared_ptr<Accessor>& refAccessor) override;

	/** Calls addEventListener of children. 
	 */
	bool addEventListener(const shared_ptr<EventListener>& refEventListener, const shared_ptr<CallIf>& refCallIf) override;
	/** Calls addEventListener of children. 
	 */
	bool addEventListener(const shared_ptr<EventListener>& refEventListener) override;
	/** Calls removeEventListener of children.
	 */
	bool removeEventListener(const shared_ptr<EventListener>& refEventListener, bool bFinalize) override;
	/** Calls removeEventListener of children.
	 */
	bool removeEventListener(const shared_ptr<EventListener>& refEventListener) override;
	/** The child device managers.
	 * @return The child device managers.
	 */
	inline std::vector< shared_ptr<ChildDeviceManager> > getChildren() const
	{
		return m_aChildDeviceManagers;
	}
protected:
	ParentDeviceManager();
	/** Initialization.
	 * Cannot be called twice.
	 * @param aChildDeviceManager The child device managers. Cannot be empty.
	 */
	void init(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager);

	/** Removes all children.
	 * This can be useful to call from the destructor of a subclass.
	 */
	void removeChildren()
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

