/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   masgtkbackend.h
 */

#ifndef STMI_MAS_GTK_BACKEND_H
#define STMI_MAS_GTK_BACKEND_H

#include <gtkmm.h>

#include <memory>
#include <string>

namespace stmi { class GtkAccessor; }
namespace stmi { class MasGtkDeviceManager; }

namespace stmi
{

namespace Private
{
namespace Mas
{

using std::shared_ptr;
using std::weak_ptr;

void gdkDeviceManagerCallbackAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data) noexcept;
void gdkDeviceManagerCallbackChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data) noexcept;
void gdkDeviceManagerCallbackRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data) noexcept;

////////////////////////////////////////////////////////////////////////////////
class GtkBackend //: public sigc::trackable
{
public:
	GtkBackend(MasGtkDeviceManager* p0Owner, const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager) noexcept;
	virtual ~GtkBackend() noexcept;

#ifdef STMI_TESTING_IFACE
	virtual
#endif
	std::string getPointerDeviceName() const noexcept { return m_refGdkPointer->get_name(); }
#ifdef STMI_TESTING_IFACE
	virtual
#endif
	std::string getKeyboardDeviceName() const noexcept { return m_refGdkKeyboard->get_name(); }

#ifdef STMI_TESTING_IFACE
	virtual
#endif
	bool isCompatible(const shared_ptr<GtkAccessor>& refGtkAccessor) const noexcept;

#ifdef STMI_TESTING_IFACE
	virtual
#else
	inline
#endif
	bool isPointerDevice(GdkDevice* p0Device) const noexcept { return (m_refGdkPointer->gobj() == p0Device); }

protected:
	void onDeviceChanged(bool bPointer) noexcept;
	void onDevicePairAdded() noexcept;
	void onDevicePairRemoved() noexcept;

private:
	friend void Private::Mas::gdkDeviceManagerCallbackAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data) noexcept;
	friend void Private::Mas::gdkDeviceManagerCallbackChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data) noexcept;
	friend void Private::Mas::gdkDeviceManagerCallbackRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data) noexcept;
	//
	void initDeviceManager() noexcept;
	void deinitDeviceManager() noexcept;
	void addDevices(GdkDevice* p0NotThisOne) noexcept;
	void gdkDeviceAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device) noexcept;
	void gdkDeviceChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device) noexcept;
	void gdkDeviceRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device) noexcept;
	bool findGdkMasterDevice(GdkDevice* p0MasterDevice, bool& bPointer) const noexcept;
private:
	MasGtkDeviceManager* m_p0Owner;
	Glib::RefPtr<Gdk::DeviceManager> m_refGdkDeviceManager;
	gulong m_nConnectHandlerDeviceAdded;
	gulong m_nConnectHandlerDeviceChanged;
	gulong m_nConnectHandlerDeviceRemoved;

	Glib::RefPtr<Gdk::Device> m_refGdkPointer;
	Glib::RefPtr<Gdk::Device> m_refGdkKeyboard;
private:
	GtkBackend(const GtkBackend& oSource) = delete;
	GtkBackend& operator=(const GtkBackend& oSource) = delete;
};

} // namespace Mas
} // namespace Private

} // namespace stmi

#endif /* STMI_MAS_GTK_BACKEND_H */
