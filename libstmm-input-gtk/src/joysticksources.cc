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
 * File:   joysticksources.cc
 */

#include "joysticksources.h"

#include <gdkmm.h>
#include <glibmm.h>

#include <gdk/gdkx.h>

#include <X11/extensions/XI2.h>

#ifndef NDEBUG
#include <iostream>
#endif //NDEBUG

namespace stmi
{

namespace Private
{
namespace Js
{

JoystickLifeSource::JoystickLifeSource(const std::string& sPath)
: Glib::Source()
, m_sPath(sPath)
, m_nINotifyFD(-1)
, m_nWatchDevFD(-1)
{
	static_assert(sizeof(int) <= sizeof(int32_t), "");
	static_assert(false == FALSE, "");
	static_assert(true == TRUE, "");
	m_nINotifyFD = inotify_init1(IN_NONBLOCK);
	if (m_nINotifyFD == -1) {
		return;
	}
	m_nWatchDevFD = inotify_add_watch(m_nINotifyFD, sPath.c_str(), IN_CREATE | IN_DELETE);
	if (m_nWatchDevFD == -1) {
		close(m_nINotifyFD);
		m_nINotifyFD = -1;
		return;
	}

	m_oINotifyPollFD.set_fd(m_nINotifyFD);
	m_oINotifyPollFD.set_events(Glib::IO_IN);
	add_poll(m_oINotifyPollFD);

	set_priority( (Glib::PRIORITY_DEFAULT > Glib::PRIORITY_LOW) ? Glib::PRIORITY_DEFAULT + 1 : Glib::PRIORITY_DEFAULT - 1);
	set_can_recurse(false);
}
JoystickLifeSource::~JoystickLifeSource()
{
	if (m_nINotifyFD != -1) {
		close(m_nINotifyFD);
	}
//std::cout << "JoystickLifeSource::~JoystickLifeSource()" << std::endl;
}
void JoystickLifeSource::addSingleFileName(const std::string& sName)
{
	if (std::find(m_aSingleFileName.begin(), m_aSingleFileName.end(), sName) == m_aSingleFileName.end()) {
		m_aSingleFileName.push_back(sName);
	}
}
void JoystickLifeSource::addNumberedFilesBaseName(const std::string& sBaseName)
{
	try {
		const Glib::ustring sUtf8BaseName = Glib::filename_to_utf8(sBaseName);
		m_oNumberedFilesBaseNames.push_back(sUtf8BaseName);
	} catch (Glib::ConvertError) {
		throw std::runtime_error("Couldn't convert file to utf8 :" + sBaseName);
	}
}
bool JoystickLifeSource::isFileNameMine(const std::string& sFileName) const
{
	// utf8 conversion probably not needed, but this isn't time critical
	Glib::ustring sUtf8FileName;
	try {
		sUtf8FileName = Glib::filename_to_utf8(sFileName);
	} catch (Glib::ConvertError) {
		return false;
	}
	const size_t nFileNameSize = sUtf8FileName.size();
	for (auto& sUtf8BaseName : m_oNumberedFilesBaseNames) {
		const size_t nBaseNameSize = sUtf8BaseName.size();
		if ((nFileNameSize > nBaseNameSize) 
				&& (sUtf8FileName.substr(0, nBaseNameSize) == sUtf8BaseName)
				&& (sUtf8FileName.find_first_not_of("0123456789", nBaseNameSize) == Glib::ustring::npos)) {
			return true; //----------------------------------------------------
		}
	}
	auto itFindSingle = std::find(m_aSingleFileName.begin(), m_aSingleFileName.end(), sFileName);
	return (itFindSingle != m_aSingleFileName.end());
}
sigc::connection JoystickLifeSource::connect(const sigc::slot<bool, const std::string&, const std::string&, bool >& slot)
{
	if (m_nINotifyFD == -1) {
		// File error, return an empty connection
		return sigc::connection();
	}
	return connect_generic(slot);
}

bool JoystickLifeSource::prepare(int& timeout)
{
	timeout = -1;

	return false;
}
bool JoystickLifeSource::check()
{
	bool bRet = false;

	if ((m_oINotifyPollFD.get_revents() & G_IO_IN) != 0) {
		bRet = true;
	}

	return bRet;
}
bool JoystickLifeSource::dispatch(sigc::slot_base* p0Slot)
{
//static int32_t s_nCounter = 0;
	bool bContinue = true;

	if (p0Slot == nullptr) {
		return bContinue; //----------------------------------------------------
	}
//std::cout << "JoystickLifeSource::dispatch" << std::endl;

	char aBuf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));

	const ssize_t nLen = read(m_oINotifyPollFD.get_fd(), aBuf, sizeof aBuf);
	if ((nLen == -1) && (errno != EAGAIN)) {
		return bContinue; //----------------------------------------------------
	}
	// If the nonblocking read() found no events to read, then
	// it returns -1 with errno set to EAGAIN. In that case,
	// we exit the loop.
	if (nLen <= 0) {
		return bContinue; //----------------------------------------------------
	}

