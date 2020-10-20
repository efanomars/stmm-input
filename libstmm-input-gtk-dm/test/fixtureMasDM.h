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
 * File:   fixtureMasDM.h
 */

#ifndef STMI_TESTING_FIXTURE_MAS_DM_H
#define STMI_TESTING_FIXTURE_MAS_DM_H

#include "fixtureGlibApp.h"

#include "fixturevariantKeyRepeatMode.h"
#include "fixturevariantEventClasses.h"

#include "fakemasgtkdevicemanager.h"
#include <stmm-input/devicemanager.h>

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

////////////////////////////////////////////////////////////////////////////////
class MasDMFixture : public GlibAppFixture
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
//std::cout << "---------->--------< bEventClassesEnable=" << bEventClassesEnable << '\n';
		const std::vector<Event::Class> aClasses = FixtureVariantEventClasses::getEventClasses();
//for (const auto& oClass : aClasses) {
//std::cout << "---------->--------< " << oClass.getId() << '\n';
//}
		//
		m_refAllEvDM = FakeMasGtkDeviceManager::create(bEventClassesEnable, aClasses, eKeyRepeatMode
													, shared_ptr<GdkKeyConverter>{}, Glib::RefPtr<Gdk::DeviceManager>{});
		assert(m_refAllEvDM.operator bool());
	}

	shared_ptr<FakeMasGtkDeviceManager> m_refAllEvDM;
};

////////////////////////////////////////////////////////////////////////////////
class MasDMOneWinFixture : public MasDMFixture
{
protected:
	void setup() override
	{
		MasDMFixture::setup();
		//
		m_refWin1 = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
		assert(m_refWin1.operator bool());
	}
protected:
	Glib::RefPtr<Gtk::Window> m_refWin1;
};

////////////////////////////////////////////////////////////////////////////////
class MasDMOneWinOneAccFixture : public MasDMOneWinFixture
{
protected:
	void setup() override
	{
		MasDMOneWinFixture::setup();
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
class MasDMOneWinOneAccOneListenerFixture : public MasDMOneWinOneAccFixture
{
protected:
	void setup() override
	{
		MasDMOneWinOneAccFixture::setup();
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

#endif /* STMI_TESTING_FIXTURE_MAS_DM_H */
