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
    metric_ = getRoutingWeight(par("metric"));

    EV << "Finishing initializing virtual router" << endl;
    lsa_ = new cMessage("linkStateAdvertisement");
    lsa_->setSchedulingPriority(1);
    scheduleAt(NOW + lsaStart_, lsa_);
}

virtualRouter::~virtualRouter()
{
    cancelAndDelete(lsa_);
//    delete lsaMsg_;
}

void virtualRouter::addTableEntry(virtualRoutingTable& table, const virtualRoutingTableEntry entry)
{
    if (table.empty())
    {
        EV << "Empty Table!" << endl;
        EV << "virtualRouter::addTableEntry - Added entry: " << endl;
        EV << "Master_ID " << entry.first << endl;
        ueEnbCost::iterator jt;
        for (auto jt: entry.second)
        {
            EV << "Owner_ID " << jt.second.first
               << " - vUE_ID " << jt.first
               << " - CQI " << jt.second.second[0].first
               << " (" << jt.second.second[0].second << ")"
               << " - ETX " << jt.second.second[1].first
               << " (" << jt.second.second[1].second << ")" << endl;
        }
        table.push_back(entry);
        return;
    }
    else
    {
        for (auto table_entry = table.begin();table_entry != table.end(); ++table_entry)    // iterate through table
        {
            // update existing entry
            if (table_entry->first == entry.first)                                          // match master ID of the 2 entries
            {
                EV << "virtualRouter::addTableEntry - Found entry for Master_ID " << table_entry->first << endl;
                // iterate through vUE list of this table entry
                for (auto it = table_entry->second.begin(); it != table_entry->second.end(); ++it)
                {
                    EV << "Owner_ID " << it->second.first
                       << " - vUE_ID " << it->first
                       << " - CQI " << it->second.second[0].first
                       << " (" << it->second.second[0].second << ")"
                       << " - ETX " << it->second.second[1].first
                       << " (" << it->second.second[1].second << ")" << endl;
                    // iterate through vUE list of the adding entry
                    for (auto jt = entry.second.begin(); jt != entry.second.end(); ++jt)
                    {
                        if (it->first == jt->first)    // match vUE ID of the 2 entries
                        {
                            EV << "Found sub-entry for vUE_ID: " << it->first << endl;
                            // if adding entry has newer CQI, update old CQI in the table
                            if (it->second.second[0].second < jt->second.second[0].second)
                            {
                                it->second.second[0].first = jt->second.second[0].first;
                                it->second.second[0].second = jt->second.second[0].second;
                            }
                            // if adding entry has newer ETX, update old ETX in the table
                            if (it->second.second[1].second < jt->second.second[1].second)
                            {
                                it->second.second[1].first = jt->second.second[1].first;
                                it->second.second[1].second = jt->second.second[1].second;
                            }
                        }
                        else
                        {
                            // @TODO if vUE sub-entry doesnt exist, add it
                        }
                    }
                    EV << "Updated entry for Master_ID " << table_entry->first << endl;
                    EV << "Owner_ID " << it->second.first
                       << " - vUE_ID " << it->first
                       << " - CQI " << it->second.second[0].first
                       << " (" << it->second.second[0].second << ")"
                       << " - ETX " << it->second.second[1].first
                       << " (" << it->second.second[1].second << ")" << endl;
                }
                return;
            }
            // if adding entry doesn't already exist in table entry, add it to the table
            else if(std::next(table_entry) == table.end())
            {
                EV << "Added entry: " << endl;
                EV << "Master_ID " << entry.first << endl;
                ueEnbCost:: iterator jt;
                for (auto jt: entry.second)
                {
                    EV << "Owner_ID " << jt.second.first
                                   << " - vUE_ID " << jt.first
                                   << " - CQI " << jt.second.second[0].first
                                   << " (" << jt.second.second[0].second << ")"
                                   << " - ETX " << jt.second.second[1].first
                                   << " (" << jt.second.second[1].second << ")" << endl;
                }
                table.push_back(entry);
                return;
            }
        }
    }
}

