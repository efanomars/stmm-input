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
 * File:   masgtkwindowdata.h
 */

#ifndef _STMI_MAS_GTK_WINDOW_DATA_H_
#define _STMI_MAS_GTK_WINDOW_DATA_H_

#include <gtkmm.h>

#include "masgtkdevicemanager.h"


namespace stmi
{

namespace Private
{
namespace Mas
{

using std::shared_ptr;
using std::weak_ptr;

class GtkWindowData final : public std::enable_shared_from_this<GtkWindowData>, public sigc::trackable
{
public:
	GtkWindowData()
	: m_p0Owner(nullptr)
	, m_bIsEnabled(false)
	{
	}
	virtual ~GtkWindowData()
	{
		//std::cout << "GtkWindowData::~GtkWindowData()" << std::endl;
		disable();
	}
	void enable(const shared_ptr<GtkAccessor>& refAccessor, MasGtkDeviceManager* p0Owner)
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
	}
	// !!! Doesn't reset accessor!
	void disable()
	{
		m_bIsEnabled = false;
		disconnect();
		m_oIsActiveConn.disconnect();
	}
	inline bool isEnabled() const { return m_bIsEnabled; }
	inline void connect()
	{
		assert(m_p0Owner != nullptr);
		Gtk::Window* p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
		if (m_p0Owner->getEventClassEnabled(typeid(KeyEvent)) && !m_oKeyPressConn.connected()) {
			m_oAddEvMask |= Gdk::KEY_PRESS_MASK;
			m_oAddEvMask |= Gdk::KEY_RELEASE_MASK;
			m_oKeyPressConn = p0GtkmmWindow->signal_key_press_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigKeyPress));
			m_oKeyReleaseConn = p0GtkmmWindow->signal_key_release_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigKeyRelease));
		}
		if (m_p0Owner->getEventClassEnabled(typeid(PointerEvent)) && !m_oMotionNotifyConn.connected()) {
			m_oAddEvMask |= Gdk::POINTER_MOTION_MASK;
			m_oAddEvMask |= Gdk::BUTTON_PRESS_MASK;
			m_oAddEvMask |= Gdk::BUTTON_RELEASE_MASK;
			m_oMotionNotifyConn = p0GtkmmWindow->signal_motion_notify_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigMotionNotify));
			m_oButtonPressConn = p0GtkmmWindow->signal_button_press_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigButtonPress));
			m_oButtonReleaseConn = p0GtkmmWindow->signal_button_release_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigButtonRelease));
		}
		if (m_p0Owner->getEventClassEnabled(typeid(PointerScrollEvent)) && !m_oScrollConn.connected()) {
			m_oAddEvMask |= Gdk::SCROLL_MASK;
			m_oScrollConn = p0GtkmmWindow->signal_scroll_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigScroll));
		}
		if (m_p0Owner->getEventClassEnabled(typeid(TouchEvent)) && !m_oTouchConn.connected()) {
			m_oAddEvMask |= Gdk::TOUCH_MASK;
			m_oTouchConn = p0GtkmmWindow->signal_touch_event().connect(sigc::mem_fun(this, &GtkWindowData::onSigTouch));
		}
		if (p0GtkmmWindow->get_realized()) {
			p0GtkmmWindow->add_events(m_oAddEvMask);
		} else {
			const Gdk::EventMask oCurMask = p0GtkmmWindow->get_events();
			const Gdk::EventMask oNewMask = oCurMask | m_oAddEvMask;
			if (oNewMask != oCurMask) {
				p0GtkmmWindow->set_events(oNewMask);
			}
		}
	}
	void disconnect()
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
	inline const shared_ptr<GtkAccessor>& getAccessor() { return m_refAccessor; }

	const GdkWindow* getWindowDataGdkWindow()
	{
		assert(m_bIsEnabled);
		assert(!m_refAccessor->isDeleted()); // Cannot receive events from a deleted window
		Gtk::Window* p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
		Glib::RefPtr<Gdk::Window> refGdkWindow = p0GtkmmWindow->get_window();
		if (!refGdkWindow) {
			return nullptr; //--------------------------------------------------
		}
		const GdkWindow* p0AccessorGdkWindow = refGdkWindow->gobj();
		return p0AccessorGdkWindow;
	}
private:
	bool onSigKeyPress(GdkEventKey* p0KeyEv) { return m_p0Owner->onKeyPress(p0KeyEv, shared_from_this()); }
	bool onSigKeyRelease(GdkEventKey* p0KeyEv) { return m_p0Owner->onKeyRelease(p0KeyEv, shared_from_this()); }
	bool onSigMotionNotify(GdkEventMotion* p0MotionEv) { return m_p0Owner->onMotionNotify(p0MotionEv, shared_from_this()); }
	bool onSigButtonPress(GdkEventButton* p0ButtonEv) { return m_p0Owner->onButtonPress(p0ButtonEv, shared_from_this()); }
	bool onSigButtonRelease(GdkEventButton* p0ButtonEv) { return m_p0Owner->onButtonRelease(p0ButtonEv, shared_from_this()); }
	bool onSigScroll(GdkEventScroll* p0ScrollEv) { return m_p0Owner->onScroll(p0ScrollEv, shared_from_this()); }
	bool onSigTouch(GdkEventTouch* p0TouchEv) { return m_p0Owner->onTouch(p0TouchEv, shared_from_this()); }
	void onSigIsActiveChanged() { m_p0Owner->onIsActiveChanged(shared_from_this()); }
private:
	//
	shared_ptr<GtkAccessor> m_refAccessor;
	MasGtkDeviceManager* m_p0Owner;
	//
	bool m_bIsEnabled;
	//
	sigc::connection m_oKeyPressConn;
	sigc::connection m_oKeyReleaseConn;
	sigc::connection m_oMotionNotifyConn;
	sigc::connection m_oButtonPressConn;
	sigc::connection m_oButtonReleaseConn;
	sigc::connection m_oScrollConn;
	sigc::connection m_oTouchConn;

	sigc::connection m_oIsActiveConn;
	//TODO this should be in MasGtkDeviceManager because it's common among all windows!
	Gdk::EventMask m_oAddEvMask; // The current mask required by this DeviceManager
};

} // namespace Mas
} // namespace Private

} // namespace stmi

#endif	/* _STMI_MAS_GTK_WINDOW_DATA_H_ */
