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
 * File:   basicdevice.h
 */

#ifndef STMI_BASIC_DEVICE_H
#define STMI_BASIC_DEVICE_H

#include "childdevicemanager.h"

#include <type_traits>
#include <string>

namespace stmi
{

/** Device class template.
 * Helper class that Device classes can subclass. It stores name and
 * owner device manager.
 * ODM = Owner DeviceManager, must be subclass of ChildDeviceManager.
 */
template<class ODM>
class BasicDevice : public Device
{
public:
	std::string getName() const override { return m_sName; }
	//
	shared_ptr<DeviceManager> getDeviceManager() const override;
	//
	shared_ptr<ODM> getOwnerDeviceManager() const { return m_refOwner.lock(); }
protected:
	/** Device template constructor.
	 * The instances of this class keep a weak reference to their owner
	 * ChildDeviceManager to avoid memory leaks.
	 * @param sName The name of the device. Cannot be empty.
	 * @param refOwnerDeviceManager The ChildDeviceManager subclass. Cannot be null.
	 */
	BasicDevice(std::string sName, const shared_ptr<ODM>& refOwnerDeviceManager)
	: m_sName(sName)
	, m_refOwner(refOwnerDeviceManager)
	{
		static_assert(std::is_base_of<ChildDeviceManager, ODM>::value
				, "BasicDevice<> template parameter has to be subclass of ChildDeviceManager");
		assert(refOwnerDeviceManager);
		assert(!sName.empty());
	}
	/** Resets the link to the owner device manager.
	 * When a device is removed from a device manager this function should 
	 * be called.
	 */
	void resetOwnerDeviceManager() { m_refOwner.reset(); }
private:
	std::string m_sName;
	weak_ptr<ODM> m_refOwner;
private:
	BasicDevice() = delete;
};

template<class ODM>
shared_ptr<DeviceManager> BasicDevice<ODM>::getDeviceManager() const
{
	auto refOwner = m_refOwner.lock();
	if (!refOwner) {
		return refOwner;
	}
	return refOwner->getRoot();
}

} // namespace stmi

#endif /* STMI_BASIC_DEVICE_H */
