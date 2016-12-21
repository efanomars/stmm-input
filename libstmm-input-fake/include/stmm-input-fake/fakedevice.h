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
 * File:   fakedevice.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef _STMI_TESTING_FAKE_DEVICE_H_
#define _STMI_TESTING_FAKE_DEVICE_H_

#include "devicemanager.h"

#include <memory>
#include <string>
#include <type_traits>


namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

/** Fake device template that implements one capability class. */
template <class CAPA>
class FakeDevice : public Device, public std::enable_shared_from_this<FakeDevice<CAPA>>
{
public:
	/** Constructor.
	 * If the name is empty one is automatically created.
	 * Instances should be created with std::make_shared().
	 * @param refDeviceManager The device manager. Can be null.
	 * @param sName The name of the device. Can be empty.
	 * @param refCapability The capability instance of the device. Cannot be null.
	 */
	FakeDevice(const shared_ptr<DeviceManager>& refDeviceManager, const std::string& sName, const shared_ptr<CAPA>& refCapability)
	: Device()
	, m_sName(sName.empty() ? std::string("TestDevice-").append(typeid(CAPA).name()) : sName)
	, m_refDeviceManager(refDeviceManager)
	, m_refCapability(refCapability)
	{
		static_assert(std::is_base_of<Capability, CAPA>::value, "");
		assert(m_refCapability);
	}
	std::string getName() const override
	{
		return m_sName;
	}
	shared_ptr<DeviceManager> getDeviceManager() const override
	{
		return m_refDeviceManager.lock();
	}
	shared_ptr<DeviceManager> getDeviceManager() override
	{
		return m_refDeviceManager.lock();
	}
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const override
	{
		if (oClass != m_refCapability->getCapabilityClass()) {
			return shared_ptr<Capability>{};
		}
		return m_refCapability;
	}
	std::vector<Capability::Class> getCapabilityClasses() const override
	{
		return {CAPA::getClass()};
	}
	shared_ptr<Capability> getCapability(int32_t nCapabilityId) const override
	{
		if (nCapabilityId != m_refCapability->getId()) {
			return shared_ptr<Capability>{};
		}
		return m_refCapability;
	}
	std::vector<int32_t> getCapabilities() const override
	{
		return {m_refCapability->getId()};
	}
private:
	std::string m_sName;
	weak_ptr<DeviceManager> m_refDeviceManager;
	shared_ptr<CAPA> m_refCapability;
private:
	FakeDevice() = delete;
};

} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FAKE_DEVICE_H_ */
