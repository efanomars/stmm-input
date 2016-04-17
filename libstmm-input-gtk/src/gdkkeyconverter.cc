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
 * File:   gdkkeyconverter.cc
 */

#include <limits>

#include "gdkkeyconverterevdev.h"

namespace stmi
{

std::shared_ptr<GdkKeyConverter> GdkKeyConverter::s_refSingletonInstance;

std::shared_ptr<GdkKeyConverter> GdkKeyConverter::getConverter()
{
	if (!s_refSingletonInstance) {
		s_refSingletonInstance = std::make_shared<GdkKeyConverterEvDev>();
	}
	return s_refSingletonInstance;
}
bool GdkKeyConverter::isConverterInstalled()
{
	return !(!s_refSingletonInstance);
}
bool GdkKeyConverter::installConverter(const std::shared_ptr<GdkKeyConverter>& ref)
{
	if (!ref) {
		return false;
	}
	if (s_refSingletonInstance) {
		return false;
	}
	s_refSingletonInstance = ref;
	return true;
}

} // namespace stmi
