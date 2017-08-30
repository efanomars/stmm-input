/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   tst88capability.h
 */

#ifndef _STMI_TST_88_CAPABILITY_H_
#define _STMI_TST_88_CAPABILITY_H_

#include <stmm-input/capability.h>


namespace stmi
{

namespace test
{

class Tst88Capability : public Capability
{
public:
	virtual int32_t getData() const = 0;
	//
	static const char* const s_sClassId;
	static const Capability::Class& getClass() { return s_oInstall.getCapabilityClass(); }
protected:
	Tst88Capability() : Capability(s_oInstall.getCapabilityClass()) {}
private:
	static RegisterClass<Tst88Capability> s_oInstall;
};

} // namespace test

} // namespace stmi

#endif /* _STMI_TST_88_CAPABILITY_H_ */
