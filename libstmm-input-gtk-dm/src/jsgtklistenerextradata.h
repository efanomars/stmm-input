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
 * File:   jsgtklistenerextradata.h
 */

#ifndef _STMI_JS_GTK_LISTENER_EXTRA_DATA_H_
#define _STMI_JS_GTK_LISTENER_EXTRA_DATA_H_

#include "jsgtkdevicemanager.h"

namespace stmi
{

namespace Private
{
namespace Js
{

class JsGtkListenerExtraData final : public stmi::JsGtkDeviceManager::ListenerExtraData
{
public:
	void reset() override
	{
		m_aCanceledButtons.clear();
		m_aCanceledHats.clear();
	}
	inline bool isButtonCanceled(int32_t nButton) const
	{
		return std::find(m_aCanceledButtons.begin(), m_aCanceledButtons.end(), nButton) != m_aCanceledButtons.end();
	}
	inline void setButtonCanceled(int32_t nButton)
	{
		m_aCanceledButtons.push_back(nButton);
	}
	inline bool isHatCanceled(int32_t nHat) const
	{
		return std::find(m_aCanceledHats.begin(), m_aCanceledHats.end(), nHat) != m_aCanceledHats.end();
	}
	inline void setHatCanceled(int32_t nHat)
	{
		m_aCanceledHats.push_back(nHat);
	}
private:
	std::vector<int32_t> m_aCanceledButtons;
	std::vector<int32_t> m_aCanceledHats;
};

} // namespace Js
} // namespace Private

} // namespace stmi

#endif /* _STMI_JS_GTK_LISTENER_EXTRA_DATA_H_ */
