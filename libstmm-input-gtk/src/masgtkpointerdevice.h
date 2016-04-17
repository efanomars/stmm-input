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
 * File:   masgtkpointerdevice.h
 */

#ifndef _STMI_MAS_GTK_POINTER_DEVICE_H_
#define _STMI_MAS_GTK_POINTER_DEVICE_H_

#include <stmm-input-base/stddevice.h>

#include "masgtkdevicemanager.h"

namespace stmi
{

namespace Private
{
namespace Mas
{

using std::shared_ptr;
using std::weak_ptr;

class GtkPointerDevice final : public StdDevice<MasGtkDeviceManager>, public TouchCapability, public PointerCapability
								, public std::enable_shared_from_this<GtkPointerDevice>
{
public:
	GtkPointerDevice(std::string sName, const Glib::RefPtr<Gdk::Device>& refPointer, const shared_ptr<MasGtkDeviceManager>& refMasGtkDeviceManager)
	: StdDevice<MasGtkDeviceManager>(sName, refMasGtkDeviceManager)
	, m_refPointer(refPointer)
	// Make sure listeners added time is smaller than the pressed time
	// so that the callback is called.
	, m_nAnyButtonPressTimeUsec(std::numeric_limits<int64_t>::max())
	, m_fLastPointerX(-1)
	, m_fLastPointerY(-1)
	{
	}
	virtual ~GtkPointerDevice();
	//
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const override;
	std::vector<Capability::Class> getCapabilityClasses() const override;
	// from capability
	shared_ptr<Device> getDevice() const override;
	shared_ptr<Device> getDevice() override;

private:
	const Glib::RefPtr<Gdk::Device>& getGdkDevice() const { return m_refPointer; }

	bool handleGdkEventMotion(GdkEventMotion* p0MotionEv, const shared_ptr<GtkWindowData>& refWindowData);
	bool handleGdkEventButton(GdkEventButton* p0ButtonEv, const shared_ptr<GtkWindowData>& refWindowData);
	bool handleGdkEventScroll(GdkEventScroll* p0ScrollEv, const shared_ptr<GtkWindowData>& refWindowData);
	bool handleGdkEventTouch(GdkEventTouch* p0TouchEv, const shared_ptr<GtkWindowData>& refWindowData);

	void cancelSelectedAccessorButtonsAndSequences();
	void cancelSelectedAccessorButtons(const shared_ptr< const std::list< MasGtkDeviceManager::ListenerData* > >& refListeners
										, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
										, MasGtkDeviceManager* p0Owner);
	void cancelSelectedAccessorSequences(const shared_ptr< const std::list< MasGtkDeviceManager::ListenerData* > >& refListeners
										, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
										, MasGtkDeviceManager* p0Owner);
	void finalizeListener(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec);
		void finalizeListenerButton(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
									, MasGtkDeviceManager* p0Owner);
		void finalizeListenerTouch(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
									, MasGtkDeviceManager* p0Owner);

	void removingDevice();

	void sendPointerEventToListener(const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
									, double fX, double fY
									, PointerEvent::POINTER_INPUT_TYPE eInputType, int32_t nButton
									, bool bWasAnyButtonPressed, bool bAnyButtonPressed
									, const shared_ptr<GtkAccessor>& refAccessor
									, MasGtkDeviceManager* p0Owner
									, shared_ptr<PointerEvent>& refEvent);
	void sendTouchEventToListener(const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
									, int64_t nSequenceStartTime, double fX, double fY
									, TouchEvent::TOUCH_INPUT_TYPE eInputType, int64_t nSequence
									, const shared_ptr<GtkAccessor>& refAccessor
									, MasGtkDeviceManager* p0Owner
									, shared_ptr<TouchEvent>& refEvent);
	inline int32_t translateGdkButtonToPointerEvent(int32_t nGdkButton)
	{
		int32_t nButton;
		switch (nGdkButton) {
			case GDK_BUTTON_PRIMARY: { nButton = 1; } break;
			case GDK_BUTTON_SECONDARY: { nButton = 2; } break;
			case GDK_BUTTON_MIDDLE: { nButton = 3; } break;
			default: {
				switch (nGdkButton) {
					case 1: { nButton = GDK_BUTTON_PRIMARY; } break;
					case 2: { nButton = GDK_BUTTON_SECONDARY; } break;
					case 3: { nButton = GDK_BUTTON_MIDDLE; } break;
					default: { nButton = nGdkButton; }
				}
			}
		}
		return nButton;
	}
private:
	const Glib::RefPtr<Gdk::Device> m_refPointer;
	// This class will make some assumptions: Gtk will grab the mouse
	// for the window as long as any button is pressed
	//TODO implement with bool m_aButton[5]; int32_t m_nTotButtons; or vector "set"
	std::vector<int32_t> m_aButtons; // Value: gdk button (1..5)
	// A listener only starts getting button and motion events after
	// all buttons are in a not-pressed state! In that case
	// this is set to std::numeric_limits<int64_t>::max().
	int64_t m_nAnyButtonPressTimeUsec;
	double m_fLastPointerX;
	double m_fLastPointerY;
	//
	struct SequenceData
	{
		int64_t m_nTouchStartTimeUsec;
		double m_fLastX;
		double m_fLastY;
	};
	std::unordered_map<GdkEventSequence*, SequenceData> m_oSequences; // Key: gdk touch sequence
	friend class  stmi::MasGtkDeviceManager;
private:
	GtkPointerDevice(const GtkPointerDevice& oSource);
	GtkPointerDevice& operator=(const GtkPointerDevice& oSource);
};

} // namespace Mas
} // namespace Private

} // namespace stmi

#endif	/* _STMI_MAS_GTK_POINTER_DEVICE_H_ */
