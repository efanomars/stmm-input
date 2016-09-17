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
 * File:   fakemasgtkbackend.h
 */

#ifndef _STMI_TESTING_FAKE_MAS_GTK_BACKEND_H_
#define _STMI_TESTING_FAKE_MAS_GTK_BACKEND_H_

#include "masgtkbackend.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{
namespace Mas
{

class FakeGtkBackend : public Private::Mas::GtkBackend
{
public:
	FakeGtkBackend(::stmi::MasGtkDeviceManager* p0Owner, const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager)
	: Private::Mas::GtkBackend(p0Owner, refGdkDeviceManager)
	, m_p0Owner(p0Owner)
	, m_refGdkDeviceManager(refGdkDeviceManager)
	, m_p0PointerDevice(reinterpret_cast<GdkDevice*>(16))
	, m_p0KeyboardDevice(reinterpret_cast<GdkDevice*>(17))
	{
		assert(refGdkDeviceManager);
	}
	std::string getPointerDeviceName() const override
	{
		std::ostringstream o;
		if (!(o << reinterpret_cast<int64_t>(m_p0PointerDevice))) {
			assert(false);
		}
		return "TestPointer~" + o.str();
	}
	std::string getKeyboardDeviceName() const override
	{
		std::ostringstream o;
		if (!(o << reinterpret_cast<int64_t>(m_p0KeyboardDevice))) {
			assert(false);
		}
		return "TestKeyboard~" + o.str();
	}
	bool isCompatible(const shared_ptr<GtkAccessor>& refGtkAccessor) const override
	{
		Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
		assert(p0GtkmmWindow != nullptr);
		Glib::RefPtr<Gdk::Display> refDisplay = p0GtkmmWindow->get_display();
		assert(refDisplay);
		Glib::RefPtr<Gdk::DeviceManager> refGdkDeviceManager = refDisplay->get_device_manager();
		assert(refGdkDeviceManager);
		return (m_refGdkDeviceManager == refGdkDeviceManager);
	}
	bool isPointerDevice(GdkDevice* p0Device) const override
	{
		return (p0Device == m_p0PointerDevice);
	}
	//
	GdkDevice* getPointerBackendDevice()
	{
		return m_p0PointerDevice;
	}
	GdkDevice* getKeyboardBackendDevice()
	{
		return m_p0PointerDevice;
	}
	//
	void simulateNewMasterDevices()
	{
		const auto nPointerDevice = reinterpret_cast<int64_t>(m_p0PointerDevice) + 16;
		const auto nKeyboardDevice = reinterpret_cast<int64_t>(m_p0KeyboardDevice) + 16;
		m_p0PointerDevice = reinterpret_cast<GdkDevice*>(nPointerDevice);
		m_p0KeyboardDevice = reinterpret_cast<GdkDevice*>(nKeyboardDevice);
		onDevicePairRemoved();
		onDevicePairAdded();
	}
	void simulateMasterDeviceChange(bool bPointer)
	{
		onDeviceChanged(bPointer);
	}
private:
	::stmi::MasGtkDeviceManager* m_p0Owner;
	Glib::RefPtr<Gdk::DeviceManager> m_refGdkDeviceManager;

	// These are identifiers and do not point to an actual GdkDevice instance
	GdkDevice* m_p0PointerDevice; // 16, (change->) 32, (change->) 48, ...
	GdkDevice* m_p0KeyboardDevice; // Always m_p0PointerDevice + 1
};

} // namespace Mas
} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FAKE_MAS_GTK_BACKEND_H_ */
