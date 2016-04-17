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
 * File:   masgtkkeyboarddevice.h
 */

#ifndef _STMI_MAS_GTK_KEYBOARD_DEVICE_H_
#define _STMI_MAS_GTK_KEYBOARD_DEVICE_H_

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

class GtkKeyboardDevice final : public StdDevice<MasGtkDeviceManager>, public KeyCapability
								, public std::enable_shared_from_this<GtkKeyboardDevice>
{
public:
	GtkKeyboardDevice(std::string sName, const Glib::RefPtr<Gdk::Device>& refKeyboard, const shared_ptr<MasGtkDeviceManager>& refMasGtkDeviceManager)
	: StdDevice<MasGtkDeviceManager>(sName, refMasGtkDeviceManager)
	, m_refGdkKeyboard(refKeyboard)
	{
	}
	virtual ~GtkKeyboardDevice();
	shared_ptr<Capability> getCapability(const Capability::Class& oClass) const override;
	std::vector<Capability::Class> getCapabilityClasses() const override;
	//
	shared_ptr<Device> getDevice() const override;
	shared_ptr<Device> getDevice() override;
	bool isKeyboard() const override { return true; }

private:
	const Glib::RefPtr<Gdk::Device>& getGdkDevice() const { return m_refGdkKeyboard; }

	bool handleGdkEventKey(GdkEventKey* p0KeyEv, const shared_ptr<GtkWindowData>& refWindowData);

	void cancelSelectedAccessorKeys();
	void finalizeListener(MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec);
	void removingDevice();

	void sendKeyEventToListener(const MasGtkDeviceManager::ListenerData& oListenerData, int64_t nEventTimeUsec
								, int64_t nTimePressedUsec
								, KeyEvent::KEY_INPUT_TYPE eInputType, HARDWARE_KEY eHardwareKey
								, const shared_ptr<GtkAccessor>& refAccessor
								, MasGtkDeviceManager* p0Owner
								, shared_ptr<KeyEvent>& refEvent);
private:
	const Glib::RefPtr<Gdk::Device> m_refGdkKeyboard;
	struct KeyData
	{
		int64_t m_nPressedTimeUsec;
	};
	std::unordered_map<int32_t, KeyData> m_oPressedKeys; // Key: (int32_t)HARDWARE_KEY
	friend class stmi::MasGtkDeviceManager;
private:
	GtkKeyboardDevice(const GtkKeyboardDevice& oSource);
	GtkKeyboardDevice& operator=(const GtkKeyboardDevice& oSource);
};

} // namespace Mas
} // namespace Private

} // namespace stmi

#endif	/* _STMI_MAS_GTK_KEYBOARD_DEVICE_H_ */
