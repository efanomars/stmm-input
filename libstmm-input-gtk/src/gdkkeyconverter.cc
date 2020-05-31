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
 * File:   gdkkeyconverter.cc
 */

#include "gdkkeyconverter.h"

#include "gdkkeyconverterevdev.h"

#include <cassert>

namespace stmi
{

shared_ptr<GdkKeyConverter> GdkKeyConverter::s_refKeyConverter{};

const shared_ptr<GdkKeyConverter>& GdkKeyConverter::getConverter() noexcept
{
	if (!s_refKeyConverter) {
		s_refKeyConverter = GdkKeyConverterEvDev::getConverter();
	}
	return s_refKeyConverter;
}

bool GdkKeyConverter::isConverterSet() noexcept
{
	return s_refKeyConverter.operator bool();
}
bool GdkKeyConverter::setConverter(const shared_ptr<GdkKeyConverter>& refConverter) noexcept
{
	assert(refConverter);
	if (s_refKeyConverter) {
		return false;
	}
	s_refKeyConverter = refConverter;
	return true;
}

} // namespace stmi
