/*
 * blockingUDPConnector.cpp
 *
 *  Created on: Dec 27, 2010
 *      Author: Miha Vitorovic
 */

/* pvAccess */
#include <pv/blockingUDP.h>
#include <pv/remote.h>

/* EPICSv3 */
#include <errlog.h>
#include <osiSock.h>

/* standard */
#include <sys/types.h>
#include <sys/socket.h>

using namespace std;

namespace epics {
    namespace pvAccess {

    Transport::shared_pointer BlockingUDPConnector::connect(TransportClient::shared_pointer const & client,
                auto_ptr<ResponseHandler>& responseHandler, osiSockAddr& bindAddress,
                short transportRevision, int16 priority) {
                    
            errlogSevPrintf(errlogInfo, "Creating datagram socket to: %s",
                    inetAddressToString(bindAddress).c_str());

            SOCKET socket = epicsSocketCreate(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if(socket==INVALID_SOCKET) {
                char errStr[64];
                epicsSocketConvertErrnoToString(errStr, sizeof(errStr));
                errlogSevPrintf(errlogMajor, "Error creating socket: %s", errStr);
                return Transport::shared_pointer();
            }

            int optval = _broadcast ? 1 : 0;
            int retval = ::setsockopt(socket, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
            if(retval<0)
            {
                errlogSevPrintf(errlogMajor, "Error setting SO_BROADCAST: %s", strerror(errno));
                epicsSocketDestroy (socket);
                return Transport::shared_pointer();
            }
            
            /*
            IPv4 multicast addresses are defined by the leading address bits of 1110, originating from the classful network design of the early Internet when this group of addresses was designated as Class D. The Classless Inter-Domain Routing (CIDR) prefix of this group is 224.0.0.0/4. The group includes the addresses from 224.0.0.0 to 239.255.255.255. Address assignments from within this range are specified in RFC 5771, an Internet Engineering Task Force (IETF) Best Current Practice document (BCP 51).*/
            
            
            // set SO_REUSEADDR or SO_REUSEPORT, OS dependant
            if (_reuseSocket)
                epicsSocketEnableAddressUseForDatagramFanout(socket);

            retval = ::bind(socket, (sockaddr*)&(bindAddress.sa), sizeof(sockaddr));
            if(retval<0) {
                errlogSevPrintf(errlogMajor, "Error binding socket: %s", strerror(errno));
                epicsSocketDestroy (socket);
                return Transport::shared_pointer();
            }

            // sockets are blocking by default
            Transport::shared_pointer transport = BlockingUDPTransport::create(responseHandler, socket, bindAddress, transportRevision);
            return transport;
        }

    }
}
