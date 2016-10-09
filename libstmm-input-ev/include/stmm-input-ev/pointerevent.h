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
 * File:   pointerevent.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef _STMI_POINTER_EVENT_H_
#define _STMI_POINTER_EVENT_H_

#include "pointercapability.h"

#include <stmm-input/xyevent.h>

#include <cassert>

namespace stmi
{

using std::weak_ptr;

/** Event generated when the button state or the position of a pointer is changed.
 * Note that the reference to the capability that generated this event is weak.
 */
class PointerEvent : public XYEvent
{
public:
	/** The type of pointer event. */
	enum POINTER_INPUT_TYPE {
		BUTTON_PRESS = 1 /**< A button is pressed. */
		, BUTTON_RELEASE = 2 /**< A button is released. */
		, POINTER_MOVE = 3 /**< Pointer is moved (while at least one button is pressed) */
		, POINTER_HOVER = 4 /**< Pointer is hovered (while no button is pressed). */
		, BUTTON_RELEASE_CANCEL = 5 /**< A button release is canceled. */
	};
	/** Constructor.
	 * When moving or hovering the pointer, the passed button must be `-1`.
	 *
	 * The grab state of the pointer is defined using `bAnyButtonPressed` and
	 * `bWasAnyButtonPressed`, see XYEvent::getXYGrabType(). If the grab state
	 * is XY_HOVER then XYEvent::getXYGrabId() returns `-1`, `1` otherwise.
	 * @param nTimeUsec Time from epoch in microseconds.
	 * @param refAccessor The accessor used to generate the event. Can be null.
	 * @param refPointerCapability The capability that generated this event. Cannot be null.
	 * @param fX The X position of the pointer.
	 * @param fY The Y position of the pointer.
	 * @param eType The type of pointer event.
	 * @param nButton The button (`>= 1`) or `-1`.
	 * @param bAnyButtonPressed Whether any button is pressed.
	 * @param bWasAnyButtonPressed Whether any button was pressed before this event.
	 */
	PointerEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
				, const shared_ptr<PointerCapability>& refPointerCapability, double fX, double fY
				, POINTER_INPUT_TYPE eType, int32_t nButton, bool bAnyButtonPressed, bool bWasAnyButtonPressed);
	/** The type of event. */
	inline POINTER_INPUT_TYPE getType() const { return m_eType; }
	/** The button involved.
	 * @return The button or `-1` if hovering or moving the pointer.
	 */
	inline int32_t getButton() const { return m_nButton; }
	/** The pointer capability that generated the event.
	 * @return The capability or null if the capability was deleted.
	 */
	inline shared_ptr<PointerCapability> getPointerCapability() const { return m_refPointerCapability.lock(); }
	//
	shared_ptr<Capability> getCapability() const override { return m_refPointerCapability.lock(); }
	/** If not hovering or moving returns the simulated keystroke.
	 *
	 * - Button 1 simulates key HK_BTN_LEFT.
	 * - Button 2 simulates key HK_BTN_RIGHT.
	 * - Button 3 simulates key HK_BTN_MIDDLE.
	 * - Buttons 4 to 9 simulate keys from HK_BTN_4 to HK_BTN_9.
	 * - Buttons from 10 are not simulated with a key.
	 *
	 * The event type BUTTON_PRESS simulates AS_KEY_PRESS, BUTTON_RELEASE simulates AS_KEY_RELEASE
	 * and BUTTON_RELEASE_CANCEL simulates AS_KEY_RELEASE_CANCEL. 
	 * @param eKey The simulated key.
	 * @param eType The type of simulated key action.
	 * @param bMoreThanOne Always `false`.
	 * @return Whether the pointer event type simulates a key.
	 */
	bool getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const override;
	//
	static constexpr int32_t s_nFirstButton = 1; /**< Valid button numbers start from 1. */
	static constexpr int32_t s_nLastSimulatedButton = 9; /**< Last simulated button number. */
	static constexpr int32_t s_nNoButton = -1; /**< Value returned by getButton() when hovering or moving. */
	//
	static constexpr int64_t s_nPointerGrabbedId = 1; /**< Means at least one button is pressed. */
	static constexpr int64_t s_nPointerNotGrabbedId = -1; /**< Means no button is pressed (POINTER_HOVER). */
	//
	static const char* const s_sClassId;
	static const Event::Class& getClass()
	{
		static const Event::Class s_oPointerClass = s_oInstall.getEventClass();
		return s_oPointerClass;
	}
protected:
	/** Sets pointer and grab fields.
	 * This method also calls XYEvent::setXYGrab().
	 * @param eType The type of pointer event.
	 * @param nButton The button (`>= 1`) or `-1`.
	 * @param bAnyButtonPressed Whether any button is pressed.
	 * @param bWasAnyButtonPressed Whether any button was pressed before this event.
	 */
	void setPointer(POINTER_INPUT_TYPE eType, int32_t nButton, bool bAnyButtonPressed, bool bWasAnyButtonPressed);
	/** Sets the capability.
	 * @param refPointerCapability The capability that generated this event. Cannot be null.
	 */
	inline void setPointerCapability(const shared_ptr<PointerCapability>& refPointerCapability)
	{
		assert(refPointerCapability);
		setCapabilityId(refPointerCapability->getId());
		m_refPointerCapability = refPointerCapability;
	}
private:
	POINTER_INPUT_TYPE m_eType;
	int32_t m_nButton;
	weak_ptr<PointerCapability> m_refPointerCapability;
	//
	static RegisterClass<PointerEvent> s_oInstall;
private:
	PointerEvent() = delete;
};


