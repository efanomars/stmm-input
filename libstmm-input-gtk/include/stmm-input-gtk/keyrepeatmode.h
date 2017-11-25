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
 * File:   keyrepeatmode.h
 */

#ifndef STMI_KEY_REPEAT_MODE_H
#define STMI_KEY_REPEAT_MODE_H

namespace stmi
{

/** Key repeat translation type.
 * Key presses unanswered by a release are not allowed.
 *
 * For example the gdk event sequence for a specific key
 *
 *     Press  Press  Press  Release
 *
 * is translated in either
 *
 *     Press Release  Press Release  Press Release  (add release mode)
 *
 * or
 *
 *     Press Cancel   Press Cancel   Press Release  (add release cancel mode)
 *
 * or
 *
 *     Press                               Release  (suppress mode)
 *
 */
class KeyRepeat
{
public:
	enum MODE {
		MODE_NOT_SET = 0 /**< Mode not set, can be used to require the default to be used. */
		, MODE_SUPPRESS = 1 /**< Suppress repeated key presses. */
		, MODE_ADD_RELEASE = 2 /**< Send a release to listeners prior a repeated key press. */
		, MODE_ADD_RELEASE_CANCEL = 3 /**< Send a release cancel to listeners prior a repeated key press. */
	};
//enum KEY_REPEAT_MODE {
//	KEY_REPEAT_MODE_SUPPRESS = 1 /**< Suppress repeated key presses. */
//	, KEY_REPEAT_MODE_ADD_RELEASE = 2 /**< Send a release to listeners prior a repeated key press. */
//	, KEY_REPEAT_MODE_ADD_RELEASE_CANCEL = 3 /**< Send a release cancel to listeners prior a repeated key press. */
//};
	/** Get the global key repeat mode (or the default if not set).
	 * If the mode was not set with setMode() the default is set and returned.
	 *
	 * This global key repeat mode can be used as an implicit parameter
	 * to device managers that are loaded as plugins by PluginsDeviceManager
	 * (libstmm-input-dl).
	 *
	 * After this function is called, the global key repeat mode can no longer
	 * be changed.
	 *
	 * The default key repeat mode is MODE_SUPPRESS.
	 * @return The converter. Cannot be null.
	 */
	static MODE getMode();
	/** Whether global key repeat mode was already set.
	 * @return Whether the mode was yet set.
	 */
	static bool isModeSet();
	/** Set the global key repeat mode (creating the default if not set).
	 * If the mode was already set by calling getMode() or this very function,
	 * the setting fails and false is returned. This means that this function should be
	 * called very early in your program before getMode() has a chance to be
	 * called (that is the plugins get loaded).
	 * @param eKeyRepeatMode Cannot be null.
	 * @return Whether the converter was set.
	 */
	static bool setMode(MODE eKeyRepeatMode);
private:
	static MODE s_eMode;
};

} // namespace stmi

#endif /* STMI_KEY_REPEAT_MODE_H */
