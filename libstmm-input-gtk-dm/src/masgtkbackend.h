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
 * File:   masgtkbackend.h
 */

#ifndef _STMI_MAS_GTK_BACKEND_H_
#define _STMI_MAS_GTK_BACKEND_H_

#include <stmm-input-gtk/gtkaccessor.h>

#include <gtkmm.h>

#include <cassert>

namespace stmi
{

class MasGtkDeviceManager;

namespace Private
{
namespace Mas
{

using std::shared_ptr;
using std::weak_ptr;

void gdkDeviceManagerCallbackAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
void gdkDeviceManagerCallbackChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
void gdkDeviceManagerCallbackRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);

////////////////////////////////////////////////////////////////////////////////
class GtkBackend //: public sigc::trackable
{
public:
	GtkBackend(MasGtkDeviceManager* p0Owner, const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager);
	virtual ~GtkBackend();

#ifdef STMI_TESTING_IFACE
	virtual
#endif
	std::string getPointerDeviceName() const { return m_refGdkPointer->get_name(); }
#ifdef STMI_TESTING_IFACE
	virtual
#endif
	std::string getKeyboardDeviceName() const { return m_refGdkKeyboard->get_name(); }

#ifdef STMI_TESTING_IFACE
	virtual
#endif
	bool isCompatible(const shared_ptr<GtkAccessor>& refGtkAccessor) const;

#ifdef STMI_TESTING_IFACE
	virtual
#else
	inline
#endif
	bool isPointerDevice(GdkDevice* p0Device) const { return (m_refGdkPointer->gobj() == p0Device); }

protected:
	void onDeviceChanged(bool bPointer);
	void onDevicePairAdded();
	void onDevicePairRemoved();

private:
	friend void Private::Mas::gdkDeviceManagerCallbackAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
	friend void Private::Mas::gdkDeviceManagerCallbackChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
	friend void Private::Mas::gdkDeviceManagerCallbackRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
	//
	void initDeviceManager();
	void deinitDeviceManager();
	void addDevices(GdkDevice* p0NotThisOne);
	void gdkDeviceAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device);
	void gdkDeviceChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device);
	void gdkDeviceRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device);
	bool findGdkMasterDevice(GdkDevice* p0MasterDevice, bool& bPointer) const;
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

#endif /* _STMI_MAS_GTK_BACKEND_H_ */
