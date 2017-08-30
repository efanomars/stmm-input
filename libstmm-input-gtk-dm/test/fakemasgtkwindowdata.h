/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   fakemasgtkwindowdata.h
 */

#ifndef _STMI_TESTING_FAKE_MAS_GTK_WINDOW_DATA_H_
#define _STMI_TESTING_FAKE_MAS_GTK_WINDOW_DATA_H_

#include "masgtkwindowdata.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{
namespace Mas
{

////////////////////////////////////////////////////////////////////////////////
class FakeGtkWindowData : public Private::Mas::GtkWindowData
{
public:
	FakeGtkWindowData()
	: Private::Mas::GtkWindowData()
	, m_bIsEnabled(false)
	, m_bKeyPressConn(false)
	, m_bKeyReleaseConn(false)
	, m_bMotionNotifyConn(false)
	, m_bButtonPressConn(false)
	, m_bButtonReleaseConn(false)
	, m_bScrollConn(false)
	, m_bTouchConn(false)
	, m_bWindowActive(false)
	{
	}
	void enable(const shared_ptr<GtkAccessor>& refAccessor, MasGtkDeviceManager* p0Owner) override
	{
		assert(refAccessor);
		assert(p0Owner != nullptr);
		setOwner(p0Owner);
		m_refAccessor = refAccessor;
		m_bIsEnabled= true;
	}
	// !!! Doesn't reset accessor!
	void disable() override
	{
		m_bIsEnabled = false;
		disconnect();
	}
	bool isEnabled() const override
	{
		return m_bIsEnabled;
	}
	void connect() override
	{
		if (isEventClassEnabled(typeid(KeyEvent)) && !m_bKeyPressConn) {
			m_bKeyPressConn = true;
			m_bKeyReleaseConn = true;
		}
		if (isEventClassEnabled(typeid(PointerEvent)) && !m_bMotionNotifyConn) {
			m_bMotionNotifyConn = true;
			m_bButtonPressConn = true;
			m_bButtonReleaseConn = true;
		}
		if (isEventClassEnabled(typeid(PointerScrollEvent)) && !m_bScrollConn) {
			m_bScrollConn = true;
		}
		if (isEventClassEnabled(typeid(TouchEvent)) && !m_bTouchConn) {
			m_bTouchConn = true;
		}
	}
	const shared_ptr<GtkAccessor>& getAccessor() override
	{
		return m_refAccessor;
	}

	bool isWindowActive() const override
	{
		return m_bWindowActive;
	}

