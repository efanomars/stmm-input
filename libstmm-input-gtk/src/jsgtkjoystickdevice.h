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
 * File:   jsgtkjoystickdevice.h
 */

#ifndef _STMI_JS_GTK_JOYSTICK_DEVICE_H_
#define _STMI_JS_GTK_JOYSTICK_DEVICE_H_

#include "jsgtkdevicemanager.h"

#include <stmm-input-base/stddevice.h>

#include <linux/joystick.h>

namespace stmi
{

namespace Private
{
namespace Js
{

inline static bool isHatAxis(JoystickCapability::AXIS eAxis)
{
	return (eAxis >= ABS_HAT0X) && (eAxis <= ABS_HAT3Y);
}

using std::shared_ptr;
using std::weak_ptr;

class JoystickDevice final : public StdDevice<JsGtkDeviceManager>, public JoystickCapability
						, public std::enable_shared_from_this<JoystickDevice>, public sigc::trackable
{
public:
	JoystickDevice(std::string sName, const shared_ptr<JsGtkDeviceManager>& refDeviceManager
					, int32_t nFD, int64_t nFileSysDeviceId
					, const std::vector<int32_t>& aButtonCode, int32_t nTotHats, const std::vector<int32_t>& aAxisCode);
	virtual ~JoystickDevice();
	//
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const override;
	std::vector<Capability::Class> getCapabilityClasses() const override;
	//
	shared_ptr<Device> getDevice() const override;
	shared_ptr<Device> getDevice() override;
	//
	bool getHasButton(JoystickCapability::BUTTON eButton) const override;
	int32_t getTotHats() const override;
	bool getHasAxis(JoystickCapability::AXIS eAxis) const override;
	bool isButtonPressed(JoystickCapability::BUTTON eButton) const override;
	HAT_VALUE getHatValue(int32_t nHat) const override;
	int32_t getAxisValue(JoystickCapability::AXIS eAxis) const override;
private:
	friend class stmi::JsGtkDeviceManager;
	void cancelSelectedAccessorButtonsAndHats();
	void cancelSelectedAccessorButtons(const shared_ptr<JoystickCapability>& refCapability
										, const shared_ptr< const std::list< JsGtkDeviceManager::ListenerData* > >& refListeners
										, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
										, JsGtkDeviceManager* p0Owner);
	void cancelSelectedAccessorHats(const shared_ptr<JoystickCapability>& refCapability
									, const shared_ptr< const std::list< JsGtkDeviceManager::ListenerData* > >& refListeners
									, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refSelectedAccessor
									, JsGtkDeviceManager* p0Owner);
	void finalizeListener(JsGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec);
	void finalizeListenerButton(JsGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
								, const shared_ptr<JoystickCapability>& refCapability, int32_t nClassIdxJoystickButtonEvent
								, const shared_ptr<GtkAccessor>& refSelectedAccessor);
	void finalizeListenerHat(JsGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
								, const shared_ptr<JoystickCapability>& refCapability, int32_t nClassIdxJoystickHatEvent
								, const shared_ptr<GtkAccessor>& refSelectedAccessor);
	void removingDevice();
	//
	bool doInputJoystickEventCallback(const struct js_event* p0JoyEvent);
	void handleButton(JsGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
					, const shared_ptr<JoystickCapability>& refThis
					, int32_t nNr, JoystickCapability::BUTTON eButton, int32_t nValue);
	void handleHat(JsGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
					, const shared_ptr<JoystickCapability>& refThis
					, int32_t nHat, bool bY, int32_t nValue);
	void handleAxis(JsGtkDeviceManager* p0Owner, const shared_ptr<GtkAccessor>& refSelectedAccessor
					, const shared_ptr<JoystickCapability>& refThis
					, JoystickCapability::AXIS eAxis, int32_t nValue);
	//
	void sendButtonEventToListener(const JsGtkDeviceManager::ListenerData& oListenerData
									, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refAccessor
									, int64_t nTimePressedUsec
									, JoystickButtonEvent::BUTTON_INPUT_TYPE eInputType
									, JoystickCapability::BUTTON eButtonId
									, const shared_ptr<JoystickCapability>& refCapability
									, int32_t nClassIdxJoystickButtonEvent
									, shared_ptr<JoystickButtonEvent>& refEvent);
	void sendHatEventToListener(const JsGtkDeviceManager::ListenerData& oListenerData
								, int64_t nEventTimeUsec, const shared_ptr<GtkAccessor>& refAccessor
								, int64_t nTimeAnyPressedUsec, int32_t nHat
								, JoystickCapability::HAT_VALUE eValue
								, JoystickCapability::HAT_VALUE ePreviousValue
								, const shared_ptr<JoystickCapability>& refCapability
								, int32_t nClassIdxJoystickHatEvent
								, shared_ptr<JoystickHatEvent>& refEvent);
	size_t getButtonNr(JoystickCapability::BUTTON eButton) const;
	size_t getAxisNr(JoystickCapability::AXIS eAxis) const;

	inline JoystickCapability::HAT_VALUE calcHatValue(int32_t nAxisX, int32_t nAxisY) const
	{
		return static_cast<JoystickCapability::HAT_VALUE>(nAxisY * 4 + nAxisX);
	}
private:
	const int32_t m_nFD; // The file descriptor is open until destructor is called
	const int64_t m_nFileSysDeviceId; // The stat rdev field (should be unique)
	struct ButtonData
	{
		bool m_bPressed;
		int64_t m_nTimePressed;
	};
	const std::vector<int32_t> m_aButtonCode; // Size: tot buttons provided by ioctl, Value: the enum JoystickCapability::BUTTON
	std::vector< ButtonData > m_aButtonPressed; // Size: m_aButtonCode.size()
	struct HatData
	{
		int64_t m_nTimePressed; // last transition from HAT_CENTER to anything else
		int32_t m_nAxisX; // Current position  0: center, 1: left, 2: right
		int32_t m_nAxisY; // Current position  0: center, 1: up, 2: down
	};
	constexpr static int32_t s_nMaxHats = 4;
	const int32_t m_nTotHats;
	std::vector< HatData > m_aHatStatus; // Size: m_nTotHats
	//
	const std::vector<int32_t> m_aAxisCode; // Size: tot axes provided by ioctl, Value: the enum JoystickCapability::AXIS
	std::vector< int32_t > m_aAxisValue; // Size: m_aAxisCode.size(), Value: current axis value [-32767, 32767]
	//
	friend class JsGtkDeviceManager;
	Glib::RefPtr<Private::Js::JoystickInputSource> m_refSource;
private:
	JoystickDevice(const JoystickDevice& oSource) = delete;
	JoystickDevice& operator=(const JoystickDevice& oSource) = delete;
};

} // namespace Js
} // namespace Private

} // namespace stmi

#endif	/* _STMI_JS_GTK_JOYSTICK_DEVICE_H_ */
