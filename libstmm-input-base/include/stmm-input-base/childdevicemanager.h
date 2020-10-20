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
 * File:   childdevicemanager.h
 */

#ifndef STMI_CHILD_DEVICE_MANAGER_H
#define STMI_CHILD_DEVICE_MANAGER_H

#include <stmm-input/devicemanager.h>

#include <memory>


namespace stmi
{

class ParentDeviceManager;

/** Defines a child device manager.
 * This allows a device manager to be combined with other device managers in a tree-like
 * structure to form a bigger one.
 * Subclasses of this class can be added to a ParentDeviceManager.
 */
class ChildDeviceManager : public DeviceManager, public std::enable_shared_from_this<ChildDeviceManager>
{
public:
	/** Gets the parent of this child device manager.
	 * If it has no parent returns null, in which case getRoot() returns this instance.
	 */
	shared_ptr<ParentDeviceManager> getParent() const noexcept;
	/** Gets the parent of this child device manager.
	 * If it has no parent returns null, in which case getRoot() returns this instance.
	 */
	shared_ptr<ParentDeviceManager> getParent() noexcept;
	/** Returns the root device manager.
	 * Returns the root device manager of tree-like structure this device manager is in.
	 * @return The root. Cannot be null.
	 */
	shared_ptr<ParentDeviceManager> getRoot() const noexcept;
	/** Returns the root device manager.
	 * Returns the root device manager of tree-like structure this device manager is in.
	 * @return The root. Cannot be null.
	 */
	shared_ptr<ParentDeviceManager> getRoot() noexcept;
	/** Tells whether this instance is also a ParentDeviceManager.
	 *
	 * @return Whether a ParentDeviceManager.
	 */
	bool isParent() const noexcept { return m_bImParent; }
	/** If a parent returns this object as a parent.
	 * @return This object as ParentDeviceManager or null if not a parent.
	 */
	shared_ptr<ParentDeviceManager> getAsParent() const noexcept;
	/** If a parent returns this object as a parent.
	 * @return This object as ParentDeviceManager or null if not a parent.
	 */
	shared_ptr<ParentDeviceManager> getAsParent() noexcept;
protected:
	/** Constructor.
	 */
	ChildDeviceManager() noexcept;

private:
	friend class ParentDeviceManager;
	ChildDeviceManager(bool bIsParent) noexcept;
	// Sets the parent of the child device manager (called by ParentDeviceManager::init())
	void setParent(const shared_ptr<ParentDeviceManager>& refParent) noexcept;

	shared_ptr<ParentDeviceManager> calcRoot() noexcept;

private:
	weak_ptr<ParentDeviceManager> m_refWeakParent;
	weak_ptr<ParentDeviceManager> m_refWeakRoot;
	bool m_bImRoot; // If true m_refWeakRoot and m_refWeakParent are null.
	bool m_bImParent;
private:
	ChildDeviceManager(const ChildDeviceManager& oSource) = delete;
	ChildDeviceManager& operator=(const ChildDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_CHILD_DEVICE_MANAGER_H */

