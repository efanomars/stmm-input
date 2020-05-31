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
 * File:   masgtkbackend.cc
 */

#include "masgtkbackend.h"

#include "masgtkdevicemanager.h"

#include <stmm-input-gtk/gtkaccessor.h>

#include <cassert>
#include <vector>

namespace stmi
{

namespace Private
{
namespace Mas
{

void gdkDeviceManagerCallbackAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data) noexcept
{
	auto p0GtkBackend = static_cast<GtkBackend*>(p0Data);
	p0GtkBackend->gdkDeviceAdded(p0DeviceManager, p0Device);
}
void gdkDeviceManagerCallbackChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data) noexcept
{
	auto p0GtkBackend = static_cast<GtkBackend*>(p0Data);
	p0GtkBackend->gdkDeviceChanged(p0DeviceManager, p0Device);
}
void gdkDeviceManagerCallbackRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data) noexcept
{
	auto p0GtkBackend = static_cast<GtkBackend*>(p0Data);
	p0GtkBackend->gdkDeviceRemoved(p0DeviceManager, p0Device);
}

GtkBackend::GtkBackend(MasGtkDeviceManager* p0Owner, const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager) noexcept
: m_p0Owner(p0Owner)
, m_nConnectHandlerDeviceAdded(0)
, m_nConnectHandlerDeviceChanged(0)
, m_nConnectHandlerDeviceRemoved(0)
{
	assert(p0Owner != nullptr);
	if (!refGdkDeviceManager) {
		// Take the default one
		Glib::RefPtr<Gdk::Display> refDefaultDisplay = Gdk::Display::get_default();
		assert(refDefaultDisplay);
		m_refGdkDeviceManager = refDefaultDisplay->get_device_manager();
	} else {
		m_refGdkDeviceManager = refGdkDeviceManager;
	}
	initDeviceManager();
	addDevices(nullptr);
}
GtkBackend::~GtkBackend() noexcept
{
	deinitDeviceManager();
}

