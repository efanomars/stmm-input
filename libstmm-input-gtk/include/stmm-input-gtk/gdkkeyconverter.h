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
 * File:   gdkkeyconverter.h
 */

#ifndef STMI_GDK_KEY_CONVERTER_H
#define STMI_GDK_KEY_CONVERTER_H

#include <stmm-input/hardwarekey.h>

#include <memory>

#include <gdk/gdk.h>
#include <glib.h>

namespace stmi
{

using std::shared_ptr;

/** Gdk key event (keycode) to hardware key conversion class.
 */
class GdkKeyConverter
{
public:
	virtual ~GdkKeyConverter() noexcept = default;
	/** Converts a Gdk keycode to a hardware key.
	 * @param nGdkKeycode The Gdk keycode.
	 * @param eHardwareKey The resulting hardware key.
	 * @return Whether the conversion was successful.
	 */
	virtual bool convertKeyCodeToHardwareKey(guint16 nGdkKeycode, HARDWARE_KEY& eHardwareKey) const noexcept = 0;
	/** Converts a GdkEventKey to a hardware key.
	 * If the keycode isn't enough to determine the hardware key, the whole
	 * key event (if available) can be used.
	 * @param p0GdkEvent The event to convert. Cannot be null.
	 * @param eHardwareKey The resulting hardware key.
	 * @return Whether the conversion was successful.
	 */
	virtual bool convertEventKeyToHardwareKey(GdkEventKey const* p0GdkEvent, HARDWARE_KEY& eHardwareKey) const noexcept = 0;
	/** Get the key converter instance (creating the default if not set).
	 * If the converter was not set with setConverter() the default one is created.
	 *
	 * This global singleton converter can be used as an implicit parameter
	 * to device managers that are loaded as plugins by PluginsDeviceManager
	 * (libstmm-input-dl).
	 *
	 * After this function is called, the global singleton converter can no longer
	 * be changed.
	 * @return The converter. Cannot be null.
	 */
	static const shared_ptr<GdkKeyConverter>& getConverter() noexcept;
	/** Whether the singleton converter was already set.
	 * @return Whether the converter was yet set.
	 */
	static bool isConverterSet() noexcept;
	/** Set the key converter instance.
	 * If the converter was already set by calling getConverter() or this very function,
	 * the setting fails and false is returned. This means that this function should be
	 * called very early in your program before getConverter() has a chance to be
	 * called (that is the plugins get loaded).
	 * @param refConverter Cannot be null.
	 * @return Whether the converter was set.
	 */
	static bool setConverter(const shared_ptr<GdkKeyConverter>& refConverter) noexcept;
private:
	static shared_ptr<GdkKeyConverter> s_refKeyConverter;
};

} // namespace stmi

#endif /* STMI_GDK_KEY_CONVERTER_H */
