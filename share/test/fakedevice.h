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

#ifndef _STMI_TESTING_FAKE_DEVICE_H_
#define _STMI_TESTING_FAKE_DEVICE_H_

#include "devicemanager.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

template <class CAPA>
class FakeDevice : public Device, public CAPA, public std::enable_shared_from_this<FakeDevice<CAPA>>
{
public:
	std::string getName() const override
	{
		static_assert(std::is_base_of<Capability, CAPA>::value, "");
		std::string sName ="TestDevice-";
		sName.append(typeid(CAPA).name());
		return sName;
	}
	shared_ptr<DeviceManager> getDeviceManager() const override
	{
		return shared_ptr<DeviceManager>{};
	}
	shared_ptr<DeviceManager> getDeviceManager() override
	{
		return shared_ptr<DeviceManager>{};
	}
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const override
	{
		shared_ptr<Capability> refCapa;
		if (oClass == typeid(CAPA)) {
			shared_ptr<const FakeDevice<CAPA>> refConstThis = FakeDevice<CAPA>::shared_from_this();
			shared_ptr<FakeDevice<CAPA>> refThis = std::const_pointer_cast<FakeDevice<CAPA>>(refConstThis);
			refCapa = refThis;
		}
		return refCapa;
	}
	std::vector<Capability::Class> getCapabilityClasses() const override
	{
		return {CAPA::getClass()};
	}
	shared_ptr<Device> getDevice() const override
	{
		shared_ptr<const FakeDevice<CAPA> > refConstThis = FakeDevice<CAPA>::shared_from_this();
		shared_ptr<FakeDevice<CAPA>> refThis = std::const_pointer_cast<FakeDevice<CAPA>>(refConstThis);
		return refThis;
	}
	shared_ptr<Device> getDevice() override
	{
		shared_ptr<FakeDevice<CAPA>> refThis = FakeDevice<CAPA>::shared_from_this();
		return refThis;
	}
	shared_ptr<Capability> getCapability(int32_t nCapabilityId) const override
	{
		if (nCapabilityId != Capability::getId()) {
			return shared_ptr<Capability>{};
		}
		shared_ptr<const FakeDevice<CAPA> > refConstThis = FakeDevice<CAPA>::shared_from_this();
		shared_ptr<FakeDevice<CAPA>> refThis = std::const_pointer_cast<FakeDevice<CAPA>>(refConstThis);
		return refThis;
	}
	std::vector<int32_t> getCapabilities() const override
	{
		return {Capability::getId()};
	}
};

} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FAKE_DEVICE_H_ */
