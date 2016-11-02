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
 * File:   device.h
 */

#ifndef _STMI_DEVICE_H_
#define _STMI_DEVICE_H_

#include "capability.h"
#include "private-namedtypes.h"

#include <typeinfo>
#include <type_traits>
#include <vector>
#include <string>
#include <memory>
#include <atomic>

namespace stmi
{

class DeviceManager;

/** Device base class */
class Device
{
public:
	virtual ~Device() = default;

	/** The device id.
	 * @return The unique id of the device.
	 */
	int32_t getId() const { return m_nDeviceId; }

	/**
	 * @return Name of the device. Not necessarily unique among devices.
	 */
	virtual std::string getName() const = 0;

	/** The device manager that manages the device.
	 * As a convention Device instances should hold a weak_ptr to their manager to avoid reference cycles.
	 * @return The device manager or null if the device is no longer attached to its manager.
	 */
	virtual shared_ptr<DeviceManager> getDeviceManager() const = 0;
	/** The device manager that manages the device.
	 * As a convention Device instances should hold a weak_ptr to their manager to avoid reference cycles.
	 * @return The device manager or null if the device is no longer attached to its manager.
	 */
	virtual shared_ptr<DeviceManager> getDeviceManager() = 0;

	/** Request a registered capability from this device.
	 * @param refCapa [out] The registered Capability subclass or null if not supported by device.
	 * @return Whether the device has the requested capability.
	 */
	template < typename TCapa >
	bool getCapability(shared_ptr<TCapa>& refCapa) const
	{
		static_assert(std::is_base_of<Capability, TCapa>::value && !std::is_base_of<TCapa, Capability>::value
						, "TCapa must be subclass of Capability");
		shared_ptr<Capability> refSubCapa = getCapability(typeid(TCapa));
		if (!refSubCapa) {
			return false; //----------------------------------------------------
		}
		assert(! refSubCapa->getCapabilityClass().isDeviceManagerCapability());
		refCapa = std::static_pointer_cast<TCapa>(refSubCapa);
		return true;
	}
	/** Requests the instance of a capability class.
	 * If the device doesn't have the capability null is returned.
	 * @param oClass The requested registered class.
	 * @return The capability or null.
	 */
	virtual shared_ptr<Capability> getCapability(const Capability::Class& oClass) const = 0;
	/** Returns the capability with the given id, or null if not found.
	 */
	virtual shared_ptr<Capability> getCapability(int32_t nCapabilityId) const = 0;
	/** Returns the ids of all the device's capabilities.
	 * @return The set of capability ids.
	 */
	virtual std::vector<int32_t> getCapabilities() const = 0;
	/** The capability classes supported by this device.
	 * @return The set of registered capability classes.
	 */
	virtual std::vector<Capability::Class> getCapabilityClasses() const = 0;
protected:
	Device();

private:
	static int32_t getNewDeviceId();
private:
	int32_t m_nDeviceId;
	static std::atomic<int32_t> s_nNewIdCounter;
private:
	Device(const Device& oSource) = delete;
	Device& operator=(const Device& oSource) = delete;
};

} // namespace stmi

#endif	/* _STMI_DEVICE_H_ */

