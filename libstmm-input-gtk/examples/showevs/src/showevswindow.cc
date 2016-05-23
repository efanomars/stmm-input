/*
 * File:   showevswindow.cc
 */

#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>

#include "showevswindow.h"

namespace example
{

namespace showevs
{

class AllKeysDialog : public Gtk::Dialog
{
public:
	virtual ~AllKeysDialog() = default;
protected:
	// Too receive most of the keys, like cursor keys, TAB, ENTER, etc. override default handlers
	bool on_key_press_event(GdkEventKey* /*p0Event*/) override
	{
		return false; // propagate
	}
	bool on_key_release_event(GdkEventKey* /*p0Event*/) override
	{
		return false; // propagate
	}
};

ShowEvsWindow::ShowEvsWindow(const shared_ptr<stmi::DeviceManager>& refDM, const Glib::ustring sTitle)
: m_refDM(refDM)
, m_bListenerAllEventsActive(false)
, m_bListenerKeyEventsActive(false)
, m_bListenerPointerEventsActive(false)
, m_bListenerTouchEventsActive(false)
, m_bListenerJoystickEventsActive(false)
, m_bListenerDeviceMgmtEventsActive(false)
, m_nEventsTextBufferTotLines(0)
{
	//
	set_title(sTitle);
	set_default_size(s_nInitialWindowSizeW, s_nInitialWindowSizeH);
	set_resizable(true);

	m_refTreeModelDevices = Gtk::ListStore::create(m_oDeviceColumns);
	m_oDevicesTreeView.set_model(m_refTreeModelDevices);

	m_oDevicesTreeView.set_headers_visible(true);
	const int32_t nNameCol = -1 + m_oDevicesTreeView.append_column("Device name", m_oDeviceColumns.m_oColName);
	m_oDevicesTreeView.append_column("Id", m_oDeviceColumns.m_oColDeviceId);
	m_oDevicesTreeView.append_column("Capabilities", m_oDeviceColumns.m_oColCapabilities);
	const int32_t nTotColumns = m_oDevicesTreeView.get_n_columns();
	for (int32_t nCol = 0; nCol < nTotColumns; ++nCol) {
		Gtk::TreeViewColumn* p0Column =  m_oDevicesTreeView.get_column(nCol);
		p0Column->set_reorderable(true);
		p0Column->set_clickable(true);
		p0Column->set_expand(nCol == nNameCol);
	}

	m_oDevicesScrolled.set_border_width(5);
	m_oDevicesScrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	m_oDevicesScrolled.add(m_oDevicesTreeView);

	m_oAllEventsCheck.set_label("Listener All (A)");
	m_oKeyEventsCheck.set_label("Listener KeyEvent (K)");
	m_oPointerEventsCheck.set_label("Listener PointerCapability (P)");
	m_oTouchEventsCheck.set_label("Listener TouchEvent (T)");
	m_oJoystickEventsCheck.set_label("Listener JoystickCapability (J)");
	m_oDeviceMgmtEventsCheck.set_label("Listener DeviceMgmtEvent (M)");

	m_oSelectEventsVBox.pack_start(m_oAllEventsCheck);
	m_oSelectEventsVBox.pack_start(m_oKeyEventsCheck);
	m_oSelectEventsVBox.pack_start(m_oPointerEventsCheck);
	m_oSelectEventsVBox.pack_start(m_oTouchEventsCheck);
	m_oSelectEventsVBox.pack_start(m_oJoystickEventsCheck);
	m_oSelectEventsVBox.pack_start(m_oDeviceMgmtEventsCheck);

	m_oWindowAButton.set_label("Window A");
	m_oWindowBButton.set_label("Window B");
	m_oDialogDButton.set_label("Dialog D");

	m_oOpenWindowsVBox.pack_start(m_oWindowAButton);
	m_oOpenWindowsVBox.pack_start(m_oWindowBButton);
	m_oOpenWindowsVBox.pack_start(m_oDialogDButton);

	m_oControlHBox.pack_start(m_oSelectEventsVBox);
	m_oControlHBox.pack_start(m_oControlAndSelectEventsSeparator, Gtk::PACK_SHRINK);
	m_oControlHBox.pack_start(m_oOpenWindowsVBox);

	m_oControlAndDevicesHPaned.pack1(m_oControlHBox, false, false);
	m_oControlAndDevicesHPaned.pack2(m_oDevicesScrolled);

	m_refEventsTextBuffer = Gtk::TextBuffer::create();
	m_refEventsTextBufferBottomMark = Gtk::TextBuffer::Mark::create("Bottom");
	m_oEventsTextView.set_editable(false);
	m_oEventsTextView.set_buffer(m_refEventsTextBuffer);
	Pango::FontDescription oMonoFont("Mono");
	m_oEventsTextView.override_font(oMonoFont);
	m_refEventsTextBuffer->add_mark(m_refEventsTextBufferBottomMark, m_refEventsTextBuffer->end());

	m_oDevicesScrolled.set_border_width(5);
	m_oDevicesScrolled.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_ALWAYS);
	m_oEventsScrolled.add(m_oEventsTextView);

