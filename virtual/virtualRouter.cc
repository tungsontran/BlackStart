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

#include "virtual/virtualRouter.h"

Define_Module(virtualRouter);

void virtualRouter::initialize(int stage)
{
    nodeId_ = getAncestorPar("macNodeId");
    binder_ = getBinder();
    lsa_ = nullptr;
    lsaTimer_ = par("lsaTimer");
    lsaStart_ = par("lsaStart");

    EV << "Finishing initializing virtual router" << endl;
    lsa_ = new cMessage("linkStateAdvertisement");
    lsa_->setSchedulingPriority(1);
    scheduleAt(NOW + lsaStart_, lsa_);
}

virtualRouter::~virtualRouter()
{
    cancelAndDelete(lsa_);
}

void virtualRouter::addTableEntry(virtualRoutingTable& table, const virtualRoutingTableEntry entry)
{
    if (table.empty())
    {
        EV << "Empty Table!" << endl;
        EV << "virtualRouter::addTableEntry - Add entry: " << endl;
        EV << "Master_ID " << entry.first << endl;
        ueEnbCqi:: iterator jt;
        for (auto jt: entry.second)
        {
            EV << "Owner_ID " << jt.second.first << " - vUE_ID " << jt.first << " - CQI " << jt.second.second << endl;
        }
        table.push_back(entry);
        return;
    }
    else
    {
        for (auto it = table.begin();it != table.end(); ++it)
        {
            // update existing entry
            if (it->first == entry.first)
            {
                EV << "virtualRouter::addTableEntry - Found entry: "<< endl;
                EV << "Master_ID " << it->first << endl;
                ueEnbCqi:: iterator jt;
                for (auto jt: entry.second)
                {
                    EV << "Owner_ID " << jt.second.first << " - vUE_ID " << jt.first << " - CQI " << jt.second.second << endl;
                }
                it->second = entry.second;
                EV << "virtualRouter::addTableEntry - Updated entry: "<< endl;
                EV << "Master_ID " << it->first << endl;
                for (auto jt: entry.second)
                {
                    EV << "Owner_ID " << jt.second.first << " - vUE_ID " << jt.first << " - CQI " << jt.second.second << endl;
                }
                return;
            }
            else if(std::next(it) == table.end())
            {
                EV << "virtualRouter::addTableEntry - Add entry: " << endl;
                EV << "Master_ID " << entry.first << endl;
                ueEnbCqi:: iterator jt;
                for (auto jt: entry.second)
                {
                    EV << "Owner_ID " << jt.second.first << " - vUE_ID " << jt.first << " - CQI " << jt.second.second << endl;
                }
                table.push_back(entry);
                return;
            }
        }
    }
}

void virtualRouter::setDirectNeighbors(const ueCqi uecqi)
{
    directNeighbors_.first = nodeId_;
    ueEnbCqi ueenbcqi;
    ueCqi::iterator it;
    for (auto it: uecqi)
    {
        ueenbcqi[it.first].first = getOwnerId(it.first);
        ueenbcqi[it.first].second = it.second;
    }
    directNeighbors_.second = ueenbcqi;
    addTableEntry(directNeighborsTable_, directNeighbors_);
    addTableEntry(networkTopoTable_, directNeighbors_);
}

virtualRoutingTable virtualRouter::getDirectNeighborsTable() const
{
    return directNeighborsTable_;
}

virtualRoutingTable virtualRouter::getNetworkTopoTable() const
{
    return networkTopoTable_;
}

virtualRoutingTable virtualRouter::getActualRoutingTable() const
{
    return actualRoutingTable_;
}

void virtualRouter::printDirectNeighbors() const
{
    EV << "virtualRouter::printDirectNeighbors(): This ENB " << directNeighbors_.first << " is connecting to the following vUEs:" << endl;
    ueCqi::iterator it;
    for (auto it: directNeighbors_.second)
    {
        EV << "virtualRouter::printDirectNeighbors(): vUE " << it.first
                                         << ", Owner E2NB " << it.second.first
                                         << ", CQI: " << it.second.second << endl;
    }
}

void virtualRouter::printTable(const virtualRoutingTable table, const char* name)
{
    EV << "virtualRouter::printTable(): ***** "<< name <<" *****" << endl;
    virtualRoutingTable::iterator tableEntry;
    for (auto tableEntry: table)
    {
        EV << "Master ENB " << tableEntry.first << endl;
        ueEnbCqi::iterator ueenbcqi;
        for (auto ueenbcqi: tableEntry.second)
        {
            EV << "Owner ENB " << ueenbcqi.second.first << ", vUE " << ueenbcqi.first << ", CQI = " << ueenbcqi.second.second << endl;
        } //@TODO direct master
    }
    EV << "virtualRouter::printTable(): ***** End Print Table *****" << endl;
}

