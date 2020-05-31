/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   accessor.h
 */

#ifndef STMI_ACCESSOR_H
#define STMI_ACCESSOR_H

#include <memory>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

/** A generic class that helps a device manager to generate events.
 * Subclass Accessor for system specific access to devices and/or subsets
 * of event sources. The subclass has to make sure no reference cycles are
 * created which lead to memory leaks.
 *
 * Example: within the GTK environment a device manager supporting pointer devices
 * uses the subclass GtkAccessor, which holds a (weak) reference to a Gtk::Window.
 * This allows it to listen to pointer events generated in the specified
 * window.
 */
class Accessor
{
public:
	virtual ~Accessor() noexcept = default;
	/** Accessor comparison.
	 * Needed to identify accessors and remove them.
	 */
	virtual bool operator==(const Accessor& oOther) const noexcept = 0;
	//
	static const shared_ptr<Accessor> s_refEmptyAccessor;
protected:
	Accessor() noexcept = default;
private:
	Accessor(const Accessor& oSource) = delete;
	Accessor& operator=(const Accessor& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_ACCESSOR_H */

