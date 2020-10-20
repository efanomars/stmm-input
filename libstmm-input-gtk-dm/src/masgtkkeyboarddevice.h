/*
 * Copyright © 2016-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   masgtkkeyboarddevice.h
 */

#ifndef STMI_MAS_GTK_KEYBOARD_DEVICE_H
#define STMI_MAS_GTK_KEYBOARD_DEVICE_H

#include "masgtkdevicemanager.h"

#include "recycler.h"

#include <stmm-input-base/basicdevice.h>
#include <stmm-input-ev/keycapability.h>
#include <stmm-input-ev/keyevent.h>
#include <stmm-input/capability.h>
#include <stmm-input/hardwarekey.h>

#include <gdk/gdk.h>

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

class GtkKeyboardDevice final : public BasicDevice<MasGtkDeviceManager>, public KeyCapability
								, public std::enable_shared_from_this<GtkKeyboardDevice>
{
public:
	GtkKeyboardDevice(const std::string& sName, const shared_ptr<MasGtkDeviceManager>& refMasGtkDeviceManager) noexcept
	: BasicDevice<MasGtkDeviceManager>(sName, refMasGtkDeviceManager)
	{
	}
	virtual ~GtkKeyboardDevice() noexcept;
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const noexcept override;
	shared_ptr<Capability> getCapability(int32_t nCapabilityId) const noexcept override;
	std::vector<int32_t> getCapabilities() const noexcept override;
	std::vector<Capability::Class> getCapabilityClasses() const noexcept override;
	//
	shared_ptr<Device> getDevice() const noexcept override;
	bool isKeyboard() const noexcept override { return true; }

private:
	friend class stmi::MasGtkDeviceManager;

	bool handleGdkEventKey(GdkEventKey* p0KeyEv, const shared_ptr<GtkWindowData>& refWindowData) noexcept;

	void cancelSelectedAccessorKeys() noexcept;
	void finalizeListener(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec) noexcept;
	void removingDevice() noexcept;
	void sendKeyEventToListener(const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
								, uint64_t nTimePressedStamp
								, KeyEvent::KEY_INPUT_TYPE eInputType, HARDWARE_KEY eHardwareKey
								, const shared_ptr<GtkAccessor>& refAccessor
								, MasGtkDeviceManager* p0Owner
								, shared_ptr<Event>& refEvent) noexcept;
private:
	struct KeyData
	{
		uint64_t m_nPressedTimeStamp;
	};
	std::unordered_map<HARDWARE_KEY, KeyData> m_oPressedKeys;
	//
	class ReKeyEvent :public KeyEvent
	{
	public:
		ReKeyEvent(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
					, const shared_ptr<KeyCapability>& refKeyCapability, KEY_INPUT_TYPE eType, HARDWARE_KEY eKey) noexcept
		: KeyEvent(nTimeUsec, refAccessor, refKeyCapability, eType, eKey)
		{
		}
		void reInit(int64_t nTimeUsec, const shared_ptr<Accessor>& refAccessor
				, const shared_ptr<KeyCapability>& refKeyCapability, KEY_INPUT_TYPE eType, HARDWARE_KEY eKey) noexcept
		{
			setTimeUsec(nTimeUsec);
			setAccessor(refAccessor);
			setKeyCapability(refKeyCapability);
			setType(eType);
			setKey(eKey);
		}
	};
	Private::Recycler<ReKeyEvent, Event> m_oKeyEventRecycler;
private:
	GtkKeyboardDevice(const GtkKeyboardDevice& oSource);
	GtkKeyboardDevice& operator=(const GtkKeyboardDevice& oSource);
};

} // namespace Mas

} // namespace Private

} // namespace stmi

#endif /* STMI_MAS_GTK_KEYBOARD_DEVICE_H */
