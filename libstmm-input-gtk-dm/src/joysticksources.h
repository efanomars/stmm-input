/*
 * Copyright © 2016-2019  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   joysticksources.h
 */

#ifndef STMI_JOYSTICK_SOURCES_H
#define STMI_JOYSTICK_SOURCES_H

#include <glibmm.h>

#include <cassert>
#include <vector>
#include <string>
#include <cstdint>

struct js_event;

namespace stmi
{

namespace Private
{
namespace Js
{

////////////////////////////////////////////////////////////////////////////////
/* INotify tracking of added and removed devices in a folder */
class JoystickLifeSource : public Glib::Source
{
public:
	explicit JoystickLifeSource(const std::string& sPath) noexcept;
	~JoystickLifeSource() noexcept;

	void addSingleFileName(const std::string& sName) noexcept;
	void addNumberedFilesBaseName(const std::string& sBaseName) noexcept;
	// A source can have only one callback type, that is the slot given as parameter.
	// bool = m_oCallback(sPath, sName, bAdd)
	// sPath the directory, sName the file added or removed, bAdd whether added (true) or removed (false)
	// The callback's return (bool) value tells whether the source should go on listening.
	sigc::connection connect(const sigc::slot<bool, const std::string&, const std::string&, bool>& oSlot) noexcept;

	inline const std::string& getPath() const noexcept { return m_sPath; }
	bool isFileNameMine(const std::string& sFileName) const noexcept;
protected:
	bool prepare(int& nTimeout) noexcept override;
	bool check() noexcept override;
	bool dispatch(sigc::slot_base* oSlot) noexcept override;

private:
	//
	const std::string m_sPath;
	std::vector<std::string> m_aSingleFileName;
	std::vector<Glib::ustring> m_oNumberedFilesBaseNames;
	//
	int32_t m_nINotifyFD;
	//
	int32_t m_nWatchDevFD;
	Glib::PollFD m_oINotifyPollFD;
	//
private:
	JoystickLifeSource() = delete;
	JoystickLifeSource(const JoystickLifeSource& oSource) = delete;
	JoystickLifeSource& operator=(const JoystickLifeSource& oSource) = delete;
};

////////////////////////////////////////////////////////////////////////////////
class DevInitTimeoutSource : public Glib::TimeoutSource
{
public:
	DevInitTimeoutSource(int32_t nInterval, const std::string& sDevFileName) noexcept;
	~DevInitTimeoutSource() noexcept;

	// cover the non-virtual superclass member function which shouldn't be used
	sigc::connection connect(const sigc::slot<bool>& /*oSlot*/) noexcept //override
	{
		assert(false);
		return sigc::connection();
	}
	sigc::connection connectSlot(const sigc::slot<bool, const std::string&, int32_t>& oSlot) noexcept;
protected:
	bool callback() noexcept;
private:
	// The signature of the callback means:
	// bContinue = m_oCallback(sFileName, nTotMillisec)
	sigc::slot<bool, const std::string&, int32_t> m_oCallback;
	//
	const std::string m_sDevFileName;
	//
	int32_t m_nInterval;
	int32_t m_nTotMillisec;
private:
	DevInitTimeoutSource() = delete;
	DevInitTimeoutSource(const DevInitTimeoutSource& oSource) = delete;
	DevInitTimeoutSource& operator=(const DevInitTimeoutSource& oSource) = delete;
};


////////////////////////////////////////////////////////////////////////////////
/* For polling joystick events */
class JoystickInputSource : public Glib::Source
{
public:
	JoystickInputSource(int32_t nFD, const std::string& sPathName, int64_t nFileSysDeviceId, int32_t nDeviceId) noexcept;
	// Closes file descriptor on destruction
	~JoystickInputSource() noexcept;

	// A source can have only one callback type, that is the slot given as parameter
	sigc::connection connect(const sigc::slot<bool, const struct ::js_event*>& oSlot) noexcept;

	inline const std::string& getJoystickPathName() const noexcept { return m_sPathName; }
	inline int32_t getJoystickFD() const noexcept { return m_oPollFD.get_fd(); }
	inline int64_t getJoystickFileSysDeviceId() const  noexcept{ return m_nFileSysDeviceId; }
	inline int32_t getJoystickId() const noexcept { return m_nDeviceId; }
protected:
	bool prepare(int& nTimeout) noexcept override;
	bool check() noexcept override;
	bool dispatch(sigc::slot_base* oSlot) noexcept override;

private:
	//
	Glib::PollFD m_oPollFD; // The file descriptor is open until destructor is called
	std::string m_sPathName; // "/dev/input/jsN"
	//
	const int64_t m_nFileSysDeviceId; // The stat rdev field (should be unique)
	const int32_t m_nDeviceId;
	//
private:
	JoystickInputSource() = delete;
	JoystickInputSource(const JoystickInputSource& oSource) = delete;
	JoystickInputSource& operator=(const JoystickInputSource& oSource) = delete;
};

} // namespace Js
} // namespace Private

} // namespace stmi

#endif /* STMI_JOYSTICK_SOURCES_H */
