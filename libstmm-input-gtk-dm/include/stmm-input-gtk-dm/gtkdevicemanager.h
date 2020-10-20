/*
 * Copyright © 2016-2020  Stefano Marsili, <stemars@gmx.ch>
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

#ifndef STMI_GTK_DEVICE_MANAGER_H
#define STMI_GTK_DEVICE_MANAGER_H

#include <stmm-input/event.h>

#include <stmm-input-gtk/keyrepeatmode.h>

#include <stmm-input-base/parentdevicemanager.h>

#include <gtkmm.h>

#include <memory>
#include <vector>
#include <string>
#include <utility>


namespace stmi { class GdkKeyConverter; }

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

/** Device manager that attaches to Gtk's main event loop.
 * The created device manager needs stmi::GtkAccessor-wrapped Gtk::Window
 * references in order to receive events, which are only sent to the currently
 * active window (provided it was added with DeviceManager::addAccessor()).
 *
 * Currently the following devices are supported:
 *    - master keyboard, pointer and touch.
 *    - joysticks mapped to files "/dev/input/jsN".
 *    - any device loaded by the plugin device manager.
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
class GtkDeviceManager : public ParentDeviceManager
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
	 * If m_refGdkConverter is null, the stmi::GdkKeyConverterEvDev converter is used.
	 *
	 * If the gdk display m_refDisplay is null, the system default is used.
	 *
	 * If m_bEnablePlugins is `true` then all plugin names in m_aEnDisablePlugins are enabled, all others disabled,
	 * if `false` then all plugin names found by this instance are enabled except those in m_aEnDisablePlugins.
	 *
	 * Example: to enable all plugins (to be understood as "don't disable any plugin name")
	 *
	 *     m_bEnablePlugins = false,  m_aEnDisablePlugins = {}   (which is the default)
	 *
	 * m_aGroups contains the groups the plugin must belong to in order to be loaded.
	 * The groups are listed in the plugin .dlp file (see PluginsDeviceManager::PluginsDeviceManager)
	 *
	 * Note: the plugins device manager might have been excluded for the build
	 * (libstmm-input-dl is not present) in which case these fields are unused.
	 */
	struct Init
	{
		bool m_bEnableEventClasses = false; /**< Whether to enable or disable all (but) aEnDisableEventClasses. Default: false. */
		std::vector<Event::Class> m_aEnDisableEventClasses; /**< The event classes to be enabled or disabled according to m_bEnableEventClasses. */
		KeyRepeat::MODE m_eKeyRepeatMode = KeyRepeat::MODE_NOT_SET; /**< How key (press) repeat is handled. If KeyRepeat::MODE_NOT_SET, default is used. Default: KeyRepeat::MODE_NOT_SET. */
		shared_ptr<GdkKeyConverter> m_refGdkConverter; /**< The gdk key event to hardware key converter. If null default is used. */
		Glib::RefPtr<Gdk::Display> m_refDisplay; /**< The gdk display. If null default is used. */
		std::string m_sAdditionalPluginPath; /**< Additional directory where to look for plugin (.dlp) files. Can be empty. */
		bool m_bAdditionalPluginPathOnly = false; /**< Only additional directory should be used. Default: false. */
		bool m_bEnablePlugins = false; /**< Whether to enable or disable all (but) m_aEnDisablePlugins. Default: false. */
		std::vector<std::string> m_aEnDisablePlugins; /**< The plugins to be enabled or disabled according to m_bEnablePlugins. */
		std::vector<std::string> m_aGroups; /**< The group names the plug-ins must belong to in order to be loaded. */
		std::string m_sAppName; /**< The application name. Can be empty. */
		bool m_bVerbose = false; /**< If true more output can be expected. Default: false. */
	};
	/** Creates a device manager.
	 * Before this call Gtk has to be already initialized (ex. with Gtk::Application::create()).
	 * @param oInit The initialization data.
	 * @return The created device manager and an empty string or null and the error string if creation failed.
	 */
	static std::pair<shared_ptr<GtkDeviceManager>, std::string> create(const Init& oInit) noexcept;
	/** Creates a device manager with default initialization.
	 * @see GtkDeviceManager::Init default values.
	 */
	static std::pair<shared_ptr<GtkDeviceManager>, std::string> create() noexcept
	{
		return create(Init{});
	}
};

} // namespace stmi

#endif /* STMI_GTK_DEVICE_MANAGER_H */
