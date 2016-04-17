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
 * File:   gtkaccessor.cc
 */

#include "gtkaccessor.h"

namespace stmi
{

namespace Private
{
namespace GtkAccessorNS
{
void notify_window_destroyed(gpointer p0Data, GObject *p0Window)
{
	GtkAccessor* p0GtkAccessor = static_cast<GtkAccessor*>(p0Data);
	assert(p0GtkAccessor != nullptr);
	p0GtkAccessor->windowWasDestroyed((GtkWindow*)p0Window);
}
} // namespace GtkAccessor
} // namespace Private

bool GtkAccessor::operator==(const Accessor& oOther) const
{
	// This is the most common case since an accessor is usually shared through
	// a shared_ptr and only one instance is needed.
	if (this == &oOther) {
		return true;
	}
	// Can do this because class is final,
	// but is it really faster than dynamic_cast?
	if (typeid(oOther) != typeid(GtkAccessor)) {
		return false;
	}
	// m_bIsDeleted is checked in case we have a window that was deleted and then recreated in
	// the same address. But! If a window is recreated twice in the same address then
	// the result is not really accurate, but why would accessors to removed windows stay around
	// that long?
	const GtkAccessor& oOtherGtkA = *(static_cast<const GtkAccessor*>(&oOther));
	return (m_p0GtkmmWindow == oOtherGtkA.m_p0GtkmmWindow) && (m_bIsDeleted == oOtherGtkA.m_bIsDeleted);
}
bool GtkAccessor::operator==(const GtkAccessor& oOther) const
{
	// This is the most common case since an accessor is usually shared through
	// a shared_ptr and only one instance is needed.
	if (this == &oOther) {
		return true;
	}
	return (m_p0GtkmmWindow == oOther.m_p0GtkmmWindow) && (m_bIsDeleted == oOther.m_bIsDeleted);
}

GtkAccessor::GtkAccessor(const Glib::RefPtr<Gtk::Window>& refGtkmmWindow)
: GtkAccessor(refGtkmmWindow.operator->())
{
}
GtkAccessor::GtkAccessor(Gtk::Window* p0GtkmmWindow)
: Accessor()
, m_bIsDeleted(false)
, m_p0GtkmmWindow(p0GtkmmWindow)
{
	assert(p0GtkmmWindow != nullptr);
	GtkWindow* p0GtkWindow = p0GtkmmWindow->gobj();
	assert(p0GtkWindow != nullptr);
	g_object_weak_ref(G_OBJECT(p0GtkWindow), Private::GtkAccessorNS::notify_window_destroyed, (gpointer)this);
}
GtkAccessor::~GtkAccessor()
{
//std::cout << "GtkAccessor::~GtkAccessor(" << (int64_t)m_p0GtkWindow << ") m_bIsDeleted=" << m_bIsDeleted << std::endl;
	if (m_bIsDeleted) {
		return;
	}
	GtkWindow* p0GtkWindow = m_p0GtkmmWindow->gobj();
	g_object_weak_unref(G_OBJECT(p0GtkWindow), Private::GtkAccessorNS::notify_window_destroyed, (gpointer)this);
}
void GtkAccessor::windowWasDestroyed(GtkWindow* /*p0GtkWindow*/)
{
//std::cout << "GtkAccessor::windowWasDestroyed(" << (int64_t)m_p0GtkWindow << ")" << std::endl;
	m_bIsDeleted = true;
}

} // namespace stmi
