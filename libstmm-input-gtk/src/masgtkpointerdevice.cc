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
 * File:   masgtkpointerdevice.cc
 */

#include "masgtkpointerdevice.h"
#include "masgtkwindowdata.h"
#include "masgtklistenerextradata.h"

namespace stmi
{

namespace Private
{
namespace Mas
{

GtkPointerDevice::~GtkPointerDevice()
{
//std::cout << "GtkPointerDevice::~GtkPointerDevice() " << getId() << std::endl;
}
shared_ptr<Device> GtkPointerDevice::getDevice() const
{
	shared_ptr<const GtkPointerDevice> refConstThis = shared_from_this();
	shared_ptr<GtkPointerDevice> refThis = std::const_pointer_cast<GtkPointerDevice>(refConstThis);
	return refThis;
}
shared_ptr<Device> GtkPointerDevice::getDevice()
{
	shared_ptr<GtkPointerDevice> refThis = shared_from_this();
	return refThis;
}
shared_ptr<Capability> GtkPointerDevice::getCapability(const Capability::Class& oClass) const
{
	shared_ptr<Capability> refCapa;
	if (oClass == typeid(PointerCapability)) {
		shared_ptr<const GtkPointerDevice> refConstThis = shared_from_this();
		shared_ptr<GtkPointerDevice> refThis = std::const_pointer_cast<GtkPointerDevice>(refConstThis);
		refCapa = shared_ptr<PointerCapability>(refThis);
	} else if (oClass == typeid(TouchCapability)) {
		shared_ptr<const GtkPointerDevice> refConstThis = shared_from_this();
		shared_ptr<GtkPointerDevice> refThis = std::const_pointer_cast<GtkPointerDevice>(refConstThis);
		refCapa = shared_ptr<TouchCapability>(refThis);
	}
	return refCapa;
}
std::vector<Capability::Class> GtkPointerDevice::getCapabilityClasses() const
{
	return {PointerCapability::getClass(), TouchCapability::getClass()};
}
bool GtkPointerDevice::handleGdkEventMotion(GdkEventMotion* p0MotionEv, const shared_ptr<GtkWindowData>& refWindowData)
{
	const bool bContinue = true;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return !bContinue; //---------------------------------------------------
	}
	const GdkWindow* p0AccessorGdkWindow = refWindowData->getWindowDataGdkWindow();
	const GdkWindow* p0GdkWindow = p0MotionEv->window;
	if (p0GdkWindow != p0AccessorGdkWindow) {
		// This device manager suppresses events sent to a modal Gtk::Dialog
		// that relate to the parent
		return bContinue; //----------------------------------------------------
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	if (p0Owner->m_refSelected != refWindowData) {
		// This device manager suppresses events sent to a not active window
		// (ex. the parent of a modal dialog)
		return bContinue; //----------------------------------------------------
	}
	auto refListeners = p0Owner->getListeners();
	#ifndef NDEBUG
	const GdkEventType eGdkType = p0MotionEv->type;
	#endif //NDEBUG
	assert(eGdkType == GDK_MOTION_NOTIFY);
	const double fX = p0MotionEv->x;
	const double fY = p0MotionEv->y;
	m_fLastPointerX = fX;
	m_fLastPointerY = fY;
	const int32_t nButton = PointerEvent::s_nNoButton;
	const bool bWasAnyButtonPressed = !m_aButtons.empty();
	const bool bAnyButtonPressed = bWasAnyButtonPressed; // GdkEventMotion doesn't change the button status
	const PointerEvent::POINTER_INPUT_TYPE eInputType = (bAnyButtonPressed ? PointerEvent::POINTER_MOVE : PointerEvent::POINTER_HOVER);
	//
	auto refSaveAccessor = refWindowData->getAccessor(); // might be removed in callbacks
	shared_ptr<PointerEvent> refEvent;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& p0ListenerData : *refListeners) {
		sendPointerEventToListener(*p0ListenerData, nEventTimeUsec, fX, fY, eInputType, nButton
									, bWasAnyButtonPressed, bAnyButtonPressed, refSaveAccessor, p0Owner
									, refEvent);
	}
	return bContinue;
}
bool GtkPointerDevice::handleGdkEventButton(GdkEventButton* p0ButtonEv, const shared_ptr<GtkWindowData>& refWindowData)
{
	const bool bContinue = true;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return !bContinue; //---------------------------------------------------
	}
	const GdkWindow* p0AccessorGdkWindow = refWindowData->getWindowDataGdkWindow();
	const GdkWindow* p0GdkWindow = p0ButtonEv->window;
	if (p0GdkWindow != p0AccessorGdkWindow) {
		// This device manager suppresses events sent to a modal Gtk::Dialog
		// that relate to the parent
		return bContinue; //----------------------------------------------------
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	if (p0Owner->m_refSelected != refWindowData) {
		// This device manager suppresses events sent to a not active window.
		//TODO There probably should be an assert(false) here
		return bContinue; //----------------------------------------------------
	}
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds(); //((int64_t)p0ButtonEv->time) * 1000;
	const GdkEventType eGdkType = p0ButtonEv->type;
	const bool bIsButtonPress = (eGdkType == GDK_BUTTON_PRESS);
	if (! (bIsButtonPress || (eGdkType == GDK_BUTTON_RELEASE))) {
		// ignore gdk double or triple clicks
		return bContinue; //----------------------------------------------------
	}
	const double fX = p0ButtonEv->x;
	const double fY = p0ButtonEv->y;
	m_fLastPointerX = fX;
	m_fLastPointerY = fY;
	const int32_t nGdkButton = p0ButtonEv->button;
	assert(nGdkButton >= 1);
	PointerEvent::POINTER_INPUT_TYPE eInputType;
	//static const int32_t nGdkButtonsMask = GDK_BUTTON1_MASK | GDK_BUTTON2_MASK | GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK;
	//const int32_t nGdkButtonsState = (eGdkModifier & nGdkButtonsMask);
	//assert((nGdkButton >= 1) && (nGdkButton <= 5));
	const int32_t nButton = translateGdkButtonToPointerEvent(nGdkButton);
	bool bWasAnyButtonPressed = !m_aButtons.empty();
	bool bAnyButtonPressed = true;
	auto itFind = std::find(m_aButtons.begin(), m_aButtons.end(), nButton);
	bool bTheButtonWasPressed = (itFind != m_aButtons.end());
	auto refWindowAccessor = refWindowData->getAccessor();
//std::cout << "GtkPointerDevice::handleGdkEventButton  nButton=" << nButton << "  bTheButtonWasPressed=" << bTheButtonWasPressed << "  bIsButtonPress=" << bIsButtonPress << std::endl;
	if (bIsButtonPress) {
		if (bTheButtonWasPressed) {
			// Some error happened, press without release, ignore.
			//TODO Maybe should send a cancel for the lost release instead!
			return bContinue; //------------------------------------------------
		}
		m_aButtons.push_back(nButton);
		//
		eInputType = PointerEvent::BUTTON_PRESS;
	} else { // GDK_BUTTON_RELEASE
		if (!bTheButtonWasPressed) {
			// orphan release, ignore it
			return bContinue; //------------------------------------------------
		}
		const int32_t nTotButtons = m_aButtons.size();
		// Is any other button pressed after the release?
		bAnyButtonPressed = (nTotButtons > 1);
		// erase the button
		if (bAnyButtonPressed) {
			*itFind = m_aButtons.back();
		}
		m_aButtons.pop_back();
		//
		eInputType = PointerEvent::BUTTON_RELEASE;
	}
	if (bAnyButtonPressed && !bWasAnyButtonPressed) {
		// from no-buttons-pressed to a-button-pressed transition
		m_nAnyButtonPressTimeUsec = nTimeUsec;
	}
	//
	shared_ptr<PointerEvent> refEvent;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	auto refListeners = p0Owner->getListeners();
	for (auto& p0ListenerData : *refListeners) {
		sendPointerEventToListener(*p0ListenerData, nEventTimeUsec, fX, fY, eInputType, nButton
									, bWasAnyButtonPressed, bAnyButtonPressed, refWindowAccessor, p0Owner
									, refEvent);
		if ((eInputType == PointerEvent::BUTTON_PRESS) 
					&& (std::find(m_aButtons.begin(), m_aButtons.end(), nButton) == m_aButtons.end())) {
			// The button was canceled by the callback (ex. removeAccessor was called)
			break; // for -------
		}
	}
	if (!bAnyButtonPressed) {
		// from a-button-pressed to no-buttons-pressed transition
		m_nAnyButtonPressTimeUsec = std::numeric_limits<int64_t>::max();
	}
	return bContinue;
}
bool GtkPointerDevice::handleGdkEventScroll(GdkEventScroll* p0ScrollEv, const shared_ptr<GtkWindowData>& refWindowData)
{
	const bool bContinue = true;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return !bContinue;
	}
	const GdkWindow* p0AccessorGdkWindow = refWindowData->getWindowDataGdkWindow();
	const GdkWindow* p0GdkWindow = p0ScrollEv->window;
	if (p0GdkWindow != p0AccessorGdkWindow) {
		// This device manager suppresses events sent to a modal Gtk::Dialog
		// that relate to the parent
		return bContinue; //----------------------------------------------------
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	if (p0Owner->m_refSelected != refWindowData) {
		// This device manager suppresses events sent to a not active window.
		//TODO There probably should be an assert(false) here
		return bContinue; //----------------------------------------------------
	}
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	#ifndef NDEBUG
	const GdkEventType eGdkType = p0ScrollEv->type;
	#endif //NDEBUG
	assert(eGdkType == GDK_SCROLL);
	const double fX = p0ScrollEv->x;
	const double fY = p0ScrollEv->y;
	GdkScrollDirection eGdkDir = p0ScrollEv->direction;
	if (eGdkDir == GDK_SCROLL_SMOOTH) {
		//TODO Create PointerSmoothScrollEvent
		return bContinue; //----------------------------------------------------
	}
	PointerScrollEvent::POINTER_SCROLL_DIR eScrollDir;
	switch (eGdkDir) {
		case GDK_SCROLL_UP: eScrollDir = PointerScrollEvent::SCROLL_UP; break;
		case GDK_SCROLL_DOWN: eScrollDir = PointerScrollEvent::SCROLL_DOWN; break;
		case GDK_SCROLL_LEFT: eScrollDir = PointerScrollEvent::SCROLL_LEFT; break;
		case GDK_SCROLL_RIGHT: eScrollDir = PointerScrollEvent::SCROLL_RIGHT; break;
		default: return bContinue; //---------------------------------------
	}
	shared_ptr<PointerScrollEvent> refEvent;
	auto refSaveAccessor = refWindowData->getAccessor();
	auto refListeners = p0Owner->getListeners();
	for (auto& p0ListenerData : *refListeners) {
		const int64_t nAddTimeUsec = p0ListenerData->getAddedTime();
		if (m_nAnyButtonPressTimeUsec < nAddTimeUsec) {
			// The listener was added after the last no-button to a-button pressed transition.
			// As for movement events a newly added listener starts to receive scroll events 
			// only after all buttons are released (the pointer is ungrabbed)
			continue; // for --------
		}
		if (!refEvent) {
			const bool bAnyButtonPressed = !m_aButtons.empty();
			refEvent = std::make_shared<PointerScrollEvent>(nTimeUsec, refSaveAccessor, p0Owner->m_refPointerDevice, eScrollDir
															, fX, fY, bAnyButtonPressed);
		}
		const bool bSent = p0ListenerData->handleEventCallIf(p0Owner->m_nClassIdxPointerScrollEvent, refEvent);
		if (bSent) {
			if (refEvent->getIsModified() || !refEvent.unique()) {
				// If the event was modified can't reuse it for the next listener.
				// If the event is referenced by another shared_ptr (ex. an event queue), can't reuse either.
				//TODO RECYCLING!? (needs subclassing)
				refEvent.reset();
			}
		}
	}
	return bContinue;
}
bool GtkPointerDevice::handleGdkEventTouch(GdkEventTouch* p0TouchEv, const shared_ptr<GtkWindowData>& refWindowData)
{
	const bool bContinue = true;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return !bContinue; //---------------------------------------------------
	}
	const GdkWindow* p0AccessorGdkWindow = refWindowData->getWindowDataGdkWindow();
	const GdkWindow* p0GdkWindow = p0TouchEv->window;
	if (p0GdkWindow != p0AccessorGdkWindow) {
		// This device manager suppresses events sent to a modal Gtk::Dialog
		// that relate to the parent
		return bContinue; //----------------------------------------------------
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	if (p0Owner->m_refSelected != refWindowData) {
		// This device manager suppresses events sent to a not active window.
		//TODO There probably should be an assert(false) here
		return bContinue; //----------------------------------------------------
	}
	auto refListeners = p0Owner->getListeners();
	const GdkEventType eGdkType = p0TouchEv->type;

	const auto fLastX = p0TouchEv->x;
	const auto fLastY = p0TouchEv->y;	
	GdkEventSequence* p0Sequence = p0TouchEv->sequence;
	TouchEvent::TOUCH_INPUT_TYPE eType;
	switch (eGdkType) {
		case GDK_TOUCH_BEGIN: eType = TouchEvent::TOUCH_BEGIN; break;
		case GDK_TOUCH_UPDATE: eType = TouchEvent::TOUCH_UPDATE; break;
		case GDK_TOUCH_END: eType = TouchEvent::TOUCH_END; break;
		case GDK_TOUCH_CANCEL: eType = TouchEvent::TOUCH_CANCEL; break;
		default: return bContinue; //-------------------------------------------
	}
	int64_t nSequenceStartTime = std::numeric_limits<int64_t>::max();
	auto itFind = m_oSequences.find(p0Sequence);
	const bool bSequenceExists = (itFind != m_oSequences.end());
	if (eType == TouchEvent::TOUCH_BEGIN) {
		if (bSequenceExists) {
			SequenceData& oSequenceData = itFind->second;
			// TOUCH_END missing + the sequence was probably recycled or GTK weirdness
			// Send TOUCH_CANCEL for old accessor before sending TOUCH_BEGIN for new one
			const auto nSequenceStartTime = oSequenceData.m_nTouchStartTimeUsec;
			const auto fLastX = oSequenceData.m_fLastX;
			const auto fLastY = oSequenceData.m_fLastY;
			auto refSaveAccessor = refWindowData->getAccessor();
			m_oSequences.erase(itFind);
			shared_ptr<TouchEvent> refEvent;
			const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
			for (auto& p0ListenerData : *refListeners) {
				sendTouchEventToListener(*p0ListenerData, nEventTimeUsec, nSequenceStartTime
										, fLastX, fLastY, TouchEvent::TOUCH_CANCEL, (int64_t)p0Sequence
										, refSaveAccessor, p0Owner, refEvent);
			}
			if (!refWindowData->isEnabled()) {
				// The accessor was removed during the callbacks!
				return bContinue; //--------------------------------------------
			}
		}
		const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
		nSequenceStartTime = nTimeUsec;
		SequenceData oSequenceData;
		oSequenceData.m_nTouchStartTimeUsec = nSequenceStartTime;
		oSequenceData.m_fLastX = fLastX;
		oSequenceData.m_fLastY = fLastY;
		m_oSequences.emplace(p0Sequence, oSequenceData);
	} else {
		if (!bSequenceExists) {
			// Missing TOUCH_BEGIN, ignore!
			return bContinue; //------------------------------------------------
		}
		SequenceData& oSequenceData = itFind->second;
		nSequenceStartTime = oSequenceData.m_nTouchStartTimeUsec;
		if (eType != TouchEvent::TOUCH_UPDATE) {
			m_oSequences.erase(itFind);
		}
	}
//std::cout << "GtkPointerDevice::handleGdkEventTouch 8" << std::endl;
	auto refSaveAccessor = refWindowData->getAccessor();
	shared_ptr<TouchEvent> refEvent;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& p0ListenerData : *refListeners) {
		sendTouchEventToListener(*p0ListenerData, nEventTimeUsec, nSequenceStartTime, fLastX, fLastY, eType, (int64_t)p0Sequence
								, refSaveAccessor, p0Owner, refEvent);
		if ( ((eType == TouchEvent::TOUCH_UPDATE) || (eType == TouchEvent::TOUCH_BEGIN))
					&& (m_oSequences.find(p0Sequence) == m_oSequences.end())) {
			// The sequence was canceled by the callback
			break; // for -------
		}
	}
//std::cout << "GtkPointerDevice::handleGdkEventTouch 9" << std::endl;
	return bContinue;
}
void GtkPointerDevice::finalizeListener(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec)
{
//std::cout << "GtkPointerDevice::finalizeListener" << std::endl;
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; //--------------------------------------------------------------
	}
	// Send XXX_CANCEL for the currently pressed buttons and open touch sequences to the listener
	if (p0Owner->getEventClassEnabled(typeid(PointerEvent))) {
		finalizeListenerButton(oListenerData, nEventTimeUsec, p0Owner);
	}
	if (p0Owner->getEventClassEnabled(typeid(TouchEvent))) {
		finalizeListenerTouch(oListenerData, nEventTimeUsec, p0Owner);
	}
}
void GtkPointerDevice::finalizeListenerButton(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec, MasGtkDeviceManager* p0Owner)
{
//std::cout << "GtkPointerDevice::finalizeListenerButton" << std::endl;
	// Note: Gdk (X11?) ensures that when a button is pressed the whole pointer is grabbed for the given window
	//       until all buttons are released!
	const int64_t nAddTimeUsec = oListenerData.getAddedTime();
	if (m_nAnyButtonPressTimeUsec < nAddTimeUsec) {
		// The listener was added after the last no-button to a-button pressed transition
		return;
	}
	auto refSelectedAccessor = p0Owner->m_refSelected->getAccessor();

	MasGtkListenerExtraData* p0ExtraData = nullptr;
	oListenerData.getExtraData(p0ExtraData);

	// work on copy
	auto aButtons = m_aButtons;

	const int32_t nTotPressedButtons = aButtons.size();
	int32_t nPressedButtonCount = 0;
	for (auto& nButton : aButtons) {
		++nPressedButtonCount;
		if (p0ExtraData->isButtonCanceled(nButton)) {
			continue; // for ----------
		}
		p0ExtraData->setButtonCanceled(nButton);
//std::cout << "   finalizing  nButton=" << nButton << std::endl;
		// make sure the last one simulates a XYGrabEvent::XY_UNGRAB_CANCEL
		// by transitioning from a-button-is-pressed to no-buttons-are-pressed
		const bool bAnyButtonPressed = (nPressedButtonCount < nTotPressedButtons);
		//TODO RECYCLING! needs subclassing of PointerEvent with public setButton + setPointer
		shared_ptr<PointerEvent> refEvent;
		sendPointerEventToListener(oListenerData, nEventTimeUsec, m_fLastPointerX, m_fLastPointerY
								, PointerEvent::BUTTON_RELEASE_CANCEL, nButton, true, bAnyButtonPressed
								, refSelectedAccessor, p0Owner, refEvent);
	}
}
void GtkPointerDevice::finalizeListenerTouch(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec, MasGtkDeviceManager* p0Owner)
{
//std::cout << "GtkPointerDevice::finalizeListenerTouch" << std::endl;
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; //--------------------------------------------------------------
	}
	auto refSelectedAccessor = refSelected->getAccessor();

	MasGtkListenerExtraData* p0ExtraData = nullptr;
	oListenerData.getExtraData(p0ExtraData);
	// work on copy
	auto oSequences = m_oSequences;
	for (auto& oPair : oSequences) {
		const GdkEventSequence* p0Sequence = oPair.first;
		const SequenceData& oSequenceData = oPair.second;
		//
		const int64_t& nTouchStartTimeUsec = oSequenceData.m_nTouchStartTimeUsec;
		const auto& fLastX = oSequenceData.m_fLastX;
		const auto& fLastY = oSequenceData.m_fLastY;
		//
		if (p0ExtraData->isSequenceCanceled(p0Sequence)) {
			continue; // for ------------
		}
		p0ExtraData->setSequenceCanceled(p0Sequence);
		//TODO RECYCLING!
		shared_ptr<TouchEvent> refEvent;
		sendTouchEventToListener(oListenerData, nEventTimeUsec, nTouchStartTimeUsec
								, fLastX, fLastY, TouchEvent::TOUCH_CANCEL, (int64_t)p0Sequence
								, refSelectedAccessor, p0Owner, refEvent);
	}
}
void GtkPointerDevice::removingDevice()
{
	resetOwnerDeviceManager();
}
void GtkPointerDevice::cancelSelectedAccessorButtonsAndSequences()
{
	auto refOwner = getOwnerDeviceManager();
	if (!refOwner) {
		return;
	}
	MasGtkDeviceManager* p0Owner = refOwner.get();
	auto& refSelected = p0Owner->m_refSelected;
	if (!refSelected) {
		return; // -------------------------------------------------------------
	}
	auto refSelectedAccessor = refSelected->getAccessor();
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();

	auto refListeners = p0Owner->getListeners();
	//
	if (p0Owner->getEventClassEnabled(typeid(PointerEvent))) {
		cancelSelectedAccessorButtons(refListeners, nEventTimeUsec, refSelectedAccessor, p0Owner);
	}
	if (p0Owner->getEventClassEnabled(typeid(TouchEvent))) {
		cancelSelectedAccessorSequences(refListeners, nEventTimeUsec, refSelectedAccessor, p0Owner);
	}
}
void GtkPointerDevice::cancelSelectedAccessorButtons(const shared_ptr< const std::list< MasGtkDeviceManager::ListenerData* > >& refListeners
													, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
													, MasGtkDeviceManager* p0Owner)
{
	// work on copy
	auto aButtons = m_aButtons;
	// cancel pressed buttons
	shared_ptr<PointerEvent> refEvent;
	bool bNoneCanceled = false;
	while (!bNoneCanceled) {
		bNoneCanceled = true;
		//
		for (auto& p0ListenerData : *refListeners) {
			MasGtkListenerExtraData* p0ExtraData = nullptr;
			p0ListenerData->getExtraData(p0ExtraData);
			const int32_t nTotNotCanceledYet = std::count_if(aButtons.begin(), aButtons.end()
									, [p0ExtraData](int32_t nButton){ return !p0ExtraData->isButtonCanceled(nButton); });
			if (nTotNotCanceledYet == 0) {
				continue; // for ---------
			}
			// find first not yet canceled button
			auto itFind = std::find_if(aButtons.begin(), aButtons.end()
									, [p0ExtraData](int32_t nButton){ return !p0ExtraData->isButtonCanceled(nButton); });
			assert(itFind != aButtons.end());
			const int32_t nButton = *itFind;

			p0ExtraData->setButtonCanceled(nButton);
			const bool bAnyButtonPressed = (nTotNotCanceledYet > 1);
			sendPointerEventToListener(*p0ListenerData, nEventTimeUsec, m_fLastPointerX, m_fLastPointerY
									, PointerEvent::BUTTON_RELEASE_CANCEL, nButton, true, bAnyButtonPressed
									, refSelectedAccessor, p0Owner, refEvent);
			bNoneCanceled = false;
		}
	}
	// remove buttons
	m_aButtons.clear();
	m_nAnyButtonPressTimeUsec = std::numeric_limits<int64_t>::max();
}
void GtkPointerDevice::cancelSelectedAccessorSequences(const shared_ptr< const std::list< MasGtkDeviceManager::ListenerData* > >& refListeners
														, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
														, MasGtkDeviceManager* p0Owner)
{
	// Cancel the touch sequences generated with the to be removed accessor.
	// work on copy
	auto oSequences = m_oSequences;
	shared_ptr<TouchEvent> refEvent;
	for (auto& oPair : oSequences) {
		const GdkEventSequence* p0Sequence = oPair.first;
		const SequenceData& oSeqData = oPair.second;
		for (auto& p0ListenerData : *refListeners) {
			MasGtkListenerExtraData* p0ExtraData = nullptr;
			p0ListenerData->getExtraData(p0ExtraData);
			if (p0ExtraData->isSequenceCanceled(p0Sequence)) {
				continue; // for p0ListenerData ------------
			}
			p0ExtraData->setSequenceCanceled(p0Sequence);
			sendTouchEventToListener(*p0ListenerData, nEventTimeUsec, oSeqData.m_nTouchStartTimeUsec
									, 0, 0, TouchEvent::TOUCH_CANCEL, (int64_t)p0Sequence
									, refSelectedAccessor, p0Owner, refEvent);
		}
	}
	// remove sequences
	m_oSequences.clear();
}
void GtkPointerDevice::sendPointerEventToListener(
											const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
											, double fX, double fY
											, PointerEvent::POINTER_INPUT_TYPE eInputType, int32_t nButton
											, bool bWasAnyButtonPressed, bool bAnyButtonPressed
											, const shared_ptr<GtkAccessor>& refAccessor
											, MasGtkDeviceManager* p0Owner
											, shared_ptr<PointerEvent>& refEvent)
{
	const int64_t nAddTimeUsec = oListenerData.getAddedTime();
	if (m_nAnyButtonPressTimeUsec < nAddTimeUsec) {
		// The listener was added after the last no-button to a-button pressed transition
		return;
	}
	if (!refEvent) {
		refEvent = std::make_shared<PointerEvent>(nEventTimeUsec, refAccessor, p0Owner->m_refPointerDevice, fX, fY
												, eInputType, nButton, bAnyButtonPressed, bWasAnyButtonPressed);
	}
	const bool bSent = oListenerData.handleEventCallIf(p0Owner->m_nClassIdxPointerEvent, refEvent);
	if (bSent) {
		if (refEvent->getIsModified() || !refEvent.unique()) {
			// If the event was modified can't reuse it for the next listener.
			// If the event is referenced by another shared_ptr (ex. an event queue), can't reuse either.
			// TODO RECYCLING!? (needs subclassing)
			refEvent.reset();
		}
	}
}
void GtkPointerDevice::sendTouchEventToListener(
											const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
											, int64_t nSequenceStartTime, double fX, double fY
											, TouchEvent::TOUCH_INPUT_TYPE eInputType, int64_t nSequence
											, const shared_ptr<GtkAccessor>& refAccessor
											, MasGtkDeviceManager* p0Owner
											, shared_ptr<TouchEvent>& refEvent)
{
	const int64_t nAddTimeUsec = oListenerData.getAddedTime();
	if (nSequenceStartTime < nAddTimeUsec) {
		// The listener was added after a touch was started, so until the touch is ended or canceled
		// it's deaf to any of its event
		return;
	}
	if (!refEvent) {
		refEvent = std::make_shared<TouchEvent>(nEventTimeUsec, refAccessor, p0Owner->m_refPointerDevice
												, eInputType, fX, fY, nSequence);
	}
	const bool bSent = oListenerData.handleEventCallIf(p0Owner->m_nClassIdxTouchEvent, refEvent);
	if (bSent) {
		if (refEvent->getIsModified() || !refEvent.unique()) {
			// If the event was modified can't reuse it for the next listener.
			// If the event is referenced by another shared_ptr (ex. an event queue), can't reuse either, it might change later.
			// TODO RECYCLING!? (needs subclassing)
			refEvent.reset();
		}
	}
}

} // namespace Mas
} // namespace Private

} // namespace stmi
