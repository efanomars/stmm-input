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
 * File:   masgtkpointerdevice.h
 */

#ifndef STMI_MAS_GTK_POINTER_DEVICE_H
#define STMI_MAS_GTK_POINTER_DEVICE_H

#include "masgtkdevicemanager.h"

#include "recycler.h"

#include <stmm-input-base/basicdevice.h>
#include <stmm-input-ev/pointercapability.h>
#include <stmm-input-ev/pointerevent.h>
#include <stmm-input-ev/touchcapability.h>
#include <stmm-input-ev/touchevent.h>
#include <stmm-input/capability.h>

#include <gdk/gdk.h>

#include <limits>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <stdint.h>

namespace stmi { class Accessor; }
namespace stmi { class Device; }
namespace stmi { class Event; }
namespace stmi { class GtkAccessor; }
namespace stmi { namespace Private { namespace Mas { class GtkWindowData; } } }

namespace stmi
{

namespace Private
{
namespace Mas
{

using std::shared_ptr;
using std::weak_ptr;

class GtkPointerDevice final : public BasicDevice<MasGtkDeviceManager>, public TouchCapability, public PointerCapability
								, public std::enable_shared_from_this<GtkPointerDevice>
{
public:
	GtkPointerDevice(const std::string& sName, const shared_ptr<MasGtkDeviceManager>& refMasGtkDeviceManager) noexcept
	: BasicDevice<MasGtkDeviceManager>(sName, refMasGtkDeviceManager)
	// Make sure listeners added time is smaller than the pressed time
	// so that the callback is called.
	, m_nAnyButtonPressTimeStamp(std::numeric_limits<uint64_t>::max())
	, m_fLastPointerX(-1)
	, m_fLastPointerY(-1)
	{
	}
	virtual ~GtkPointerDevice() noexcept;
	//
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const noexcept override;
	shared_ptr<Capability> getCapability(int32_t nCapabilityId) const noexcept override;
	std::vector<int32_t> getCapabilities() const noexcept override;
	std::vector<Capability::Class> getCapabilityClasses() const noexcept override;
	// from capability
	shared_ptr<Device> getDevice() const noexcept override;

private:
	friend class  stmi::MasGtkDeviceManager;

	bool handleGdkEventMotion(GdkEventMotion* p0MotionEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept;
	bool handleGdkEventButton(GdkEventButton* p0ButtonEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept;
	bool handleGdkEventScroll(GdkEventScroll* p0ScrollEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept;
	bool handleGdkEventTouch(GdkEventTouch* p0TouchEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept;

	void cancelSelectedAccessorButtonsAndSequences() noexcept;
	void cancelSelectedAccessorButtons(const shared_ptr< const std::list< MasGtkDeviceManager::ListenerData* > >& refListeners
										, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
										, MasGtkDeviceManager* p0Owner) noexcept;
	void cancelSelectedAccessorSequences(const shared_ptr< const std::list< MasGtkDeviceManager::ListenerData* > >& refListeners
										, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
										, MasGtkDeviceManager* p0Owner) noexcept;
	void finalizeListener(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec) noexcept;
		void finalizeListenerButton(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
									, MasGtkDeviceManager* p0Owner) noexcept;
		void finalizeListenerTouch(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
									, MasGtkDeviceManager* p0Owner) noexcept;

	void removingDevice() noexcept;
	void sendPointerEventToListener(const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
									, double fX, double fY
									, PointerEvent::POINTER_INPUT_TYPE eInputType, int32_t nButton
									, bool bWasAnyButtonPressed, bool bAnyButtonPressed
									, const shared_ptr<GtkAccessor>& refAccessor
									, MasGtkDeviceManager* p0Owner
									, shared_ptr<Event>& refEvent) noexcept;
	void sendTouchEventToListener(const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
									, uint64_t nSequenceStartTimeStamp, double fX, double fY
									, TouchEvent::TOUCH_INPUT_TYPE eInputType, int64_t nSequence
									, const shared_ptr<GtkAccessor>& refAccessor
									, MasGtkDeviceManager* p0Owner
									, shared_ptr<Event>& refEvent) noexcept;
	inline int32_t translateGdkButtonToPointerEvent(int32_t nGdkButton) noexcept
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
	// This class will make some assumptions: Gtk will grab the mouse
	// for the window as long as any button is pressed
	std::vector<int32_t> m_aButtons; // Value: gdk button (1..5)
	// A listener only starts getting button and motion events after
	// all buttons are in a not-pressed state! In that case
	// this is set to std::numeric_limits<int64_t>::max().
	uint64_t m_nAnyButtonPressTimeStamp;
	double m_fLastPointerX;
	double m_fLastPointerY;
	//
	struct SequenceData
	{
		uint64_t m_nTouchStartTimeStamp;
		double m_fLastX;
		double m_fLastY;
	};
	std::unordered_map<GdkEventSequence*, SequenceData> m_oSequences; // Key: gdk touch sequence
	//
	class RePointerEvent :public PointerEvent
	{
	public:
		RePointerEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
						, const shared_ptr<PointerCapability>& refPointerCapability, double fX, double fY
						, POINTER_INPUT_TYPE eType, int32_t nButton, bool bAnyButtonPressed, bool bWasAnyButtonPressed) noexcept
		: PointerEvent(nTimeUsec, refAccessor, refPointerCapability, fX, fY, eType, nButton, bAnyButtonPressed, bWasAnyButtonPressed)
		{
		}
		void reInit(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<PointerCapability>& refPointerCapability, double fX, double fY
					, POINTER_INPUT_TYPE eType, int32_t nButton, bool bAnyButtonPressed, bool bWasAnyButtonPressed) noexcept
		{
			setTimeUsec(nTimeUsec);
			setAccessor(refAccessor);
			setPointer(eType, nButton, bAnyButtonPressed, bWasAnyButtonPressed);
			setPointerCapability(refPointerCapability);
			setX(fX);
			setY(fY);
			setIsModified(false);
		}
	};
	Private::Recycler<RePointerEvent, Event> m_oPointerEventRecycler;
	//
	class RePointerScrollEvent :public PointerScrollEvent
	{
	public:
		RePointerScrollEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
							, const shared_ptr<PointerCapability>& refPointerCapability
							, POINTER_SCROLL_DIR eScrollDir, double fX, double fY, bool bAnyButtonPressed) noexcept
		: PointerScrollEvent(nTimeUsec, refAccessor, refPointerCapability, eScrollDir, fX, fY, bAnyButtonPressed)
		{
		}
		void reInit(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<PointerCapability>& refPointerCapability
					, POINTER_SCROLL_DIR eScrollDir, double fX, double fY, bool bAnyButtonPressed) noexcept
		{
			setTimeUsec(nTimeUsec);
			setAccessor(refAccessor);
			setPointerScroll(eScrollDir, bAnyButtonPressed);
			setPointerCapability(refPointerCapability);
			setX(fX);
			setY(fY);
			setIsModified(false);
		}
	};
	Private::Recycler<RePointerScrollEvent, Event> m_oPointerScrollEventRecycler;
	//
	class ReTouchEvent :public TouchEvent
	{
	public:
		ReTouchEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<TouchCapability>& refTouchCapability, TOUCH_INPUT_TYPE eType
					, double fX, double fY, int64_t nFingerId) noexcept
		: TouchEvent(nTimeUsec, refAccessor, refTouchCapability, eType, fX, fY, nFingerId)
		{
		}
		void reInit(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<TouchCapability>& refTouchCapability, TOUCH_INPUT_TYPE eType
					, double fX, double fY, int64_t nFingerId) noexcept
		{
			setTimeUsec(nTimeUsec);
			setAccessor(refAccessor);
			setTouchCapability(refTouchCapability);
			setTypeAndFinger(eType, nFingerId);
			setX(fX);
			setY(fY);
			setIsModified(false);
		}
	};
	Private::Recycler<ReTouchEvent, Event> m_oTouchEventRecycler;
private:
	GtkPointerDevice(const GtkPointerDevice& oSource) = delete;
	GtkPointerDevice& operator=(const GtkPointerDevice& oSource) = delete;
};

} // namespace Mas
} // namespace Private

} // namespace stmi

#endif /* STMI_MAS_GTK_POINTER_DEVICE_H */
