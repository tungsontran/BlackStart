//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef __EVALUATION_FOR_THE_ICT_SYSTEM_RESTORATION_METHODS_COEAGENT_H_
#define __EVALUATION_FOR_THE_ICT_SYSTEM_RESTORATION_METHODS_COEAGENT_H_

#include <omnetpp.h>
#include <map>
#include "inet/networklayer/common/L3AddressResolver.h"
#include "inet/networklayer/ipv4/IPv4Datagram.h"
#include "corenetwork/binder/LteBinder.h"
#include "epc/gtp_common.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class CoeAgent : public cSimpleModule
{

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
