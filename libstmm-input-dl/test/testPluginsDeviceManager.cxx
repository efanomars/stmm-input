/*
 * Copyright © 2017-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testPluginsDeviceManager.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "pluginsdevicemanager.h"

#include "tst88capability.h"
#include "keyevent.h"
#include "devicemgmtcapability.h"
#include "devicemgmtevent.h"

#include <stmm-input/capability.h>
#include <stmm-input/device.h>

#include <string>
#include <typeinfo>

#include "testconfig.h"

namespace stmi
{

using std::shared_ptr;
using std::weak_ptr;

namespace testing
{

class PluginsFixture
{
public:
	static bool isUniqueSet(const std::vector<int32_t>& aSet)
	{
		// All id must be unique
		const auto nTotDevs = aSet.size();
		for (std::size_t nIdxA = 0; nIdxA < nTotDevs; ++nIdxA) {
			for (auto nIdxB = nIdxA + 1; nIdxB < nTotDevs; ++nIdxB) {
				if (aSet[nIdxA] == aSet[nIdxB]) {
					return false;
				}
			}
		}
		return true;
	}
};

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(PluginsFixture, "CreatePluginsDeviceManager")
{
	PluginsDeviceManager::Init oInit;
	oInit.m_sAdditionalPluginPath = std::string(testing::config::getTestSourceDir()) + "/plugins";
	oInit.m_bAdditionalPluginPathOnly = true;
	auto oPairPluginsDM = PluginsDeviceManager::create(std::move(oInit));
	auto& refPluginsDM = oPairPluginsDM.first;

	REQUIRE(oPairPluginsDM.second.empty());
	REQUIRE(refPluginsDM.operator bool());
	// addAccessor abused to add devices, parameter is ignored
	refPluginsDM->addAccessor(shared_ptr<Accessor>{});
	refPluginsDM->addAccessor(shared_ptr<Accessor>{});
	//refPluginsDM->enableEventClass(KeyEvent::getClass());
	//refPluginsDM->enableEventClass(KeyEvent::getClass());
	auto aDMClasses = refPluginsDM->getCapabilityClasses();
	REQUIRE(aDMClasses.size() == 1);

	auto aDMDeviceClasses = refPluginsDM->getDeviceCapabilityClasses();
//std::cout << "aDMDeviceClasses.size()=" << aDMDeviceClasses.size() << '\n';
	REQUIRE(aDMDeviceClasses.size() == 3);

	auto aDMDevices = refPluginsDM->getDevices();
	REQUIRE(aDMDevices.size() == 4);
	REQUIRE(isUniqueSet(aDMDevices));
	auto aTst88Classes = refPluginsDM->getDevicesWithCapabilityClass(test::Tst88Capability::getClass());
	REQUIRE(aTst88Classes.size() == 2);
	REQUIRE(isUniqueSet(aTst88Classes));
	auto aKeyClasses = refPluginsDM->getDevicesWithCapabilityClass(KeyCapability::getClass());
	REQUIRE(aKeyClasses.size() == 2);
	REQUIRE(isUniqueSet(aKeyClasses));

	auto refDevice1 = refPluginsDM->getDevice(aTst88Classes[0]);
	REQUIRE(refDevice1->getCapabilities().size() == 2);
	shared_ptr<test::Tst88Capability> refTestCapa;
	const bool bHasTestCapa = refDevice1->getCapability(refTestCapa);
	REQUIRE(bHasTestCapa);
	REQUIRE(refTestCapa->getData() >= 70);
	shared_ptr<KeyCapability> refKeyCapa;
	const bool bHasKeyCapa = refDevice1->getCapability(refKeyCapa);
	REQUIRE(bHasKeyCapa);

	int32_t nUnknownCapaDeviceId = -1;
	for (int32_t nDevId : aDMDevices) {
		const auto itFound = std::find(aTst88Classes.begin(), aTst88Classes.end(), nDevId);
		if (itFound == aTst88Classes.end()) {
			nUnknownCapaDeviceId = nDevId;
			break;
		}
	}
	REQUIRE(nUnknownCapaDeviceId >= 0);
	auto refDeviceU = refPluginsDM->getDevice(nUnknownCapaDeviceId);
	auto aUCapaClasses = refDeviceU->getCapabilityClasses();
	REQUIRE(aUCapaClasses.size() == 1);

	const Capability::Class& oCapaClass = aUCapaClasses[0];
	REQUIRE(oCapaClass.getId() == "stmi::test::Tst99");
}

////////////////////////////////////////////////////////////////////////////////
//TEST_CASE_METHOD(PluginsFixture, "CreatePluginsDeviceManagerEnable")
TEST_CASE_METHOD(PluginsFixture, "CreatePluginsDeviceManagerEnable")
{
	PluginsDeviceManager::Init oInit;
	oInit.m_sAdditionalPluginPath = std::string(testing::config::getTestSourceDir()) + "/plugins";
	oInit.m_bAdditionalPluginPathOnly = true;
	oInit.m_bEnablePlugins = true;
	oInit.m_aEnDisablePlugins.push_back("tst88xdevicemanager");
	auto oPairPluginsDM = PluginsDeviceManager::create(std::move(oInit));
	auto& refPluginsDM = oPairPluginsDM.first;

	REQUIRE(oPairPluginsDM.second.empty());
	REQUIRE(refPluginsDM.operator bool());
	// addAccessor abused to add devices, parameter is ignored
	refPluginsDM->addAccessor(shared_ptr<Accessor>{});
	refPluginsDM->addAccessor(shared_ptr<Accessor>{});

	auto aDMDeviceClasses = refPluginsDM->getDeviceCapabilityClasses();
	REQUIRE(aDMDeviceClasses.size() == 2);

	auto aDMDevices = refPluginsDM->getDevices();
	REQUIRE(aDMDevices.size() == 2);
	REQUIRE(isUniqueSet(aDMDevices));
	auto aTst88Classes = refPluginsDM->getDevicesWithCapabilityClass(test::Tst88Capability::getClass());
	REQUIRE(aTst88Classes.size() == 2);
	REQUIRE(isUniqueSet(aTst88Classes));
	auto aKeyClasses = refPluginsDM->getDevicesWithCapabilityClass(KeyCapability::getClass());
	REQUIRE(aKeyClasses.size() == 2);
	REQUIRE(isUniqueSet(aKeyClasses));
}

////////////////////////////////////////////////////////////////////////////////
TEST_CASE_METHOD(PluginsFixture, "SendUnknownEventToLister")
{
	PluginsDeviceManager::Init oInit;
	oInit.m_sAdditionalPluginPath = std::string(testing::config::getTestSourceDir()) + "/plugins";
	oInit.m_bAdditionalPluginPathOnly = true;
	oInit.m_bEnablePlugins = false;
	oInit.m_aEnDisablePlugins.push_back("tst88xdevicemanager");
	auto oPairPluginsDM = PluginsDeviceManager::create(std::move(oInit));
	auto& refPluginsDM = oPairPluginsDM.first;

	REQUIRE(oPairPluginsDM.second.empty());
	REQUIRE(refPluginsDM.operator bool());

	std::vector<shared_ptr<stmi::Event>> aReceived;
	auto refListener = std::make_shared<stmi::EventListener>(
			[&](const shared_ptr<stmi::Event>& refEvent)
			{
				aReceived.push_back(refEvent);
			});
	const bool bAdded = refPluginsDM->addEventListener(refListener);
	REQUIRE(bAdded);
	const bool bAdded2 = refPluginsDM->addEventListener(refListener);
	REQUIRE_FALSE(bAdded2);

	// addAccessor abused to add devices, parameter is ignored
	refPluginsDM->addAccessor(shared_ptr<Accessor>{});
	refPluginsDM->addAccessor(shared_ptr<Accessor>{});

	REQUIRE(aReceived.size() == 2);
	aReceived.clear();

	auto aDMDeviceClasses = refPluginsDM->getDeviceCapabilityClasses();
	REQUIRE(aDMDeviceClasses.size() == 1);

	auto aDMDevices = refPluginsDM->getDevices();
	REQUIRE(aDMDevices.size() == 2);
	REQUIRE(isUniqueSet(aDMDevices));
	auto aTst88Classes = refPluginsDM->getDevicesWithCapabilityClass(test::Tst88Capability::getClass());
	REQUIRE(aTst88Classes.size() == 0);
	auto aKeyClasses = refPluginsDM->getDevicesWithCapabilityClass(KeyCapability::getClass());
	REQUIRE(aKeyClasses.size() == 0);

	// Sends event
	refPluginsDM->enableEventClass(KeyEvent::getClass());
//std::cout << "   ----" << aReceived.size() << '\n';
	REQUIRE(aReceived.size() == 2);
}

} // namespace testing

} // namespace stmi
