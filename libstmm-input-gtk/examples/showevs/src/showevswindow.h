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
 * File:   showevswindow.h
 */

#ifndef _EXAMPLE_SHOW_EVS_WINDOW_
#define _EXAMPLE_SHOW_EVS_WINDOW_

#include "inputstrings.h"

#include <stmm-input-ev/stmm-input-ev.h>
#include <stmm-input-gtk/gtkaccessor.h>

#include <gtkmm.h>
#include <gdkmm.h>

#include <string>
#include <memory>

namespace example
{

namespace showevs
{

using std::shared_ptr;
using std::weak_ptr;

class ShowEvsWindow : public Gtk::Window
{
public:
	ShowEvsWindow(const shared_ptr<stmi::DeviceManager>& refDM, const Glib::ustring sTitle);
	virtual ~ShowEvsWindow();
private:
	void handleAllEvents(const shared_ptr<stmi::Event>& refEvent);
	void handleKeyEvents(const shared_ptr<stmi::Event>& refEvent);
	void handlePointerEvents(const shared_ptr<stmi::Event>& refEvent);
	void handleTouchEvents(const shared_ptr<stmi::Event>& refEvent);
	void handleJoystickEvents(const shared_ptr<stmi::Event>& refEvent);
	void handleDeviceMgmtEvents(const shared_ptr<stmi::Event>& refEvent);

	void handleDeviceMgmtEventsPermanent(const shared_ptr<stmi::Event>& refEvent);

	void printEvent(const std::string& sPre, const shared_ptr<stmi::Event>& refEvent);
	void printKeyEvent(const std::string& sPre, const shared_ptr<stmi::KeyEvent>& refEvent);
	void printPointerEvent(const std::string& sPre, const shared_ptr<stmi::PointerEvent>& refEvent);
	void printPointerScrollEvent(const std::string& sPre, const shared_ptr<stmi::PointerScrollEvent>& refEvent);
	void printTouchEvent(const std::string& sPre, const shared_ptr<stmi::TouchEvent>& refEvent);
	void printJoystickHatEvent(const std::string& sPre, const shared_ptr<stmi::JoystickHatEvent>& refEvent);
	void printJoystickButtonEvent(const std::string& sPre, const shared_ptr<stmi::JoystickButtonEvent>& refEvent);
	void printJoystickAxisEvent(const std::string& sPre, const shared_ptr<stmi::JoystickAxisEvent>& refEvent);
	void printDeviceMgmtEvent(const std::string& sPre, const shared_ptr<stmi::DeviceMgmtEvent>& refEvent);
	Glib::ustring getEventString(const std::string& sPre, const shared_ptr<stmi::Event>& refEvent);
	void printString(const Glib::ustring& sStr);

	void recreateDeviceList();

	void onSelectAllEventsClicked();
	void onSelectKeyEventsClicked();
	void onSelectPointerEventsClicked();
	void onSelectTouchEventsClicked();
	void onSelectJoystickEventsClicked();
	void onSelectDeviceMgmtEventsClicked();
	void onSelectEventsClicked(Gtk::CheckButton& oEventsCheck, bool& bListenerEventsActive
								, shared_ptr<stmi::EventListener>& refListener, shared_ptr<stmi::CallIf>& refCallIf);

	void onWindowAButtonClicked();
	void onWindowBButtonClicked();
	void onDialogDButtonClicked();

	void onWindowAHide();
	void onWindowBHide();

private:
	const shared_ptr<stmi::DeviceManager>& m_refDM;
	shared_ptr<stmi::GtkAccessor> m_refWinAAccessor;
	shared_ptr<stmi::GtkAccessor> m_refWinBAccessor;
	shared_ptr<stmi::GtkAccessor> m_refDiaDAccessor;

	bool m_bListenerAllEventsActive;
	bool m_bListenerKeyEventsActive;
	bool m_bListenerPointerEventsActive;
	bool m_bListenerTouchEventsActive;
	bool m_bListenerJoystickEventsActive;
	bool m_bListenerDeviceMgmtEventsActive;

	shared_ptr<stmi::EventListener> m_refListenerAllEvents;
	shared_ptr<stmi::EventListener> m_refListenerKeyEvents;
	shared_ptr<stmi::EventListener> m_refListenerPointerEvents;
	shared_ptr<stmi::EventListener> m_refListenerTouchEvents;
	shared_ptr<stmi::EventListener> m_refListenerJoystickEvents;
	shared_ptr<stmi::EventListener> m_refListenerDeviceMgmtEvents;
	shared_ptr<stmi::CallIf> m_refCallIfAll;
	shared_ptr<stmi::CallIf> m_refCallIfKey;
	shared_ptr<stmi::CallIf> m_refCallIfPointer;
	shared_ptr<stmi::CallIf> m_refCallIfTouch;
	shared_ptr<stmi::CallIf> m_refCallIfJoystick;
	shared_ptr<stmi::CallIf> m_refCallIfDeviceMgmt;

	shared_ptr<stmi::EventListener> m_refListenerDeviceMgmtEventsPermanent;

	Glib::RefPtr<Gtk::Window> m_refWindowA;
	Glib::RefPtr<Gtk::Window> m_refWindowB;
	Glib::RefPtr<Gtk::Dialog> m_refDialogD;

	Gtk::VPaned m_oMainVPaned;
	Gtk::HPaned m_oControlAndDevicesHPaned;
		Gtk::HBox m_oControlHBox;
			Gtk::VBox m_oSelectEventsVBox;
				Gtk::CheckButton m_oAllEventsCheck;
				Gtk::CheckButton m_oKeyEventsCheck;
				Gtk::CheckButton m_oPointerEventsCheck;
				Gtk::CheckButton m_oTouchEventsCheck;
				Gtk::CheckButton m_oJoystickEventsCheck;
				Gtk::CheckButton m_oDeviceMgmtEventsCheck;
			Gtk::HSeparator m_oControlAndSelectEventsSeparator;
			Gtk::VBox m_oOpenWindowsVBox;
				Gtk::Button m_oWindowAButton;
				Gtk::Button m_oWindowBButton;
				Gtk::Button m_oDialogDButton;
		Gtk::ScrolledWindow m_oDevicesScrolled;
			Gtk::TreeView m_oDevicesTreeView;
	Gtk::ScrolledWindow m_oEventsScrolled;
		Gtk::TextView m_oEventsTextView;

	Glib::RefPtr<Gtk::TextBuffer> m_refEventsTextBuffer;
	Glib::RefPtr<Gtk::TextBuffer::Mark> m_refEventsTextBufferBottomMark;
	int32_t m_nEventsTextBufferTotLines;
	static constexpr int32_t s_nEventsTextBufferMaxLines = 2000;

	class DevicesColumns : public Gtk::TreeModel::ColumnRecord
	{
	public:
		DevicesColumns()
		{
			add(m_oColDeviceId);
			add(m_oColName);
			add(m_oColCapabilities);
		}
		Gtk::TreeModelColumn<Glib::ustring> m_oColDeviceId;
		Gtk::TreeModelColumn<Glib::ustring> m_oColName;
		Gtk::TreeModelColumn<Glib::ustring> m_oColCapabilities;
	};
	DevicesColumns m_oDeviceColumns;
	Glib::RefPtr<Gtk::ListStore> m_refTreeModelDevices;

	InputStrings m_oInputStrings;

	static constexpr int32_t s_nInitialWindowSizeW = 800;
	static constexpr int32_t s_nInitialWindowSizeH = 500;
};

} // namespace showevs

} // namespace example

#endif	/* _EXAMPLE_SHOW_EVS_WINDOW_ */
