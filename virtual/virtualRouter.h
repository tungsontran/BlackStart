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
#include "virtual/algorithm/dijkstra.h"

using namespace omnetpp;

class virtualRouter : public cSimpleModule
{
    LteBinder* binder_;
    /// Link State self message
    cMessage* lsa_;
    double lsaTimer_;
    double lsaStart_;
  protected:
    MacNodeId nodeId_;
    virtualRouterState* state_;

    virtualRoutingTableEntry directNeighbors_;

    virtualRoutingTable directNeighborsTable_;
    virtualRoutingTable networkTopoTable_;
    virtualRoutingTable actualRoutingTable_;

    simsignal_t sendLsaHello;

    virtual void initialize(int stage) override;
  public:
//    virtualRouter();
    virtual ~virtualRouter();
    void addTableEntry(virtualRoutingTable& table, const virtualRoutingTableEntry entry);
    void setDirectNeighbors(const ueCqi uecqi);
    void setDirectNeighborsTable(const virtualRoutingTableEntry entry);
    virtualRoutingTable getDirectNeighborsTable() const;
    virtualRoutingTable getNetworkTopoTable() const;
    virtualRoutingTable getActualRoutingTable() const;
    void printDirectNeighbors() const;
    void printTable(const virtualRoutingTable table, const char* name);
    virtual void handleMessage(cMessage *msg) override;
    void sendLSA();
    void createAdjMatrix();
    void computeRoute();
};

#endif
