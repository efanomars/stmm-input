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

#include "masgtkdevicemanager.h"
#include "recycler.h"

#include <gtkmm.h>


namespace stmi
{

namespace Private
{
namespace Mas
{

using std::shared_ptr;
using std::weak_ptr;

class GtkWindowData : public std::enable_shared_from_this<GtkWindowData>, public sigc::trackable
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
	void reInit() {}

	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	void enable(const shared_ptr<GtkAccessor>& refAccessor, MasGtkDeviceManager* p0Owner);
	// !!! Doesn't reset accessor!
	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	void disable();
	
	#ifdef STMI_TESTING_IFACE
	virtual
	#else
	inline
	#endif
	bool isEnabled() const { return m_bIsEnabled; }

	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	void connect();

	#ifdef STMI_TESTING_IFACE
	virtual
	#else
	inline
	#endif
	const shared_ptr<GtkAccessor>& getAccessor() { return m_refAccessor; }

	#ifdef STMI_TESTING_IFACE
	virtual
	#else
	inline
	#endif
	bool isWindowActive() const
	{
		assert(m_refAccessor);
		auto p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
		return p0GtkmmWindow->get_realized() && p0GtkmmWindow->get_visible() && p0GtkmmWindow->is_active();
	}

	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	bool isAccesorWindow(GdkWindow* p0Window) const;
protected:
	inline void setOwner(MasGtkDeviceManager* p0Owner)
	{
		m_p0Owner = p0Owner;
	}
	inline bool onSigKeyPress(GdkEventKey* p0KeyEv) { return m_p0Owner->onKeyPress(p0KeyEv, shared_from_this()); }
	inline bool onSigKeyRelease(GdkEventKey* p0KeyEv) { return m_p0Owner->onKeyRelease(p0KeyEv, shared_from_this()); }
	inline bool onSigMotionNotify(GdkEventMotion* p0MotionEv) { return m_p0Owner->onMotionNotify(p0MotionEv, shared_from_this()); }
	inline bool onSigButtonPress(GdkEventButton* p0ButtonEv) { return m_p0Owner->onButtonPress(p0ButtonEv, shared_from_this()); }
	inline bool onSigButtonRelease(GdkEventButton* p0ButtonEv) { return m_p0Owner->onButtonRelease(p0ButtonEv, shared_from_this()); }
	inline bool onSigScroll(GdkEventScroll* p0ScrollEv) { return m_p0Owner->onScroll(p0ScrollEv, shared_from_this()); }
	inline bool onSigTouch(GdkEventTouch* p0TouchEv) { return m_p0Owner->onTouch(p0TouchEv, shared_from_this()); }
	inline void onSigIsActiveChanged() { m_p0Owner->onIsActiveChanged(shared_from_this()); }

	inline bool isEventClassEnabled(const Event::Class& oEventClass) const
	{
		assert(m_p0Owner != nullptr);
		return m_p0Owner->isEventClassEnabled(oEventClass);
	}
private:
	void disconnect();

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

////////////////////////////////////////////////////////////////////////////////
class GtkWindowDataFactory
{
public:
	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	std::shared_ptr<GtkWindowData> create()
	{
		return m_oRecycler.create();
	}
private:
	Recycler<GtkWindowData> m_oRecycler;
};

} // namespace Mas
} // namespace Private

} // namespace stmi

#endif	/* _STMI_MAS_GTK_WINDOW_DATA_H_ */
