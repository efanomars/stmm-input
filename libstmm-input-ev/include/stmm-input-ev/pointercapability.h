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
 * File:   pointercapability.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef _STMI_POINTER_CAPABILITY_H_
#define _STMI_POINTER_CAPABILITY_H_

#include <stmm-input/capability.h>

namespace stmi
{

/** The pointer capability of a device.
 * Devices with this capability can generate pointer and scroll events.
 */
class PointerCapability : public Capability
{
public:
	//
	static const char* const s_sClassId;
	static const Capability::Class& getClass() { return s_oInstall.getCapabilityClass(); }
protected:
	PointerCapability() : Capability(s_oInstall.getCapabilityClass()) {}
private:
	static RegisterClass<PointerCapability> s_oInstall;
};

} // namespace stmi

#endif	/* _STMI_POINTER_CAPABILITY_H_ */
