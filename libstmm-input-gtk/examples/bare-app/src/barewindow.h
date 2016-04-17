/*
 * File:   barewindow.h
 */

#ifndef _BARE_WINDOW_
#define _BARE_WINDOW_

#include <string>
#include <memory>

#include <gtkmm.h>
#include <gdkmm.h>

#include <stmm-input/devicemanager.h>
#include <stmm-input-gtk/gtkaccessor.h>

namespace example
{

namespace bareapp
{

using std::shared_ptr;
using std::weak_ptr;

class BareWindow : public Gtk::Window
{
public:
	BareWindow(const Glib::ustring sTitle);
	virtual ~BareWindow();
protected:
	// Too receive most of the keys, like TAB, ENTER, etc. override default handlers
	bool on_key_press_event(GdkEventKey* p0Event) override;
	bool on_key_release_event(GdkEventKey* p0Event) override;
};

} // namespace bareapp

} // namespace example

#endif	/* _BARE_WINDOW_ */

