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
 * File:   masgtkwindowdata.cc
 */

#include "masgtkwindowdata.h"


namespace stmi
{

namespace Private
{
namespace Mas
{

void GtkWindowData::enable(const shared_ptr<GtkAccessor>& refAccessor, MasGtkDeviceManager* p0Owner)
{
	assert(refAccessor);
	assert(p0Owner != nullptr);
	assert(!m_oKeyPressConn.connected());
	assert(!m_oKeyReleaseConn.connected());
	assert(!m_oMotionNotifyConn.connected());
	assert(!m_oButtonPressConn.connected());
	assert(!m_oButtonReleaseConn.connected());
	assert(!m_oScrollConn.connected());
	assert(!m_oTouchConn.connected());
	assert(!m_oIsActiveConn.connected());
	m_oAddEvMask = (Gdk::EventMask)0;
	m_refAccessor = refAccessor;
	m_p0Owner = p0Owner;
	m_bIsEnabled = true;
	Gtk::Window* p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
	m_oIsActiveConn = p0GtkmmWindow->property_is_active().signal_changed().connect(sigc::mem_fun(this, &GtkWindowData::onSigIsActiveChanged));
	assert(m_oIsActiveConn.connected());
}
void GtkWindowData::disable()
{
	m_bIsEnabled = false;
	disconnect();
	m_oIsActiveConn.disconnect();
	m_p0Owner = nullptr;
	// DO NOT CLEAR m_refAccessor because despite being disabled
	// the object's accessor is still used afterwards to
	// send cancels to listeners
}
void GtkWindowData::connect()
{
	assert(m_p0Owner != nullptr);
	Gtk::Window* p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
	if (isEventClassEnabled(typeid(KeyEvent)) && !m_oKeyPressConn.connected()) {
		m_oAddEvMask |= Gdk::KEY_PRESS_MASK;
		m_oAddEvMask |= Gdk::KEY_RELEASE_MASK;
		m_oKeyPressConn = p0GtkmmWindow->signal_key_press_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigKeyPress));
		m_oKeyReleaseConn = p0GtkmmWindow->signal_key_release_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigKeyRelease));
	}
	if (isEventClassEnabled(typeid(PointerEvent)) && !m_oMotionNotifyConn.connected()) {
		m_oAddEvMask |= Gdk::POINTER_MOTION_MASK;
		m_oAddEvMask |= Gdk::BUTTON_PRESS_MASK;
		m_oAddEvMask |= Gdk::BUTTON_RELEASE_MASK;
		m_oMotionNotifyConn = p0GtkmmWindow->signal_motion_notify_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigMotionNotify));
		m_oButtonPressConn = p0GtkmmWindow->signal_button_press_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigButtonPress));
		m_oButtonReleaseConn = p0GtkmmWindow->signal_button_release_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigButtonRelease));
	}
	if (isEventClassEnabled(typeid(PointerScrollEvent)) && !m_oScrollConn.connected()) {
		m_oAddEvMask |= Gdk::SCROLL_MASK;
		m_oScrollConn = p0GtkmmWindow->signal_scroll_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigScroll));
	}
	if (isEventClassEnabled(typeid(TouchEvent)) && !m_oTouchConn.connected()) {
		m_oAddEvMask |= Gdk::TOUCH_MASK;
		m_oTouchConn = p0GtkmmWindow->signal_touch_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigTouch));
	}
	if (p0GtkmmWindow->get_realized()) {
		p0GtkmmWindow->add_events(m_oAddEvMask);
	} else {
		const Gdk::EventMask oCurMask = p0GtkmmWindow->get_events();
		const Gdk::EventMask oNewMask = oCurMask | m_oAddEvMask;
		if (oNewMask != oCurMask) {
			m_oAddEvMask = oNewMask;
			p0GtkmmWindow->set_events(oNewMask);
		}
	}
}
void GtkWindowData::disconnect()
{
//std::cout << "GtkWindowData::disconnect()" << std::endl;
	m_oKeyPressConn.disconnect();
	m_oKeyReleaseConn.disconnect();
	m_oMotionNotifyConn.disconnect();
	m_oButtonPressConn.disconnect();
	m_oButtonReleaseConn.disconnect();
	m_oScrollConn.disconnect();
	m_oTouchConn.disconnect();
}
bool GtkWindowData::isAccesorWindow(GdkWindow* p0Window) const
{
	assert(m_bIsEnabled);
	assert(!m_refAccessor->isDeleted()); // Cannot receive events from a deleted window
	Gtk::Window* p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
	Glib::RefPtr<Gdk::Window> refGdkWindow = p0GtkmmWindow->get_window();
	if (!refGdkWindow) {
		return false;
	}
	const GdkWindow* p0AccessorGdkWindow = refGdkWindow->gobj();
	return (p0Window == p0AccessorGdkWindow);
}

} // namespace Mas
} // namespace Private

} // namespace stmi
