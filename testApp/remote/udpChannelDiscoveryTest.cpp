// Test channel discovery over UDP

#include <stdlib.h>

#include <epicsExit.h>
#include <epicsUnitTest.h>
#include <testMain.h>

#include <pv/logger.h>
#include <pv/pvAccess.h>
#include <pv/serverContext.h>
#include <pv/clientFactory.h>

#include "channelDiscoveryTest.h"

#define TESTSERVERNOMAIN
#include "testServer.cpp"

namespace EPVA = epics::pvAccess;


int runAllTests() {
    ChannelDiscoveryTest cdTest;
    testPlan(cdTest.getNumberOfTests());
    testDiag("Channel discovery type: UDP search");

    EPVA::Configuration::shared_pointer baseConfig(ConfigurationBuilder()
        .add("EPICS_PVAS_INTF_ADDR_LIST", "127.0.0.1")
        .add("EPICS_PVA_ADDR_LIST", "127.0.0.1")
        .add("EPICS_PVA_AUTO_ADDR_LIST", "0")
        .add("EPICS_PVA_SERVER_PORT", "0")
        .add("EPICS_PVA_BROADCAST_PORT", "0")
        .push_map()
        .build());

    EPVA::Configuration::shared_pointer serverConfig(ConfigurationBuilder()
        .push_config(baseConfig)
        .push_map()
        .build());

    TestServer::shared_pointer testServer(new TestServer(serverConfig));
    testDiag("Test server is using ports TCP: %u, UDP Broadcast: %u",
        testServer->getServerPort(),
        testServer->getBroadcastPort());

    EPVA::Configuration::shared_pointer clientConfig(ConfigurationBuilder()
        .push_config(baseConfig)
        .add("EPICS_PVA_BROADCAST_PORT", testServer->getBroadcastPort())
        .push_map()
        .build());

    ConfigurationFactory::registerConfiguration("pvAccess-client", clientConfig);
    testDiag("Starting client factory");
    epics::pvAccess::ClientFactory::start();
    return cdTest.runAllTests();
}

MAIN(testUdpChannelDiscovery)
{
    try{
        SET_LOG_LEVEL(logLevelError);
        return runAllTests();
    }
    catch(std::exception& e) {
        PRINT_EXCEPTION(e);
        std::cerr << "Unhandled exception: " << e.what() << "\n";
        return 1;
    }
}
