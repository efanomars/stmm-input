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
 * File:   masgtkdevicemanager.h
 */

#ifndef STMI_MAS_GTK_DEVICE_MANAGER_H
#define STMI_MAS_GTK_DEVICE_MANAGER_H

#include <stmm-input-gtk/keyrepeatmode.h>

#include <stmm-input-ev/stddevicemanager.h>


#include <stmm-input/event.h>

#include <gtkmm.h>

#include <vector>
#include <string>
#include <memory>
#include <utility>

#include <stdint.h>

namespace stmi { class Accessor; }
namespace stmi { class GdkKeyConverter; }

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace Private
{
namespace Mas
{
	class GtkBackend;
	class GtkWindowData;
	class GtkWindowDataFactory;
	class GtkKeyboardDevice;
	class GtkPointerDevice;
	class MasGtkListenerExtraData;
}
}

/** Handles the xinput master keyboard and master pointer (mouse and touch) devices.
 * An instance only handles the devices of one Gdk::Display's device manager.
 *
 * An event (of type stmi::KeyEvent, stmi::PointerEvent, stmi::PointerScrollEvent,
 * or stmi::TouchEvent) sent to listeners by this device manager is tied
 * to a Gtk::Window, which has to be added with MasGtkDeviceManager::addAccessor()
 * wrapped in a stmi::GtkAccessor.
 * Events are only sent to the currently active window. When the active window changes
 * cancel events are sent to the old active window for each pressed key, button
 * and open touch.
 */
class MasGtkDeviceManager : public StdDeviceManager
{
public:
	/** Creates an instance this class.
	 * If the passed gdk device manager is null the one of the default
	 * Gdk::Display is used.
	 *
	 * If bEnableEventClasses is `true` then all event classes in aEnDisableEventClasses are enabled, all others disabled,
	 * if `false` then all event classes supported by this instance are enabled except those in aEnDisableEventClasses.
	 * MasGtkDeviceManager doesn't allow disabling event classes once constructed, only enabling.
	 *
	 * Example: To enable all the event classes supported by this instance (currently stmi::KeyEvent, stmi::PointerEvent,
	 * stmi::PointerScrollEvent, stmi::TouchEvent and stmi::DeviceMgmtEvent) pass
	 *
	 *     bEnableEventClasses = false,  aEnDisableEventClasses = {}
	 *
	 * @param bEnableEventClasses Whether to enable or disable all but aEnDisableEventClasses.
	 * @param aEnDisableEventClasses The event classes to be enabled or disabled according to bEnableEventClasses.
	 * @param eKeyRepeatMode Key repeat translation type.
	 * @param refGdkConverter refGdkConverter The gdk key event to hardware key converter. Cannot be null.
	 * @param refGdkDeviceManager The backend gdk device manager. Can be null.
	 * @return The created device manager and an empty string or null and an error if creation failed.
	 */
	static std::pair<shared_ptr<MasGtkDeviceManager>, std::string> create(bool bEnableEventClasses
											, const std::vector<Event::Class>& aEnDisableEventClasses
											, KeyRepeat::MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter
											, const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager) noexcept;

	virtual ~MasGtkDeviceManager() noexcept;

	void enableEventClass(const Event::Class& oEventClass) noexcept override;

	/** Adds a stmi::GtkAccessor-wrapped Gtk::Window from which events should be received. 
	 * An instance of this class needs a stmi::GtkAccessor for each active Gtk::Window
	 * listeners want to receive events from.
	 * If the type of parameter refAccessor is not stmi::GtkAccessor, `false` is returned.
	 * If it is and its Gtk::Window has not the same Gdk::DeviceManager as the one passed 
	 * as parameter to create the device manager (MasGtkDeviceManager::create()),
	 * `false` is returned.
	 * If the Gdk::DeviceManager is the same and the window isn't already added 
	 * `true` is returned, `false` otherwise.
	 */
	bool addAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override;
	/** Removes a stmi::GtkAccessor-wrapped Gtk::Window added with addAccessor().
	 * If the device manager has the accessor, `true` is returned, `false` otherwise.
	 * 
	 * This function doesn't delete the window itself, it just tells the device manager
	 * to stop tracking it.
	 * 
	 * Cancels are sent to listeners for each pressed key, button and open touch.
	 */
	bool removeAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override;
	/** Tells whether a window is already tracked by the device manager.
	 * @param refAccessor The wrapper of the Gtk::Window.
	 * @return Whether the window is currently tracked by the device manager.
	 */
	bool hasAccessor(const shared_ptr<Accessor>& refAccessor) noexcept override;
protected:
	void finalizeListener(ListenerData& oListenerData) noexcept override;
	/** Constructor. */
	MasGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses
						, KeyRepeat::MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter) noexcept;
	/** Initializes the device manager. */
	void init(std::unique_ptr<Private::Mas::GtkWindowDataFactory>& refFactory
			, std::unique_ptr<Private::Mas::GtkBackend>& refBackend) noexcept;