	m_oMainVPaned.pack1(m_oControlAndDevicesHPaned, false, false);
	m_oMainVPaned.pack2(m_oEventsScrolled);

	add(m_oMainVPaned);

	m_oAllEventsCheck.set_active(false);
	m_oKeyEventsCheck.set_active(false);
	m_oPointerEventsCheck.set_active(false);
	m_oTouchEventsCheck.set_active(false);
	m_oJoystickEventsCheck.set_active(false);
	m_oDeviceMgmtEventsCheck.set_active(false);

	m_oAllEventsCheck.signal_clicked().connect( sigc::mem_fun(*this, &ShowEvsWindow::onSelectAllEventsClicked) );
	m_oKeyEventsCheck.signal_clicked().connect( sigc::mem_fun(*this, &ShowEvsWindow::onSelectKeyEventsClicked) );
	m_oPointerEventsCheck.signal_clicked().connect( sigc::mem_fun(*this, &ShowEvsWindow::onSelectPointerEventsClicked) );
	m_oTouchEventsCheck.signal_clicked().connect( sigc::mem_fun(*this, &ShowEvsWindow::onSelectTouchEventsClicked) );
	m_oJoystickEventsCheck.signal_clicked().connect( sigc::mem_fun(*this, &ShowEvsWindow::onSelectJoystickEventsClicked) );
	m_oDeviceMgmtEventsCheck.signal_clicked().connect( sigc::mem_fun(*this, &ShowEvsWindow::onSelectDeviceMgmtEventsClicked) );

	m_oWindowAButton.signal_clicked().connect( sigc::mem_fun(*this, &ShowEvsWindow::onWindowAButtonClicked) );
	m_oWindowBButton.signal_clicked().connect( sigc::mem_fun(*this, &ShowEvsWindow::onWindowBButtonClicked) );
	m_oDialogDButton.signal_clicked().connect( sigc::mem_fun(*this, &ShowEvsWindow::onDialogDButtonClicked) );

	m_refListenerAllEvents = std::make_shared<stmi::EventListener>(
						std::bind( &ShowEvsWindow::handleAllEvents, this, std::placeholders::_1 ) );
	m_refCallIfAll = shared_ptr<stmi::CallIf>{};

	m_refListenerKeyEvents = std::make_shared<stmi::EventListener>(
						std::bind( &ShowEvsWindow::handleKeyEvents, this, std::placeholders::_1 ) );
	m_refCallIfKey = std::make_shared<stmi::CallIfEventClass>(stmi::KeyEvent::getClass());

