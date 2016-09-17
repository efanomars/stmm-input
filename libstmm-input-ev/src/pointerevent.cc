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
 * File:   pointerevent.cc
 */

#include "pointerevent.h"

namespace stmi
{

const char* const PointerEvent::s_sClassId = "stmi::Pointer:PointerEvent";
Event::RegisterClass<PointerEvent> PointerEvent::s_oInstall(s_sClassId);

PointerEvent::PointerEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
							, const shared_ptr<PointerCapability>& refPointerCapability, double fX, double fY
							, POINTER_INPUT_TYPE eType, int32_t nButton, bool bAnyButtonPressed, bool bWasAnyButtonPressed)
: XYEvent(s_oInstall.getEventClass(), nTimeUsec, (refPointerCapability ? refPointerCapability->getId() : -1)
			, refAccessor, fX, fY, XY_HOVER, PointerEvent::s_nPointerNotGrabbedId)
, m_eType(eType)
, m_nButton(nButton)
, m_refPointerCapability(refPointerCapability)
{
	setPointer(eType, nButton, bAnyButtonPressed, bWasAnyButtonPressed);
}
bool PointerEvent::getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const
{
	if (!((m_nButton >= s_nFirstButton) && (m_nButton <= s_nLastSimulatedButton))) {
		return false; //----------------------------
	}
	if (m_eType == BUTTON_PRESS) {
		eType = AS_KEY_PRESS;
	} else if (m_eType == BUTTON_RELEASE) {
		eType = AS_KEY_RELEASE;
	} else if (m_eType == BUTTON_RELEASE_CANCEL) {
		eType = AS_KEY_RELEASE_CANCEL;
	} else {
		return false; //----------------------------
	}

	static_assert(s_nFirstButton == 1, "");
	static_assert(s_nLastSimulatedButton <= 9, "");
	switch (m_nButton) {
		case 1: eKey = HK_BTN_LEFT; break;
		case 2: eKey = HK_BTN_RIGHT; break;
		case 3: eKey = HK_BTN_MIDDLE; break;
		default: eKey = static_cast<HARDWARE_KEY>(HK_BTN_0 + m_nButton);
	}
	bMoreThanOne = false;
	return true;
}

void PointerEvent::setPointer(POINTER_INPUT_TYPE eType, int32_t nButton, bool bAnyButtonPressed, bool bWasAnyButtonPressed)
{
	#ifndef NDEBUG
	{
		assert((eType >= BUTTON_PRESS) && (eType <= BUTTON_RELEASE_CANCEL));
		const bool bValidButton = (nButton >= s_nFirstButton);
		const bool bMoveOrHover = ((eType == POINTER_MOVE) || (eType == POINTER_HOVER));
		assert((bMoveOrHover && (nButton == s_nNoButton)) || ((!bMoveOrHover) && bValidButton));
	}
	#endif //NDEBUG
	m_eType = eType;
	m_nButton = nButton;
	XY_GRAB_INPUT_TYPE eXYGrabType = XY_HOVER;
	if (m_eType == BUTTON_PRESS) {
		if (!bWasAnyButtonPressed) {
			eXYGrabType = XY_GRAB;
		} else {
			assert(bAnyButtonPressed);
			eXYGrabType = XY_MOVE;
		}
	} else if (m_eType == BUTTON_RELEASE) {
		assert(bWasAnyButtonPressed);
		if (!bAnyButtonPressed) {
			eXYGrabType = XY_UNGRAB;
		} else {
			eXYGrabType = XY_MOVE;
		}
	} else if (m_eType == POINTER_MOVE) {
		assert(bAnyButtonPressed);
		assert(bWasAnyButtonPressed);
		eXYGrabType = XY_MOVE;
	} else if (m_eType == BUTTON_RELEASE_CANCEL) {
		assert(bWasAnyButtonPressed);
		if (!bAnyButtonPressed) {
			// It was the last button pressed
			eXYGrabType = XY_UNGRAB_CANCEL;
		} else {
			// If there are other buttons pressed the current grab remains active!
			eXYGrabType = XY_MOVE;
		}
	} else if (m_eType == POINTER_HOVER) {
		assert(!bAnyButtonPressed);
		assert(!bWasAnyButtonPressed);
		eXYGrabType = XY_HOVER;
	} else {
		assert(false);
	}
	if (eXYGrabType != XY_HOVER) {
		// 1 is the Grab Id for "any button"
		setXYGrab(eXYGrabType, s_nPointerGrabbedId);
	} else {
		setXYGrab(eXYGrabType, s_nPointerNotGrabbedId);
	}
}


////////////////////////////////////////////////////////////////////////////////
const char* const PointerScrollEvent::s_sClassId = "stmi::Pointer:PointerScrollEvent";
Event::RegisterClass<PointerScrollEvent> PointerScrollEvent::s_oInstall(s_sClassId);

PointerScrollEvent::PointerScrollEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
										, const shared_ptr<PointerCapability>& refPointerCapability
										, POINTER_SCROLL_DIR eScrollDir, double fX, double fY, bool bAnyButtonPressed)
: XYEvent(s_oInstall.getEventClass(), nTimeUsec, (refPointerCapability ? refPointerCapability->getId() : -1)
			, refAccessor, fX, fY, XY_HOVER, PointerEvent::s_nPointerNotGrabbedId)
, m_eScrollDir(eScrollDir)
, m_refPointerCapability(refPointerCapability)
{
//std::cout << "PointerScrollEvent::PointerScrollEvent   eScrollDir=" << m_eScrollDir << "  fX=" << fX << " fY=" << fY << std::endl;
	setPointerScroll(eScrollDir, bAnyButtonPressed);
}
bool PointerScrollEvent::getAsKey(HARDWARE_KEY& eKey, AS_KEY_INPUT_TYPE& eType, bool& bMoreThanOne) const
{
	switch (m_eScrollDir) {
	case SCROLL_UP: { eKey = HK_SCROLLUP; } break;
	case SCROLL_DOWN: { eKey = HK_SCROLLDOWN; } break;
	case SCROLL_LEFT: { eKey = HK_X_SCROLL_LEFT; } break;
	case SCROLL_RIGHT: { eKey = HK_X_SCROLL_RIGHT; } break;
	default: return false; //---------------------------------------------------
	}
	eType = Event::AS_KEY_PRESS;
	bMoreThanOne = true;
	return true;
}
std::vector< std::pair<HARDWARE_KEY, Event::AS_KEY_INPUT_TYPE> > PointerScrollEvent::getAsKeys() const
{
	HARDWARE_KEY eKey;
	AS_KEY_INPUT_TYPE eType;
	bool bMoreThanOne = false;
	const bool bOk = getAsKey(eKey, eType, bMoreThanOne);
	if (!bOk) {
		return {}; //-----------------------------------------------------------
	}
	return { std::make_pair(eKey, Event::AS_KEY_PRESS), std::make_pair(eKey, Event::AS_KEY_RELEASE) };
}

void PointerScrollEvent::setPointerScroll(POINTER_SCROLL_DIR eScrollDir, bool bAnyButtonPressed)
{
	assert((eScrollDir >= SCROLL_UP) && (eScrollDir <= SCROLL_RIGHT));
	m_eScrollDir = eScrollDir;
	if (bAnyButtonPressed) {
		setXYGrab(XY_MOVE, PointerEvent::s_nPointerGrabbedId);
	} else {
		setXYGrab(XY_HOVER, PointerEvent::s_nPointerNotGrabbedId);
	}
}

} // namespace stmi

