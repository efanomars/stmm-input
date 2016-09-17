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
 * File:   fixtureAllDM.h
 */

#ifndef _STMI_TESTING_FIXTURE_ALL_DM_H_
#define _STMI_TESTING_FIXTURE_ALL_DM_H_

#include "fixtureGlibApp.h"

#include "fixturevariantKeyRepeatMode.h"
#include "fixturevariantEventClasses.h"

#include "fakemasgtkdevicemanager.h"
#include "fakeflogtkdevicemanager.h"
#include "fakejsgtkdevicemanager.h"

#include <stmm-input-base/parentdevicemanager.h>
#include <stmm-input/devicemanager.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMFixture : public GlibAppFixture
					, public FixtureVariantKeyRepeatMode
					, public FixtureVariantEventClassesEnable
					, public FixtureVariantEventClasses
{
protected:
	void SetUp() override
	{
		GlibAppFixture::SetUp();
		//
		const KEY_REPEAT_MODE eKeyRepeatMode = FixtureVariantKeyRepeatMode::getKeyRepeatMode();
		const bool bEventClassesEnable = FixtureVariantEventClassesEnable::getEnable();
		const std::vector<Event::Class> aClasses = FixtureVariantEventClasses::getEventClasses();
		//
		m_refFloAllEvDM = FakeFloGtkDeviceManager::create(bEventClassesEnable, aClasses, eKeyRepeatMode
													, shared_ptr<GdkKeyConverter>{}, Glib::RefPtr<Gdk::Display>{});
		m_refMasAllEvDM = FakeMasGtkDeviceManager::create(bEventClassesEnable, aClasses, eKeyRepeatMode
													, shared_ptr<GdkKeyConverter>{}, Glib::RefPtr<Gdk::DeviceManager>{});
		m_refJsAllEvDM = FakeJsGtkDeviceManager::create(false, {});
		m_refAllEvDM = ParentDeviceManager::create({m_refMasAllEvDM, m_refFloAllEvDM, m_refJsAllEvDM});
		assert(m_refAllEvDM.operator bool());
	}
	void TearDown() override
	{
		m_refJsAllEvDM.reset();
		m_refMasAllEvDM.reset();
		m_refFloAllEvDM.reset();
		m_refAllEvDM.reset();
		GlibAppFixture::TearDown();
	}
public:
	shared_ptr<DeviceManager> m_refAllEvDM;
	shared_ptr<FakeFloGtkDeviceManager> m_refFloAllEvDM;
	shared_ptr<FakeMasGtkDeviceManager> m_refMasAllEvDM;
	shared_ptr<FakeJsGtkDeviceManager> m_refJsAllEvDM;
};

////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMOneWinFixture : public AllEvAllDMFixture
{
protected:
	void SetUp() override
	{
		AllEvAllDMFixture::SetUp();
		m_refWin1 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
		assert(m_refWin1.operator bool());
	}
	void TearDown() override
	{
		m_refWin1.reset();
		AllEvAllDMFixture::TearDown();
	}
public:
	Glib::RefPtr<Gtk::Window> m_refWin1;
};

////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMOneWinOneAccFixture : public AllEvAllDMOneWinFixture
{
protected:
	void SetUp() override
	{
		AllEvAllDMOneWinFixture::SetUp();
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
		AllEvAllDMOneWinFixture::TearDown();
	}
public:
	shared_ptr<stmi::GtkAccessor> m_refGtkAccessor1;
};

////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMOneWinOneAccDevsFixture : public AllEvAllDMOneWinOneAccFixture
{
protected:
	void SetUp() override
	{
		AllEvAllDMOneWinOneAccFixture::SetUp();
		m_p0FakeFloBackend = m_refFloAllEvDM->getBackend();
		assert(m_p0FakeFloBackend != nullptr);
		m_nXDeviceId = m_p0FakeFloBackend->simulateNewDevice();
		assert(m_nXDeviceId >= 0);
		//
		m_p0FakeMasBackend = m_refMasAllEvDM->getBackend();
		assert(m_p0FakeMasBackend != nullptr);
		//
		m_p0FakeJsBackend = m_refJsAllEvDM->getBackend();
		assert(m_p0FakeJsBackend != nullptr);
		m_nJsDeviceId = m_p0FakeJsBackend->simulateNewDevice("TestJoystick"
							, {BTN_A, BTN_B, BTN_X, BTN_Y, BTN_TL, BTN_TR, BTN_SELECT, BTN_START, BTN_MODE}
							, 1
							, {ABS_X, ABS_Y, ABS_RX, ABS_RY, ABS_Z, ABS_RZ, ABS_HAT0X, ABS_HAT0Y});
		assert(m_nJsDeviceId >= 0);
	}
	void TearDown() override
	{
		m_nJsDeviceId = -1;
		m_p0FakeJsBackend = nullptr;
		//
		m_p0FakeMasBackend = nullptr;
		//
		m_nXDeviceId = -1;
		m_p0FakeFloBackend = nullptr;
		AllEvAllDMOneWinOneAccFixture::TearDown();
	}
public:
	Flo::FakeGtkBackend* m_p0FakeFloBackend;
	int32_t m_nXDeviceId;
	//
	Mas::FakeGtkBackend* m_p0FakeMasBackend;
	//
	Js::FakeGtkBackend* m_p0FakeJsBackend;
	int32_t m_nJsDeviceId;
};

////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMOneWinOneAccOneDevsListenerFixture : public AllEvAllDMOneWinOneAccDevsFixture
{
protected:
	void SetUp() override
	{
		AllEvAllDMOneWinOneAccDevsFixture::SetUp();
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
		AllEvAllDMOneWinOneAccDevsFixture::TearDown();
	}
public:
	std::vector< shared_ptr<stmi::Event> > m_aReceivedEvents1;
	shared_ptr<stmi::EventListener> m_refListener1;
};

} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FIXTURE_ALL_DM_H_ */