	struct inotify_event* p0Event = (struct inotify_event *)aBuf;
	for (char* p0Cur = aBuf; p0Cur < (aBuf + nLen); p0Cur += sizeof(struct inotify_event) + p0Event->len) {
		p0Event = (struct inotify_event *)p0Cur;
		if ((p0Event->mask & IN_ISDIR) != 0) {
			continue; // for p0Cur --------
		}
		bool bAdd = true;
		if ((p0Event->mask & IN_DELETE) != 0) {
			bAdd = false;
		} else if ((p0Event->mask & IN_CREATE) != 0) {
			//
		} else {
			continue; // for p0Cur --------
		}
		std::string sFileName(p0Event->name);
		if (isFileNameMine(sFileName)) {
			bContinue = (*static_cast<sigc::slot<bool, const std::string&, const std::string&, bool>*>(p0Slot))(m_sPath, sFileName, bAdd);
		}
		if (!bContinue) {
			break; // for p0Cur --------
		}
	}
	return bContinue;
}

////////////////////////////////////////////////////////////////////////////////
DevInitTimeoutSource::DevInitTimeoutSource(int32_t nInterval, const std::string& sDevFileName)
: Glib::TimeoutSource(nInterval)
, m_sDevFileName(sDevFileName)
, m_nInterval(nInterval)
, m_nTotMillisec(0)
{
	assert(nInterval >= 0);
}
DevInitTimeoutSource::~DevInitTimeoutSource()
{
//std::cout << "DevInitTimeoutSource::~DevInitTimeoutSource()" << std::endl;
}
sigc::connection DevInitTimeoutSource::connectSlot(const sigc::slot<bool, const std::string&, int32_t>& oSlot)
{
	m_oCallback = oSlot;
	const sigc::slot<bool> oNoParamSlot = sigc::mem_fun(this, &DevInitTimeoutSource::callback);
	return connect_generic(oNoParamSlot);
}
bool DevInitTimeoutSource::callback()
{
	if (m_nInterval < 0) {
		return false;
	}
	if (!m_oCallback) {
		return false;
	}
	m_nTotMillisec += m_nInterval;
	if (m_nInterval == 0) {
		m_nInterval = -1; // try just once
	}
	return m_oCallback(m_sDevFileName, m_nTotMillisec);
}

////////////////////////////////////////////////////////////////////////////////
JoystickInputSource::JoystickInputSource(int32_t nFD, const std::string& sPathName)
: m_sPathName(sPathName)
{
	static_assert(sizeof(int) <= sizeof(int32_t), "");
	static_assert(false == FALSE, "");
	static_assert(true == TRUE, "");
	//
	m_oPollFD.set_fd(nFD);
	m_oPollFD.set_events(Glib::IO_IN);
	add_poll(m_oPollFD);
	set_can_recurse(false);
}
JoystickInputSource::~JoystickInputSource()
{
//std::cout << "JoystickInputSource::~JoystickInputSource()" << std::endl;
}
sigc::connection JoystickInputSource::connect(const sigc::slot<bool, const struct ::js_event*>& slot)
{
	return connect_generic(slot);
}
	//int32_t JoystickInputSource::getJoystickFD() const
	//{
	//	return m_oPollFD.get_fd();
	//}
bool JoystickInputSource::prepare(int& timeout)
{
	timeout = -1;

	return false;
}
bool JoystickInputSource::check()
{
	if ((m_oPollFD.get_revents() & G_IO_IN) != 0) {
		return true;
	}
	return false;
}
bool JoystickInputSource::dispatch(sigc::slot_base* p0Slot)
{
//static int32_t s_nCounter = 0;
	bool bContinue = true;

	if (p0Slot == nullptr) {
		return bContinue;
	}
//std::cout << "JoystickInputSource::dispatch" << std::endl;

	if ((m_oPollFD.get_revents() & G_IO_IN) != 0) {
		struct ::js_event aEvent[32];
		const int32_t nBufLen = sizeof aEvent;
		while (true) {
			const int32_t nReadLen = read(m_oPollFD.get_fd(), aEvent, nBufLen);
			if (nReadLen < 0) {
				break; // while --------------------
			}
			const int32_t nTotEvents = nReadLen / sizeof(aEvent[0]);
			for (int32_t nIdx = 0; nIdx < nTotEvents; ++nIdx) {
				struct ::js_event* p0Event = &(aEvent[nIdx]);
				bContinue = (*static_cast<sigc::slot<bool, const struct ::js_event*>*>(p0Slot))(p0Event);
				if (!bContinue) {
					// interrupt dispatching
					return bContinue; // ---------------------------------------
				}
			}
		}
	}
	return bContinue;
}

} // namespace Js
} // namespace Private

} // namespace stmi