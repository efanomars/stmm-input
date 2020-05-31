/*
 * Copyright © 2016-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   jsgtkbackend.h
 */

#ifndef STMI_JS_GTK_BACKEND_H
#define STMI_JS_GTK_BACKEND_H

#include "jsgtkdevicemanager.h"
#include "joysticksources.h"

#include <gtkmm.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <stdint.h>

namespace stmi { class JsDeviceFiles; }
namespace stmi { namespace Private { namespace Js { class JoystickDevice; } } }


namespace stmi
{

namespace Private
{
namespace Js
{

using std::shared_ptr;
using std::weak_ptr;
using std::unique_ptr;

////////////////////////////////////////////////////////////////////////////////
class GtkBackend
{
public:
	static std::pair<unique_ptr<GtkBackend>, std::string> create(JsGtkDeviceManager* p0Owner, const JsDeviceFiles& oDeviceFiles, bool bCreateDefault) noexcept;
	virtual ~GtkBackend() = default;
protected:
	explicit GtkBackend(JsGtkDeviceManager* p0Owner) noexcept;
	std::string init(const JsDeviceFiles& oDeviceFiles, bool bCreateDefault) noexcept;
	// For FakeGtkBackend
	void onDeviceRemoved(int32_t nId) noexcept
	{
		m_p0Owner->onDeviceRemoved(nId);
	}
	// For FakeGtkBackend
	const shared_ptr<Private::Js::JoystickDevice>& onDeviceAdded(const std::string& sName, const std::vector<int32_t>& aButtonCode
												, int32_t nTotHats, const std::vector<int32_t>& aAxisCode) noexcept
	{
		return m_p0Owner->onDeviceAdded(sName, aButtonCode, nTotHats, aAxisCode);
	}
private:
	// returns error string
	std::string addDevices() noexcept;
	// returns false if convert error
	static bool splitPathName(const std::string& sPathName, std::string& sPath, std::string& sName) noexcept;
	// Ex. ("/","dev4") -> "/dev4",   ("/dev/input/","js7") -> "/dev/input/js7",   ("/dev/input","js8") -> "/dev/input/js8"
	// returns false if convert error
	static bool composePathName(const std::string& sPath, const std::string& sName, std::string& sPathName) noexcept;
	// Remove trailing '/' if not root directory.
	// Ex. "/dev/input/" -> "/dev/input",  "/" -> "/"
	// Returns false if convert error
	static bool cleanPath(std::string& sPath) noexcept;

	// bAdd==true, file added, otherwise file removed
	bool doINotifyEventCallback(const std::string& sPath, const std::string& sName, bool bAdd) noexcept;
	// this gives the device file time to be set up
	bool doTimeoutSourceCallback(const std::string& sPathName, int32_t nElapsedMillisec) noexcept;

	bool addIfJoystick(const std::string& sPathName) noexcept;
	bool getButtonMapping(int32_t nFD, int32_t nTotButtons, std::vector<int32_t>& aButtonCode) noexcept;
	bool getAxisMapping(int32_t nFD, int32_t nTotAxes, std::vector<int32_t>& aAxisCode) noexcept;

private:
	JsGtkDeviceManager* m_p0Owner;
	// The INotify Sources, one for each distinct path in oDeviceFiles passed to init()
	std::unordered_map<std::string, shared_ptr<Private::Js::JoystickLifeSource> > m_oPathSources; // Key: sPath

	std::vector< Glib::RefPtr<JoystickInputSource> > m_aInputSources;

	static const char* const s_sDefaultPathBase;

	constexpr static int32_t s_nTimeoutRetryMsec = 1000;
	constexpr static int32_t s_nTimeoutMaxRetryMsec = 4 * s_nTimeoutRetryMsec;
private:
	GtkBackend(const GtkBackend& oSource) = delete;
	GtkBackend& operator=(const GtkBackend& oSource) = delete;
};

} // namespace Js
} // namespace Private

} // namespace stmi

#endif /* STMI_JS_GTK_BACKEND_H */
