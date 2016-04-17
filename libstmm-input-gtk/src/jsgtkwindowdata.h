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
 * File:   jsgtkwindowdata.h
 */

#ifndef _STMI_JS_GTK_WINDOW_DATA_H_
#define _STMI_JS_GTK_WINDOW_DATA_H_

#include <gtkmm.h>

#include "jsgtkdevicemanager.h"


namespace stmi
{

namespace Private
{
namespace Js
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
		//std::cout << "Js::GtkWindowData::~GtkWindowData()" << std::endl;
		disable();
	}
	void enable(const shared_ptr<GtkAccessor>& refAccessor, JsGtkDeviceManager* p0Owner)
	{
		assert(refAccessor);
		assert(p0Owner != nullptr);
		assert(!m_oIsActiveConn.connected());
		m_refAccessor = refAccessor;
		if (m_refAccessor->isDeleted()) {
			return; //----------------------------------------------------------
		}
		m_p0Owner = p0Owner;
		Gtk::Window* p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
		m_oIsActiveConn = p0GtkmmWindow->property_is_active().signal_changed().connect(sigc::mem_fun(this, &GtkWindowData::onSigIsActiveChanged));
		m_bIsEnabled = true;
	}
	// !!! Doesn't reset accessor!
	void disable()
	{
		m_bIsEnabled = false;
		m_oIsActiveConn.disconnect();
	}
	inline bool isEnabled() const { return m_bIsEnabled; }
	inline const shared_ptr<GtkAccessor>& getAccessor() { return m_refAccessor; }

private:
	void onSigIsActiveChanged() { m_p0Owner->onIsActiveChanged(shared_from_this()); }
private:
	//
	shared_ptr<GtkAccessor> m_refAccessor;
	JsGtkDeviceManager* m_p0Owner;
	//
	bool m_bIsEnabled;
	//
	sigc::connection m_oIsActiveConn;
};

} // namespace Js
} // namespace Private

} // namespace stmi

#endif	/* _STMI_MAS_GTK_WINDOW_DATA_H_ */
