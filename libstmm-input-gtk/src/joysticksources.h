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
 * File:   joysticksources.h
 */

#ifndef _STMI_JOYSTICK_SOURCES_H_
#define _STMI_JOYSTICK_SOURCES_H_

#include <gtkmm.h>

#include <linux/joystick.h>

#include <sys/inotify.h>

#include <cassert>
#include <vector>
#include <string>

namespace stmi
{

namespace Private
{
namespace Js
{

// INotify tracking of added and removed devices in a folder
class JoystickLifeSource : public Glib::Source
{
public:
	JoystickLifeSource(const std::string& sPath);
	virtual ~JoystickLifeSource();

	void addSingleFileName(const std::string& sName);
	void addNumberedFilesBaseName(const std::string& sBaseName);
	// A source can have only one callback type, that is the slot given as parameter.
	// bool = m_oCallback(sPath, sName, bAdd)
	// sPath the directory, sName the file added or removed, bAdd whether added (true) or removed (false)
	// The callback`s return (bool) value tells whether the source should go on listening.
	sigc::connection connect(const sigc::slot<bool, const std::string&, const std::string&, bool>& slot);

	inline const std::string& getPath() const { return m_sPath; }
	bool isFileNameMine(const std::string& sFileName) const;
protected:
	bool prepare(int& timeout) override;
	bool check() override;
	bool dispatch(sigc::slot_base* slot) override;

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
	JoystickLifeSource(const JoystickLifeSource& oSource) = delete;
	JoystickLifeSource& operator=(const JoystickLifeSource& oSource) = delete;
};

////////////////////////////////////////////////////////////////////////////////
class DevInitTimeoutSource : public Glib::TimeoutSource
{
public:
	DevInitTimeoutSource(int32_t nInterval, const std::string& sDevFileName);
	virtual ~DevInitTimeoutSource();

	// cover the non-virtual superclass member function which shouldn't be used
	sigc::connection connect(const sigc::slot<bool>& /*oSlot*/) //override
	{
		assert(false);
		return sigc::connection();
	}
	sigc::connection connectSlot(const sigc::slot<bool, const std::string&, int32_t>& oSlot);
protected:
	bool callback();
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
};


////////////////////////////////////////////////////////////////////////////////
// For polling joystick events
class JoystickInputSource : public Glib::Source
{
public:
	JoystickInputSource(int32_t nFD, const std::string& sPathName);
	// Closes file descriptor on destruction
	virtual ~JoystickInputSource();

	// A source can have only one callback type, that is the slot given as parameter
	sigc::connection connect(const sigc::slot<bool, const struct ::js_event*>& slot);

	//int32_t getJoystickFD() const;
	const std::string& getJoystickPathName() const { return m_sPathName; }
protected:
	bool prepare(int& timeout) override;
	bool check() override;
	bool dispatch(sigc::slot_base* slot) override;

private:
	//
	Glib::PollFD m_oPollFD;
	std::string m_sPathName; // "/dev/input/jsN"
	//
private:
	JoystickInputSource(const JoystickInputSource& oSource) = delete;
	JoystickInputSource& operator=(const JoystickInputSource& oSource) = delete;
};

} // namespace Js
} // namespace Private

} // namespace stmi

#endif /* _STMI_JOYSTICK_SOURCES_H_ */