	bool isAccesorWindow(GdkWindow* p0Window) const override
	{
		assert(m_bIsEnabled);
		assert(!m_refAccessor->isDeleted()); // Cannot receive events from a deleted window
		GdkWindow* p0AccessorGdkWindow = reinterpret_cast<GdkWindow*>(m_refAccessor->getGtkmmWindow());
		return (p0Window == p0AccessorGdkWindow);
	}
	bool simulateKeyPress(GdkEventKey* p0KeyEv)
	{
		assert(m_bKeyPressConn);
		p0KeyEv->window = reinterpret_cast<GdkWindow*>(m_refAccessor->getGtkmmWindow());
		return onSigKeyPress(p0KeyEv);
	}
	bool simulateKeyPress(guint nKeyval, guint16 nHardwareKeycode)
	{
		GdkEventKey oGdkEventKey;
		oGdkEventKey.type = ::GDK_KEY_PRESS;
		oGdkEventKey.send_event = FALSE; // ignored
		oGdkEventKey.time = 111; // ignored
		oGdkEventKey.state = 222; // ignored
		oGdkEventKey.keyval = nKeyval;
		oGdkEventKey.hardware_keycode = nHardwareKeycode;
		oGdkEventKey.group = 0;  // ignored
		oGdkEventKey.is_modifier = TRUE; // ignored
		return simulateKeyPress(&oGdkEventKey);
	}
	bool simulateKeyRelease(GdkEventKey* p0KeyEv)
	{
		assert(m_bKeyReleaseConn);
		p0KeyEv->window = reinterpret_cast<GdkWindow*>(m_refAccessor->getGtkmmWindow());
		return onSigKeyRelease(p0KeyEv);
	}
	bool simulateKeyRelease(guint nKeyval, guint16 nHardwareKeycode)
	{
		GdkEventKey oGdkEventKey;
		oGdkEventKey.type = ::GDK_KEY_RELEASE;
		oGdkEventKey.send_event = FALSE; // ignored
		oGdkEventKey.time = 111; // ignored
		oGdkEventKey.state = 222; // ignored
		oGdkEventKey.keyval = nKeyval;
		oGdkEventKey.hardware_keycode = nHardwareKeycode;
		oGdkEventKey.group = 0;  // ignored
		oGdkEventKey.is_modifier = TRUE; // ignored
		return simulateKeyRelease(&oGdkEventKey);
	}
	bool simulateMotionNotify(GdkEventMotion* p0MotionEv)
	{
		assert(m_bMotionNotifyConn);
		p0MotionEv->window = reinterpret_cast<GdkWindow*>(m_refAccessor->getGtkmmWindow());
		return onSigMotionNotify(p0MotionEv);
	}
	bool simulateMotionNotify(gdouble fX, gdouble fY, GdkDevice *p0Device)
	{
		GdkEventMotion oGdkEventMotion;
		oGdkEventMotion.type = ::GDK_MOTION_NOTIFY;
		oGdkEventMotion.send_event = FALSE; // ignored
		oGdkEventMotion.time = 111; // ignored
		oGdkEventMotion.state = 222; // ignored
		oGdkEventMotion.x = fX;
		oGdkEventMotion.y = fY;
		oGdkEventMotion.axes = nullptr; // ignored
		oGdkEventMotion.is_hint = FALSE; // ignored
		oGdkEventMotion.device = p0Device;
		return simulateMotionNotify(&oGdkEventMotion);
	}
	bool simulateButtonPress(GdkEventButton* p0ButtonEv)
	{
		assert(m_bButtonPressConn);
		p0ButtonEv->window = reinterpret_cast<GdkWindow*>(m_refAccessor->getGtkmmWindow());
		return onSigButtonPress(p0ButtonEv);
	}
	bool simulateButtonPress(gdouble fX, gdouble fY, guint nButton, GdkDevice *p0Device)
	{
		GdkEventButton oGdkEventButton;
		oGdkEventButton.type = ::GDK_BUTTON_PRESS;
		oGdkEventButton.send_event = FALSE; // ignored
		oGdkEventButton.time = 111; // ignored
		oGdkEventButton.state = 222; // ignored
		oGdkEventButton.x = fX;
		oGdkEventButton.y = fY;
		oGdkEventButton.axes = nullptr; // ignored
		oGdkEventButton.button = nButton;
		oGdkEventButton.device = p0Device;
		return simulateButtonPress(&oGdkEventButton);
	}
	bool simulateButtonRelease(GdkEventButton* p0ButtonEv)
	{
		assert(m_bButtonReleaseConn);
		p0ButtonEv->window = reinterpret_cast<GdkWindow*>(m_refAccessor->getGtkmmWindow());
		return onSigButtonRelease(p0ButtonEv);
	}
	bool simulateButtonRelease(gdouble fX, gdouble fY, guint nButton, GdkDevice *p0Device)
	{
		GdkEventButton oGdkEventButton;
		oGdkEventButton.type = ::GDK_BUTTON_RELEASE;
		oGdkEventButton.send_event = FALSE; // ignored
		oGdkEventButton.time = 111; // ignored
		oGdkEventButton.state = 222; // ignored
		oGdkEventButton.x = fX;
		oGdkEventButton.y = fY;
		oGdkEventButton.axes = nullptr; // ignored
		oGdkEventButton.button = nButton;
		oGdkEventButton.device = p0Device;
		return simulateButtonRelease(&oGdkEventButton);
	}
	bool simulateScroll(GdkEventScroll* p0ScrollEv)
	{
		assert(m_bScrollConn);
		p0ScrollEv->window = reinterpret_cast<GdkWindow*>(m_refAccessor->getGtkmmWindow());
		return onSigScroll(p0ScrollEv);
	}
	bool simulateScroll(gdouble fX, gdouble fY, GdkScrollDirection eDirection, GdkDevice *p0Device)
	{
		GdkEventScroll oGdkEventScroll;
		oGdkEventScroll.type = ::GDK_SCROLL;
		oGdkEventScroll.send_event = FALSE; // ignored
		oGdkEventScroll.time = 111; // ignored
		oGdkEventScroll.state = 222; // ignored
		oGdkEventScroll.x = fX;
		oGdkEventScroll.y = fY;
		oGdkEventScroll.direction = eDirection;
		oGdkEventScroll.device = p0Device;
		oGdkEventScroll.x_root = fX; // ignored
		oGdkEventScroll.y_root = fY; // ignored
		oGdkEventScroll.delta_x = 1; // ignored
		oGdkEventScroll.delta_y = 1; // ignored
		return simulateScroll(&oGdkEventScroll);
	}
	bool simulateTouch(GdkEventTouch* p0TouchEv)
	{
		assert(m_bTouchConn);
		p0TouchEv->window = reinterpret_cast<GdkWindow*>(m_refAccessor->getGtkmmWindow());
		return onSigTouch(p0TouchEv);
	}
	bool simulateTouch(GdkEventType eType, gdouble fX, gdouble fY, GdkEventSequence *p0Sequence, GdkDevice *p0Device)
	{
		GdkEventTouch oGdkEventTouch;
		oGdkEventTouch.type = eType;
		oGdkEventTouch.send_event = FALSE; // ignored
		oGdkEventTouch.time = 111; // ignored
		oGdkEventTouch.state = 222; // ignored
		oGdkEventTouch.x = fX;
		oGdkEventTouch.y = fY;
		oGdkEventTouch.axes = nullptr; // ignored
		oGdkEventTouch.sequence = p0Sequence;
		oGdkEventTouch.device = p0Device;
		oGdkEventTouch.emulating_pointer = FALSE; // ignored
		oGdkEventTouch.x_root = fX; // ignored
		oGdkEventTouch.y_root = fY; // ignored
		return simulateTouch(&oGdkEventTouch);
	}
	void simulateWindowSetActive(bool bActive)
	{
		if (m_bWindowActive == bActive) {
			// no change
			return;
		}
		m_bWindowActive = bActive;
		onSigIsActiveChanged();
	}

private:
	void disconnect()
	{
//std::cout << "GtkWindowData::disconnect()" << std::endl;
		m_bKeyPressConn = false;
		m_bKeyReleaseConn = false;
		m_bMotionNotifyConn = false;
		m_bButtonPressConn = false;
		m_bButtonReleaseConn = false;
		m_bScrollConn = false;
		m_bTouchConn = false;
	}
	
private:
	shared_ptr<GtkAccessor> m_refAccessor;
	//
	bool m_bIsEnabled;
	//
	bool m_bKeyPressConn;
	bool m_bKeyReleaseConn;
	bool m_bMotionNotifyConn;
	bool m_bButtonPressConn;
	bool m_bButtonReleaseConn;
	bool m_bScrollConn;
	bool m_bTouchConn;
	//
	bool m_bWindowActive;

};

////////////////////////////////////////////////////////////////////////////////
class FakeGtkWindowDataFactory : public Private::Mas::GtkWindowDataFactory
{
public:
	std::shared_ptr<Private::Mas::GtkWindowData> create() override
	{
		for (auto& refGtkWindowData : m_aFreePool) {
			if (refGtkWindowData.use_count() == 1) {
				// Recycle
				return refGtkWindowData; //-------------------------------------
			}
		}
		// The data is left in the pool!
		m_aFreePool.emplace_back(std::make_shared<FakeGtkWindowData>());
		return m_aFreePool.back();
	}
	const std::shared_ptr<FakeGtkWindowData>& getFakeWindowData(Gtk::Window* p0GtkmmWindow)
	{
		for (auto& refGtkWindowData : m_aFreePool) {
			if (!(refGtkWindowData.use_count() == 1)) {
				auto& refAccessor = refGtkWindowData->getAccessor();
				if ((refAccessor) && (!refAccessor->isDeleted()) && (refAccessor->getGtkmmWindow() == p0GtkmmWindow)) {
					return refGtkWindowData;
				}
			}
		}
		static std::shared_ptr<FakeGtkWindowData> s_refEmpty{};
		return s_refEmpty;
	}
	// There should be max one active window at any time
	// Any way this function returns the first active found.
	const std::shared_ptr<FakeGtkWindowData>& getActiveWindowData()
	{
		for (auto& refGtkWindowData : m_aFreePool) {
			if (!(refGtkWindowData.use_count() == 1)) {
				auto& refAccessor = refGtkWindowData->getAccessor();
				if ((refAccessor) && (!refAccessor->isDeleted()) && refGtkWindowData->isWindowActive()) {
					return refGtkWindowData;
				}
			}
		}
		static std::shared_ptr<FakeGtkWindowData> s_refEmpty{};
		return s_refEmpty;
	}
private:
	// The objects in the free pool might still be in use when the
	// removal of the accessor was done during a callback. This is detected
	// through the ref count of the shared_ptr.
	std::vector< std::shared_ptr<FakeGtkWindowData> > m_aFreePool;
};

} // namespace Mas
} // namespace testing

} // namespace stmi

#endif /* _STMI_TESTING_FAKE_MAS_GTK_WINDOW_DATA_H_ */
