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
 * File:   childdevicemanager.h
 */

#ifndef _STMI_CHILD_DEVICE_MANAGER_H_
#define _STMI_CHILD_DEVICE_MANAGER_H_

#include "devicemanager.h"

namespace stmi
{

class ParentDeviceManager;

/** Defines a child device manager.
 * This allows a device manager to be combined with other device managers in a tree-like
 * structure to form a bigger one.
 * Subclasses can be added to a ParentDeviceManager.
 */
class ChildDeviceManager : public DeviceManager, public std::enable_shared_from_this<ChildDeviceManager>
{
public:
	/** Gets the parent of this child device manager.
	 * If it has no parent returns null, in which case getRoot() returns this instance.
	 */
	shared_ptr<ParentDeviceManager> getParent() const;
	/** Gets the parent of this child device manager.
	 * If it has no parent returns null, in which case getRoot() returns this instance.
	 */
	shared_ptr<ParentDeviceManager> getParent();
	/** Returns the root device manager.
	 * Returns the root device manager of tree-like structure this device manager is in.
	 * @return The root. Cannot be null.
	 */
	shared_ptr<DeviceManager> getRoot() const;
	/** Returns the root device manager.
	 * Returns the root device manager of tree-like structure this device manager is in.
	 * @return The root. Cannot be null.
	 */
	shared_ptr<DeviceManager> getRoot();
protected:
	friend class ParentDeviceManager;
	ChildDeviceManager();

private:
	// Sets the parent of the child device manager (called by ParentDeviceManager::init())
	void setParent(const shared_ptr<ParentDeviceManager>& refParent);

	shared_ptr<ParentDeviceManager> calcRoot();

private:
	weak_ptr<ParentDeviceManager> m_refWeakParent;
	bool m_bImRoot; // If true m_refWeakRoot and m_refWeakParent are null.
	weak_ptr<ParentDeviceManager> m_refWeakRoot;
private:
	ChildDeviceManager(const ChildDeviceManager& oSource) = delete;
	ChildDeviceManager& operator=(const ChildDeviceManager& oSource) = delete;
};

////////////////////////////////////////////////////////////////////////////////
/** Device manager container class.
 * The contained device managers (the children) must be subclasses of ChildDeviceManager.
 */
class ParentDeviceManager : public ChildDeviceManager
{
public:
	virtual ~ParentDeviceManager();
	/** Creates an instance of a parent device manager.
	 * @param aChildDeviceManager An array of non null child device managers.
	 * @return The created instance.
	 */
	static shared_ptr<ParentDeviceManager> create(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager);

	/** @see getDevices() */
	shared_ptr<Device> getDevice(int32_t nDeviceId) const override;

	/** The union of the capability classes of all the children. */
	std::vector<Capability::Class> getCapabilityClasses() const override;
	/** The union of the event types of all the children. */
	std::vector<Event::Class> getEventClasses() const override;

	/** The union of the devices of all the children with a capability class. */
	std::vector<int32_t> getDevicesWithCapabilityClass(const Capability::Class& oCapabilityClass) const override;

	/** The union of the devices of all the children. */
	std::vector<int32_t> getDevices() const override;

	/** Tells whether an event class is enabled in at least one of the children. */
	bool getEventClassEnabled(const Event::Class& oEventClass) const override;
	/** Calls the function for all the  children. */
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
	/** Calls removeEventListener of children.
	 */
	bool removeEventListener(const shared_ptr<EventListener>& refEventListener, bool bFinalize) override;
protected:
	ParentDeviceManager();
	/** Initialization.
	 * Cannot be called twice.
	 * @param aChildDeviceManager The child device managers. Cannot be empty. Cannot be null.
	 */
	void init(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager);

	/** 
	 * @return The child device managers.
	 */
	// The children of a parent cannot be removed before the parent is removed,
	// this ensures that it's always the root that has to be removed first
	// which can be detected through ChildDeviceManager::m_refWeakRoot being null.
	inline std::vector< shared_ptr<ChildDeviceManager> > getChildren() const
	{
		return m_aChildDeviceManager;
	}

private:
	std::vector< shared_ptr<ChildDeviceManager> > m_aChildDeviceManager;
private:
	ParentDeviceManager(const ParentDeviceManager& oSource) = delete;
	ParentDeviceManager& operator=(const ParentDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif	/* _STMI_CHILD_DEVICE_MANAGER_H_ */

