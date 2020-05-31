/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   fakemasgtkbackend.h
 */

#ifndef STMI_TESTING_FAKE_MAS_GTK_BACKEND_H
#define STMI_TESTING_FAKE_MAS_GTK_BACKEND_H

#include "masgtkbackend.h"

#include <stmm-input-gtk/gtkaccessor.h>

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
	FakeGtkBackend(::stmi::MasGtkDeviceManager* p0Owner, const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager) noexcept
	: Private::Mas::GtkBackend(p0Owner, refGdkDeviceManager)
	, m_refGdkDeviceManager(refGdkDeviceManager)
	, m_p0PointerDevice(reinterpret_cast<GdkDevice*>(16))
	, m_p0KeyboardDevice(reinterpret_cast<GdkDevice*>(17))
	{
		assert(refGdkDeviceManager);
	}
	std::string getPointerDeviceName() const noexcept override
	{
		std::ostringstream o;
		if (!(o << reinterpret_cast<int64_t>(m_p0PointerDevice))) {
			assert(false);
		}
		return "TestPointer~" + o.str();
	}
	std::string getKeyboardDeviceName() const noexcept override
	{
		std::ostringstream o;
		if (!(o << reinterpret_cast<int64_t>(m_p0KeyboardDevice))) {
			assert(false);
		}
		return "TestKeyboard~" + o.str();
	}
	bool isCompatible(const shared_ptr<GtkAccessor>& refGtkAccessor) const noexcept override
	{
		Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
		assert(p0GtkmmWindow != nullptr);
		Glib::RefPtr<Gdk::Display> refDisplay = p0GtkmmWindow->get_display();
		assert(refDisplay);
		Glib::RefPtr<Gdk::DeviceManager> refGdkDeviceManager = refDisplay->get_device_manager();
		assert(refGdkDeviceManager);
		return (m_refGdkDeviceManager == refGdkDeviceManager);
	}
	bool isPointerDevice(GdkDevice* p0Device) const noexcept override
	{
		return (p0Device == m_p0PointerDevice);
	}
	//
	GdkDevice* getPointerBackendDevice() noexcept
	{
		return m_p0PointerDevice;
	}
	GdkDevice* getKeyboardBackendDevice() noexcept
	{
		return m_p0PointerDevice;
	}
	//
	void simulateNewMasterDevices() noexcept
	{
		const auto nPointerDevice = reinterpret_cast<int64_t>(m_p0PointerDevice) + 16;
		const auto nKeyboardDevice = reinterpret_cast<int64_t>(m_p0KeyboardDevice) + 16;
		m_p0PointerDevice = reinterpret_cast<GdkDevice*>(nPointerDevice);
		m_p0KeyboardDevice = reinterpret_cast<GdkDevice*>(nKeyboardDevice);
		onDevicePairRemoved();
		onDevicePairAdded();
	}
	void simulateMasterDeviceChange(bool bPointer) noexcept
	{
		onDeviceChanged(bPointer);
	}
private:
	Glib::RefPtr<Gdk::DeviceManager> m_refGdkDeviceManager;

	// These are identifiers and do not point to an actual GdkDevice instance
	GdkDevice* m_p0PointerDevice; // 16, (change->) 32, (change->) 48, ...
	GdkDevice* m_p0KeyboardDevice; // Always m_p0PointerDevice + 1
};

} // namespace Mas
} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FAKE_MAS_GTK_BACKEND_H */
