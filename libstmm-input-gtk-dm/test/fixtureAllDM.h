/*
 * Copyright © 2016-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   fixtureAllDM.h
 */

#ifndef STMI_TESTING_FIXTURE_ALL_DM_H
#define STMI_TESTING_FIXTURE_ALL_DM_H

#include "fixtureGlibApp.h"

#include "fixturevariantKeyRepeatMode.h"
#include "fixturevariantEventClasses.h"

#include "fakemasgtkdevicemanager.h"
#include "fakejsgtkdevicemanager.h"

#include "stmm-input-gtk-dm.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class TheParentDeviceManager : public ParentDeviceManager
{
public:
	static shared_ptr<TheParentDeviceManager> create(const std::vector< shared_ptr<ChildDeviceManager> >& aChildDeviceManager) noexcept
	{
		auto refDM = std::make_shared<TheParentDeviceManager>();
		refDM->init(aChildDeviceManager);
		return refDM;
	}
protected:
	using ParentDeviceManager::ParentDeviceManager;
};
////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMFixture : public GlibAppFixture
					, public FixtureVariantKeyRepeatMode
					, public FixtureVariantEventClassesEnable
					, public FixtureVariantEventClasses
{
protected:
	void setup() override
	{
		GlibAppFixture::setup();
		//
		const KeyRepeat::MODE eKeyRepeatMode = FixtureVariantKeyRepeatMode::getKeyRepeatMode();
		const bool bEventClassesEnable = FixtureVariantEventClassesEnable::getEnable();
		const std::vector<Event::Class> aClasses = FixtureVariantEventClasses::getEventClasses();
		//
		m_refMasAllEvDM = FakeMasGtkDeviceManager::create(bEventClassesEnable, aClasses, eKeyRepeatMode
													, shared_ptr<GdkKeyConverter>{}, Glib::RefPtr<Gdk::DeviceManager>{});
		m_refJsAllEvDM = FakeJsGtkDeviceManager::create(false, {});
		m_refAllEvDM = TheParentDeviceManager::create(std::vector<shared_ptr<ChildDeviceManager>>{m_refMasAllEvDM, m_refJsAllEvDM});
		assert(m_refAllEvDM.operator bool());
	}
protected:
	shared_ptr<DeviceManager> m_refAllEvDM;
	shared_ptr<FakeMasGtkDeviceManager> m_refMasAllEvDM;
	shared_ptr<FakeJsGtkDeviceManager> m_refJsAllEvDM;
};

////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMOneWinFixture : public AllEvAllDMFixture
{
protected:
	void setup() override
	{
		AllEvAllDMFixture::setup();
		//
		m_refWin1 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
		assert(m_refWin1.operator bool());
	}
protected:
	Glib::RefPtr<Gtk::Window> m_refWin1;
};

////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMOneWinOneAccFixture : public AllEvAllDMOneWinFixture
{
protected:
	void setup() override
	{
		AllEvAllDMOneWinFixture::setup();
		//
		m_refGtkAccessor1 = std::make_shared<stmi::GtkAccessor>(m_refWin1);
		assert(m_refGtkAccessor1.operator bool());
		#ifndef NDEBUG
		const bool bAdded =
		#endif
		m_refAllEvDM->addAccessor(m_refGtkAccessor1);
		assert(bAdded);
	}
protected:
	shared_ptr<stmi::GtkAccessor> m_refGtkAccessor1;
};

////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMOneWinOneAccDevsFixture : public AllEvAllDMOneWinOneAccFixture
{
protected:
	void setup() override
	{
		AllEvAllDMOneWinOneAccFixture::setup();
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
protected:
	Mas::FakeGtkBackend* m_p0FakeMasBackend;
	//
	Js::FakeGtkBackend* m_p0FakeJsBackend;
	int32_t m_nJsDeviceId;
};

////////////////////////////////////////////////////////////////////////////////
class AllEvAllDMOneWinOneAccOneDevsListenerFixture : public AllEvAllDMOneWinOneAccDevsFixture
{
protected:
	void setup() override
	{
		AllEvAllDMOneWinOneAccDevsFixture::setup();
		//
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

#endif /* STMI_TESTING_FIXTURE_ALL_DM_H */
