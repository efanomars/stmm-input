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
 * File:   flogtkdevicemanager.cc
 */

#include "flogtkdevicemanager.h"

#include "floatingsources.h"
#include "flogtkwindowdata.h"
#include "flogtklistenerextradata.h"
#include "flogtkxkeyboarddevice.h"

#include <gdkmm/device.h>
#include <gdkmm/devicemanager.h>

#include <glibmm/signalproxy.h>
#include <glibmm/ustring.h>

#include <gdk/gdk.h>
#include <gdk/gdkx.h>

namespace stmi
{

namespace Private
{
namespace Flo
{
void gdkDeviceManagerCallbackAdded(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data)
{
	auto p0FloGtkDeviceManager = static_cast<FloGtkDeviceManager*>(p0Data);
	p0FloGtkDeviceManager->gdkDeviceAdded(p0DeviceManager, p0Device);
}
void gdkDeviceManagerCallbackChanged(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data)
{
	auto p0FloGtkDeviceManager = static_cast<FloGtkDeviceManager*>(p0Data);
	p0FloGtkDeviceManager->gdkDeviceChanged(p0DeviceManager, p0Device);
}
void gdkDeviceManagerCallbackRemoved(GdkDeviceManager *p0DeviceManager, GdkDevice* p0Device, gpointer p0Data)
{
	auto p0FloGtkDeviceManager = static_cast<FloGtkDeviceManager*>(p0Data);
	p0FloGtkDeviceManager->gdkDeviceRemoved(p0DeviceManager, p0Device);
}
} // namespace Flo
} // namespace Private

////////////////////////////////////////////////////////////////////////////////
using Private::Flo::GtkWindowData;
using Private::Flo::GtkXKeyboardDevice;
using Private::Flo::XIEventSource;
using Private::Flo::FloGtkListenerExtraData;

shared_ptr<FloGtkDeviceManager> FloGtkDeviceManager::create(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
															, KEY_REPEAT_MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter
															, const Glib::RefPtr<Gdk::Display>& refDisplay)
{
	shared_ptr<FloGtkDeviceManager> refInstance(new FloGtkDeviceManager(bEnableEventClasses, aEnDisableEventClass
																		, eKeyRepeatMode, refGdkConverter));
	refInstance->init(refDisplay); // init might throw
	return refInstance;
}

FloGtkDeviceManager::FloGtkDeviceManager(bool bEnableEventClasses, const std::vector<Event::Class>& aEnDisableEventClass
										, KEY_REPEAT_MODE eKeyRepeatMode, const shared_ptr<GdkKeyConverter>& refGdkConverter)
: StdDeviceManager({typeid(DeviceMgmtCapability), typeid(KeyCapability)}
					, {typeid(DeviceMgmtEvent), typeid(KeyEvent)}
					, bEnableEventClasses, aEnDisableEventClass)
, m_nCancelingNestedDepth(0)
, m_p0XDisplay(nullptr)
, m_nConnectHandlerDeviceAdded(0)
, m_nConnectHandlerDeviceChanged(0)
, m_nConnectHandlerDeviceRemoved(0)
, m_eKeyRepeatMode(eKeyRepeatMode)
, m_refGdkConverter(refGdkConverter)
, m_oConverter(*m_refGdkConverter)
, m_nClassIdxKeyEvent(getEventClassIndex(typeid(KeyEvent)))
, m_nClassIdxDeviceMgmtEvent(getEventClassIndex(typeid(DeviceMgmtEvent)))
{
	assert(refGdkConverter);
	assert((eKeyRepeatMode >= KEY_REPEAT_MODE_SUPPRESS) && (eKeyRepeatMode <= KEY_REPEAT_MODE_ADD_RELEASE_CANCEL));
	// The whole implementation of this class is based on this assumption
	static_assert(sizeof(int) <= sizeof(int32_t), "");
}
FloGtkDeviceManager::~FloGtkDeviceManager()
{
	deinitDeviceManager();
//std::cout << "FloGtkDeviceManager::~FloGtkDeviceManager()" << std::endl;
}
shared_ptr<DeviceManager> FloGtkDeviceManager::getDeviceManager() const
{
	return getRoot();
}
shared_ptr<DeviceManager> FloGtkDeviceManager::getDeviceManager()
{
	return getRoot();
}
void FloGtkDeviceManager::initDeviceManager()
{
	assert(m_refGdkDisplay);
	assert(m_nConnectHandlerDeviceAdded == 0);
	auto refDeviceManager = m_refGdkDisplay->get_device_manager();
	GdkDeviceManager* p0GdkDeviceManager = refDeviceManager->gobj();
	gpointer p0Data = this;
	m_nConnectHandlerDeviceAdded = g_signal_connect(p0GdkDeviceManager, "device-added", G_CALLBACK(Private::Flo::gdkDeviceManagerCallbackAdded), p0Data);
	assert(m_nConnectHandlerDeviceAdded > 0);
	m_nConnectHandlerDeviceChanged = g_signal_connect(p0GdkDeviceManager, "device-changed", G_CALLBACK(Private::Flo::gdkDeviceManagerCallbackChanged), p0Data);
	assert(m_nConnectHandlerDeviceChanged > 0);
	m_nConnectHandlerDeviceRemoved = g_signal_connect(p0GdkDeviceManager, "device-removed", G_CALLBACK(Private::Flo::gdkDeviceManagerCallbackRemoved), p0Data);
	assert(m_nConnectHandlerDeviceRemoved > 0);
}
void FloGtkDeviceManager::deinitDeviceManager()
{
	if (m_refGdkDisplay && (m_nConnectHandlerDeviceAdded > 0)) {
		auto refDeviceManager = m_refGdkDisplay->get_device_manager();
		GdkDeviceManager* p0GdkDeviceManager = refDeviceManager->gobj();
		if (g_signal_handler_is_connected(p0GdkDeviceManager, m_nConnectHandlerDeviceAdded)) {
			g_signal_handler_disconnect(p0GdkDeviceManager, m_nConnectHandlerDeviceAdded);
		}
		if (g_signal_handler_is_connected(p0GdkDeviceManager, m_nConnectHandlerDeviceChanged)) {
			g_signal_handler_disconnect(p0GdkDeviceManager, m_nConnectHandlerDeviceChanged);
		}
		if (g_signal_handler_is_connected(p0GdkDeviceManager, m_nConnectHandlerDeviceRemoved)) {
			g_signal_handler_disconnect(p0GdkDeviceManager, m_nConnectHandlerDeviceRemoved);
		}
	}
}
void FloGtkDeviceManager::enableEventClass(const Event::Class& oEventClass)
{
	StdDeviceManager::enableEventClass(oEventClass);
	adjustConnectionsAfterEnablingClass();
}
void FloGtkDeviceManager::adjustConnectionsAfterEnablingClass()
{
//std::cout << "FloGtkDeviceManager::adjustConnectionsAfterEnablingClass(" << (int64_t)this << ")" << std::endl;
	if (!getEventClassEnabled(typeid(KeyEvent))) {
		return;
	}
	for (auto& oPair : m_aXWindowData) {
		auto& refWindowData = oPair.second;
		if (! refWindowData->isEnabled()) {
			continue; // for --------
		}
		auto& refAccessor = refWindowData->getAccessor();
		if (refAccessor->isDeleted()) {
			continue; // for --------
		}
		refWindowData->connectAllDevices();
	}
	if (m_refSelected) {
		selectAccessor(m_refSelected);
	}
}
void FloGtkDeviceManager::init(const Glib::RefPtr<Gdk::Display>& refDisplay)
{
	assert(!m_refGdkDisplay);
	if (!refDisplay) {
		m_refGdkDisplay = Gdk::Display::get_default();
	} else {
		m_refGdkDisplay = refDisplay;
	}
	GdkDisplay* p0GdkDisplay = m_refGdkDisplay->gobj();
	m_p0XDisplay = gdk_x11_display_get_xdisplay(p0GdkDisplay);
	assert(m_p0XDisplay != nullptr);

	// init xi event source
	std::string sError;
	const bool bXIOk = initXI(sError);
	if (!bXIOk) {
		assert(!sError.empty());
		throw std::runtime_error(sError); //------------------------------------
	}
	// init device existence callbacks
	initDeviceManager();
	// add the floating devices
	addDevices();
}
void FloGtkDeviceManager::addDevices()
{
	assert(m_refGdkDisplay);
	assert(m_oKeyboardDevices.empty());

	auto refGdkDeviceManager = m_refGdkDisplay->get_device_manager();
	const std::vector< Glib::RefPtr< Gdk::Device > > aDevice = refGdkDeviceManager->list_devices(Gdk::DEVICE_TYPE_FLOATING);
	for (auto& refFloatingDevice : aDevice) {
		const Glib::RefPtr<Gdk::Device>& refKeyboard = refFloatingDevice;
		GdkDevice* p0GdkDevice = refKeyboard->gobj();
		addFloatingDevice(p0GdkDevice);
	}
}
shared_ptr<GtkXKeyboardDevice> FloGtkDeviceManager::addFloatingDevice(GdkDevice* p0GdkDevice)
{
	shared_ptr<GtkXKeyboardDevice> refGtkXKeyboard;
	const GdkInputSource eSource = gdk_device_get_source(p0GdkDevice);
	if (eSource == GDK_SOURCE_KEYBOARD) {
		// add the floating keyboard
		const int nXDeviceId = gdk_x11_device_get_id(p0GdkDevice);
		const Glib::ustring sDeviceName(gdk_device_get_name(p0GdkDevice));
		refGtkXKeyboard = createGtkXKeyboardDevice(nXDeviceId, sDeviceName);
	} else {
		//TODO also support relative motion of pointer device!! (define an event)
		int32_t nTotKeys = gdk_device_get_n_keys(p0GdkDevice);
		if (nTotKeys > 0) {
			// This pointer has not only buttons but also keys
			// ex. Logitech Touch Keyboard K400r only appears as Pointer even though it has a keyboard
			const int nXDeviceId = gdk_x11_device_get_id(p0GdkDevice);
			const Glib::ustring sDeviceName(gdk_device_get_name(p0GdkDevice));
			refGtkXKeyboard = createGtkXKeyboardDevice(nXDeviceId, sDeviceName);
		}
	}
	return refGtkXKeyboard;
}
shared_ptr<GtkXKeyboardDevice> FloGtkDeviceManager::createGtkXKeyboardDevice(int nXDeviceId, const Glib::ustring& sDeviceName)
{
	shared_ptr<ChildDeviceManager> refChildThis = shared_from_this();
	assert(std::dynamic_pointer_cast<FloGtkDeviceManager>(refChildThis));
	auto refThis = std::static_pointer_cast<FloGtkDeviceManager>(refChildThis);
	auto refGtkXKeyboard = std::make_shared<GtkXKeyboardDevice>(sDeviceName, refThis);
	auto oPair = m_oKeyboardDevices.insert(std::make_pair(nXDeviceId, refGtkXKeyboard));
	const bool bInserted = oPair.second;
	if (bInserted) {
		const bool bAdded = StdDeviceManager::addDevice(refGtkXKeyboard);
		if (!bAdded) {
			assert(false); // internal error, duplicate id!
			refGtkXKeyboard.reset();
		} else {
			// successfully added
			// for each accessor (window) select xi events
			connectDeviceToAllWindows(nXDeviceId);
		}
	} else {
		assert(false); // duplicate x11 device id
		refGtkXKeyboard.reset();
	}
	return refGtkXKeyboard;
}
bool FloGtkDeviceManager::initXI(std::string& sError)
{
	int nXIOpcode, nEvent, nError;
	if (!XQueryExtension( m_p0XDisplay, "XInputExtension", &nXIOpcode, &nEvent, &nError)) {
		sError = "FloGtkDeviceManager: X Input extension not available.";
		return false;
	}
	// We need XI 2.2
	int nMajor = 2;
	int nMinor = 2;

	int nRes = XIQueryVersion( m_p0XDisplay, &nMajor, &nMinor );
	if ( nRes == BadRequest ) {
		sError = "FloGtkDeviceManager: No XI2 support. Server supports version " + std::to_string(nMajor) + "." + std::to_string(nMinor) + " only.";
		return false;
	} else if ( nRes != Success ) {
		sError = "FloGtkDeviceManager: internal XI Error!";
		return false;
	} else {
		//std::cout << "FloGtkDeviceManager: XI2 supported. Server provides version " << nMajor << "." << nMinor << std::endl;
	}

	XFlush( m_p0XDisplay );

	assert(!m_refXIEventSource);
	m_refXIEventSource = Glib::RefPtr<XIEventSource>(new XIEventSource(m_refGdkDisplay.operator->(), nXIOpcode));
	m_refXIEventSource->connect(sigc::mem_fun(this, &FloGtkDeviceManager::doXIDeviceEventCallback));
	m_refXIEventSource->attach();
	return true;
}
void FloGtkDeviceManager::finalizeListener(ListenerData& oListenerData)
{
	++m_nCancelingNestedDepth;
	const int64_t nEventTimeUsec = DeviceManager::getNowTimeMicroseconds();
	for (auto& oPair : m_oKeyboardDevices) {
		shared_ptr<GtkXKeyboardDevice>& refGtkXKeyboard = oPair.second;
		refGtkXKeyboard->finalizeListener(oListenerData, nEventTimeUsec);
	}
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
bool FloGtkDeviceManager::findWindow(Gtk::Window* p0GtkmmWindow
				, std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itFind)
{
	for (itFind = m_aGtkWindowData.begin(); itFind != m_aGtkWindowData.end(); ++itFind) {
		if (itFind->first == p0GtkmmWindow) {
			return true;
		}
	}
	return false;
}
bool FloGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor, bool& bValid
				, std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itFind)
{
	bValid = false;
	if (!refAccessor) {
		return false; //--------------------------------------------------------
	}
	shared_ptr<GtkAccessor> refGtkAccessor = std::dynamic_pointer_cast<GtkAccessor>(refAccessor);
	if (!refGtkAccessor) {
		return false; //--------------------------------------------------------
	}
	bValid = true;
	Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
	const bool bFoundWindow = findWindow(p0GtkmmWindow, itFind);
	return bFoundWindow;
}
bool FloGtkDeviceManager::hasAccessor(const shared_ptr<Accessor>& refAccessor)
{
	bool bValid;
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	return hasAccessor(refAccessor, bValid, itFind);
}
bool FloGtkDeviceManager::addAccessor(const shared_ptr<Accessor>& refAccessor)
{
//std::cout << "FloGtkDeviceManager::addAccessor(" << (int64_t)this << ")" << std::endl;
	bool bValid;
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	const bool bHasAccessor = hasAccessor(refAccessor, bValid, itFind);
	if (!bValid) {
		return false; //--------------------------------------------------------
	}
	shared_ptr<GtkAccessor> refGtkAccessor = std::static_pointer_cast<GtkAccessor>(refAccessor);
	if (refGtkAccessor->isDeleted()) {
		// the window was already destroyed, unusable
		// but if a zombie GtkAccessorData is still around, remove it
		if (bHasAccessor) {
			removeAccessor(itFind);
		}
		return false; //--------------------------------------------------------
	}
	if (bHasAccessor) {
		// Accessor is already present
		return false; //--------------------------------------------------------
	}
	Gtk::Window* p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
	assert(p0GtkmmWindow != nullptr);
	Glib::RefPtr<Gdk::Display> refDisplay = p0GtkmmWindow->get_display();
	assert(refDisplay);
	if (!(m_refGdkDisplay == refDisplay)) {
		// must have same display
		return false; //--------------------------------------------------------
	}

	m_aGtkWindowData.emplace_back(p0GtkmmWindow, getGtkWindowData());
	shared_ptr<GtkWindowData> refData = m_aGtkWindowData.back().second;
	GtkWindowData& oData = *refData;

	::Window nXWinId = None;
	const bool bIsRealized = p0GtkmmWindow->get_realized();
	if (bIsRealized) {
		Glib::RefPtr<Gdk::Window> refWindow = p0GtkmmWindow->get_window();
		GdkWindow* p0GdkWindow = refWindow->gobj();
		nXWinId = gdk_x11_window_get_xid(p0GdkWindow);
		m_aXWindowData.emplace_back(nXWinId, refData);
	}
	oData.enable(refGtkAccessor, nXWinId, this);

	const bool bIsActive = bIsRealized && p0GtkmmWindow->get_visible() && p0GtkmmWindow->is_active();
	if (!m_refSelected) {
		if (bIsActive) {
			if (getEventClassEnabled(typeid(KeyEvent))) {
				refData->connectAllDevices();
			}
			selectAccessor(refData);
		}
	} else {
		if (bIsActive) {
			auto refSelectedAccessor = m_refSelected->getAccessor();
			if (refSelectedAccessor->isDeleted()) {
				const auto p0SelectedGtkmmWindow = refSelectedAccessor->getGtkmmWindow();
				std::vector< std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itSelectedFind;
				#ifndef NDEBUG
				const bool bFoundWindow = 
				#endif //NDEBUG
				findWindow(p0SelectedGtkmmWindow, itSelectedFind);
				assert(bFoundWindow);
				removeAccessor(itSelectedFind);
			} else {
				// This shouldn't happen: the added window is active while another is still selected.
				deselectAccessor();
			}
			if (getEventClassEnabled(typeid(KeyEvent))) {
				refData->connectAllDevices();
			}
			selectAccessor(refData);
		} else {
			deselectAccessor();
		}
	}
//std::cout << "FloGtkDeviceManager::addAccessor()   m_aGtkWindowData.size()=" << m_aGtkWindowData.size() << std::endl;
	return true;
}
bool FloGtkDeviceManager::removeAccessor(const shared_ptr<Accessor>& refAccessor)
{
	std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator itFind;
	bool bValid;
	const bool bHasAccessor = hasAccessor(refAccessor, bValid, itFind);
	if (!bHasAccessor) {
		return false; //--------------------------------------------------------
	}
	removeAccessor(itFind);
	return true;
}
void FloGtkDeviceManager::removeAccessor(const std::vector<std::pair<Gtk::Window*, shared_ptr<GtkWindowData> > >::iterator& itGtkData)
{
//std::cout << "FloGtkDeviceManager::removeAccessor()   m_aGtkWindowData.size()=" << m_aGtkWindowData.size() << std::endl;
	shared_ptr<GtkWindowData>& refData = itGtkData->second;
	const ::Window nXWinId = refData->getXWindow();

	const bool bIsSelected = (m_refSelected == refData);

	refData->disable(); // doesn't clear accessor!

	// move from m_oWindows to m_aFreePool
	m_aFreePool.emplace_back();
	m_aFreePool.back().swap(refData);
	assert(!itGtkData->second);
	assert(m_aFreePool.back());
	//
	m_aGtkWindowData.erase(itGtkData);
	if (nXWinId != None) {
		auto itFindX = std::find_if(m_aXWindowData.begin(), m_aXWindowData.end(),
				[nXWinId](const std::pair<::Window, shared_ptr<GtkWindowData> >& oPair)
				{
					return oPair.first == nXWinId;
				}
				);
		if (itFindX != m_aXWindowData.end()) {
			m_aXWindowData.erase(itFindX);
		}
	}

	if (bIsSelected) {
		// refData reference avoids recycling during cancel callbacks!
		deselectAccessor();
	}
}
void FloGtkDeviceManager::selectAccessor(const shared_ptr<GtkWindowData>& refData)
{
//std::cout << "FloGtkDeviceManager::selectAccessor  accessor=" << (int64_t)&(*(refData->getAccessor())) << "  XWinId=" << refData->getXWindow() << std::endl;
	m_refSelected = refData;
	focusSelectedWindow();
}
void FloGtkDeviceManager::deselectAccessor()
{
//std::cout << "FloGtkDeviceManager::deselectAccessor  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << std::endl;
	++m_nCancelingNestedDepth;
	//
	for (auto& oPair : m_oKeyboardDevices) {
		shared_ptr<GtkXKeyboardDevice>& refKeyboard = oPair.second;
		// cancel all keys that are pressed for the currently selected accessor
		refKeyboard->cancelSelectedAccessorKeys();
	}
	m_refSelected.reset();
	focusSelectedWindow();
	//
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
void FloGtkDeviceManager::cancelDeviceKeys(const shared_ptr<GtkXKeyboardDevice>& refGtkXKeyboard)
{
//std::cout << "FloGtkDeviceManager::cancelDeviceKeys  m_nCancelingNestedDepth=" << m_nCancelingNestedDepth << std::endl;
	++m_nCancelingNestedDepth;
	//
	refGtkXKeyboard->cancelSelectedAccessorKeys();
	//
	--m_nCancelingNestedDepth;
	if (m_nCancelingNestedDepth == 0) {
		resetExtraDataOfAllListeners();
	}
}
void FloGtkDeviceManager::onIsActiveChanged(const shared_ptr<GtkWindowData>& refWindowData)
{
//std::cout << "FloGtkDeviceManager::onIsActiveChanged  accessor=" << (int64_t)&(*(refWindowData->getAccessor()));
	if (!refWindowData->isEnabled()) {
//std::cout << std::endl;
		return;
	}
	auto& refGtkAccessor = refWindowData->getAccessor();
	bool bIsActive = false;
	if (!refGtkAccessor->isDeleted()) {
		auto p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
		bIsActive = p0GtkmmWindow->get_realized() && p0GtkmmWindow->get_visible() && p0GtkmmWindow->is_active();
	}
//std::cout << " active=" << bIsActive << std::endl;
	const bool bIsSelected = (refWindowData == m_refSelected);
	if (bIsActive) {
		if (bIsSelected) {
			// Activating the already active window ... shouldn't happen.
			return; //----------------------------------------------------------
		}
		if (m_refSelected) {
			deselectAccessor();
		}
		if (refWindowData->getXWindow() == None) {
			// When added the window wasn't realized
			auto p0GtkmmWindow = refGtkAccessor->getGtkmmWindow();
			Glib::RefPtr<Gdk::Window> refWindow = p0GtkmmWindow->get_window();
			GdkWindow* p0GdkWindow = refWindow->gobj();
			::Window nXWinId = gdk_x11_window_get_xid(p0GdkWindow);
			m_aXWindowData.emplace_back(nXWinId, refWindowData);
			refWindowData->setXWindow(nXWinId);
		}
		if (getEventClassEnabled(typeid(KeyEvent))) {
			refWindowData->connectAllDevices();
		}
		selectAccessor(refWindowData);
	} else {
		if (bIsSelected) {
			// Send cancels for open keys, buttons and sequences
			deselectAccessor();
		}
	}
}
void FloGtkDeviceManager::focusSelectedWindow()
{
	if (getEventClassEnabled(typeid(KeyEvent))) {
		focusDevicesToXWindow(m_refSelected ? m_refSelected->getXWindow() : None);
	}
}
void FloGtkDeviceManager::focusDevicesToXWindow(::Window nXWinId)
{
//std::cout << "FloGtkDeviceManager::focusDevicesToXWindow  nXWinId=" << nXWinId << std::endl;

	for (auto& oPair : m_oKeyboardDevices) {
		const int32_t nDeviceId = oPair.first;
		focusDeviceToXWindow(nDeviceId, nXWinId);
	}
}
void FloGtkDeviceManager::focusDeviceToXWindow(int32_t nXDeviceId, ::Window nXWinId)
{
//std::cout << "FloGtkDeviceManager::focusDeviceToXWindow  nXWinId=" << nXWinId << " nDeviceId=" << nXDeviceId << std::endl;
	::Window nCheckXWinId;
	Status nCheckStatus = XIGetFocus(m_p0XDisplay, nXDeviceId, &nCheckXWinId);
	if ((nCheckStatus == BadValue) || (nCheckStatus == BadWindow) || (nCheckStatus == BadMatch)) {
		std::cout << "FloGtkDeviceManager::focusDeviceToXWindow() XIGetFocus failed for XI device=" << nXDeviceId << std::endl;
	}
	if (nCheckXWinId == nXWinId) {
//std::cout << "          already nDeviceId's focus window" << std::endl;
		return; //--------------------------------------------------------------
	}
	Status nStatus = XISetFocus(m_p0XDisplay, nXDeviceId, nXWinId, CurrentTime);
	if ((nStatus == BadValue) || (nStatus == BadWindow) || (nStatus == BadMatch)) {
		std::cout << "FloGtkDeviceManager::focusDeviceToXWindow() XISetFocus failed for XI device=" << nXDeviceId << "  nXWinId=" << nXWinId << std::endl;
	}
}
void FloGtkDeviceManager::connectDeviceToAllWindows(int32_t nXDeviceId)
{
//std::cout << "FloGtkDeviceManager::connectDeviceToAllWindows()  nXDeviceId=" << nXDeviceId << std::endl;
	//
	for (auto& oPair : m_aXWindowData) {
		auto& refWindowData = oPair.second;
		if (! refWindowData->isEnabled()) {
			continue; // for --------
		}
		auto& refAccessor = refWindowData->getAccessor();
		if (refAccessor->isDeleted()) {
			continue; // for --------
		}
		refWindowData->connectDevice(nXDeviceId);
	}
}
std::shared_ptr<GtkWindowData> FloGtkDeviceManager::getGtkWindowData()
{
	const size_t nSize = m_aFreePool.size();
	size_t nIdx = 0;
	for (auto itWindowData = m_aFreePool.begin(); itWindowData != m_aFreePool.end(); ++itWindowData) {
		shared_ptr<GtkWindowData>& refGtkWindowData = *itWindowData;
		if (refGtkWindowData.unique()) {
			shared_ptr<GtkWindowData> refReuse = refGtkWindowData;
			if (nSize > 1) {
				m_aFreePool[nIdx] = m_aFreePool[nSize - 1];
			}
			m_aFreePool.pop_back();
			return refReuse; //-------------------------------------------------
		}
		++nIdx;
	}
	return std::make_shared<GtkWindowData>();
}

void FloGtkDeviceManager::sendDeviceMgmtToListeners(const DeviceMgmtEvent::DEVICE_MGMT_TYPE& eMgmtType, const shared_ptr<Device>& refDevice)
{
	if (!getEventClassEnabled(typeid(DeviceMgmtEvent))) {
		return;
	}
	shared_ptr<ChildDeviceManager> refChildThis = shared_from_this();
	assert(std::dynamic_pointer_cast<FloGtkDeviceManager>(refChildThis));
	auto refThis = std::static_pointer_cast<FloGtkDeviceManager>(refChildThis);
	auto refCapa = std::static_pointer_cast<DeviceMgmtCapability>(refThis);
	//
	const int64_t nTimeUsec = DeviceManager::getNowTimeMicroseconds();
	//
	shared_ptr<DeviceMgmtEvent> refEvent = std::make_shared<DeviceMgmtEvent>(nTimeUsec, refCapa, eMgmtType, refDevice);
	auto refListeners = getListeners();
	for (auto& p0ListenerData : *refListeners) {
		p0ListenerData->handleEventCallIf(m_nClassIdxDeviceMgmtEvent, refEvent);
	}
}
void FloGtkDeviceManager::gdkDeviceAdded(GdkDeviceManager *
#ifndef NDEBUG
p0DeviceManager
#endif //NDEBUG
, GdkDevice* p0Device)
{
	assert(m_refGdkDisplay);
	#ifndef NDEBUG
	auto refGdkDeviceManager = m_refGdkDisplay->get_device_manager();
	#endif //NDEBUG
	assert(p0DeviceManager == refGdkDeviceManager->gobj());
	GdkDeviceType eDeviceType = gdk_device_get_device_type(p0Device);
	if (eDeviceType != GDK_DEVICE_TYPE_FLOATING) {
		// This manager has only floating devices, doesn't care about masters or slaves
		return; //--------------------------------------------------------------
	}
	shared_ptr<GtkXKeyboardDevice> refGtkXKeyboard = addFloatingDevice(p0Device);
	if (!refGtkXKeyboard) {
		return; //--------------------------------------------------------------
	}
	const int nXDeviceId = gdk_x11_device_get_id(p0Device);
	focusDeviceToXWindow(nXDeviceId, m_refSelected ? m_refSelected->getXWindow() : None);
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_ADDED, refGtkXKeyboard);
}
void FloGtkDeviceManager::gdkDeviceChanged(GdkDeviceManager *
#ifndef NDEBUG
p0DeviceManager
#endif //NDEBUG
, GdkDevice* p0Device)
{
//std::cout << "FloGtkDeviceManager::gdkDeviceChanged " << std::endl;
//std::cout << "FloGtkDeviceManager(" << intptr_t(this) << ")::gdkDeviceChanged  m_refGdkDeviceManager=" << intptr_t(m_refGdkDeviceManager.operator->()) << std::endl;
	assert(m_refGdkDisplay);
	#ifndef NDEBUG
	auto refGdkDeviceManager = m_refGdkDisplay->get_device_manager();
	#endif //NDEBUG
	assert(p0DeviceManager == refGdkDeviceManager->gobj());
	GdkDeviceType eDeviceType = gdk_device_get_device_type(p0Device);
	const int nXDeviceId = gdk_x11_device_get_id(p0Device);
	auto itFind = m_oKeyboardDevices.find(nXDeviceId);
	const bool bManaged = (itFind != m_oKeyboardDevices.end());
	const bool bIsFloating = (eDeviceType == GDK_DEVICE_TYPE_FLOATING);
	if (bManaged) {
		const shared_ptr<GtkXKeyboardDevice>& refGtkXKeyboard = itFind->second;
		if (!bIsFloating) {
			cancelDeviceKeys(refGtkXKeyboard);
			refGtkXKeyboard->removingDevice();

			auto refRemovedGtkXKeyboard = refGtkXKeyboard;
			#ifndef NDEBUG
			const bool bRemoved =
			#endif //NDEBUG
			StdDeviceManager::removeDevice(refGtkXKeyboard);
			assert(bRemoved);
			m_oKeyboardDevices.erase(itFind);
			//
			sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_REMOVED, refRemovedGtkXKeyboard);
		} else {
			sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_CHANGED, refGtkXKeyboard);
		}
	} else {
		if (bIsFloating) {
			// a slave became a floating device, add it
			shared_ptr<GtkXKeyboardDevice> refGtkXKeyboard = addFloatingDevice(p0Device);
			if (!refGtkXKeyboard) {
				// It probably has no keys, ignore
				return; //--------------------------------------------------------------
			}
			focusDeviceToXWindow(nXDeviceId, m_refSelected ? m_refSelected->getXWindow() : None);
			sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_ADDED, refGtkXKeyboard);
		} else {
			// don't care about non floating
			return; //----------------------------------------------------------
		}
	}
}
void FloGtkDeviceManager::gdkDeviceRemoved(GdkDeviceManager *
#ifndef NDEBUG
p0DeviceManager
#endif //NDEBUG
, GdkDevice* p0Device)
{
//std::cout << "FloGtkDeviceManager::gdkDeviceRemoved " << std::endl;
	assert(m_refGdkDisplay);
	#ifndef NDEBUG
	auto refGdkDeviceManager = m_refGdkDisplay->get_device_manager();
	#endif //NDEBUG
	assert(p0DeviceManager == refGdkDeviceManager->gobj());
	GdkDeviceType eDeviceType = gdk_device_get_device_type(p0Device);
	if (eDeviceType != GDK_DEVICE_TYPE_FLOATING) {
		// This manager has only floating devices, doesn't care about masters or slaves
		return; //--------------------------------------------------------------
	}
	const int nXDeviceId = gdk_x11_device_get_id(p0Device);
	auto itFind = m_oKeyboardDevices.find(nXDeviceId);
	if (itFind == m_oKeyboardDevices.end()) {
		// It's probably a floating pointer with no keys
		return; //--------------------------------------------------------------
	}
	const shared_ptr<GtkXKeyboardDevice>& refGtkXKeyboard = itFind->second;
	cancelDeviceKeys(refGtkXKeyboard);
	refGtkXKeyboard->removingDevice();

	auto refRemovedGtkXKeyboard = refGtkXKeyboard;
	#ifndef NDEBUG
	const bool bRemoved =
	#endif //NDEBUG
	StdDeviceManager::removeDevice(refGtkXKeyboard);
	assert(bRemoved);
	m_oKeyboardDevices.erase(itFind);
	//
	sendDeviceMgmtToListeners(DeviceMgmtEvent::DEVICE_MGMT_REMOVED, refRemovedGtkXKeyboard);
}
bool FloGtkDeviceManager::doXIDeviceEventCallback(XIDeviceEvent* p0XIDeviceEvent)
{
//std::cout << "FloGtkDeviceManager::doXIDeviceEventCallback(" << (int64_t)this << ") deviceid=" << p0XIDeviceEvent->deviceid << " nXWinId=" << p0XIDeviceEvent->event << std::endl;
	const bool bContinue = true;
	const int nDeviceId = p0XIDeviceEvent->deviceid;
	auto itFindDevice = m_oKeyboardDevices.find(nDeviceId);
	if (itFindDevice == m_oKeyboardDevices.end()) {
		return bContinue; //----------------------------------------------------
	}
	shared_ptr<GtkXKeyboardDevice>& refGtkXKeyboardDevice = itFindDevice->second;
	//
//std::cout << "FloGtkDeviceManager::doXIDeviceEventCallback m_aXWindowData.size()=" << m_aXWindowData.size() << std::endl;
	::Window nXWinId = p0XIDeviceEvent->event;
	for (auto itFindAccessor = m_aXWindowData.begin(); itFindAccessor != m_aXWindowData.end(); ++itFindAccessor) {
		shared_ptr<GtkWindowData>& refGtkWindowData = itFindAccessor->second;
		const ::Window nFindXWinId = refGtkWindowData->getXWindow();
//std::cout << "FloGtkDeviceManager::doXIDeviceEventCallback nFindXWinId=" << nFindXWinId << std::endl;
		if (nFindXWinId == nXWinId) {
			assert(refGtkWindowData->getAccessor());
			return refGtkXKeyboardDevice->handleXIDeviceEvent(p0XIDeviceEvent, refGtkWindowData); //-----
		}
	}
	return bContinue; //----------------------------------------------------
}

} // namespace stmi