////////////////////////////////////////////////////////////////////////////////
/** Event generated when scrolling the pointer.
 * Note that the reference to the capability that generated this event is weak.
 */
class PointerScrollEvent : public XYEvent
{
public:
	/** The direction of the scroll. */
	enum POINTER_SCROLL_DIR {
		SCROLL_UP = 1
		, SCROLL_DOWN = 2
		, SCROLL_LEFT = 3
		, SCROLL_RIGHT = 4
	};
	/** Constructor.
	 * Parameter bAnyButtonPressed is used to determine the current grab state.
	 * @param nTimeUsec Time from epoch in microseconds.
	 * @param refAccessor The accessor used to generate the event. Can be null.
	 * @param refPointerCapability The capability that generated this event. Cannot be null.
	 * @param eScrollDir The direction of the scroll.
	 * @param fX The X position of the pointer.
	 * @param fY The Y position of the pointer.
	 * @param bAnyButtonPressed Whether any button is pressed.
	 */
	PointerScrollEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
						, const shared_ptr<PointerCapability>& refPointerCapability
						, POINTER_SCROLL_DIR eScrollDir, double fX, double fY, bool bAnyButtonPressed);
	/** The direction of the scroll. */
	inline POINTER_SCROLL_DIR getScrollDir() const { return m_eScrollDir; }
	/** The pointer capability that generated the event.
	 * @return The capability or null if the capability was deleted.
	 */
	inline shared_ptr<PointerCapability> getPointerCapability() const { return m_refPointerCapability.lock(); }
	//
	shared_ptr<Capability> getCapability() const override { return m_refPointerCapability.lock(); }
	/** The first (press) of two simulated key actions (press and release).
	 * To get both actions call getAsKeys().
	 * The simulated keys are stmi::HK_SCROLLUP, stmi::HK_SCROLLDOWN,
	 * stmi::HK_X_SCROLL_LEFT, stmi::HK_X_SCROLL_RIGHT.
	 * @param eKey The key that is simulated by this event.
	 * @param eType Always `AS_KEY_PRESS`.
	 * @param bMoreThanOne Always `true`
	 * @return Always `true`.
	 */
	bool getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const override;
	/** The simulated keys (press and release).
	 * - PointerScrollEvent::SCROLL_UP simulates key stmi::HK_SCROLLUP.
	 * - PointerScrollEvent::SCROLL_DOWN simulates key stmi::HK_SCROLLDOWN.
	 * - PointerScrollEvent::SCROLL_LEFT simulates key stmi::HK_X_SCROLL_LEFT.
	 * - PointerScrollEvent::SCROLL_RIGHT simulates key stmi::HK_X_SCROLL_RIGHT.
	 *
	 * The returned vector is `vector{ pair(key, AS_KEY_PRESS), pair(key, AS_KEY_RELEASE) }`.
	 * @return A press and a release of the simulated key.
	 */
	std::vector< std::pair<HARDWARE_KEY, AS_KEY_INPUT_TYPE> > getAsKeys() const override;
	//
	static const char* const s_sClassId;
	static const Event::Class& getClass()
	{
		static const Event::Class s_oPonterScrollClass = s_oInstall.getEventClass();
		return s_oPonterScrollClass;
	}
protected:
	/** Sets scroll and grab fields.
	 * This method also calls XYEvent::setXYGrab().
	 * @param eScrollDir The direction of the scroll.
	 * @param bAnyButtonPressed Whether any button is pressed.
	 */
	void setPointerScroll(POINTER_SCROLL_DIR eScrollDir, bool bAnyButtonPressed);
	/** Sets the capability.
	 * @param refPointerCapability The capability that generated this event. Cannot be null.
	 */
	inline void setPointerCapability(const shared_ptr<PointerCapability>& refPointerCapability)
	{
		assert(refPointerCapability);
		setCapabilityId(refPointerCapability->getId());
		m_refPointerCapability = refPointerCapability;
	}
private:
	POINTER_SCROLL_DIR m_eScrollDir;
	weak_ptr<PointerCapability> m_refPointerCapability;
	//
	static RegisterClass<PointerScrollEvent> s_oInstall;
private:
	PointerScrollEvent() = delete;
};

} // namespace stmi

#endif	/* _STMI_POINTER_EVENT_H_ */

