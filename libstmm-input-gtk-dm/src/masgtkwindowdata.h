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
 * File:   masgtkwindowdata.h
 */

#ifndef STMI_MAS_GTK_WINDOW_DATA_H
#define STMI_MAS_GTK_WINDOW_DATA_H

#include "masgtkdevicemanager.h"

#include "recycler.h"

#include <stmm-input-gtk/gtkaccessor.h>
#include <stmm-input/event.h>

#include <gtkmm.h>

#include <memory>
#include <vector>
#include <cassert>

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
	GtkWindowData() noexcept
	: m_p0Owner(nullptr)
	, m_bIsEnabled(false)
	{
	}
	virtual ~GtkWindowData() noexcept
	{
		//std::cout << "GtkWindowData::~GtkWindowData()" << std::endl;
		disable();
	}
	void reInit() noexcept {}

	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	void enable(const shared_ptr<GtkAccessor>& refAccessor, MasGtkDeviceManager* p0Owner) noexcept;
	// !!! Doesn't reset accessor!
	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	void disable() noexcept;
	
	#ifdef STMI_TESTING_IFACE
	virtual
	#else
	inline
	#endif
	bool isEnabled() const noexcept { return m_bIsEnabled; }

	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	void connect() noexcept;

	#ifdef STMI_TESTING_IFACE
	virtual
	#else
	inline
	#endif
	const shared_ptr<GtkAccessor>& getAccessor() noexcept { return m_refAccessor; }

	#ifdef STMI_TESTING_IFACE
	virtual
	#else
	inline
	#endif
	bool isWindowActive() const noexcept
	{
		assert(m_refAccessor);
		auto p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
		return p0GtkmmWindow->get_realized() && p0GtkmmWindow->get_visible() && p0GtkmmWindow->is_active();
	}

	#ifdef STMI_TESTING_IFACE
	virtual
	#endif
	bool isAccesorWindow(GdkWindow* p0Window) const noexcept;
protected:
	inline void setOwner(MasGtkDeviceManager* p0Owner) noexcept
	{
		m_p0Owner = p0Owner;
	}
	inline bool onSigKeyPress(GdkEventKey* p0KeyEv) noexcept { return m_p0Owner->onKeyPress(p0KeyEv, shared_from_this()); }
	inline bool onSigKeyRelease(GdkEventKey* p0KeyEv) noexcept { return m_p0Owner->onKeyRelease(p0KeyEv, shared_from_this()); }
	inline bool onSigMotionNotify(GdkEventMotion* p0MotionEv) noexcept { return m_p0Owner->onMotionNotify(p0MotionEv, shared_from_this()); }
	inline bool onSigButtonPress(GdkEventButton* p0ButtonEv) noexcept { return m_p0Owner->onButtonPress(p0ButtonEv, shared_from_this()); }
	inline bool onSigButtonRelease(GdkEventButton* p0ButtonEv) noexcept { return m_p0Owner->onButtonRelease(p0ButtonEv, shared_from_this()); }
	inline bool onSigScroll(GdkEventScroll* p0ScrollEv) noexcept { return m_p0Owner->onScroll(p0ScrollEv, shared_from_this()); }
	inline bool onSigTouch(GdkEventTouch* p0TouchEv) noexcept { return m_p0Owner->onTouch(p0TouchEv, shared_from_this()); }
	inline void onSigIsActiveChanged() noexcept { m_p0Owner->onIsActiveChanged(shared_from_this()); }

	inline bool isEventClassEnabled(const Event::Class& oEventClass) const noexcept
	{
		assert(m_p0Owner != nullptr);
		return m_p0Owner->isEventClassEnabled(oEventClass);
	}
private:
	void disconnect() noexcept;

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
	shared_ptr<GtkWindowData> create() noexcept
	{
		shared_ptr<GtkWindowData> ref;
		m_oRecycler.create(ref);
		return ref;
	}
	#ifdef STMI_TESTING_IFACE
	virtual ~GtkWindowDataFactory() noexcept = default;
	#endif
private:
	Recycler<GtkWindowData> m_oRecycler;
};

} // namespace Mas
} // namespace Private

} // namespace stmi

#endif /* STMI_MAS_GTK_WINDOW_DATA_H */
