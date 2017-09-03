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
 * File:   fixtureJsDM.h
 */

#ifndef STMI_TESTING_FIXTURE_JS_DM_H
#define STMI_TESTING_FIXTURE_JS_DM_H

#include "fixtureGlibApp.h"

#include "fixturevariantEventClasses.h"

#include "fakejsgtkdevicemanager.h"
#include <stmm-input/devicemanager.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
class JsDMFixture : public GlibAppFixture
					, public FixtureVariantEventClassesEnable
					, public FixtureVariantEventClasses
{
protected:
	void SetUp() override
	{
		GlibAppFixture::SetUp();
		//
		const bool bEventClassesEnable = FixtureVariantEventClassesEnable::getEnable();
		const std::vector<Event::Class> aClasses = FixtureVariantEventClasses::getEventClasses();

		m_refAllEvDM = FakeJsGtkDeviceManager::create(bEventClassesEnable, aClasses);
		assert(m_refAllEvDM.operator bool());
	}
	void TearDown() override
	{
		m_refAllEvDM.reset();
		GlibAppFixture::TearDown();
	}
public:
	shared_ptr<FakeJsGtkDeviceManager> m_refAllEvDM;
};

////////////////////////////////////////////////////////////////////////////////
class JsDMOneWinFixture : public JsDMFixture
{
protected:
	void SetUp() override
	{
		JsDMFixture::SetUp();
		m_refWin1 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
		assert(m_refWin1.operator bool());
	}
	void TearDown() override
	{
		m_refWin1.reset();
		JsDMFixture::TearDown();
	}
public:
	Glib::RefPtr<Gtk::Window> m_refWin1;
};

////////////////////////////////////////////////////////////////////////////////
class JsDMOneWinOneAccFixture : public JsDMOneWinFixture
{
protected:
	void SetUp() override
	{
		JsDMOneWinFixture::SetUp();
		m_refGtkAccessor1 = std::make_shared<stmi::GtkAccessor>(m_refWin1);
		assert(m_refGtkAccessor1.operator bool());
		#ifndef NDEBUG
		const bool bAdded = 
		#endif
		m_refAllEvDM->addAccessor(m_refGtkAccessor1);
		assert(bAdded);
	}
	void TearDown() override
	{
		m_refGtkAccessor1.reset();
		JsDMOneWinFixture::TearDown();
	}
public:
	shared_ptr<stmi::GtkAccessor> m_refGtkAccessor1;
};

////////////////////////////////////////////////////////////////////////////////
class JsDMOneWinOneAccOneDevFixture : public JsDMOneWinOneAccFixture
{
protected:
	void SetUp() override
	{
		JsDMOneWinOneAccFixture::SetUp();
		m_p0FakeBackend = m_refAllEvDM->getBackend();
		assert(m_p0FakeBackend != nullptr);
		m_nDeviceId = m_p0FakeBackend->simulateNewDevice("TestJoystick"
							, {BTN_A, BTN_B, BTN_X, BTN_Y, BTN_TL, BTN_TR, BTN_SELECT, BTN_START, BTN_MODE}
							, 1
							, {ABS_X, ABS_Y, ABS_RX, ABS_RY, ABS_Z, ABS_RZ, ABS_HAT0X, ABS_HAT0Y});
		assert(m_nDeviceId >= 0);
	}
	void TearDown() override
	{
		m_nDeviceId = -1;
		m_p0FakeBackend = nullptr;
		JsDMOneWinOneAccFixture::TearDown();
	}
public:
	Js::FakeGtkBackend* m_p0FakeBackend;
	int32_t m_nDeviceId;
};

////////////////////////////////////////////////////////////////////////////////
class JsDMOneWinOneAccOneDevOneListenerFixture : public JsDMOneWinOneAccOneDevFixture
{
protected:
	void SetUp() override
	{
		JsDMOneWinOneAccOneDevFixture::SetUp();
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
		JsDMOneWinOneAccOneDevFixture::TearDown();
	}
public:
	std::vector< shared_ptr<stmi::Event> > m_aReceivedEvents1;
	shared_ptr<stmi::EventListener> m_refListener1;
};

} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FIXTURE_JS_DM_H */
