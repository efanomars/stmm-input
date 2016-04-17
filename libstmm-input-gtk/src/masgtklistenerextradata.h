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
 * File:   masgtklistenerextradata.h
 */

#ifndef _STMI_MAS_GTK_LISTENER_EXTRA_DATA_H_
#define _STMI_MAS_GTK_LISTENER_EXTRA_DATA_H_

#include "masgtkdevicemanager.h"

namespace stmi
{

namespace Private
{
namespace Mas
{

class MasGtkListenerExtraData final : public stmi::MasGtkDeviceManager::ListenerExtraData
{
public:
	void reset() override
	{
		m_aCanceledKeys.clear();
		m_aCanceledButtons.clear();
		m_aCanceledSequences.clear();
	}
	inline bool isKeyCanceled(int32_t nKey) const
	{
		return std::find(m_aCanceledKeys.begin(), m_aCanceledKeys.end(), nKey) != m_aCanceledKeys.end();
	}
	inline void setKeyCanceled(int32_t nKey)
	{
		m_aCanceledKeys.push_back(nKey);
	}
	inline bool isButtonCanceled(int32_t nButton) const
	{
		return std::find(m_aCanceledButtons.begin(), m_aCanceledButtons.end(), nButton) != m_aCanceledButtons.end();
	}
	inline void setButtonCanceled(int32_t nButton)
	{
		m_aCanceledButtons.push_back(nButton);
	}
	inline bool isSequenceCanceled(const GdkEventSequence* p0Sequence) const
	{
		return std::find(m_aCanceledSequences.begin(), m_aCanceledSequences.end(), p0Sequence) != m_aCanceledSequences.end();
	}
	inline void setSequenceCanceled(const GdkEventSequence* p0Sequence)
	{
		m_aCanceledSequences.push_back(p0Sequence);
	}
private:
	std::vector<int32_t> m_aCanceledKeys;
	std::vector<int32_t> m_aCanceledButtons;
	std::vector<const GdkEventSequence*> m_aCanceledSequences;
};

} // namespace Mas
} // namespace Private

} // namespace stmi

#endif	/* _STMI_MAS_GTK_LISTENER_EXTRA_DATA_H_ */
