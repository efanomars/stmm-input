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
 * File:   gtkdevicemanager.h
 */

#ifndef _STMI_GTK_DEVICE_MANAGER_H_
#define _STMI_GTK_DEVICE_MANAGER_H_

#include "keyrepeatmode.h"
#include "gdkkeyconverter.h"

#include <stmm-input-ev/stdparentdevicemanager.h>

#include <gtkmm.h>

#include <memory>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

/** Device manager that attaches to Gtk's main event loop.
 * The created device manager need stmi::GtkAccessor-wrapped Gtk::Window
 * references in order to receive events, which are only sent to the currently
 * active window (provided it was added with DeviceManager::addAccessor()).
 * 
 * Currently the following devices are supported:
 *    - master keyboard, pointer and touch.
 *    - floating (xinput) devices with keys.
 *    - joysticks mapped to files "/dev/input/jsN".
 *
 * The supported events types are:
 *    - stmi::KeyEvent
 *    - stmi::PointerEvent and stmi::PointerScrollEvent
 *    - stmi::TouchEvent
 *    - stmi::JoystickHatEvent, stmi::JoystickButtonEvent and JoystickAxisEvent
 *    - stmi::DeviceMgmtEvent
 * 
 * When there is a change of active window, all the keys, buttons, touches
 * are canceled. 
 * Example: for each pressed keys a KeyEvent of type KeyEvent::KEY_RELEASE_CANCEL
 * is sent to the listeners.
 */
class GtkDeviceManager : public StdParentDeviceManager
{
public:
	/** Creates a device manager.
	 * Before this call Gtk has to be already initialized (ex. with Gtk::Application::create()).
	 *
	 * If bEnableEventClasses is `true` then all event classes in aEnDisableEventClass are enabled, all others disabled,
	 * if `false` then all event classes supported by this instance are enabled except those in aEnDisableEventClass.
	 *
	 * Example: To enable just events of type KeyEvent and PointerEvent set parameters
	 *
	 *     bEnableEventClasses = true,  aEnDisableEventClass = {stmi::KeyEvent::getClass(), stmi::PointerEvent::getClass()}
	 *
	 * If `refGdkConverter` is null, the stmi::GdkKeyConverterEvDev converter is used.
	 *
	 * If the gdk display `refDisplay` is null, the system default is used.
	 *
	 * @param bEnableEventClasses Whether to enable or disable all (but) aEnDisableEventType.
	 * @param aEnDisableEventClass The event classes to be enabled or disabled according to bEnableEventClasses.
	 * @param eKeyRepeatMode How key repeat is handled.
	 * @param refGdkConverter refGdkConverter The gdk key event to hardware key converter. Cannot be null.
	 * @param refDisplay The gdk display. Can be null.
	 * @return The created device manager or null if creation failed.
	 */
	static shared_ptr<GtkDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
												, KEY_REPEAT_MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter
												, const Glib::RefPtr<Gdk::Display>& refDisplay);
	/** Creates a device manager.
	 * The device manager is created for the default display.
	 * @see create(bool, const std::vector<Event::Class>&, KEY_REPEAT_MODE, const Glib::RefPtr<Gdk::Display>&)
	 */
	static shared_ptr<GtkDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
												, KEY_REPEAT_MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter);
	/** Creates a device manager.
	 * The device manager is created for the default key converter and default display.
	 * @see create(bool, const std::vector<Event::Class>&, KEY_REPEAT_MODE, const Glib::RefPtr<Gdk::Display>&)
	 */
	static shared_ptr<GtkDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
											, KEY_REPEAT_MODE eKeyRepeatMode);
	/** Creates a device manager.
	 * The device manager is created for the default display.
	 * Repeated key presses without release are suppressed.
	 * @see create(bool, const std::vector<Event::Class>&, KEY_REPEAT_MODE, const Glib::RefPtr<Gdk::Display>&)
	 */
	static shared_ptr<GtkDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass);
	/** Creates a device manager.
	 * The device manager is created for the default display.
	 * Repeated key presses without release are suppressed.
	 * All event classes are enabled.
	 * @see create(bool, const std::vector<Event::Class>&, KEY_REPEAT_MODE, const Glib::RefPtr<Gdk::Display>&)
	 */
	static shared_ptr<GtkDeviceManager> create();
};

} // namespace stmi

#endif	/* _STMI_GTK_DEVICE_MANAGER_H_ */
