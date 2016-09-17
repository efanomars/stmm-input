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
 * File:   fixturevariantKeyRepeatMode.h
 */

#ifndef _STMI_TESTING_FIXTURE_VARIANT_KEY_REPEAT_MODE_H_
#define _STMI_TESTING_FIXTURE_VARIANT_KEY_REPEAT_MODE_H_

#include "keyrepeatmode.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class FixtureVariantKeyRepeatMode_Suppress
{
};
class FixtureVariantKeyRepeatMode_AddRelease
{
};
class FixtureVariantKeyRepeatMode_AddReleaseCancel
{
};
class FixtureVariantKeyRepeatMode
{
public:
	virtual ~FixtureVariantKeyRepeatMode() = default;
protected:
	KEY_REPEAT_MODE getKeyRepeatMode()
	{
		KEY_REPEAT_MODE eKeyRepeatMode = KEY_REPEAT_MODE_SUPPRESS;
		if (dynamic_cast<FixtureVariantKeyRepeatMode_Suppress*>(this) != nullptr) {
			eKeyRepeatMode = KEY_REPEAT_MODE_SUPPRESS;
		} else if (dynamic_cast<FixtureVariantKeyRepeatMode_AddRelease*>(this) != nullptr) {
			eKeyRepeatMode = KEY_REPEAT_MODE_ADD_RELEASE;
		} else if (dynamic_cast<FixtureVariantKeyRepeatMode_AddReleaseCancel*>(this) != nullptr) {
			eKeyRepeatMode = KEY_REPEAT_MODE_ADD_RELEASE_CANCEL;
		}
		return eKeyRepeatMode;
	}
};

} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FIXTURE_VARIANT_KEY_REPEAT_MODE_H_ */
