/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   fakemasgtkdevicemanager.h
 */

#ifndef STMI_TESTING_FAKE_MAS_GTK_DEVICE_MANAGER_H
#define STMI_TESTING_FAKE_MAS_GTK_DEVICE_MANAGER_H

#include "masgtkdevicemanager.h"

#include <stmm-input-gtk/gdkkeyconverterevdev.h>

#include "fakemasgtkbackend.h"
#include "fakemasgtkwindowdata.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class FakeMasGtkDeviceManager : public MasGtkDeviceManager
{
public:
	static shared_ptr<FakeMasGtkDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses
												, KeyRepeat::MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter
												, const Glib::RefPtr<Gdk::DeviceManager>& refGdkDeviceManager) noexcept
	{
		shared_ptr<FakeMasGtkDeviceManager> refInstance(
				new FakeMasGtkDeviceManager(bEnableEventClasses, aEnDisableEventClasses
											, eKeyRepeatMode
											, (refGdkConverter ? refGdkConverter : GdkKeyConverterEvDev::getConverter())));
//std::cout << "FakeMasGtkDeviceManager::create 1" << '\n';
		auto refBackend = std::make_unique<Mas::FakeGtkBackend>(refInstance.operator->()
				, (refGdkDeviceManager ? refGdkDeviceManager : Gdk::Display::get_default()->get_device_manager()));
		auto refFactory = std::make_unique<Mas::FakeGtkWindowDataFactory>();
		refInstance->init(refFactory, refBackend);
		return refInstance;
	}
	FakeMasGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClasses
						, KeyRepeat::MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter) noexcept
	: MasGtkDeviceManager(bEnableEventClasses, aEnDisableEventClasses, eKeyRepeatMode, refGdkConverter)
	, m_p0Factory(nullptr)
	, m_p0Backend(nullptr)
	{
//std::cout << "FakeMasGtkDeviceManager::FakeMasGtkDeviceManager" << '\n';
	}

	bool makeWindowActive(const shared_ptr<stmi::GtkAccessor>& refGtkAccessor) noexcept
	{
		assert(m_p0Factory != nullptr);
		assert(refGtkAccessor);
		auto p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
		if (refGtkAccessor->isDeleted()) {
			return false; //-----------------------------------------------------
		}
		auto& refOldWinData = m_p0Factory->getActiveWindowData();
		if (refOldWinData) {
			if (refOldWinData->getAccessor()->getGtkmmWindow() == p0GtkmmWindow) {
				// already active
				return true; //-----------------------------------------------------
			}
			// set old active to inactive
			refOldWinData->simulateWindowSetActive(false);
		}
		auto& refNewWinData = m_p0Factory->getFakeWindowData(p0GtkmmWindow);
		refNewWinData->simulateWindowSetActive(true);
		return true;
	}
	Mas::FakeGtkWindowDataFactory* getFactory() noexcept { return m_p0Factory; }
	Mas::FakeGtkBackend* getBackend() noexcept { return m_p0Backend; }
protected:
	void init(std::unique_ptr<Mas::FakeGtkWindowDataFactory>& refFactory
			, std::unique_ptr<Mas::FakeGtkBackend>& refBackend)
	{
		m_p0Factory = refFactory.get();
		m_p0Backend = refBackend.get();
		std::unique_ptr<Private::Mas::GtkWindowDataFactory> refF(refFactory.release());
		std::unique_ptr<Private::Mas::GtkBackend> refB(refBackend.release());
		assert(m_p0Factory == refF.get());
		assert(m_p0Backend == refB.get());
		MasGtkDeviceManager::init(refF, refB);
	}
private:
	Mas::FakeGtkWindowDataFactory* m_p0Factory;
	Mas::FakeGtkBackend* m_p0Backend;
};

} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FAKE_MAS_GTK_DEVICE_MANAGER_H */
