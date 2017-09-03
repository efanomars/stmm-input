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
 * File:   touchcapability.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMI_TOUCH_CAPABILITY_H
#define STMI_TOUCH_CAPABILITY_H

#include <stmm-input/capability.h>

namespace stmi
{

/** The touch capability of a device.
 * Devices with this capability can generate touch events.
 */
class TouchCapability : public Capability
{
public:
	//
	static const char* const s_sClassId;
	static const Capability::Class& getClass() { return s_oInstall.getCapabilityClass(); }
protected:
	TouchCapability() : Capability(s_oInstall.getCapabilityClass()) {}
private:
	static RegisterClass<TouchCapability> s_oInstall;
};

} // namespace stmi

#endif /* STMI_TOUCH_CAPABILITY_H */
