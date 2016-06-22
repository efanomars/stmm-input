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
 * File:   flogtkdevicemanager.h
 */

#ifndef _STMI_FLO_GTK_DEVICE_MANAGER_H_
#define _STMI_FLO_GTK_DEVICE_MANAGER_H_

#include "gtkaccessor.h"
#include "gdkkeyconverter.h"
#include "keyrepeatmode.h"

#include <stmm-input-base/stddevice.h>
#include <stmm-input-base/stddevicemanager.h>

#include <stmm-input-ev/keycapability.h>
#include <stmm-input-ev/keyevent.h>
#include <stmm-input-ev/devicemgmtcapability.h>
#include <stmm-input-ev/devicemgmtevent.h>

#include <gtkmm.h>

#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>

#include <unordered_map>
#include <vector>
#include <string>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace Private
{
namespace Flo
{
	class GtkWindowData;
	class GtkXKeyboardDevice;
	class XIEventSource;
	class FloGtkListenerExtraData;
	void gdkDeviceManagerCallbackAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
	void gdkDeviceManagerCallbackChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
	void gdkDeviceManagerCallbackRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
}
}

/** Handles XInput2 floating devices (both keyboards and pointers) that have keys.
 * An instance only handles the devices of one Gdk::Display.
 * 
 * An event (of type stmi::KeyEvent) sent to listeners by this device manager is tied
 * to a Gtk::Window, which has to be added with FloGtkDeviceManager::addAccessor()
 * wrapped in a stmi::GtkAccessor.
 * Events are only sent to the currently active window. When the active window changes
 * cancel events are sent to the old active window for each pressed key.
 */
class FloGtkDeviceManager : public StdDeviceManager, public DeviceMgmtCapability, public sigc::trackable
{
public:
	/** Creates an instance of this class.
 	 * If the passed gdk display is null the system default is used.
	 * 
	 * If bEnableEventClasses is `true` then all event classes in aEnDisableEventClass are enabled, all others disabled,
	 * if `false` then all event classes supported by this instance are enabled except those in aEnDisableEventClass.
	 * FloGtkDeviceManager doesn't allow disabling event classes once constructed, only enabling.
	 *
	 * Example: To enable all the event classes supported by this instance (currently KeyEvent and DeviceMgmtEvent) pass
	 *
	 *     bEnableEventClasses = false,  aEnDisableEventClass = {}
	 *
	 * @param bEnableEventClasses Whether to enable or disable all but aEnDisableEventType.
	 * @param aEnDisableEventClass The event classes to be enabled or disabled according to bEnableEventClasses.
	 * @param eKeyRepeatMode Key repeat translation type.
	 * @param refGdkConverter The keycode to hardware key converter. Cannot be null.
	 * @param refDisplay The display used to initialize XInput2. Can be null.
	 * @return The created instance.
	 * @throws std::runtime_error is if an error occurs (example: XInput2 is not supported).
	 */
	static shared_ptr<FloGtkDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
												, KEY_REPEAT_MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter
												, const Glib::RefPtr<Gdk::Display>& refDisplay);
	virtual ~FloGtkDeviceManager();

	void enableEventClass(const Event::Class& oEventClass) override;

	/** Adds a stmi::GtkAccessor-wrapped Gtk::Window from which events should be received. 
	 * An instance of this class needs a stmi::GtkAccessor for each active Gtk::Window
	 * listeners want to receive events from.
	 * If the type of parameter refAccessor is not stmi::GtkAccessor, `false` is returned.
	 * If it is and its Gtk::Window has not the same Gdk::Display as the one passed 
	 * as parameter to create the device manager (FloGtkDeviceManager::create()),
	 * `false` is returned.
	 * If the display is the same and the window isn't already added `true` is returned,
	 * `false` otherwise.
	 */
	bool addAccessor(const shared_ptr<Accessor>& refAccessor) override;
	/** Removes a stmi::GtkAccessor-wrapped Gtk::Window added with addAccessor().
	 * If the device manager has the accessor, `true` is returned, `false` otherwise.
	 * 
	 * This function doesn't delete the window itself, it just tells the device manager
	 * to stop tracking it.
	 * 
	 * Cancels are sent to listeners for each pressed key.
	 */
	bool removeAccessor(const shared_ptr<Accessor>& refAccessor) override;
	/** Tells whether a window is already tracked by the device manager.
	 * @param refAccessor The wrapper of the Gtk::Window.
	 * @return Whether the window is currently tracked by the device manager.
	 */
	bool hasAccessor(const shared_ptr<Accessor>& refAccessor) override;
	// from DeviceMgmtCapability
	shared_ptr<DeviceManager> getDeviceManager() const override;
	shared_ptr<DeviceManager> getDeviceManager() override;
protected:
	void finalizeListener(ListenerData& oListenerData) override;
