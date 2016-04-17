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
 * File:   fakepointerdevice.h
 */

#ifndef _STMI_TESTING_FAKE_POINTER_DEVICE_H_
#define _STMI_TESTING_FAKE_POINTER_DEVICE_H_

#include "devicemanager.h"
#include "pointercapability.h"
#include "fakedevice.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class FakePointerDevice : public FakeDevice<PointerCapability>
{
public:
};

} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FAKE_POINTER_DEVICE_H_ */
