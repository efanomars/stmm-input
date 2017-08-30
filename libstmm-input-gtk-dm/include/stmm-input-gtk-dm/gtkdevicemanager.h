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
 * File:   gtkdevicemanager.h
 */

#ifndef _STMI_GTK_DEVICE_MANAGER_H_
#define _STMI_GTK_DEVICE_MANAGER_H_

#include "keyrepeatmode.h"
#include "gdkkeyconverter.h"

#include <stmm-input-ev/stdparentdevicemanager.h>

#include <gtkmm.h>

#include <memory>
#include <vector>
#include <string>


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
	/** Initialization data.
	 * If m_bEnableEventClasses is `true` then all event classes in m_aEnDisableEventClasses are enabled, all others disabled,
	 * if `false` then all event classes supported by this instance are enabled except those in m_aEnDisableEventClasses.
	 *
	 * Example: To enable just events of type KeyEvent and PointerEvent set parameters
	 *
	 *     m_bEnableEventClasses = true,  m_aEnDisableEventClasses = {stmi::KeyEvent::getClass(), stmi::PointerEvent::getClass()}
	 *
	 * If `refGdkConverter` is null, the stmi::GdkKeyConverterEvDev converter is used.
	 *
	 * If the gdk display `refDisplay` is null, the system default is used.
	 *
	 * If m_bEnablePlugins is `true` then all plugin names in m_aEnDisablePlugins are enabled, all others disabled,
	 * if `false` then all plugin names found by this instance are enabled except those in m_aEnDisablePlugins.
	 *
	 * Example: to enable all plugins (to be understood as "don't disable any plugin name")
	 *
	 *     m_bEnablePlugins = false,  m_aEnDisablePlugins = {}   (which is the default)
	 *
	 * Note: the plugins device manager might have been excluded for the build
	 * (libstmm-input-dl is not present) in which case these fields are unused.
	 */
	struct Init
	{
		bool m_bEnableEventClasses = false; /**< Whether to enable or disable all (but) aEnDisableEventClasses. */
		std::vector<Event::Class> m_aEnDisableEventClasses; /**< The event classes to be enabled or disabled according to m_bEnableEventClasses. */
		KEY_REPEAT_MODE m_eKeyRepeatMode = KEY_REPEAT_MODE_SUPPRESS; /**< How key (press) repeat is handled. */
		shared_ptr<GdkKeyConverter> m_refGdkConverter; /**< The gdk key event to hardware key converter. If null default is used. */
		Glib::RefPtr<Gdk::Display> m_refDisplay; /**< The gdk display. If null default is used. */
		std::string m_sAdditionalPluginPath; /**< Additional directory where to look for plugin (.dlp) files. Can be empty. */
		bool m_bAdditionalPluginPathOnly = false; /**< Only additional directory should be used. */
		bool m_bEnablePlugins = false; /**< Whether to enable or disable all (but) m_aEnDisablePlugins. */
		std::vector<std::string> m_aEnDisablePlugins; /**< The plugins to be enabled or disabled according to m_bEnablePlugins. */
		std::string m_sAppName; /**< The application name. Can be empty. */
	};
	/** Creates a device manager.
	 * Before this call Gtk has to be already initialized (ex. with Gtk::Application::create()).
	 * @param oInit The initialization data.
	 * @return The created device manager or null if creation failed.
	 */
	static shared_ptr<GtkDeviceManager> create(const Init& oInit);
	/** Creates a device manager with default initialization.
	 * @see GtkDeviceManager::Init default values.
	 */
	static shared_ptr<GtkDeviceManager> create()
	{
		return create(Init{});
	}
};

} // namespace stmi

#endif /* _STMI_GTK_DEVICE_MANAGER_H_ */
