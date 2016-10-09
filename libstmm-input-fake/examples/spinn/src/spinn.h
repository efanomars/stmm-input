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
 * File:   spinn.h
 */

#ifndef _EXAMPLE_SPINN_SPINN_
#define _EXAMPLE_SPINN_SPINN_

#include <stmm-input/devicemanager.h>

#include <sigc++/signal.h>

namespace example
{

namespace spinn
{

using std::shared_ptr;

class Spinn
{
public:
	Spinn(const shared_ptr<stmi::DeviceManager>& refDeviceManager)
	: m_refDeviceManager(refDeviceManager)
	, m_nValue(0)
	{
		m_refListenerAllEvents = std::make_shared<stmi::EventListener>(
									std::bind( &Spinn::handleAllEvents, this, std::placeholders::_1 ) );
		m_refDeviceManager->addEventListener(m_refListenerAllEvents);
	}
	inline int32_t getValue() const
	{
		return m_nValue;
	}

	/* Emits when value has changed. */
	sigc::signal<void> m_oVauleChangedSignal;

private:
	void handleAllEvents(const shared_ptr<stmi::Event>& refEvent);
private:
	shared_ptr<stmi::DeviceManager> m_refDeviceManager;
	shared_ptr<stmi::EventListener> m_refListenerAllEvents;
	//
	int32_t m_nValue;
};

} // namespace spinn

} // namespace example

#endif	/* _EXAMPLE_SPINN_SPINN_ */
