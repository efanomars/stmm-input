/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   xyevent.h
 */

#ifndef _STMI_XY_EVENT_H_
#define _STMI_XY_EVENT_H_

#include "event.h"

#include <cassert>

namespace stmi
{

/** Base class for pointer-like events that hold coordinates.
 * This class provides an interface (that is independent from the actual
 * registered event class) for grabbing a pointer (mouse, touch finger, etc.).
 * It also allows to change the coordinates of the event.
 *
 * If a subclass of XYEvent has, in addition to (fX,fY), more coordinates (fX2,fY2), (fX3,fY3), ...
 * it should keep (fX,fY) as the main coordinates, and override translateXY(double fDX, double fDY)
 * and scaleXY(double fSX, double fSY) to change the additional coordinates and call
 * `setIsModified(true)` to mark the instance as modified.
 *
 * Usage example within a callback:
 *
 *     void SomeListener::handleEvent(const std::shared_ptr<stmi::Event>& refEvent) {
 *         ...
 *         const stmi::Event::Class& oC = refEvent->getEventClass();
 *         if (oC.isXYEvent()) {
 *             auto pXYEvent = static_cast<stmi::XYEvent*>(refEvent);
 *             if (pXYEvent->getX() >= 100) {
 *                 pXYEvent->translateXY(-100, 0);
 *                 pXYEvent->scaleXY(0.5, 0.5);
 *         ...
 *     }
 */
class XYEvent : public Event
{
public:
	/** The main X coordinate.
	 * @return X coordinate.
	 */
	inline double getX() const { return m_fX; }
	/** The main Y coordinate.
	 * @return Y coordinate.
	 */
	inline double getY() const { return m_fY; }
	/** Whether the event coordinates were modified.
	 * Tells whether `translateXY(double, double)` or `scaleXY(double, double)` were
	 * called and modified the event's coordinates.
	 * @return The modified state.
	 */
	inline bool getIsModified() const { return m_bIsModified; }

	/** The type of grab the event represents.
	 * If the event has no grab representation then XY_HOVER should be used.
	 */
	enum XY_GRAB_INPUT_TYPE {
		XY_HOVER = 0 /**< Example: mouse: it might mean 'no button is pressed'. */
		, XY_GRAB = 1 /**< Example: mouse: first button pressed. Touch: finger pressed. */
		, XY_MOVE = 2 /**< Example: mouse: pointer move with one or more pressed buttons. */
		, XY_UNGRAB = 3 /**< Example: mouse: 'last pressed button released`. */
		, XY_UNGRAB_CANCEL = 4 /**< Example: mouse: sent when an event listener is removed while buttons are pressed. */
	};
	/** Get the grab type of the event.
	 * If no grab or don't care about it returns XY_HOVER.
	 * Example: When a touch is started type is XY_GRAB, when it moves it's XY_MOVE
	 * and when it's ended it's XY_UNGRAB.
	 * @return The grab type.
	 */
	inline XY_GRAB_INPUT_TYPE getXYGrabType() const { return m_eXYGrabType; }
	/** Identifies what is grabbed.
	 * Ex. For a touch device it could be a finger.
	 * @return The id or undefined if `getXYGrabType() == XY_HOVER`.
	 */
	inline int64_t getXYGrabId() const { return m_nGrabId; }

	//TODO make only one interface transformXY(Matrix) and keep history of transformations
	/** Translates the xy coordinates.
	 * Can be overridden if the event subclass contains additional coords.
	 * If any of the values is changed sets the modified flag to `true`.
	 * @param fDX Value added to the Xs of the event.
	 * @param fDY Value added to the Ys of the event.
	 */
	virtual void translateXY(double fDX, double fDY);
	// This method multiplies (getX(), getY()) by (fSX, fSY), but can be overridden
	// to scale other coords of the event subclass (ex. multi-touch)
	/** Scales the xy coordinates.
	 * Can be overridden if the event subclass contains additional coords.
	 * If any of the values is changed sets the modified flag to `true`.
	 * @param fSX Value multiplied to the Xs of the event.
	 * @param fSY Value multiplied to the Ys of the event.
	 */
	virtual void scaleXY(double fSX, double fSY);
protected:
	/** Constructor without grab info.
	 * Sets grab type to `XY_HOVER` and the modified flag to `false`.
	 * @param oClass See Event::Event(const Class&, int64_t, const shared_ptr<Accessor>&)
	 * @param nTimeUsec See Event::Event(const Class&, int64_t, const shared_ptr<Accessor>&)
	 * @param nCapabilityId The id of the capability that generated this event. Must be >= 0.
	 * @param refAccessor See Event::Event(const Class&, int64_t, const shared_ptr<Accessor>&)
	 * @param fX The X coordinate.
	 * @param fY The Y coordinate.
	 */
	XYEvent(const Class& oClass, int64_t nTimeUsec, int32_t nCapabilityId
			, const shared_ptr<Accessor>& refAccessor, double fX, double fY);
	/** Constructor.
	 * The modified flag is set to `false`.
	 * @param oClass See Event::Event(const Class&, int64_t, const shared_ptr<Accessor>&)
	 * @param nTimeUsec See Event::Event(const Class&, int64_t, const shared_ptr<Accessor>&)
	 * @param nCapabilityId The id of the capability that generated this event. Must be >= 0.
	 * @param refAccessor See Event::Event(const Class&, int64_t, const shared_ptr<Accessor>&)
	 * @param fX The X coordinate.
	 * @param fY The Y coordinate.
	 * @param eXYGrabType The grab type.
	 * @param nGrabId The grab id.
	 */
	XYEvent(const Class& oClass, int64_t nTimeUsec, int32_t nCapabilityId
			, const shared_ptr<Accessor>& refAccessor, double fX, double fY
			, XY_GRAB_INPUT_TYPE eXYGrabType, int64_t nGrabId);
	/** Sets X.
	 * The modified flag is not changed by this function.
	 * @param fX The new value.
	 */
	inline void setX(double fX) { m_fX = fX; }
	/** Sets Y.
	 * The modified flag is not changed by this function.
	 * @param fY The new value.
	 */
	inline void setY(double fY) { m_fY = fY; }
	/** Sets the modified flag.
	 * Listeners receiving the event should only call this function with argument
	 * `true`. Device managers owning and recycling the event call this function 
	 * with argument `false`.
	 * @param bIsModified The new flag value.
	 */
	inline void setIsModified(bool bIsModified) { m_bIsModified = bIsModified; }
	/** Sets the grab type and id.
	 * The modified flag is not changed by this function.
	 * @param eXYGrabType The new grab type.
	 * @param nGrabId The new id.
	 */
	inline void setXYGrab(XY_GRAB_INPUT_TYPE eXYGrabType, int64_t nGrabId)
	{
		assert((eXYGrabType >= XY_HOVER) && (eXYGrabType <= XY_UNGRAB_CANCEL));
		m_eXYGrabType = eXYGrabType;
		m_nGrabId = nGrabId;
	}
private:
	double m_fX;
	double m_fY;
	bool m_bIsModified;
	//
	XY_GRAB_INPUT_TYPE m_eXYGrabType;
	int64_t m_nGrabId;
private:
	XYEvent() = delete;
};

} // namespace stmi

#endif /* _STMI_XY_EVENT_H_ */