	m_refListenerPointerEvents = std::make_shared<stmi::EventListener>(
						std::bind( &ShowEvsWindow::handlePointerEvents, this, std::placeholders::_1 ) );
	m_refCallIfPointer = std::make_shared<stmi::CallIfCapabilityClass>(stmi::PointerCapability::getClass());

	m_refListenerTouchEvents = std::make_shared<stmi::EventListener>(
						std::bind( &ShowEvsWindow::handleTouchEvents, this, std::placeholders::_1 ) );
	m_refCallIfTouch = std::make_shared<stmi::CallIfEventClass>(stmi::TouchEvent::getClass());

	m_refListenerJoystickEvents = std::make_shared<stmi::EventListener>(
						std::bind( &ShowEvsWindow::handleJoystickEvents, this, std::placeholders::_1 ) );
	m_refCallIfJoystick = std::make_shared<stmi::CallIfCapabilityClass>(stmi::JoystickCapability::getClass());

	m_refListenerDeviceMgmtEvents = std::make_shared<stmi::EventListener>(
						std::bind( &ShowEvsWindow::handleDeviceMgmtEvents, this, std::placeholders::_1 ) );
	m_refCallIfDeviceMgmt = std::make_shared<stmi::CallIfEventClass>(stmi::DeviceMgmtEvent::getClass());

	m_refListenerDeviceMgmtEventsPermanent  = std::make_shared<stmi::EventListener>(
						std::bind( &ShowEvsWindow::handleDeviceMgmtEventsPermanent, this, std::placeholders::_1 ) );
	m_refDM->addEventListener(m_refListenerDeviceMgmtEventsPermanent, m_refCallIfDeviceMgmt);

	m_oAllEventsCheck.set_active(true);
	onSelectAllEventsClicked();

	show_all_children();

