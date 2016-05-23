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
 * File:   gdkkeyconverter.h
 */

#ifndef _STMI_GDK_KEY_CONVERTER_H_
#define _STMI_GDK_KEY_CONVERTER_H_

#include <cassert>
#include <iostream>
#include <memory>

#include <gdk/gdk.h>

#include <stmm-input/event.h>

namespace stmi
{

/** Gdk key event (keycode) to hardware key conversion class.
 */
class GdkKeyConverter
{
public:
	virtual ~GdkKeyConverter() = default;
	/** Converts a Gdk keycode to a hardware key.
	 * @param nGdkKeycode The Gdk keycode.
	 * @param eHardwareKey The resulting hardware key.
	 * @return Whether the conversion was successful.
	 */
	virtual bool convertKeyCodeToHardwareKey(guint16 nGdkKeycode, HARDWARE_KEY& eHardwareKey) const = 0;
	/** Converts a GdkEventKey to a hardware key.
	 * If the keycode isn't enough to determine the hardware key, the whole
	 * key event (if available) can be used.
	 * @param p0GdkEvent The event to convert. Cannot be null.
	 * @param eHardwareKey The resulting hardware key.
	 * @return Whether the conversion was successful.
	 */
	virtual bool convertEventKeyToHardwareKey(GdkEventKey const* p0GdkEvent, HARDWARE_KEY& eHardwareKey) const = 0;
};

} // namespace stmi

#endif	/* _STMI_GDK_KEY_CONVERTER_H_ */
