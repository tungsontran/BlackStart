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

#ifndef __LTE_AIRCOMPMANAGER_H_
#define __LTE_AIRCOMPMANAGER_H_

#include "common/LteCommon.h"
#include "stack/mac/layer/LteMacEnb.h"
#include "stack/mac/buffer/LteMacBuffer.h"

typedef enum {
    COMP_CLIENT,
    COMP_CLIENT_COORDINATOR,
    COMP_COORDINATOR
} airCompNodeType;

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class airCompManager : public cSimpleModule
{
  protected:
    MacNodeId nodeId_;

    // reference to the MAC layer
    LteMacEnb* mac_;

    // number of available bands
    int numBands_;

    // period between two coordination instances
    double coordinationPeriod_;

    /// Self messages
    cMessage* compClientTick_;
    cMessage* compCoordinatorTick_;

    // Comp Node Type specification (client, client and coordinator, coordinator only)
    airCompNodeType nodeType_;

    // Last received usable bands
    UsableBands usableBands_;

    // ID of the coordinator
    MacNodeId coordinatorId_;

    // IDs of the eNB that are slaves of this master node
    std::vector<MacNodeId> clientList_;

    // statistics
    simsignal_t compReservedBlocks_;



  public:
    airCompManager() {}
    virtual ~airCompManager() {}

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
