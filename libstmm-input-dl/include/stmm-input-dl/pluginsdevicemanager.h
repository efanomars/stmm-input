/*
 * Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   pluginsdevicemanager.h
 */

#ifndef STMI_PLUGINS_DEVICE_MANAGER_H
#define STMI_PLUGINS_DEVICE_MANAGER_H

#include <stmm-input-base/parentdevicemanager.h>

#include <stmm-input/capability.h>
#include <stmm-input/event.h>

#include <vector>
#include <string>
#include <memory>
#include <utility>

namespace stmi { class ChildDeviceManager; }


namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

/** Dynamic plug-in loader device manager class.
 * The contained device managers (the children) must be subclasses of ChildDeviceManager.
 * A child device manager must be defined within its own shared library, compiled with
 * the "-Wl,-E" flag (g++ only!) that implements the (extern "C") interface defined in
 * <stmm-input-dl/plugininterface.h>:
 *
 *     shared_ptr<ChildDeviceManager> createPlugin(const std::string sAppName, bool bEnableEventClasses
 *                                   , const std::vector<Event::Class>& aEnDisableEventClasses);
 *
 * The plugins (.dlp) are found by an instance of this class by searching the plugin paths.
 * The first to be searched is the optional one passed to the create() function.
 * The second is the local system path (ex. /usr/local/etc/stmm-input-dl).
 * The third (if library installed in /usr/lib)) is the system path (ex. /etc/stmm-input-dl).
 *
 * A plugin file name (ex. 90myplugin.dlp) is composed by the rank within the path
 * (the first two digits, here 90), the suffix (.dlp) and the plugin name (myplugin).
 * If two or more files define the same plugin name only the one with smallest
 * rank number is loaded (70foo.dlp has higher rank than 90foo.dlp).
 *
 * The content of a plugin file has the following format:
 *
 *     # Lines that start with #=#+#=# separate blocks.
 *     # Other lines that start with # are comments. Comments are ignored.
 *     # The first block of (non comment) lines are the library paths. These strings
 *     # are passed to the dlopen() function. If one succeeds to load the plugin,
 *     # an instance is created by calling the createPlugin function (see above).
 *     libmyplugin.so.0
 *     libmyplugin.so
 *     /usr/local/lib/libmyplugin.so.0
 *     /usr/lib/libmyplugin.so.0
 *     #=#+#=# This line marks the next block ...
 *     # ... which contains the plugins names that the to be loaded plugin depends on.
 *     # This causes them to be loaded with dlopen before this one.
 *     # These names must be present as .dlp files on any of the plugin paths,
 *     # otherwise they are ignored. The loading of myplugin.dlp library is still
 *     # attempted anyway. Note that the dependency doesn't include the ".dlp".
 *     my-other-plugin
 *     #=#+#=# The next block is a description of the plugin.
 *     myplugin creates devices that do this and that.
 *     #=#+#=# The next block contains a list of groups the plugin belongs to.
 *     # Groups can be used by PluginsDeviceManager to selectively load only
 *     # a subset of the enabled plugins. Each line contains a group name.
 *     # A group name is made of following characters (regex notation):
 *     #     [a-zA-Z][a-zA-Z0-9]*
 *     gtk
 *
 * Plugins can be disabled without removing their .dlp file. In each plugin path
 * and additionally in the local user path (ex. ~/.local/etc/stmm-input-dl)
 * the optional file "disabled-dlps.txt" defines the disabled plugin names.
 * The plugin names (without the .dlp extension) are listed one per line.
 * Example: if you want to disable the above plugin
 *
 *     # Comments are ignored here too.
 *     # This is "disabled-dlps.txt" in directory ~/.local/share/stmm-input-dl
 *     myplugin
 *     # If a plugin name doesn't exist as .dlp file, it is just skipped.
 *     some-other-plugin
 */
class PluginsDeviceManager : public ParentDeviceManager
{
public:
	virtual ~PluginsDeviceManager() noexcept;
	/** Initialization data.
	 * If m_bEnableEventClasses is `true` then all event classes in m_aEnDisableEventClasses are enabled, all others disabled,
	 * if `false` then all event classes supported by this instance are enabled except those in m_aEnDisableEventClasses.
	 *
	 * Example: To enable just events of type KeyEvent and PointerEvent set parameters
	 *
	 *     m_bEnableEventClasses = true,  m_aEnDisableEventClasses = {stmi::KeyEvent::getClass(), stmi::PointerEvent::getClass()}
	 *
	 * The m_bEnableEventClasses and m_aEnDisableEventClasses parameters to this function are
	 * also passed to the plug-in device managers, which are stored as children of this class.
	 * The application name (m_sAppName) is passed to the child plug ins.
	 *
	 * If m_bEnablePlugins is `true` then all plugin names in m_aEnDisablePlugins are enabled, all others disabled,
	 * if `false` then all plugin names found by this instance are enabled except those in m_aEnDisablePlugins.
	 *
	 * Example: to enable all plugins (to be understood as "don't disable any plugin name")
	 *
	 *     m_bEnablePlugins = false,  m_aEnDisablePlugins = {}   (which is the default)
	 *
	 * The plug-in names in m_aEnDisablePlugins don't include the first two digits and the".dlp" extension
	 * of the corresponding plug-in file.
	 *
	 * If m_aGroups is empty, the plugin's groups are ignored (all are loaded).
	 * Otherwise, in order to be loaded, the plugin either must have no group declared or
	 * at least one of its groups must be in m_aGroups.
	 */
	struct Init
	{
		bool m_bEnableEventClasses = false; /**< Whether to enable or disable all (but) aEnDisableEventClasses. */
		std::vector<Event::Class> m_aEnDisableEventClasses; /**< The event classes to be enabled or disabled according to m_bEnableEventClasses. */
		std::string m_sAdditionalPluginPath; /**< Priority path where to look for plug-ins. Can be empty. */
		bool m_bAdditionalPluginPathOnly = false; /**< Only m_sAdditionalPluginPath should be used. */
		bool m_bEnablePlugins = false; /**< Whether to enable or disable all (but) aEnDisablePlugins. */
		std::vector<std::string> m_aEnDisablePlugins; /**< The plug-in names to be enabled or disabled according to bEnablePlugins. */
		std::vector<std::string> m_aGroups; /**< The group names used to select the plug-ins. */
		std::string m_sAppName; /**< The application name. Can be empty. */
		bool m_bVerbose = false; /**< If true more output can be expected. Default: false. */
	};
	/** Create an instance.
	 * @param oInit The initialization data.
	 * @return The created device manager and an empty string or null and an error if creation failed or null and an empty string if no plugins loaded.
	 */
	static std::pair<shared_ptr<PluginsDeviceManager>, std::string> create(Init&& oInit) noexcept;

protected:
	void init(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager) noexcept
	{
		ParentDeviceManager::init(aChildDeviceManager);
	}
private:
	friend shared_ptr<PluginsDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses) noexcept;
	PluginsDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses
						, std::vector<void *>&& aPluginHandles) noexcept;

private:
	//
	bool m_bEnableEventClasses;
	std::vector<Event::Class> m_aEnDisableEventClass;
	std::vector<void *> m_aPluginHandles;
private:
	PluginsDeviceManager() = delete;
	PluginsDeviceManager(const PluginsDeviceManager& oSource) = delete;
	PluginsDeviceManager& operator=(const PluginsDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_PLUGINS_DEVICE_MANAGER_H */

