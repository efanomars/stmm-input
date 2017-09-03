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
 * File:   devicemgmtcapability.h
 */

#ifndef STMI_DEVICE_MGMT_CAPABILITY_H
#define STMI_DEVICE_MGMT_CAPABILITY_H

#include <stmm-input/capability.h>

#include <vector>
#include <memory>

namespace stmi
{

using std::shared_ptr;

/** Device manager's capability for tree like composed device managers.
 * The capabilities of leaf device managers might not be directly accessible.
 * This device manager capability allows to retrieve other capabilities from
 * inner and leaf node device managers. An example of could be a ServerCapability
 * which allows to to start, stop the server or request status information.
 *
 * This capability is also used by DeviceMgmtEvent to inform listeners of added,
 * removed and changed devices.
 */
class DeviceMgmtCapability : public DeviceManagerCapability
{
public:
	/** Requests the device managers capabilities of a certain class.
	 * Same as getNodeDeviceManagerCapabilities(const Capability::Class& oClass)
	 * but without the need to statically cast.
	 * @param aCapabilities [out] The vector of registered DeviceManagerCapability subclasses.
	 */
	template < typename TCapa >
	void getNodeDeviceManagerCapabilities(std::vector<shared_ptr<TCapa>>& aCapabilities) const
	{
		static_assert(std::is_base_of<Capability, TCapa>::value && !std::is_base_of<TCapa, DeviceManagerCapability>::value
						, "TCapa must be subclass of DeviceManagerCapability");
		std::vector<shared_ptr<Capability>> aSubCapas = getNodeDeviceManagerCapabilities(typeid(TCapa));
		aCapabilities.clear();
		if (aSubCapas.empty()) {
			return; //----------------------------------------------------------
		}
		for (auto& refSubCapa : aSubCapas) {
			assert(refSubCapa->getCapabilityClass().isDeviceManagerCapability());
			aCapabilities.push_back(std::static_pointer_cast<TCapa>(refSubCapa));
		}
		return;
	}

	/** Requests the device managers capabilities of a certain class.
	 * When device managers are composed in a tree like structure the
	 * root sort of hides the inner nodes. This function allows to access
	 * the capabilities of the owner device manager and all its descendants.
	 *
	 * The returned capabilities can safely be cast to the requested class
	 * with static_cast or std::static_pointer_cast.
	 * @param oClass The requested registered device manager capability class.
	 * @return The capabilities.
	 */
	virtual std::vector<shared_ptr<Capability>> getNodeDeviceManagerCapabilities(const Capability::Class& oClass) const = 0;
	/** The root device manager.
	 * If the owner device manager of this capability is an inner node of a tree like structure
	 * of device managers the root of it is returned otherwise the same as
	 * DeviceManagerCapability::getDeviceManager() is returned.
	 * @return The root device manager. Cannot be null.
	 */
	virtual shared_ptr<DeviceManager> getRootDeviceManager() const = 0;

	static const char* const s_sClassId;
	static const Capability::Class& getClass() { return s_oInstall.getCapabilityClass(); }
protected:
	DeviceMgmtCapability();
private:
	static RegisterClass<DeviceMgmtCapability> s_oInstall;
private:
};

} // namespace stmi

#endif /* STMI_DEVICE_MGMT_CAPABILITY_H */