void GtkBackend::initDeviceManager() noexcept
{
	assert(m_refGdkDeviceManager);
	assert(m_nConnectHandlerDeviceAdded == 0);
	GdkDeviceManager* p0GdkDeviceManager = m_refGdkDeviceManager->gobj();
	m_nConnectHandlerDeviceAdded = g_signal_connect(p0GdkDeviceManager, "device-added", G_CALLBACK(Private::Mas::gdkDeviceManagerCallbackAdded), this);
	assert(m_nConnectHandlerDeviceAdded > 0);
	m_nConnectHandlerDeviceChanged = g_signal_connect(p0GdkDeviceManager, "device-changed", G_CALLBACK(Private::Mas::gdkDeviceManagerCallbackChanged), this);
	assert(m_nConnectHandlerDeviceChanged > 0);
	m_nConnectHandlerDeviceRemoved = g_signal_connect(p0GdkDeviceManager, "device-removed", G_CALLBACK(Private::Mas::gdkDeviceManagerCallbackRemoved), this);
	assert(m_nConnectHandlerDeviceRemoved > 0);
}
void GtkBackend::deinitDeviceManager() noexcept
{
	if (m_refGdkDeviceManager && (m_nConnectHandlerDeviceAdded > 0)) {
		GdkDeviceManager* p0GdkDeviceManager = m_refGdkDeviceManager->gobj();
		if (g_signal_handler_is_connected(p0GdkDeviceManager, m_nConnectHandlerDeviceAdded)) {
			g_signal_handler_disconnect(p0GdkDeviceManager, m_nConnectHandlerDeviceAdded);
		}
		if (g_signal_handler_is_connected(p0GdkDeviceManager, m_nConnectHandlerDeviceChanged)) {
			g_signal_handler_disconnect(p0GdkDeviceManager, m_nConnectHandlerDeviceChanged);
		}
		if (g_signal_handler_is_connected(p0GdkDeviceManager, m_nConnectHandlerDeviceRemoved)) {
			g_signal_handler_disconnect(p0GdkDeviceManager, m_nConnectHandlerDeviceRemoved);
		}
	}
}
void GtkBackend::addDevices(GdkDevice* p0NotThisOne) noexcept
{
	assert(m_refGdkDeviceManager);
	assert(!m_refGdkKeyboard);
	assert(!m_refGdkPointer);

	const std::vector< Glib::RefPtr< Gdk::Device > > aDevice = m_refGdkDeviceManager->list_devices(Gdk::DEVICE_TYPE_MASTER);
	for (auto& refMasterDevice : aDevice) {
		const Gdk::InputSource eSource = refMasterDevice->get_source();
		if (eSource == Gdk::SOURCE_KEYBOARD) {
			// add the master keyboard and its associated master pointer
			const Glib::RefPtr<Gdk::Device>& refKeyboard = refMasterDevice;
			const Glib::RefPtr<Gdk::Device> refPointer = refMasterDevice->get_associated_device();
			if (!((refKeyboard->gobj() == p0NotThisOne) || (refPointer->gobj() == p0NotThisOne))) {
				// We don't want to recreate a master device that is about to be removed
				if (!m_refGdkKeyboard) {
					m_refGdkKeyboard = refKeyboard;
				}
				if (!m_refGdkPointer) {
					m_refGdkPointer = refPointer;
				}
			}
		}
	}
	assert(m_refGdkKeyboard);
	assert(m_refGdkPointer);
	if (p0NotThisOne != nullptr) {
		onDevicePairAdded();
	} else {
		// Called from the constructor, the owner isn't initialized yet
		// since the instance of this class is passed to its init function
	}
}
void GtkBackend::gdkDeviceAdded(
GdkDeviceManager*
#ifndef NDEBUG
p0GdkDeviceManager
#endif //NDEBUG
, GdkDevice* p0Device) noexcept
{
	assert(p0GdkDeviceManager == m_refGdkDeviceManager->gobj());
	GdkDeviceType eDeviceType = gdk_device_get_device_type(p0Device);
	if (eDeviceType != GDK_DEVICE_TYPE_SLAVE) {
		// This manager doesn't care about floating or other master devices
		return; //--------------------------------------------------------------
	}
	// get the master
	GdkDevice* p0MasterDevice = gdk_device_get_associated_device(p0Device);
	// find it
	bool bPointer;
	const bool bManagedDevice = findGdkMasterDevice(p0MasterDevice, bPointer);
	if (!bManagedDevice) {
		// The slave is being added to another master
		return; //----------------------------------------------------------
	}
	onDeviceChanged(bPointer);
}
void GtkBackend::gdkDeviceChanged(
GdkDeviceManager*
#ifndef NDEBUG
p0GdkDeviceManager
#endif //NDEBUG
, GdkDevice* p0Device) noexcept
{
	assert(p0GdkDeviceManager == m_refGdkDeviceManager->gobj());
	GdkDeviceType eDeviceType = gdk_device_get_device_type(p0Device);
	if (eDeviceType != GDK_DEVICE_TYPE_MASTER) {
		// This manager has only master devices
		return; //--------------------------------------------------------------
	}
	bool bPointer;
	const bool bManagedDevice = findGdkMasterDevice(p0Device, bPointer);
	if (!bManagedDevice) {
		// It's not the "main" (our) master
		return; //----------------------------------------------------------
	}
	onDeviceChanged(bPointer);
}
void GtkBackend::gdkDeviceRemoved(
GdkDeviceManager *
#ifndef NDEBUG
p0GdkDeviceManager
#endif //NDEBUG
, GdkDevice* p0Device) noexcept
{
	assert(p0GdkDeviceManager == m_refGdkDeviceManager->gobj());
	GdkDeviceType eDeviceType = gdk_device_get_device_type(p0Device);
	if (eDeviceType == GDK_DEVICE_TYPE_FLOATING) {
		// This manager has only master devices, doesn't care about floating
		return; //--------------------------------------------------------------
	}
	bool bPointer;
	if (eDeviceType == GDK_DEVICE_TYPE_SLAVE) {
//std::cout << "MasGtkDeviceManager::gdkDeviceRemoved  SLAVE REMOVED" << std::endl;
		// get the master
		GdkDevice* p0MasterDevice = gdk_device_get_associated_device(p0Device);
		// find it
		const bool bManagedDevice = findGdkMasterDevice(p0MasterDevice, bPointer);
		if (!bManagedDevice) {
			// It isn't a slave of "our" master
			return; //----------------------------------------------------------
		}
		// Removing one of its slaves has changed the master
		onDeviceChanged(bPointer);
	} else {
		// removing a master device
		assert(eDeviceType == GDK_DEVICE_TYPE_MASTER);
		const bool bManagedDevice = findGdkMasterDevice(p0Device, bPointer);
		if (!bManagedDevice) {
			// It's another master, we don't care
			return; //----------------------------------------------------------
		}
		onDevicePairRemoved();
		// One of "our" masters was removed, add the alternative master pair (keyboard+pointer)
		// there always should be one
		addDevices(p0Device);
	}
}
bool GtkBackend::findGdkMasterDevice(GdkDevice* p0MasterDevice, bool& bPointer) const noexcept
{
	if (m_refGdkPointer && (m_refGdkPointer->gobj() == p0MasterDevice)) {
		bPointer = true;
		return true;
	} else if (m_refGdkKeyboard && (m_refGdkKeyboard->gobj() == p0MasterDevice)) {
		bPointer = false;
		return true;
	}
	return false;
}
bool GtkBackend::isCompatible(const shared_ptr<GtkAccessor>& refGtkAccessor) const noexcept
{
	Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
	assert(p0GtkmmWindow != nullptr);
	Glib::RefPtr<Gdk::Display> refDisplay = p0GtkmmWindow->get_display();
	assert(refDisplay);
	Glib::RefPtr<Gdk::DeviceManager> refGdkDeviceManager = refDisplay->get_device_manager();
	assert(refGdkDeviceManager);
	return (m_refGdkDeviceManager == refGdkDeviceManager);
}
void GtkBackend::onDeviceChanged(bool bPointer) noexcept
{
	m_p0Owner->onDeviceChanged(bPointer);
}
void GtkBackend::onDevicePairAdded() noexcept
{
	m_p0Owner->onDevicePairAdded();
}
void GtkBackend::onDevicePairRemoved() noexcept
{
	m_p0Owner->onDevicePairRemoved();
}

} // namespace Mas
} // namespace Private

} // namespace stmi
