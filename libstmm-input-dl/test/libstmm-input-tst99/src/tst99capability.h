/*
 * Copyright © 2017-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   tst99capability.h
 */

#ifndef STMI_TST_99_CAPABILITY_H
#define STMI_TST_99_CAPABILITY_H

#include <stmm-input/capability.h>


namespace stmi
{

class Tst99Capability : public Capability
{
public:
	virtual int32_t getData() const noexcept = 0;
	//
	static const char* const s_sClassId;
	static const Capability::Class& getClass() noexcept { return s_oInstall.getCapabilityClass(); }
protected:
	Tst99Capability() noexcept : Capability(s_oInstall.getCapabilityClass()) {}
private:
	static RegisterClass<Tst99Capability> s_oInstall;
};

} // namespace stmi

#endif /* STMI_TST_99_CAPABILITY_H */
