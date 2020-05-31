/*
 * Copyright © 2016-2017  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   fixtureFakeDM.h
 */

#ifndef STMI_TESTING_FIXTURE_FAKE_DM_H
#define STMI_TESTING_FIXTURE_FAKE_DM_H

#include "fakedevicemanager.h"

#include <stmm-input/devicemanager.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
class FakeDMFixture
{
public:
	FakeDMFixture()
	{
		//
		m_refAllEvDM = std::make_shared<FakeDeviceManager>();
		assert(m_refAllEvDM.operator bool());
	}
protected:
	shared_ptr<FakeDeviceManager> m_refAllEvDM;
};

////////////////////////////////////////////////////////////////////////////////
class FakeDMOneListenerFixture : public FakeDMFixture
{
public:
	FakeDMOneListenerFixture() : FakeDMFixture()
	{
		m_refListener1 = std::make_shared<stmi::EventListener>(
				[&](const shared_ptr<stmi::Event>& refEvent)
				{
					m_aReceivedEvents1.emplace_back(refEvent);
				});
		#ifndef NDEBUG
		const bool bListenerAdded =
		#endif
		m_refAllEvDM->addEventListener(m_refListener1, std::shared_ptr<stmi::CallIf>{});
		assert(bListenerAdded);
	}
protected:
	std::vector< shared_ptr<stmi::Event> > m_aReceivedEvents1;
	shared_ptr<stmi::EventListener> m_refListener1;
};


} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FIXTURE_FAKE_DM_H */