private:
	void onDeviceChanged(bool bPointer) noexcept;
	void onDevicePairAdded() noexcept;
	void onDevicePairRemoved() noexcept;

	bool onKeyPress(GdkEventKey* p0KeyEv, const shared_ptr<Private::Mas::GtkWindowData>& refWindowData) noexcept;
	bool onKeyRelease(GdkEventKey* p0KeyEv, const shared_ptr<Private::Mas::GtkWindowData>& refWindowData) noexcept;

	bool onMotionNotify(GdkEventMotion* p0MotionEv, const shared_ptr<Private::Mas::GtkWindowData>& refWindowData) noexcept;
	bool onButtonPress(GdkEventButton* p0ButtonEv, const shared_ptr<Private::Mas::GtkWindowData>& refWindowData) noexcept;
	bool onButtonRelease(GdkEventButton* p0ButtonEv, const shared_ptr<Private::Mas::GtkWindowData>& refWindowData) noexcept;
	bool onScroll(GdkEventScroll* p0ScrollEv, const shared_ptr<Private::Mas::GtkWindowData>& refWindowData) noexcept;

	bool onTouch(GdkEventTouch* p0TouchEv, const shared_ptr<Private::Mas::GtkWindowData>& refWindowData) noexcept;

	void adjustConnectionsAfterEnablingClass() noexcept;

	void addDevices() noexcept;
	void removeDevices() noexcept;

	bool findWindow(Gtk::Window* p0GtkmmWindow
				, std::vector< std::pair<Gtk::Window*, shared_ptr<Private::Mas::GtkWindowData> > >::iterator& itFind) noexcept;
	bool hasAccessor(const shared_ptr<Accessor>& refAccessor, bool& bValid
				, std::vector< std::pair<Gtk::Window*, shared_ptr<Private::Mas::GtkWindowData> > >::iterator& itFind) noexcept;
	void removeAccessor(const std::vector<std::pair<Gtk::Window*, shared_ptr<Private::Mas::GtkWindowData> > >::iterator& itGtkData) noexcept;
	void deselectAccessor() noexcept;
	void cancelDevices() noexcept;
	void onIsActiveChanged(const shared_ptr<Private::Mas::GtkWindowData>& refWindowData) noexcept;

	friend class Private::Mas::GtkBackend;
	friend class Private::Mas::GtkWindowData;
	friend class Private::Mas::GtkKeyboardDevice;
	friend class Private::Mas::GtkPointerDevice;
	friend class Private::Mas::MasGtkListenerExtraData;
private:
	// The GtkAccessor (GtkWindowData::m_refAccessor) will tell 
	// when the window gets deleted. The accessor can also be removed
	// explicitely during a listener callback.
	std::vector<std::pair<Gtk::Window*, shared_ptr<Private::Mas::GtkWindowData> > > m_aGtkWindowData;
	// The currently active accessor (window), can be null.
	std::shared_ptr<Private::Mas::GtkWindowData> m_refSelected;
	// Invariants:
	// - if m_refSelected is null, no key or button or sequence is "open" in the (two) devices
	// - if m_refSelected is not null, all "open" keys, buttons and sequences were generated
	//   for the selected window.

	int32_t m_nCancelingNestedDepth;

	std::unique_ptr<Private::Mas::GtkWindowDataFactory> m_refFactory;
	std::unique_ptr<Private::Mas::GtkBackend> m_refBackend;
	// Main master keyboard + pointer
	shared_ptr<Private::Mas::GtkKeyboardDevice> m_refKeyboardDevice;
	shared_ptr<Private::Mas::GtkPointerDevice> m_refPointerDevice;
	KeyRepeat::MODE m_eKeyRepeatMode;

	const shared_ptr<GdkKeyConverter> m_refGdkConverter;
	// Fast access reference to converter
	const GdkKeyConverter& m_oConverter;
	//
	const int32_t m_nClassIdxKeyEvent;
	const int32_t m_nClassIdxPointerEvent;
	const int32_t m_nClassIdxPointerScrollEvent;
	const int32_t m_nClassIdxTouchEvent;
private:
	MasGtkDeviceManager(const MasGtkDeviceManager& oSource) = delete;
	MasGtkDeviceManager& operator=(const MasGtkDeviceManager& oSource) = delete;
};

} // namespace stmi

#endif /* STMI_MAS_GTK_DEVICE_MANAGER_H */
