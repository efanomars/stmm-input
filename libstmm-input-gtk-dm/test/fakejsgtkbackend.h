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
 * File:   fakejsgtkbackend.h
 */

#ifndef STMI_TESTING_FAKE_JS_GTK_BACKEND_H
#define STMI_TESTING_FAKE_JS_GTK_BACKEND_H

#include "jsgtkbackend.h"
#include "jsgtkjoystickdevice.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{
namespace Js
{

class FakeGtkBackend : public Private::Js::GtkBackend
{
public:
	FakeGtkBackend(::stmi::JsGtkDeviceManager* p0Owner)
	: Private::Js::GtkBackend(p0Owner, JsDeviceFiles{}, false)
	{
	}
	//
	int32_t simulateNewDevice(const std::string& sName, const std::vector<int32_t>& aButtonCode
												, int32_t nTotHats, const std::vector<int32_t>& aAxisCode)
	{
		assert(nTotHats >= 0);
		#ifndef NDEBUG
		for (auto nIdx = 0u; nIdx < aButtonCode.size(); ++nIdx) {
			const int32_t nButtonCode = aButtonCode[nIdx];
			const JoystickCapability::BUTTON eButton = static_cast<JoystickCapability::BUTTON>(nButtonCode);
			assert(JoystickCapability::isValidButton(eButton));
			for (auto nIdx2 = nIdx + 1; nIdx2 < aButtonCode.size(); ++nIdx2) {
				const int32_t nButtonCode2 = aButtonCode[nIdx2];
				assert(nButtonCode != nButtonCode2); // button duplicate not allowed.
			}
		}
		for (auto nIdx = 0u; nIdx < aAxisCode.size(); ++nIdx) {
			const int32_t nAxisCode = aAxisCode[nIdx];
			const JoystickCapability::AXIS eAxis = static_cast<JoystickCapability::AXIS>(nAxisCode);
			assert(JoystickCapability::isValidAxis(eAxis) || Private::Js::JoystickDevice::isHatAxis(nAxisCode));
			for (auto nIdx2 = nIdx + 1; nIdx2 < aAxisCode.size(); ++nIdx2) {
				const int32_t nAxisCode2 = aAxisCode[nIdx2];
				assert(nAxisCode != nAxisCode2); // button duplicate not allowed.
			}
		}
		#endif //NDEBUG
		auto& refJoystick = onDeviceAdded(sName, aButtonCode, nTotHats, aAxisCode);
		m_aJoysticks.push_back(refJoystick);
		return refJoystick->getDeviceId();
	}
	void simulateRemoveDevice(int32_t nDeviceId)
	{
		const int32_t nIdx = findDeviceId(nDeviceId);
		assert(nIdx >= 0);
		const int32_t nLastIdx = static_cast<int32_t>(m_aJoysticks.size()) - 1;
		if (nIdx < nLastIdx) {
			m_aJoysticks[nIdx].swap(m_aJoysticks[nLastIdx]);
		}
		m_aJoysticks.pop_back();
		onDeviceRemoved(nDeviceId);
	}
	bool simulateJsEvent(int32_t nDeviceId, const struct js_event* p0JsEvent)
	{
		const int32_t nIdx = findDeviceId(nDeviceId);
//std::cout << "simulateJsEvent  nIdx=" << nIdx << '\n';
		assert(nIdx >= 0);
		return m_aJoysticks[nIdx]->doInputJoystickEventCallback(p0JsEvent);
	}
private:
	int32_t findDeviceId(int32_t nDeviceId) const
	{
		auto itFind = std::find_if(m_aJoysticks.begin(), m_aJoysticks.end()
				, [&](const shared_ptr<Private::Js::JoystickDevice>& refJoystick)
				{
					return (refJoystick->getDeviceId() == nDeviceId);
				});
		if (itFind == m_aJoysticks.end()) {
			return -1;
		}
		return std::distance(m_aJoysticks.begin(), itFind);
	}
private:
	std::vector< shared_ptr<Private::Js::JoystickDevice> > m_aJoysticks;
};

} // namespace Js
} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FAKE_JS_GTK_BACKEND_H */