void virtualRouter::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        sendLSA();
        EV << "E2NB " << nodeId_ << " is sending Network Topo Table" << endl;
        scheduleAt(NOW + lsaTimer_, msg);
        return;
    }
    else
    {
        if (!strcmp(msg->getName(),"LSA_HELLO"))
        {
            RoutingTableMsg* msg_temp = check_and_cast<RoutingTableMsg*>(msg);
            virtualRoutingTable table_temp = msg_temp->getTable();
            MacNodeId enbID = getAncestorPar("macNodeId");
            EV << "Updating Network Topo Table for ENB " << enbID << endl;
            for (auto it = table_temp.begin(); it != table_temp.end(); ++it)
            {
                addTableEntry(networkTopoTable_,*it);
            }
            printTable(networkTopoTable_,"Network Topo Table");
            // create graph for routing, map all existing nodes to vertices
            adjmap_ = createAdjMap();
            adj_ = createAdjMatrix(adjmap_);
            delete(msg);
        }
    }
}

void virtualRouter::sendLSA()
{
    Enter_Method("sendLSA");
    ueEnbCqi::iterator it;
    for (auto it: directNeighbors_.second)
    {
        RoutingTableMsg* msg = new RoutingTableMsg();

        MacNodeId srcId = nodeId_;
        msg->setSourceId(srcId);
        L3Address srcAddr = binder_->getL3Address(srcId);
        msg->setSourceAddr(srcAddr.toIPv4());

        msg->setTable(networkTopoTable_);
        msg->setName("LSA_HELLO");
        msg->setByteLength(40); //@TODO table size?

        MacNodeId dstId = it.first; //@TODO seperate for UL & DL
        msg->setDestId(dstId);
        L3Address dstAddr = binder_->getL3Address(dstId);
        msg->setDestAddr(dstAddr.toIPv4());

        IPv4Datagram *datagram = new IPv4Datagram();
        datagram->setSourceAddress(srcAddr);
        datagram->setDestinationAddress(dstAddr);
        datagram->setTransportProtocol(IP_PROT_NONE);
        datagram->setName("LSA_HELLO");
        datagram->encapsulate(msg);

        send(datagram,"ipOut");

//        emit(sendLsaHello,datagram);
    }
}

adjMap virtualRouter::createAdjMap()
{
    // map nodeID of all nodes (e2NB, vUE) to vertex ID
    // eg: [1 2 3 64005 64006] -> [1 2 3 4 5]
    MacNodeId enbCount = binder_->getNodeCounter("ENB");
    MacNodeId vueCount = binder_->getNodeCounter("VUE");
    adjMap adjmap = getAdjMap(enbCount,vueCount);
    return adjmap;
}

adjMatrix virtualRouter::createAdjMatrix(adjMap& adjmap)
{
   // get the number of vertices for the adjacency matrix
   int V = adjmap.size();
   // initialize the adjacency matrix
   adjMatrix adj(V);
   // map value from network topo table (nodeID) to adjacency matrix (vertex ID)
   for (auto it: networkTopoTable_)
   {
       MacNodeId u = getAdjIndex(adjmap, it.first);            // Master ID
       for (auto jt: it.second)
       {
           MacNodeId v = getAdjIndex(adjmap, jt.first);        // vUE ID
           Cqi w = 16 - jt.second.second;                      // weight
           addEdge(adj,u,v,w);
           MacNodeId o = getAdjIndex(adjmap, jt.second.first); // Owner ID
           addEdge(adj,o,v,1);
       }
   }
   EV << "***** Adjacency Matrix *****" << endl;
   for (int i = 0; i < V; i++)
   {
       EV << "u: " << i << endl;
       for (auto j: adj[i])
       {
           EV << "  v: " << j.first << ", w: " << j.second << endl;
       }
   }
   EV << "****************************" << endl;
   return adj;
}

MacNodeId virtualRouter::computeRoute(MacNodeId dest)
{
    assert(!adj_.empty() || !adjmap_.empty());
    int V = adjmap_.size();
    MacNodeId src = getAdjIndex(adjmap_,nodeId_);
    MacNodeId dst = getAdjIndex(adjmap_,dest);
    std::vector<MacNodeId> parent = dijkstra(adj_,V,src,adjmap_);
    MacNodeId nextHop = adjmap_[getNextHop(parent, src, dst)];
    return nextHop;
}
