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
 * File:   gdkkeyconverterevdev.h
 */

#ifndef STMI_GDK_KEY_CONVERTER_EVDEV_H
#define STMI_GDK_KEY_CONVERTER_EVDEV_H

#include "gdkkeyconverter.h"

#include <stmm-input/hardwarekey.h>

#include <gdk/gdk.h>
#include <glib.h>

#include <memory>

namespace stmi
{

using std::shared_ptr;

/** Converts from evdev gdk keycode to hardware key (`linux/input.h`).
 * Evdev formula: `eHardwareKey = nGdkKeycode - 8`.
 */
class GdkKeyConverterEvDev final : public GdkKeyConverter
{
public:
	bool convertKeyCodeToHardwareKey(guint16 nGdkKeycode, HARDWARE_KEY& eHardwareKey) const noexcept override;
	bool convertEventKeyToHardwareKey(GdkEventKey const* p0GdkEvent, HARDWARE_KEY& eHardwareKey) const noexcept override;

	/** Returns the singleton evdev converter.
	 * @return Singleton converter instance. Cannot be null.
	 */
	static const shared_ptr<GdkKeyConverterEvDev>& getConverter() noexcept;
private:
	// Constructor only accessible to getConverter()
	GdkKeyConverterEvDev() noexcept {};
};

} // namespace stmi

#endif /* STMI_GDK_KEY_CONVERTER_EVDEV_H */
