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
 * File:   keycapability.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMI_KEY_CAPABILITY_H
#define STMI_KEY_CAPABILITY_H

#include <stmm-input/capability.h>

namespace stmi
{

/** The keys capability of a device.
 * Devices with this capability can generate keyboard events.
 */
class KeyCapability : public Capability
{
public:
	/** Tells whether the keys supported by the device are enough to consider
	 * the device a keyboard.
	 */
	// TODO is this application (game?) specific?
	virtual bool isKeyboard() const = 0;
	//
	static const char* const s_sClassId;
	static const Capability::Class& getClass() { return s_oInstall.getCapabilityClass(); }
protected:
	KeyCapability() : Capability(s_oInstall.getCapabilityClass()) {}
private:
	static RegisterClass<KeyCapability> s_oInstall;
};

} // namespace stmi

#endif /* STMI_KEY_CAPABILITY_H */