void virtualRouter::setDirectNeighborsCQI(const ueCqi uecqi)
{
    directNeighbors_.first = nodeId_;                                         // entry header (master ID)
    ueCqi::iterator it;
    for (auto it: uecqi)
    {
        directNeighbors_.second[it.first].first = getOwnerId(it.first);       // owner ID corresponding to vUE ID
        directNeighbors_.second[it.first].second[0].first = it.second;        // CQI
        directNeighbors_.second[it.first].second[0].second = NOW;             // CQI write time stamp
    }
//    addTableEntry(directNeighborsTable_, directNeighbors_);
    addTableEntry(networkTopoTable_, directNeighbors_);
    printDirectNeighbors();
}

void virtualRouter::setDirectNeighborsETX(const ueEtx ueetx)
{
    directNeighbors_.second[ueetx.first].second[1].first = ueetx.second;      // ETX
    directNeighbors_.second[ueetx.first].second[1].second = NOW;              // ETX write time stamp
//    addTableEntry(directNeighborsTable_, directNeighbors_);
    addTableEntry(networkTopoTable_, directNeighbors_);
    printDirectNeighbors();
}

virtualRoutingTableEntry virtualRouter::getDirectNeighbors()
{
    return directNeighbors_;
}

virtualRoutingTable virtualRouter::getDirectNeighborsTable()
{
    return directNeighborsTable_;
}

virtualRoutingTable virtualRouter::getNetworkTopoTable()
{
    return networkTopoTable_;
}

void virtualRouter::printDirectNeighbors()
{
    EV << "virtualRouter::printDirectNeighbors(): This ENB " << directNeighbors_.first << " is connecting to the following vUEs:" << endl;
    ueCqi::iterator it;
    for (auto it: directNeighbors_.second)
    {
        EV << "virtualRouter::printDirectNeighbors(): vUE " << it.first
                                         << ", Owner E2NB " << it.second.first
                                         << ", CQI: " << it.second.second[0].first
                                         << " (" << it.second.second[0].second << ")"
                                         << ", ETX: " << it.second.second[1].first
                                         << " (" << it.second.second[1].second << ")" << endl;
    }
}

void virtualRouter::printTable(const virtualRoutingTable table, const char* name)
{
    EV << "virtualRouter::printTable(): ***** "<< name <<" *****" << endl;
    virtualRoutingTable::iterator tableEntry;
    for (auto tableEntry: table)
    {
        EV << "Master ENB " << tableEntry.first << endl;
        for (auto it: tableEntry.second)
        {
            EV << "Owner ENB " << it.second.first
               << ", vUE " << it.first
               << ", CQI = " << it.second.second[0].first
               << " (" << it.second.second[0].second << ")"
               << ", ETX = " << it.second.second[1].first
               << " (" << it.second.second[0].second << ")" << endl;
        }
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
    adj_ = createAdjMatrix(adjmap_, metric_);
    delete(msg);
}

void virtualRouter::sendLSA()
{
    // LSA on DL
    Enter_Method("sendLSA");
    ueEnbCost::iterator it;
    for (auto it: directNeighbors_.second)
    {
        RoutingTableMsg* lsaMsg_ = new RoutingTableMsg();

        MacNodeId srcId = nodeId_;  // ID of this e2NB
        lsaMsg_->setSourceId(srcId);
        L3Address srcAddr = binder_->getL3Address(srcId);
        lsaMsg_->setSourceAddr(srcAddr.toIPv4());

        lsaMsg_->setTable(networkTopoTable_);
        lsaMsg_->setName("LSA_HELLO");
        lsaMsg_->setByteLength(40); //@TODO table size?

        MacNodeId dstId = it.first; // ID of peering vUE
        lsaMsg_->setDestId(dstId);
        L3Address dstAddr = binder_->getL3Address(dstId);
        lsaMsg_->setDestAddr(dstAddr.toIPv4());

        IPv4Datagram *datagram = new IPv4Datagram();
        datagram->setSourceAddress(srcAddr);
        datagram->setDestinationAddress(dstAddr);
        datagram->setTransportProtocol(IP_PROT_NONE);
        datagram->setName("LSA_HELLO");
        datagram->encapsulate(lsaMsg_);

        send(datagram,"ipOut");
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

adjMatrix virtualRouter::createAdjMatrix(const adjMap& adjmap, routingWeight weight)
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
           double w;
           switch (weight){
           case HOP:
               w = 1;                                          // weighting by hop count
               break;
           case CQI:
               w = 16 - jt.second.second[0].first;                      // weighting by CQI
//               w = 1/jt.second.second[0];                      // weighting by CQI
               break;
           default:
               throw cRuntimeError("createAdjMatrix: invalid weight!");
           }
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
