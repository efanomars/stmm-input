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
 * File:   jsgtkjoystickdevice.h
 */

#ifndef STMI_JS_GTK_JOYSTICK_DEVICE_H
#define STMI_JS_GTK_JOYSTICK_DEVICE_H

#include "jsgtkdevicemanager.h"

#include "recycler.h"

#include <stmm-input-ev/joystickcapability.h>
#include <stmm-input-ev/joystickevent.h>
#include <stmm-input-base/basicdevice.h>
#include <stmm-input/capability.h>
#include <stmm-input/device.h>

#include <list>
#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include <stddef.h>

#include <linux/joystick.h>

namespace stmi { class Accessor; }
namespace stmi { class Event; }
namespace stmi { class GtkAccessor; }

namespace stmi
{

namespace Private
{
namespace Js
{


using std::shared_ptr;
using std::weak_ptr;

class JoystickDevice final : public BasicDevice<JsGtkDeviceManager>, public JoystickCapability
						, public std::enable_shared_from_this<JoystickDevice>, public sigc::trackable
{
public:
	JoystickDevice(const std::string& sName, const shared_ptr<JsGtkDeviceManager>& refDeviceManager
					, const std::vector<int32_t>& aButtonCode, int32_t nTotHats, const std::vector<int32_t>& aAxisCode) noexcept;
	virtual ~JoystickDevice() noexcept;
	//
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const noexcept override;
	shared_ptr<Capability> getCapability(int32_t nCapabilityId) const noexcept override;
	std::vector<int32_t> getCapabilities() const noexcept override;
	std::vector<Capability::Class> getCapabilityClasses() const noexcept override;
	//
	shared_ptr<Device> getDevice() const noexcept override;
	//
	bool getHasButton(JoystickCapability::BUTTON eButton) const noexcept override;
	int32_t getTotHats() const noexcept override;
	bool getHasAxis(JoystickCapability::AXIS eAxis) const noexcept override;
	bool isButtonPressed(JoystickCapability::BUTTON eButton) const noexcept override;
	HAT_VALUE getHatValue(int32_t nHat) const noexcept override;
	int32_t getAxisValue(JoystickCapability::AXIS eAxis) const noexcept override;

	inline int32_t getDeviceId() const noexcept { return Device::getId(); }

	constexpr static bool isHatAxis(int32_t nLinuxAxis) noexcept
	{
		return (nLinuxAxis >= ABS_HAT0X) && (nLinuxAxis <= ABS_HAT3Y);
	}

	// This is public so that there's no need to friend GtkBackend (or even FakeGtkBackend)
	bool doInputJoystickEventCallback(const struct ::js_event* p0JoyEvent) noexcept;
private:
	size_t getButtonNr(JoystickCapability::BUTTON eButton) const noexcept;
	size_t getAxisNr(JoystickCapability::AXIS eAxis) const noexcept;
	//
	friend class stmi::JsGtkDeviceManager;
	void cancelSelectedAccessorButtonsAndHats() noexcept;
	void cancelSelectedAccessorButtons(const shared_ptr<JoystickCapability>& refCapability
										, const shared_ptr< const std::list< JsGtkDeviceManager::ListenerData* > >& refListeners
										, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
										, JsGtkDeviceManager* p0Owner) noexcept;
	void cancelSelectedAccessorHats(const shared_ptr<JoystickCapability>& refCapability
									, const shared_ptr< const std::list< JsGtkDeviceManager::ListenerData* > >& refListeners
									, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
									, JsGtkDeviceManager* p0Owner) noexcept;
	void finalizeListener(JsGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec) noexcept;
	void finalizeListenerButton(JsGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
								, const shared_ptr<JoystickCapability>& refCapability, int32_t nClassIdxJoystickButtonEvent
								, const shared_ptr<GtkAccessor>& refSelectedAccessor) noexcept;
	void finalizeListenerHat(JsGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
								, const shared_ptr<JoystickCapability>& refCapability, int32_t nClassIdxJoystickHatEvent
								, const shared_ptr<GtkAccessor>& refSelectedAccessor) noexcept;
	void removingDevice() noexcept;
	//
	void handleButton(JsGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
					, const shared_ptr<JoystickCapability>& refThis
					, int32_t nNr, JoystickCapability::BUTTON eButton, int32_t nValue) noexcept;
	void handleHat(JsGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
					, const shared_ptr<JoystickCapability>& refThis
					, int32_t nHat, bool bY, int32_t nValue) noexcept;
	void handleAxis(JsGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
					, const shared_ptr<JoystickCapability>& refThis
					, JoystickCapability::AXIS eAxis, int32_t nValue) noexcept;
	//
	void sendButtonEventToListener(const JsGtkDeviceManager::ListenerData& oListenerData
									, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refAccessor
									, uint64_t nPressedTimeStamp
									, JoystickButtonEvent::BUTTON_INPUT_TYPE eInputType
									, JoystickCapability::BUTTON eButtonId
									, const shared_ptr<JoystickCapability>& refCapability
									, int32_t nClassIdxJoystickButtonEvent
									, shared_ptr<Event>& refEvent) noexcept;
	void sendHatEventToListener(const JsGtkDeviceManager::ListenerData& oListenerData
								, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refAccessor
								, uint64_t nPressedTimeStamp, int32_t nHat
								, JoystickCapability::HAT_VALUE eValue
								, JoystickCapability::HAT_VALUE ePreviousValue
								, const shared_ptr<JoystickCapability>& refCapability
								, int32_t nClassIdxJoystickHatEvent
								, shared_ptr<Event>& refEvent) noexcept;

	inline JoystickCapability::HAT_VALUE calcHatValue(int32_t nAxisX, int32_t nAxisY) const noexcept
	{
		return static_cast<JoystickCapability::HAT_VALUE>(nAxisY * 4 + nAxisX);
	}
private:
	struct ButtonData
	{
		bool m_bPressed;
		uint64_t m_nPressedTimeStamp;
	};
	const std::vector<int32_t> m_aButtonCode; // Size: tot buttons provided by ioctl, Value: the enum JoystickCapability::BUTTON
	std::vector< ButtonData > m_aButtonPressed; // Size: m_aButtonCode.size()
	struct HatData
	{
		uint64_t m_nPressedTimeStamp; // last transition from JoystickCapability::HAT_CENTER (0,0) to anything else
		int32_t m_nAxisX; // Current position  0: center, 1: left, 2: right (see calcHatValue)
		int32_t m_nAxisY; // Current position  0: center, 1: up, 2: down  (see calcHatValue)
	};
	constexpr static int32_t s_nMaxHats = 4;
	const int32_t m_nTotHats;
	std::vector< HatData > m_aHatStatus; // Size: m_nTotHats
	//
	const std::vector<int32_t> m_aAxisCode; // Size: tot axes provided by ioctl, Value: the enum JoystickCapability::AXIS
	std::vector< int32_t > m_aAxisValue; // Size: m_aAxisCode.size(), Value: current axis value [-32767, 32767]
	//
	class ReJoystickHatEvent :public JoystickHatEvent
	{
	public:
		ReJoystickHatEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
							, const shared_ptr<JoystickCapability>& refJoystickCapability, int32_t nHat
							, JoystickCapability::HAT_VALUE eValue, JoystickCapability::HAT_VALUE ePreviousValue) noexcept
		: JoystickHatEvent(nTimeUsec, refAccessor, refJoystickCapability, nHat, eValue, ePreviousValue)
		{
		}
		void reInit(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<JoystickCapability>& refJoystickCapability, int32_t nHat
					, JoystickCapability::HAT_VALUE eValue, JoystickCapability::HAT_VALUE ePreviousValue) noexcept
		{
			setTimeUsec(nTimeUsec);
			setAccessor(refAccessor);
			setHat(nHat);
			setValue(eValue, ePreviousValue);
			setJoystickCapability(refJoystickCapability);
		}
	};
	Private::Recycler<ReJoystickHatEvent, Event> m_oJoystickHatEventRecycler;
	//
	class ReJoystickButtonEvent :public JoystickButtonEvent
	{
	public:
		ReJoystickButtonEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
								, const shared_ptr<JoystickCapability>& refJoystickCapability
								, BUTTON_INPUT_TYPE eType, JoystickCapability::BUTTON eButton) noexcept
		: JoystickButtonEvent(nTimeUsec, refAccessor, refJoystickCapability, eType, eButton)
		{
		}
		void reInit(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<JoystickCapability>& refJoystickCapability
					, BUTTON_INPUT_TYPE eType, JoystickCapability::BUTTON eButton) noexcept
		{
			setTimeUsec(nTimeUsec);
			setAccessor(refAccessor);
			setType(eType);
			setButton(eButton);
			setJoystickCapability(refJoystickCapability);
		}
	};
	Private::Recycler<ReJoystickButtonEvent, Event> m_oJoystickButtonEventRecycler;
	//
	class ReJoystickAxisEvent :public JoystickAxisEvent
	{
	public:
		ReJoystickAxisEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
								, const shared_ptr<JoystickCapability>& refJoystickCapability
								, JoystickCapability::AXIS eAxis, int32_t nValue) noexcept
		: JoystickAxisEvent(nTimeUsec, refAccessor, refJoystickCapability, eAxis, nValue)
		{
		}
		void reInit(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<JoystickCapability>& refJoystickCapability
					, JoystickCapability::AXIS eAxis, int32_t nValue) noexcept
		{
			setTimeUsec(nTimeUsec);
			setAccessor(refAccessor);
			setAxis(eAxis);
			setValue(nValue);
			setJoystickCapability(refJoystickCapability);
		}
	};
	Private::Recycler<ReJoystickAxisEvent, Event> m_oJoystickAxisEventRecycler;
private:
	JoystickDevice(const JoystickDevice& oSource) = delete;
	JoystickDevice& operator=(const JoystickDevice& oSource) = delete;
};

} // namespace Js
} // namespace Private

} // namespace stmi

#endif /* STMI_JS_GTK_JOYSTICK_DEVICE_H */
