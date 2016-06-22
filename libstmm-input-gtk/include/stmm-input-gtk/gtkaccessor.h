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
 * File:   gtkaccesor.h
 */

#ifndef _STMI_GTK_ACCESSOR_H_
#define _STMI_GTK_ACCESSOR_H_

#include <stmm-input/accessor.h>

#include <gtkmm.h>

namespace stmi
{

namespace Private
{
namespace GtkAccessorNS
{
void notify_window_destroyed(gpointer p0Data, GObject *p0Window);
} // namespace GtkAccessorNS
} // namespace Private

/** Accessor wrapping a Gtk::Window.
 * Device managers that want to receive mouse and keyboard events directed 
 * to a gtk window need this accessor.
 */
// In absence of a weak_ptr in the Glib library an instance of this class
// will be informed through a callback by its window if it's about to be deleted.
class GtkAccessor final : public Accessor
{
public:
	/** Accessor comparison.
	 * The other accessor has to be a GtkAccessor too and have the same window.
	 * Comparison works also when the window has been deleted.
	 * @param oOther The other instance this is comparing to.
	 * @return Whether they're equal.
	 */
	bool operator==(const Accessor& oOther) const override;
	/** GtkAccessor comparison.
	 * Two accessors are equal if their window is the same.
	 * This works also when the window has already been deleted.
	 * @param oOther The other instance this is comparing to.
	 * @return Whether they're equal.
	 */
	bool operator==(const GtkAccessor& oOther) const;
	/** Constructs a Gtk::Window wrapper.
	 * This class keeps a weak reference to the Gtk::Window.
	 * @param refGtkmmWindow The window. Cannot be null.
	 */
	GtkAccessor(const Glib::RefPtr<Gtk::Window>& refGtkmmWindow);
	/** Constructs a Gtk::Window wrapper.
	 * This class keeps a weak reference to the Gtk::Window.
	 * @param p0GtkmmWindow The window. Cannot be null.
	 */
	GtkAccessor(Gtk::Window* p0GtkmmWindow);
	virtual ~GtkAccessor();
	/** Tells whether the window was deleted.
	 * If it's the case the value returned by getGtkmmWindow()
	 * no longer is a valid pointer, but still can be useful to identify and
	 * remove data structures in a device manager.
	 * @return Whether the window was deleted.
	 */
	inline bool isDeleted() const { return m_bIsDeleted; }
	/** The Gtk::Window pointer.
	 * To know whether the pointer is valid call isDeleted().
	 * @return The non owning Gtk::Window pointer.
	 */
	inline Gtk::Window* getGtkmmWindow() const { return m_p0GtkmmWindow; }
private:
	friend void Private::GtkAccessorNS::notify_window_destroyed(gpointer p0Data, GObject *p0Window);
	void windowWasDestroyed(GtkWindow* p0GtkWindow);
private:
	bool m_bIsDeleted;
	Gtk::Window* m_p0GtkmmWindow;
};

} // namespace stmi

#endif	/* _STMI_GTK_ACCESSOR_H_ */