	recreateDeviceList();
}
ShowEvsWindow::~ShowEvsWindow()
{
}
void ShowEvsWindow::handleAllEvents(const shared_ptr<stmi::Event>& refEvent)
{
	printEvent("A", refEvent);
}
void ShowEvsWindow::handleKeyEvents(const shared_ptr<stmi::Event>& refEvent)
{
	assert(std::dynamic_pointer_cast<stmi::KeyEvent>(refEvent));
	auto refKeyEvent = std::static_pointer_cast<stmi::KeyEvent>(refEvent);
	printKeyEvent("K", refKeyEvent);
}
void ShowEvsWindow::handlePointerEvents(const shared_ptr<stmi::Event>& refEvent)
{
	printEvent("P", refEvent);
}
void ShowEvsWindow::handleTouchEvents(const shared_ptr<stmi::Event>& refEvent)
{
	assert(std::dynamic_pointer_cast<stmi::TouchEvent>(refEvent));
	auto refTouchEvent = std::static_pointer_cast<stmi::TouchEvent>(refEvent);
	printTouchEvent("T", refTouchEvent);
}
void ShowEvsWindow::handleJoystickEvents(const shared_ptr<stmi::Event>& refEvent)
{
	printEvent("J", refEvent);
}
void ShowEvsWindow::handleDeviceMgmtEvents(const shared_ptr<stmi::Event>& refEvent)
{
	assert(std::dynamic_pointer_cast<stmi::DeviceMgmtEvent>(refEvent));
	auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
	printDeviceMgmtEvent("M", refDeviceMgmtEvent);
}
void ShowEvsWindow::handleDeviceMgmtEventsPermanent(const shared_ptr<stmi::Event>& refEvent)
{
	assert(std::dynamic_pointer_cast<stmi::DeviceMgmtEvent>(refEvent));
	auto refDeviceMgmtEvent = std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent);
	if (refDeviceMgmtEvent->getDeviceMgmtType() != stmi::DeviceMgmtEvent::DEVICE_MGMT_CHANGED) {
		recreateDeviceList();
	}
}
void ShowEvsWindow::printEvent(const std::string& sPre, const shared_ptr<stmi::Event>& refEvent)
{
	const stmi::Event::Class& oC = refEvent->getEventClass();
	auto& oT = oC.getTypeInfo();
	if (oT == typeid(stmi::KeyEvent)) {
		printKeyEvent(sPre, std::static_pointer_cast<stmi::KeyEvent>(refEvent));
	} else if (oT == typeid(stmi::PointerEvent)) {
		printPointerEvent(sPre, std::static_pointer_cast<stmi::PointerEvent>(refEvent));
	} else if (oT == typeid(stmi::PointerScrollEvent)) {
		printPointerScrollEvent(sPre, std::static_pointer_cast<stmi::PointerScrollEvent>(refEvent));
	} else if (oT == typeid(stmi::TouchEvent)) {
		printTouchEvent(sPre, std::static_pointer_cast<stmi::TouchEvent>(refEvent));
	} else if (oT == typeid(stmi::JoystickButtonEvent)) {
		printJoystickButtonEvent(sPre, std::static_pointer_cast<stmi::JoystickButtonEvent>(refEvent));
	} else if (oT == typeid(stmi::JoystickHatEvent)) {
		printJoystickHatEvent(sPre, std::static_pointer_cast<stmi::JoystickHatEvent>(refEvent));
	} else if (oT == typeid(stmi::JoystickAxisEvent)) {
		printJoystickAxisEvent(sPre, std::static_pointer_cast<stmi::JoystickAxisEvent>(refEvent));
	} else if (oT == typeid(stmi::DeviceMgmtEvent)) {
		printDeviceMgmtEvent(sPre, std::static_pointer_cast<stmi::DeviceMgmtEvent>(refEvent));
	} else {
		std::cout << "[" << sPre << "] unknown class id:" << oC.getId() << "  type_info.name:" << oT.name() << std::endl;
	}
}
Glib::ustring ShowEvsWindow::getEventString(const std::string& sPre, const shared_ptr<stmi::Event>& refEvent)
{
	const auto nTimeUsec = refEvent->getTimeUsec();
	Glib::ustring sDevId = "  ";
	auto refCapability = refEvent->getCapability();
	if (refCapability) {
		auto refDevice = refCapability->getDevice();
		if (refDevice) {
			const auto nId = refDevice->getId();
			sDevId = Glib::ustring::compose("%1", nId);
			const int32_t nLen = sDevId.length();
			sDevId.insert(0, std::max(0, 2 - nLen), ' ');
		}
	}
	const auto refAccessor = refEvent->getAccessor();
	std::string sAcc("    ");
	if (refAccessor) {
		if (refAccessor == m_refWinAAccessor) {
			sAcc = "WinA";
		} else if (refAccessor == m_refWinBAccessor) {
			sAcc = "WinB";
		} else if (refAccessor == m_refDiaDAccessor) {
			sAcc = "DiaD";
		} else {
			sAcc = "Main";
		}
	}
	// Shorten event id (leave out "namespace" prefix if any)
	auto sEvClassId = refEvent->getEventClass().getId();
	auto nLastPos = sEvClassId.rfind("::");
	if (nLastPos != std::string::npos) {
		sEvClassId.erase(0, nLastPos + 2);
	}

	return Glib::ustring::compose("[%1][%2] Dev:%3 %4 (%5)", sPre, sAcc, sDevId, sEvClassId, nTimeUsec);
}
void ShowEvsWindow::printKeyEvent(const std::string& sPre, const shared_ptr<stmi::KeyEvent>& refEvent)
{
	const auto sEventStr = getEventString(sPre, refEvent);
	const stmi::HARDWARE_KEY eKey = refEvent->getKey();
	const stmi::KeyEvent::KEY_INPUT_TYPE eType = refEvent->getType();
	Glib::ustring sType;
	if (eType == stmi::KeyEvent::KEY_PRESS) {
		sType = "Press";
	} else if (eType == stmi::KeyEvent::KEY_RELEASE) {
		sType = "Release";
	} else if (eType == stmi::KeyEvent::KEY_RELEASE_CANCEL) {
		sType = "Cancel";
	} else {
		sType = "Error";
	}
	const auto sShow = Glib::ustring::compose("%1 \"%2\" :%3", sEventStr
						, m_oInputStrings.getKeyString(eKey), sType);
	printString(sShow);
}
void ShowEvsWindow::printPointerEvent(const std::string& sPre, const shared_ptr<stmi::PointerEvent>& refEvent)
{
	const auto sEventStr = getEventString(sPre, refEvent);
	const int32_t nButton = refEvent->getButton();
	const stmi::PointerEvent::POINTER_INPUT_TYPE eType = refEvent->getType();
	std::string sType;
	if (eType == stmi::PointerEvent::POINTER_HOVER) {
		sType = "Hover";
	} else if (eType == stmi::PointerEvent::POINTER_MOVE) {
		sType = "Move";
	} else if (eType == stmi::PointerEvent::BUTTON_PRESS) {
		sType = "Press";
	} else if (eType == stmi::PointerEvent::BUTTON_RELEASE) {
		sType = "Release";
	} else if (eType == stmi::PointerEvent::BUTTON_RELEASE_CANCEL) {
		sType = "Cancel";
	} else {
		sType = "Error";
	}
	const auto sShow = Glib::ustring::compose("%1 Button:%2 :%3  %4,%5", sEventStr
											, nButton, sType, refEvent->getX(), refEvent->getY());
	printString(sShow);
}
void ShowEvsWindow::printPointerScrollEvent(const std::string& sPre, const shared_ptr<stmi::PointerScrollEvent>& refEvent)
{
	const auto sEventStr = getEventString(sPre, refEvent);
	stmi::PointerScrollEvent::POINTER_SCROLL_DIR eDir = refEvent->getScrollDir();
	std::string sDir;
	if (eDir == stmi::PointerScrollEvent::SCROLL_DOWN) {
		sDir = "Down";
	} else if (eDir == stmi::PointerScrollEvent::SCROLL_UP) {
		sDir = "Up";
	} else if (eDir == stmi::PointerScrollEvent::SCROLL_LEFT) {
		sDir = "Left";
	} else if (eDir == stmi::PointerScrollEvent::SCROLL_RIGHT) {
		sDir = "Right";
	} else {
		sDir = "Error";
	}
	const auto sShow = Glib::ustring::compose("%1 Dir:%2  %3,%4", sEventStr
											, sDir, refEvent->getX(), refEvent->getY());
	printString(sShow);
}
void ShowEvsWindow::printTouchEvent(const std::string& sPre, const shared_ptr<stmi::TouchEvent>& refEvent)
{
	const auto sEventStr = getEventString(sPre, refEvent);
	const auto nFingerId = refEvent->getFingerId();
	const stmi::TouchEvent::TOUCH_INPUT_TYPE eType = refEvent->getType();
	std::string sType;
	if (eType == stmi::TouchEvent::TOUCH_BEGIN) {
		sType = "Begin";
	} else if (eType == stmi::TouchEvent::TOUCH_END) {
		sType = "End";
	} else if (eType == stmi::TouchEvent::TOUCH_UPDATE) {
		sType = "Update";
	} else if (eType == stmi::TouchEvent::TOUCH_CANCEL) {
		sType = "Cancel";
	} else {
		sType = "Error";
	}
	const auto sShow = Glib::ustring::compose("%1 Finger:%2 :%3  %4,%5", sEventStr
											, nFingerId, sType, refEvent->getX(), refEvent->getY());
	printString(sShow);
}
void ShowEvsWindow::printJoystickHatEvent(const std::string& sPre, const shared_ptr<stmi::JoystickHatEvent>& refEvent)
{
	const auto sEventStr = getEventString(sPre, refEvent);
	const auto nHat = refEvent->getHat();
	const stmi::JoystickCapability::HAT_VALUE eValue = refEvent->getValue();
	std::string sHat;
	const int32_t nValue = (int32_t)eValue;
	if ((nValue & stmi::JoystickCapability::HAT_LEFT) != 0) {
		sHat = "LEFT";
	} else if ((nValue & stmi::JoystickCapability::HAT_RIGHT) != 0) {
		sHat = "RIGHT";
	}
	if ((nValue & stmi::JoystickCapability::HAT_UP) != 0) {
		sHat += "UP";
	} else if ((nValue & stmi::JoystickCapability::HAT_DOWN) != 0) {
		sHat += "DOWN";
	}
	if (sHat.empty()) {
		if (eValue == stmi::JoystickCapability::HAT_CENTER) {
			sHat = "CENTER";
		} else if (eValue == stmi::JoystickCapability::HAT_CENTER_CANCEL) {
			sHat = "CANCEL";
		} else {
			sHat = "???";
		}
	}
	const auto sShow = Glib::ustring::compose("%1 Hat:%2 =%3", sEventStr
											, nHat, sHat);
	printString(sShow);
}
void ShowEvsWindow::printJoystickButtonEvent(const std::string& sPre, const shared_ptr<stmi::JoystickButtonEvent>& refEvent)
{
	const auto sEventStr = getEventString(sPre, refEvent);
	const stmi::JoystickCapability::BUTTON eButton = refEvent->getButton();
	std::string sButton = m_oInputStrings.getKeyString((stmi::HARDWARE_KEY)eButton);
	if (sButton.empty()) {
		sButton = "???";
	}
	const stmi::JoystickButtonEvent::BUTTON_INPUT_TYPE eType = refEvent->getType();
	std::string sType;
	if (eType == stmi::JoystickButtonEvent::BUTTON_PRESS) {
		sType = "Press  ";
	} else if (eType == stmi::JoystickButtonEvent::BUTTON_RELEASE) {
		sType = "Release";
	} else if (eType == stmi::JoystickButtonEvent::BUTTON_RELEASE_CANCEL) {
		sType = "Cancel ";
	} else {
		sType = "Error  ";
	}
	const auto sShow = Glib::ustring::compose("%1 Button:%2 :%3", sEventStr
											, sButton, sType);
	printString(sShow);
}
void ShowEvsWindow::printJoystickAxisEvent(const std::string& sPre, const shared_ptr<stmi::JoystickAxisEvent>& refEvent)
{
	const auto sEventStr = getEventString(sPre, refEvent);
	const stmi::JoystickCapability::AXIS eAxis = refEvent->getAxis();
	std::string sAxis = m_oInputStrings.getAxisString(eAxis);
	if (sAxis.empty()) {
		sAxis = "???";
	}
	const int32_t nValue = refEvent->getValue();
	const auto sShow = Glib::ustring::compose("%1 Axis:%2 =%3", sEventStr
											, sAxis, nValue);
	printString(sShow);
}
void ShowEvsWindow::printDeviceMgmtEvent(const std::string& sPre, const shared_ptr<stmi::DeviceMgmtEvent>& refEvent)
{
	const auto sEventStr = getEventString(sPre, refEvent);
	stmi::DeviceMgmtEvent::DEVICE_MGMT_TYPE eType = refEvent->getDeviceMgmtType();
	std::string sType;
	if (eType == stmi::DeviceMgmtEvent::DEVICE_MGMT_ADDED) {
		sType = "Added  ";
	} else if (eType == stmi::DeviceMgmtEvent::DEVICE_MGMT_CHANGED) {
		sType = "Changed";
	} else if (eType == stmi::DeviceMgmtEvent::DEVICE_MGMT_REMOVED) {
		sType = "Removed";
	} else {
		sType = "Error  ";
	}
	const auto sDevice = refEvent->getDevice()->getName();
	const int32_t nDevice = refEvent->getDevice()->getId();
	const auto sShow = Glib::ustring::compose("%1 Device:%2 (%3) %4", sEventStr
											, sDevice, nDevice, sType);
	printString(sShow);
}
void ShowEvsWindow::printString(const Glib::ustring& sStr)
{
	if (m_nEventsTextBufferTotLines >= s_nEventsTextBufferMaxLines) {
		auto itLine1 = m_refEventsTextBuffer->get_iter_at_line(1);
		m_refEventsTextBuffer->erase(m_refEventsTextBuffer->begin(), itLine1);
		--m_nEventsTextBufferTotLines;
	}
	m_refEventsTextBuffer->insert(m_refEventsTextBuffer->end(), sStr + "\n");
	++m_nEventsTextBufferTotLines;

	m_refEventsTextBuffer->place_cursor(m_refEventsTextBuffer->end());
	m_refEventsTextBuffer->move_mark(m_refEventsTextBufferBottomMark, m_refEventsTextBuffer->end());
	m_oEventsTextView.scroll_to(m_refEventsTextBufferBottomMark, 0.1);
}
void ShowEvsWindow::recreateDeviceList()
{
	m_refTreeModelDevices->clear();

	std::vector<int32_t> aDevId = m_refDM->getDevices();
	std::sort(aDevId.begin(), aDevId.end());
	for (int32_t nDevId : aDevId) {
//std::cout << "recreateDeviceList nDevId=" << nDevId << std::endl;
		auto refDevice = m_refDM->getDevice(nDevId);
		auto sDevName = refDevice->getName();
		std::vector<stmi::Capability::Class> aCapaClass = refDevice->getCapabilityClasses();
		Glib::ustring sCapas;
		for (auto oCapaClass : aCapaClass) {
			if (!sCapas.empty()) {
				sCapas = sCapas + ", ";
			}
			auto sCapaClassId = oCapaClass.getId();
			auto nLastPos = sCapaClassId.rfind("::");
			if (nLastPos != std::string::npos) {
				sCapaClassId.erase(0, nLastPos + 2);
			}
			sCapas = sCapas + sCapaClassId;
		}
//std::cout << "recreateDeviceList sCapas=" << sCapas << std::endl;
		Gtk::ListStore::iterator itListDevice = m_refTreeModelDevices->append();
		(*itListDevice)[m_oDeviceColumns.m_oColName] = sDevName;
		(*itListDevice)[m_oDeviceColumns.m_oColDeviceId] = Glib::ustring::compose("%1", nDevId);
		(*itListDevice)[m_oDeviceColumns.m_oColCapabilities] = sCapas;
	}
}
void ShowEvsWindow::onSelectAllEventsClicked()
{
	onSelectEventsClicked(m_oAllEventsCheck, m_bListenerAllEventsActive
						, m_refListenerAllEvents, m_refCallIfAll);
}
void ShowEvsWindow::onSelectKeyEventsClicked()
{
	onSelectEventsClicked(m_oKeyEventsCheck, m_bListenerKeyEventsActive
						, m_refListenerKeyEvents, m_refCallIfKey);
}
void ShowEvsWindow::onSelectPointerEventsClicked()
{
	onSelectEventsClicked(m_oPointerEventsCheck, m_bListenerPointerEventsActive
						, m_refListenerPointerEvents, m_refCallIfPointer);
}
void ShowEvsWindow::onSelectTouchEventsClicked()
{
	onSelectEventsClicked(m_oTouchEventsCheck, m_bListenerTouchEventsActive
						, m_refListenerTouchEvents, m_refCallIfTouch);
}
void ShowEvsWindow::onSelectJoystickEventsClicked()
{
	onSelectEventsClicked(m_oJoystickEventsCheck, m_bListenerJoystickEventsActive
						, m_refListenerJoystickEvents, m_refCallIfJoystick);
}
void ShowEvsWindow::onSelectDeviceMgmtEventsClicked()
{
	onSelectEventsClicked(m_oDeviceMgmtEventsCheck, m_bListenerDeviceMgmtEventsActive
						, m_refListenerDeviceMgmtEvents, m_refCallIfDeviceMgmt);
}
void ShowEvsWindow::onSelectEventsClicked(Gtk::CheckButton& oEventsCheck, bool& bListenerEventsActive
										, shared_ptr<stmi::EventListener>& refListener
										, shared_ptr<stmi::CallIf>& refCallIf)
{
	const bool bIsActive = oEventsCheck.get_active();
	if (bIsActive == bListenerEventsActive) {
		return;
	}
	if (bIsActive) {
		m_refDM->addEventListener(refListener, refCallIf);
	} else {
		m_refDM->removeEventListener(refListener, true);
	}
	bListenerEventsActive = bIsActive;
}
void ShowEvsWindow::onWindowAButtonClicked()
{
	if (!m_refWindowA) {
		m_refWindowA = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
		m_refWindowA->set_title("WindowA (Free)");
		m_refWindowA->set_default_size(s_nInitialWindowSizeW / 2, s_nInitialWindowSizeH / 2);
		m_refWindowA->set_resizable(true);
		m_refWindowA->signal_hide().connect(sigc::mem_fun(this, &ShowEvsWindow::onWindowAHide));
		//
		m_refWinAAccessor = std::make_shared<stmi::GtkAccessor>(m_refWindowA);
		m_refDM->addAccessor(m_refWinAAccessor);
		m_refWindowA->present();
		return;
	}
	const bool bIsVisible = m_refWindowA->get_realized() && m_refWindowA->get_visible();
	if (bIsVisible) {
		m_refWindowA->hide();
	} else {
		m_refDM->addAccessor(m_refWinAAccessor);
		m_refWindowA->present();
	}
}
void ShowEvsWindow::onWindowAHide()
{
	m_refDM->removeAccessor(m_refWinAAccessor);
}
void ShowEvsWindow::onWindowBButtonClicked()
{
	if (!m_refWindowB) {
		m_refWindowB = Glib::RefPtr<Gtk::Window>(new Gtk::Window());
		m_refWindowB->set_title("WindowB (Parent)");
		m_refWindowB->set_default_size(s_nInitialWindowSizeW / 1.4, s_nInitialWindowSizeH / 1.8);

		m_refWindowB->signal_hide().connect(sigc::mem_fun(this, &ShowEvsWindow::onWindowAHide));
		m_refWindowB->set_transient_for(*this);
		m_refWindowB->set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
		//
		m_refWinBAccessor = std::make_shared<stmi::GtkAccessor>(m_refWindowB);
		m_refDM->addAccessor(m_refWinBAccessor);
		m_refWindowB->present();
		return;
	}
	const bool bIsVisible = m_refWindowB->get_realized() && m_refWindowB->get_visible();
	if (bIsVisible) {
		m_refWindowB->hide();
	} else {
		m_refDM->addAccessor(m_refWinBAccessor);
		m_refWindowB->present();
	}
}
void ShowEvsWindow::onWindowBHide()
{
	m_refDM->removeAccessor(m_refWinBAccessor);
}
void ShowEvsWindow::onDialogDButtonClicked()
{
	if (!m_refDialogD) {
		m_refDialogD = Glib::RefPtr<AllKeysDialog>(new AllKeysDialog());
		m_refDialogD->set_title("DialogD (Modal)");
		m_refDialogD->set_default_size(s_nInitialWindowSizeW / 1.5, s_nInitialWindowSizeH / 1.7);
		m_refDialogD->set_transient_for(*this);
		//
		m_refDiaDAccessor = std::make_shared<stmi::GtkAccessor>(m_refDialogD);
		m_refDM->addAccessor(m_refDiaDAccessor);
	}
	m_refDialogD->run();
	m_refDialogD->hide();
	m_refDM->removeAccessor(m_refDiaDAccessor);
}

} // namespace showevs

} // namespace example
