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
 * File:   fakejsmasgtkdevicemanager.h
 */

#ifndef _STMI_TESTING_FAKE_JS_GTK_DEVICE_MANAGER_H_
#define _STMI_TESTING_FAKE_JS_GTK_DEVICE_MANAGER_H_

#include "jsgtkdevicemanager.h"

#include "fakejsgtkbackend.h"
#include "fakejsgtkwindowdata.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class FakeJsGtkDeviceManager : public JsGtkDeviceManager
{
public:
	static shared_ptr<FakeJsGtkDeviceManager> create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass)
	{
		shared_ptr<FakeJsGtkDeviceManager> refInstance(
				new FakeJsGtkDeviceManager(bEnableEventClasses, aEnDisableEventClass));
//std::cout << "FakeJsGtkDeviceManager::create 1" << '\n';
		auto refBackend = std::make_unique<Js::FakeGtkBackend>(refInstance.operator->());
		auto refFactory = std::make_unique<Js::FakeGtkWindowDataFactory>(); //refBackend.operator->()
		refInstance->init(refFactory, refBackend);
		return refInstance;
	}
	FakeJsGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass)
	: JsGtkDeviceManager(bEnableEventClasses, aEnDisableEventClass)
	, m_p0Factory(nullptr)
	, m_p0Backend(nullptr)
	{
//std::cout << "FakeJsGtkDeviceManager::FakeJsGtkDeviceManager" << '\n';
	}

	bool makeWindowActive(const shared_ptr<stmi::GtkAccessor>& refGtkAccessor)
	{
//std::cout << "FakeJsGtkDeviceManager::makeWindowActive 0" << '\n';
		assert(m_p0Factory != nullptr);
		assert(refGtkAccessor);
		auto p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
		if (refGtkAccessor->isDeleted()) {
			return false; //-----------------------------------------------------
		}
		auto& refOldWinData = m_p0Factory->getActiveWindowData();
		if (refOldWinData) {
			assert(refOldWinData->getAccessor());
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
	Js::FakeGtkWindowDataFactory* getFactory() { return m_p0Factory; }
	Js::FakeGtkBackend* getBackend() { return m_p0Backend; }
protected:
	void init(std::unique_ptr<Js::FakeGtkWindowDataFactory>& refFactory
			, std::unique_ptr<Js::FakeGtkBackend>& refBackend)
	{
		m_p0Factory = refFactory.get();
		m_p0Backend = refBackend.get();
		std::unique_ptr<Private::Js::GtkWindowDataFactory> refF(refFactory.release());
		std::unique_ptr<Private::Js::GtkBackend> refB(refBackend.release());
		assert(m_p0Factory == refF.get());
		assert(m_p0Backend == refB.get());
		JsGtkDeviceManager::init(refF, refB);
	}
private:
	Js::FakeGtkWindowDataFactory* m_p0Factory;
	Js::FakeGtkBackend* m_p0Backend;
};

} // namespace testing

} // namespace stmi

#endif	/* _STMI_TESTING_FAKE_JS_GTK_DEVICE_MANAGER_H_ */
