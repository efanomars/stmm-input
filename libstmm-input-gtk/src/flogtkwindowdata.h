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
 * File:   flogtkwindowdata.h
 */

#ifndef _STMI_FLO_GTK_WINDOW_DATA_H_
#define _STMI_FLO_GTK_WINDOW_DATA_H_

#include <gtkmm.h>

#include <X11/extensions/XI2.h>
#include <X11/extensions/XInput2.h>

#include "flogtkdevicemanager.h"

namespace stmi
{

namespace Private
{
namespace Flo
{

using std::shared_ptr;
using std::weak_ptr;

class GtkWindowData final : public std::enable_shared_from_this<GtkWindowData>, public sigc::trackable
{
public:
	GtkWindowData()
	: m_p0Owner(nullptr)
	, m_nXWinId(None)
	, m_bIsEnabled(false)
	, m_bIsRealized(false)
	{
	}
	virtual ~GtkWindowData()
	{
		disable();
	}
	//
	void enable(const shared_ptr<GtkAccessor>& refAccessor, ::Window nXWinId, FloGtkDeviceManager* p0Owner)
	{
		assert(refAccessor);
		assert(p0Owner != nullptr);
		assert(!m_oIsActiveConn.connected());
		m_refAccessor = refAccessor;
		m_nXWinId = nXWinId;
		m_p0Owner = p0Owner;
		Gtk::Window* p0GtkmmWindow = m_refAccessor->getGtkmmWindow();
		m_oIsActiveConn = p0GtkmmWindow->property_is_active().signal_changed().connect(sigc::mem_fun(this, &GtkWindowData::onSigIsActiveChanged));
		m_bIsEnabled = true;
	}
	void disable()
	{
		m_oIsActiveConn.disconnect();
		disconnectAllDevices();
		m_bIsEnabled = false;
	}
	inline bool isEnabled() const { return m_bIsEnabled; }
	inline void connectAllDevices()
	{
		setXIEventsForAllDevices(true);
	}
	inline void disconnectAllDevices()
	{
		setXIEventsForAllDevices(false);
	}
	inline void connectDevice(int32_t nXDeviceId)
	{
		setXIEventsForDevice(nXDeviceId, true);
	}
	inline void disconnectDevice(int32_t nXDeviceId)
	{
		setXIEventsForDevice(nXDeviceId, false);
	}
	//
	inline const shared_ptr<GtkAccessor>& getAccessor() { return m_refAccessor; }
	inline ::Window getXWindow() const { return m_nXWinId; }
	inline void setXWindow(::Window nXWinId)
	{
		assert(nXWinId != None);
		m_nXWinId = nXWinId;
	}
	//
private:
	//
	void onSigIsActiveChanged() { m_p0Owner->onIsActiveChanged(shared_from_this()); }
	void setXIEventsForAllDevices(bool bSet)
	{
		assert(m_p0Owner != nullptr);
		for (auto& oPair : m_p0Owner->m_oKeyboardDevices) {
			const int32_t nDeviceId = oPair.first;
			const bool bConnectedAll = setXIEventsForDevice(nDeviceId, bSet);
			if (bConnectedAll) {
				break; // for -----------
			}
		}
	}
	// returns true if connected all because just realized
	bool setXIEventsForDevice(int32_t nXDeviceId, bool bSet)
	{
//std::cout << "Flo::GtkWindowData::selectXIEvents()  nXDeviceId=" << nXDeviceId << "  nXWinId=" << m_nXWinId << "  bSet=" << bSet << std::endl;
		assert(m_p0Owner != nullptr);
		assert(m_bIsEnabled);

		if (!m_bIsRealized) {
			if (m_refAccessor->isDeleted()) {
				return false; //------------------------------------------------
			}
			const bool bIsRealized = m_refAccessor->getGtkmmWindow()->get_realized();
			if (!bIsRealized) {
				// the xwindow doesn't even exist yet!'
				// just return, set a flag that tells to connect to all devices
				// as soon as the window is real
				return false; //------------------------------------------------
			}
			m_bIsRealized = true;
//std::cout << "Flo::GtkWindowData::selectXIEvents()  just realized!" << std::endl;
			setXIEventsForAllDevices(bSet);
			return true; //-----------------------------------------------------
		}
		XIEventMask oEvMasks[ 1 ];

		// TODO maybe use GetMask | key press + key release
		unsigned char oMask1[ ( XI_LASTEVENT + 7 ) / 8 ];
		memset( oMask1, 0, sizeof( oMask1 ) );

		// Select for key events from all devices.
		if (bSet) {
			XISetMask( oMask1, XI_KeyPress );
			XISetMask( oMask1, XI_KeyRelease );
		}
		oEvMasks[ 0 ].deviceid = nXDeviceId; //XIAllDevices;
		oEvMasks[ 0 ].mask_len = sizeof( oMask1 );
		oEvMasks[ 0 ].mask = oMask1;

		auto nStatus = XISelectEvents( m_p0Owner->m_p0XDisplay, m_nXWinId, oEvMasks, sizeof(oEvMasks) / sizeof(oEvMasks[0]));
		if ((nStatus == BadValue) || (nStatus == BadWindow)) {
			std::cout << "FloGtkDeviceManager: XISelectEvents oEvMasks returns ";
			if (nStatus == BadValue) {
				std::cout << "BadValue";
			} else if (nStatus == BadWindow) {
				std::cout << "BadWindow";
			} else {
				assert(false);
			}
			std::cout << std::endl;
			assert(false);
		}
		return false;
	}
private:
	FloGtkDeviceManager* m_p0Owner;
	shared_ptr<GtkAccessor> m_refAccessor;
	::Window m_nXWinId;
	//
	bool m_bIsEnabled;
	bool m_bIsRealized;

	sigc::connection m_oIsActiveConn;
};

} // namespace Flo
} // namespace Private

} // namespace stmi

#endif	/* _STMI_FLO_GTK_WINDOW_DATA_H_ */
