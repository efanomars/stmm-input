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
 * File:   fixtureFakeDM.h
 */

#ifndef _STMI_TESTING_FIXTURE_FAKE_DM_H_
#define _STMI_TESTING_FIXTURE_FAKE_DM_H_

#include "fakedevicemanager.h"

#include <stmm-input/devicemanager.h>

#include <gtest/gtest.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
class FakeDMFixture : public ::testing::Test
{
protected:
	void SetUp() override
	{
		//
		m_refAllEvDM = std::make_shared<FakeDeviceManager>();
		assert(m_refAllEvDM.operator bool());
	}
	void TearDown() override
	{
		m_refAllEvDM.reset();
	}
public:
	shared_ptr<FakeDeviceManager> m_refAllEvDM;
};

////////////////////////////////////////////////////////////////////////////////
class FakeDMOneListenerFixture : public FakeDMFixture
{
protected:
	void SetUp() override
	{
		FakeDMFixture::SetUp();
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
	void TearDown() override
	{
		m_refListener1.reset();
		m_aReceivedEvents1.clear();
		FakeDMFixture::TearDown();
	}
public:
	std::vector< shared_ptr<stmi::Event> > m_aReceivedEvents1;
	shared_ptr<stmi::EventListener> m_refListener1;
};


} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FIXTURE_FAKE_DM_H_ */
