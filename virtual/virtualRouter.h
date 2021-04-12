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

#ifndef __EVALUATION_FOR_THE_ICT_SYSTEM_RESTORATION_METHODS_VIRTUALROUTER_H_
#define __EVALUATION_FOR_THE_ICT_SYSTEM_RESTORATION_METHODS_VIRTUALROUTER_H_

#include <omnetpp.h>
#include "common/LteCommon.h"
#include "virtual/packet/RoutingTableMsg.h"
#include "corenetwork/binder/LteBinder.h"
#include "inet/networklayer/ipv4/IPv4Datagram.h"

using namespace omnetpp;

class virtualRouter : public cSimpleModule
{
    LteBinder* binder_;
    /// Link State self message
    cMessage* lsa_;
    double lsaTimer_;
    double lsaStart_;
  protected:
    virtualRouterState* state_;

    virtualRoutingTableEntry directNeighbors_;
    MacNodeId nodeId_;
    virtualRoutingTable directNeighborsTable_;
    virtualRoutingTable networkTopoTable_;
    virtualRoutingTable actualRoutingTable_;

    simsignal_t sendLsaHello;

    virtual void initialize(int stage) override;
  public:
//    virtualRouter();
    virtual ~virtualRouter();
    virtualRoutingTableEntry getTableEntry(const MacNodeId nodeId, const virtualRoutingTable table);
    void addTableEntry(virtualRoutingTable& table, const virtualRoutingTableEntry entry);
    void setDirectNeighbors(const ueCqi uecqi);
    virtualRoutingTable getDirectNeighborsTable() const;
    void setDirectNeighborsTable(const virtualRoutingTableEntry entry);
    virtualRoutingTable getNetworkTopoTable() const;
    void setNetworkTopoTable(const virtualRoutingTableEntry entry);
    virtualRoutingTable getActualRoutingTable() const;
    void setActualRoutingTable(const virtualRoutingTableEntry entry);
    void printDirectNeighbors() const;
    void printTable(const virtualRoutingTable table, const char* name);

    void sendLSA();
    virtual void handleMessage(cMessage *msg) override;
};

#endif
