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
 * File:   fakejsgtkwindowdata.h
 */

#ifndef STMI_TESTING_FAKE_JS_GTK_WINDOW_DATA_H
#define STMI_TESTING_FAKE_JS_GTK_WINDOW_DATA_H

#include "jsgtkwindowdata.h"
#include "fakejsgtkbackend.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{
namespace Js
{

class FakeGtkWindowDataFactory;

////////////////////////////////////////////////////////////////////////////////
class FakeGtkWindowData : public Private::Js::GtkWindowData
{
public:
	FakeGtkWindowData() noexcept
	: Private::Js::GtkWindowData()
	, m_bIsEnabled(false)
	, m_bWindowActive(false)
	{
	}
	void enable(const shared_ptr<GtkAccessor>& refAccessor, JsGtkDeviceManager* p0Owner) noexcept override
	{
		assert(refAccessor);
		assert(p0Owner != nullptr);
		setOwner(p0Owner);
		m_refAccessor = refAccessor;
		m_bIsEnabled= true;
	}
	// !!! Doesn't reset accessor!
	void disable() noexcept override
	{
		m_bIsEnabled = false;
		disconnect();
	}
	bool isEnabled() const noexcept override
	{
		return m_bIsEnabled;
	}
	const shared_ptr<GtkAccessor>& getAccessor() noexcept override
	{
		return m_refAccessor;
	}
	bool isWindowActive() const noexcept override
	{
		return m_bWindowActive;
	}

	//
	void simulateWindowSetActive(bool bActive) noexcept
	{
		if (m_bWindowActive == bActive) {
			// no change
			return;
		}
		m_bWindowActive = bActive;
		onSigIsActiveChanged();
	}

private:
	void disconnect() noexcept
	{
//std::cout << "GtkWindowData::disconnect()" << std::endl;
		m_aConnectedDevices.clear();
	}

private:
	friend class FakeGtkWindowDataFactory;
	shared_ptr<GtkAccessor> m_refAccessor;
	//
	std::vector<int32_t> m_aConnectedDevices; // Value: device id
	//
	bool m_bIsEnabled;
	//
	bool m_bWindowActive;
};

////////////////////////////////////////////////////////////////////////////////
class FakeGtkWindowDataFactory : public Private::Js::GtkWindowDataFactory
{
public:
	FakeGtkWindowDataFactory() noexcept // FakeGtkBackend* p0FakeGtkBackend
	: Private::Js::GtkWindowDataFactory()
	{
	}
	std::shared_ptr<Private::Js::GtkWindowData> create() noexcept override
	{
		for (auto& refGtkWindowData : m_aFreePool) {
			if (refGtkWindowData.use_count() == 1) {
				// Recycle
				return refGtkWindowData; //-------------------------------------
			}
		}
		// The data is left in the pool!
		m_aFreePool.emplace_back(std::make_shared<FakeGtkWindowData>()); //m_p0FakeGtkBackend
		auto& refNew = m_aFreePool.back();
		return refNew;
	}
	const std::shared_ptr<FakeGtkWindowData>& getFakeWindowData(Gtk::Window* p0GtkmmWindow) noexcept
	{
		for (auto& refGtkWindowData : m_aFreePool) {
			if (!(refGtkWindowData.use_count() == 1)) {
				auto& refAccessor = refGtkWindowData->getAccessor();
				if ((refAccessor) && (!refAccessor->isDeleted()) && (refAccessor->getGtkmmWindow() == p0GtkmmWindow)) {
					return refGtkWindowData;
				}
			}
		}
		static std::shared_ptr<FakeGtkWindowData> s_refEmpty{};
		return s_refEmpty;
	}
	// There should be max one active window at any time
	// Any way this function returns the first active found.
	const std::shared_ptr<FakeGtkWindowData>& getActiveWindowData() noexcept
	{
		for (auto& refGtkWindowData : m_aFreePool) {
			if (!(refGtkWindowData.use_count() == 1)) {
				auto& refAccessor = refGtkWindowData->getAccessor();
				if ((refAccessor) && (!refAccessor->isDeleted()) && refGtkWindowData->isWindowActive()) {
					return refGtkWindowData;
				}
			}
		}
		static std::shared_ptr<FakeGtkWindowData> s_refEmpty{};
		return s_refEmpty;
	}
private:
	// The objects in the free pool might still be in use when the
	// removal of the accessor was done during a callback. This is detected
	// through the ref count of the shared_ptr.
	std::vector< std::shared_ptr<FakeGtkWindowData> > m_aFreePool;
};

} // namespace Js
} // namespace testing

} // namespace stmi

#endif /* STMI_TESTING_FAKE_JS_GTK_WINDOW_DATA_H */