private:
	FloGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
						, KEY_REPEAT_MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter);
	// throws
	void init(const Glib::RefPtr<Gdk::Display>& refDisplay);

	void initDeviceManager();
	void deinitDeviceManager();
	friend void Private::Flo::gdkDeviceManagerCallbackAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
	friend void Private::Flo::gdkDeviceManagerCallbackChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
	friend void Private::Flo::gdkDeviceManagerCallbackRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data);
	void gdkDeviceAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device);
	void gdkDeviceChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device);
	void gdkDeviceRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device);

	void adjustConnectionsAfterEnablingClass();

	void addDevices();
	shared_ptr<Private::Flo::GtkXKeyboardDevice> addFloatingDevice(GdkDevice* p0GdkDevice);
	//
	shared_ptr<Private::Flo::GtkXKeyboardDevice> createGtkXKeyboardDevice(int nXDeviceId, const Glib::ustring& sDeviceName);

	bool findWindow(Gtk::Window* p0GtkmmWindow
				, std::vector< std::pair<Gtk::Window*, shared_ptr<Private::Flo::GtkWindowData> > >::iterator& itFind);
	bool hasAccessor(const shared_ptr<Accessor>& refAccessor, bool& bValid
					, std::vector<std::pair<Gtk::Window*, shared_ptr<Private::Flo::GtkWindowData> > >::iterator& itFind);
	void removeAccessor(const std::vector<std::pair<Gtk::Window*, shared_ptr<Private::Flo::GtkWindowData> > >::iterator& itGtkData);
	void onIsActiveChanged(const shared_ptr<Private::Flo::GtkWindowData>& refWindowData);
	std::shared_ptr<Private::Flo::GtkWindowData> getGtkWindowData();

	bool initXI(std::string& sError);
	void connectDeviceToAllWindows(int32_t nXDeviceId);

	void selectAccessor(const shared_ptr<Private::Flo::GtkWindowData>& refData);
	void cancelDeviceKeys(const shared_ptr<Private::Flo::GtkXKeyboardDevice>& refGtkXKeyboard);
	void deselectAccessor();
	void focusSelectedWindow();
	void focusDevicesToXWindow(::Window nXWinId);
	void focusDeviceToXWindow(int32_t nXDeviceId, ::Window nXWinId);

	bool doXIDeviceEventCallback(XIDeviceEvent* p0XIDeviceEvent);

	void sendDeviceMgmtToListeners(const DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice);

	friend class Private::Flo::GtkWindowData;
	friend class Private::Flo::GtkXKeyboardDevice;
	friend class Private::Flo::XIEventSource;
	friend class Private::Flo::FloGtkListenerExtraData;
private:
	// The GtkAccessor (GtkWindowData::m_refAccessor) will tell 
	// when the window gets deleted. The accessor can also be removed
	// explicitely during a listener callback. In both cases the
	// shared_ptr is removed from m_aGtkWindowData and m_aXWindowData and 
	// added to m_aFreePool.
	std::vector<std::pair<Gtk::Window*, shared_ptr<Private::Flo::GtkWindowData> > > m_aGtkWindowData;
	std::vector<std::pair<::Window, shared_ptr<Private::Flo::GtkWindowData> > > m_aXWindowData;
	// The objects in the free pool might still be in use when the
	// removal of the accessor was done during a callback. This is detected
	// through the ref count of the shared_ptr.
	std::vector< std::shared_ptr<Private::Flo::GtkWindowData> > m_aFreePool;
	// The currently active accessor (window), can be null.
	std::shared_ptr<Private::Flo::GtkWindowData> m_refSelected;
	// Invariants:
	// - if m_refSelected is null, no key is pressed in any of the devices
	// - if m_refSelected is not null, all pressed keys in any of the devices
	//   were generated for the selected window.

	int32_t m_nCancelingNestedDepth;

	Glib::RefPtr<Gdk::Display> m_refGdkDisplay;
	Display* m_p0XDisplay; // shortcut from m_refGdkDisplay
	//
	gulong m_nConnectHandlerDeviceAdded;
	gulong m_nConnectHandlerDeviceChanged;
	gulong m_nConnectHandlerDeviceRemoved;

	std::unordered_map<int32_t, shared_ptr<Private::Flo::GtkXKeyboardDevice> > m_oKeyboardDevices; // Key: X device id
	Glib::RefPtr<Private::Flo::XIEventSource> m_refXIEventSource;

	KEY_REPEAT_MODE m_eKeyRepeatMode;

	const shared_ptr<GdkKeyConverter> m_refGdkConverter;
	// Fast access reference to converter
	const GdkKeyConverter& m_oConverter;
	//
	const int32_t m_nClassIdxKeyEvent;
	const int32_t m_nClassIdxDeviceMgmtEvent;
private:
	FloGtkDeviceManager() = delete;
	FloGtkDeviceManager(const FloGtkDeviceManager& oSource) = delete;
	FloGtkDeviceManager& operator=(const FloGtkDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif	/* _STMI_FLO_GTK_DEVICE_MANAGER_H_ */
