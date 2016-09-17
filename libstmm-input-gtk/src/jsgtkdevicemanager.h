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
 * File:   jsgtkdevicemanager.h
 */

#ifndef _STMI_JS_GTK_DEVICE_MANAGER_H_
#define _STMI_JS_GTK_DEVICE_MANAGER_H_

#include "gtkaccessor.h"
#include "jsdevicefiles.h"

#include <stmm-input-base/stddevicemanager.h>
#include <stmm-input-base/stddevice.h>
#include <stmm-input-ev/devicemgmtevent.h>
#include <stmm-input-ev/devicemgmtcapability.h>
#include <stmm-input-ev/joystickcapability.h>
#include <stmm-input-ev/joystickevent.h>

#include <gtkmm.h>

#include <unordered_map>
#include <vector>
#include <string>


namespace stmi
{

namespace Private
{
namespace Js
{
	class GtkBackend;
	class JoystickDevice;
	class GtkWindowData;
	class GtkWindowDataFactory;
	class JsGtkListenerExtraData;
} // namespace Js
} // namespace Private

/** Handles joysticks according to the <linux/joystick.h> interface.
 * An event (of type stmi::JoystickButtonEvent, JoystickHatEvent or JoystickAxisEvent)
 * sent to listeners by this device manager is tied to a Gtk::Window, which has 
 * to be added with JsGtkDeviceManager::addAccessor() wrapped in a stmi::GtkAccessor.
 * Events are only sent to the currently active window. When the active window changes
 * cancel events are sent to the old active window for each pressed buttons and
 * not centered hat.
 */
class JsGtkDeviceManager : public StdDeviceManager , public DeviceMgmtCapability, public sigc::trackable
{
public:
	/** Creates an instance this class.
	 * If no device file is given as parameter the default files are used,
	 * "/dev/input/js0", "/dev/input/js1", ...
	 *
	 * If bEnableEventClasses is `true` then all event classes in aEnDisableEventClass are enabled, all others disabled,
	 * if `false` then all event classes supported by this instance are enabled except those in aEnDisableEventClass.
	 * JsGtkDeviceManager doesn't allow disabling event classes once constructed, only enabling.
	 *
	 * Example: To enable all the event classes supported by this instance (currently JoystickButtonEvent,
	 * JoystickHatEvent, JoystickAxisEvent and DeviceMgmtEvent) pass
	 *
	 *     bEnableEventClasses = false,  aEnDisableEventClass = {}
	 *
	 * @param bEnableEventClasses Whether to enable or disable all but aEnDisableEventType.
	 * @param aEnDisableEventClass The event classes to be enabled or disabled according to bEnableEventClasses.
	 * @param oDeviceFiles The device files compatible with the <linux/joystick.h> interface.
	 * @return The created instance.
	 */
	static shared_ptr<JsGtkDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
												, const JsDeviceFiles& oDeviceFiles);
	virtual ~JsGtkDeviceManager();

	void enableEventClass(const Event::Class& oEventClass) override;

	/** Adds a stmi::GtkAccessor-wrapped Gtk::Window from which events should be received. 
	 * An instance of this class needs a stmi::GtkAccessor for each active Gtk::Window
	 * listeners want to receive events from.
	 * If the type of parameter refAccessor is not stmi::GtkAccessor, `false` is returned.
	 * If it is and the window isn't already added `true` is returned, `false` otherwise.
	 */
	bool addAccessor(const shared_ptr<Accessor>& refAccessor) override;
	/** Removes a stmi::GtkAccessor-wrapped Gtk::Window added with addAccessor().
	 * If the device manager has the accessor, `true` is returned, `false` otherwise.
	 * 
	 * This function doesn't delete the window itself, it just tells the device manager
	 * to stop tracking it.
	 * 
	 * Cancels are sent to listeners for each pressed button and hat.
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
	JsGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass);
	void init(std::unique_ptr<Private::Js::GtkWindowDataFactory>& refFactory
								, std::unique_ptr<Private::Js::GtkBackend>& refBackend);
private:
	friend class Private::Js::GtkBackend;
	const shared_ptr<Private::Js::JoystickDevice>& onDeviceAdded(const std::string& sName, const std::vector<int32_t>& aButtonCode
																, int32_t nTotHats, const std::vector<int32_t>& aAxisCode);
	void onDeviceRemoved(int32_t nJoystickId);

	void sendDeviceMgmtToListeners(const DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice);

	bool findWindow(Gtk::Window* p0GtkmmWindow
					, std::vector< std::pair<Gtk::Window*, shared_ptr<Private::Js::GtkWindowData> > >::iterator& itFind);
	bool hasAccessor(const shared_ptr<Accessor>& refAccessor, bool& bValid
					, std::vector< std::pair<Gtk::Window*, shared_ptr<Private::Js::GtkWindowData> > >::iterator& itFind);
	void removeAccessor(const std::vector< std::pair<Gtk::Window*, shared_ptr<Private::Js::GtkWindowData> > >::iterator& itGtkData);
	void cancelDeviceButtonsAndHats(const shared_ptr<Private::Js::JoystickDevice>& refJoystickDevice);
	void selectAccessor(const shared_ptr<Private::Js::GtkWindowData>& refData);
	void deselectAccessor();
	void onIsActiveChanged(const shared_ptr<Private::Js::GtkWindowData>& refWindowData);
	
	friend class Private::Js::GtkWindowData;
	friend class Private::Js::JoystickDevice;
	friend class Private::Js::JsGtkListenerExtraData;
private:
	std::unique_ptr<Private::Js::GtkWindowDataFactory> m_refFactory;
	std::unique_ptr<Private::Js::GtkBackend> m_refBackend;
	// The GtkAccessor (GtkWindowData::m_refAccessor) will tell 
	// when the window gets deleted. The accessor can also be removed
	// explicitely during a listener callback.
	std::vector<std::pair<Gtk::Window*, shared_ptr<Private::Js::GtkWindowData> > > m_aGtkWindowData;
	// The currently active accessor (window), can be null.
	std::shared_ptr<Private::Js::GtkWindowData> m_refSelected;
	// Invariants:
	// - if m_refSelected is null, no button or hat is pressed in any of the joystick devices
	// - if m_refSelected is not null, all pressed buttons and hats were generated
	//   for the selected window.

	int32_t m_nCancelingNestedDepth;
	//
	std::vector< shared_ptr<Private::Js::JoystickDevice> > m_aJoysticks;
	//
	uint64_t m_nHatEventTypeEnabledTimeStamp;
	//
	const int32_t m_nClassIdxJoystickButtonEvent;
	const int32_t m_nClassIdxJoystickHatEvent;
	const int32_t m_nClassIdxJoystickAxisEvent;
	const int32_t m_nClassIdxDeviceMgmtEvent;
	//
private:
	JsGtkDeviceManager(const JsGtkDeviceManager& oSource) = delete;
	JsGtkDeviceManager& operator=(const JsGtkDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif	/* _STMI_JS_GTK_DEVICE_MANAGER_H_ */

