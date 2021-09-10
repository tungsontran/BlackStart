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
    mac_ = check_and_cast<LteMacEnb*>(getMacByMacNodeId(nodeId_));
    cellInfo_ = mac_->getCellInfo();

    lsa_ = nullptr;
    warmUpPeriod_ = par("warmUpPeriod");
    lsaTimerWarmup_ = par("lsaTimerWarmup");
    lsaTimer_ = par("lsaTimer");
    lsaStart_ = par("lsaStart");
    metric_ = getRoutingMetric(par("metric"));

    EV << "Finishing initializing virtual router" << endl;
    lsa_ = new cMessage("linkStateAdvertisement");
    lsa_->setSchedulingPriority(1);
    scheduleAt(NOW + lsaStart_, lsa_);

    if (stage == inet::INITSTAGE_NETWORK_LAYER)
    {
        amc_ = mac_->getAmc();
    }
}

virtualRouter::~virtualRouter()
{
    cancelAndDelete(lsa_);
//    delete lsaMsg_;
}

void virtualRouter::addTableEntry(virtualRoutingTable& table, virtualRoutingTableEntry entry)
{
    if (table.empty())
    {
        EV << "Empty Table!" << endl;
        EV << "virtualRouter::addTableEntry - Added entry: " << endl;
        EV << "Master_ID " << entry.masterID << endl;
        for (auto jt: entry.ueEnbCost)
        {
            EV << "Owner_ID " << jt.second.ownerID
               << " - vUE_ID " << jt.first
               << " - CQI UL " << jt.second.cqiUL.first
               << " (" << jt.second.cqiUL.second << ")"
               << " - CQI DL " << jt.second.cqiDL.first
               << " (" << jt.second.cqiDL.second << ")"
               << " - ETX UL " << jt.second.etxUL.first
               << " (" << jt.second.etxUL.second << ")"
               << " - ETX DL " << jt.second.etxDL.first
               << " (" << jt.second.etxDL.second << ")" << endl;
            EV << "BandCQI UL {" << jt.second.bandCqiUL.first.at(0);
            for (int i = 1; i < jt.second.bandCqiUL.first.size(); ++i)
                EV << ", " << jt.second.bandCqiUL.first.at(i);
            EV << "} (" << jt.second.bandCqiUL.second << ")" << endl;
            EV << "BandCQI DL {" << jt.second.bandCqiDL.first.at(0);
            for (int i = 1; i < jt.second.bandCqiDL.first.size(); ++i)
                EV << ", " << jt.second.bandCqiDL.first.at(i);
            EV << "} (" << jt.second.bandCqiDL.second << ")" << endl;
        }
        table.push_back(entry);
        return;
    }
    else
    {
        for (auto table_entry = table.begin();table_entry != table.end(); ++table_entry)    // iterate through table
        {
            // update existing entry
            if (table_entry->masterID == entry.masterID)                                    // match master ID of the 2 entries
            {
                EV << "*********************************************************************" << endl;
                EV << "virtualRouter::addTableEntry - Found entry for Master_ID " << table_entry->masterID << endl;
                // iterate through vUE list of this table entry
                for (auto it = table_entry->ueEnbCost.begin(); it != table_entry->ueEnbCost.end(); ++it)
                {
                    EV << "Owner_ID " << it->second.ownerID
                       << " - vUE_ID " << it->first
                       << " - CQI UL " << it->second.cqiUL.first
                       << " (" << it->second.cqiUL.second << ")"
                       << " - CQI DL " << it->second.cqiDL.first
                       << " (" << it->second.cqiDL.second << ")"
                       << " - ETX UL " << it->second.etxUL.first
                       << " (" << it->second.etxUL.second << ")"
                       << " - ETX DL " << it->second.etxDL.first
                       << " (" << it->second.etxDL.second << ")" << endl;
                    EV << "BandCQI UL {" << it->second.bandCqiUL.first.at(0);
                    for (int i = 1; i < it->second.bandCqiUL.first.size(); ++i)
                        EV << ", " << it->second.bandCqiUL.first.at(i);
                    EV << "} (" << it->second.bandCqiUL.second << ")" << endl;
                    EV << "BandCQI DL {" << it->second.bandCqiDL.first.at(0);
                    for (int i = 1; i < it->second.bandCqiDL.first.size(); ++i)
                        EV << ", " << it->second.bandCqiDL.first.at(i);
                    EV << "} (" << it->second.bandCqiDL.second << ")" << endl;
                    // iterate through vUE list of the adding entry
                    for (auto jt = entry.ueEnbCost.begin(); jt != entry.ueEnbCost.end(); ++jt)
                    {
                        if (it->first == jt->first)    // match vUE ID of the 2 entries
                        {
                            EV << "Found sub-entry for vUE " << jt->first << endl;
                            // if adding entry has newer UL CQI, update old CQI in the table
                            if (it->second.cqiUL.second < jt->second.cqiUL.second)
                            {
                                it->second.cqiUL.first = jt->second.cqiUL.first;
                                it->second.cqiUL.second = jt->second.cqiUL.second;
                            }
                            // if adding entry has newer DL CQI, update old CQI in the table
                            if (it->second.cqiDL.second < jt->second.cqiDL.second)
                            {
                                it->second.cqiDL.first = jt->second.cqiDL.first;
                                it->second.cqiDL.second = jt->second.cqiDL.second;
                            }
                            // if adding entry has newer UL ETX, update old ETX in the table
                            if (it->second.etxUL.second < jt->second.etxUL.second)
                            {
                                it->second.etxUL.first = jt->second.etxUL.first;
                                it->second.etxUL.second = jt->second.etxUL.second;
                            }
                            // if adding entry has newer DL ETX, update old ETX in the table
                            if (it->second.etxDL.second < jt->second.etxDL.second)
                            {
                                it->second.etxDL.first = jt->second.etxDL.first;
                                it->second.etxDL.second = jt->second.etxDL.second;
                            }
                            // if adding entry has newer UL band CQI, update old band CQI in the table
                            if (it->second.bandCqiUL.second < jt->second.bandCqiUL.second)
                            {
                                it->second.bandCqiUL.first = jt->second.bandCqiUL.first;
                                it->second.bandCqiUL.second = jt->second.bandCqiUL.second;
                            }
                            // if adding entry has newer DL band CQI, update old band CQI in the table
                            if (it->second.bandCqiDL.second < jt->second.bandCqiDL.second)
                            {
                                it->second.bandCqiDL.first = jt->second.bandCqiDL.first;
                                it->second.bandCqiDL.second = jt->second.bandCqiDL.second;
                            }
                            entry.ueEnbCost.erase(jt);
                        }
                        if (std::next(it) == table_entry->ueEnbCost.end())
                        {
                            EV << "Sub-entry for vUE " << jt->first << " not found, adding it to the table" << endl;
                            table_entry->ueEnbCost.insert(std::make_pair(jt->first,jt->second));
                        }
                    }
                    EV << "Updated entry for Master_ID " << table_entry->masterID << endl;
                    EV << "Owner_ID " << it->second.ownerID
                       << " - vUE_ID " << it->first
                       << " - CQI UL " << it->second.cqiUL.first
                       << " (" << it->second.cqiUL.second << ")"
                       << " - CQI DL " << it->second.cqiDL.first
                       << " (" << it->second.cqiDL.second << ")"
                       << " - ETX UL " << it->second.etxUL.first
                       << " (" << it->second.etxUL.second << ")"
                       << " - ETX DL " << it->second.etxDL.first
                       << " (" << it->second.etxDL.second << ")" << endl;
                    EV << "BandCQI UL {" << it->second.bandCqiUL.first.at(0);
                    for (int i = 1; i < it->second.bandCqiUL.first.size(); ++i)
                        EV << ", " << it->second.bandCqiUL.first.at(i);
                    EV << "} (" << it->second.bandCqiUL.second << ")" << endl;
                    EV << "BandCQI DL {" << it->second.bandCqiDL.first.at(0);
                    for (int i = 1; i < it->second.bandCqiDL.first.size(); ++i)
                        EV << ", " << it->second.bandCqiDL.first.at(i);
                    EV << "} (" << it->second.bandCqiDL.second << ")" << endl;
                    EV << "*********************************************************************" << endl;
                }
                return;
            }
            // if adding entry doesn't already exist in table entry, add it to the table
            if(std::next(table_entry) == table.end())
            {
                EV << "Added entry: " << endl;
                EV << "Master_ID " << entry.masterID << endl;
                for (auto jt: entry.ueEnbCost)
                {
                    EV << "Owner_ID " << jt.second.ownerID
                       << " - vUE_ID " << jt.first
                       << " - CQI UL " << jt.second.cqiUL.first
                       << " (" << jt.second.cqiUL.second << ")"
                       << " - CQI DL " << jt.second.cqiDL.first
                       << " (" << jt.second.cqiDL.second << ")"
                       << " - ETX UL " << jt.second.etxUL.first
                       << " (" << jt.second.etxUL.second << ")"
                       << " - ETX DL " << jt.second.etxDL.first
                       << " (" << jt.second.etxDL.second << ")" << endl;
                    EV << "BandCQI UL {" << jt.second.bandCqiUL.first.at(0);
                    for (int i = 1; i < jt.second.bandCqiUL.first.size(); ++i)
                        EV << ", " << jt.second.bandCqiUL.first.at(i);
                    EV << "} (" << jt.second.bandCqiUL.second << ")" << endl;
                    EV << "BandCQI DL {" << jt.second.bandCqiDL.first.at(0);
                    for (int i = 1; i < jt.second.bandCqiDL.first.size(); ++i)
                        EV << ", " << jt.second.bandCqiDL.first.at(i);
                    EV << "} (" << jt.second.bandCqiDL.second << ")" << endl;
                }
                table.push_back(entry);
                return;
            }
        }
    }
}

void virtualRouter::setDirectNeighborsCQI(const MacNodeId vUEid, const Cqi CqiUL, const Cqi CqiDL, const CqiVector bandCqiUL, const CqiVector bandCqiDL)
{
    directNeighbors_.masterID = nodeId_;                               // entry header (master ID)

    directNeighbors_.ueEnbCost[vUEid].ownerID = getOwnerId(vUEid);     // owner ID corresponding to vUE ID
    directNeighbors_.ueEnbCost[vUEid].cqiUL.first = CqiUL;             // CQI UL
    directNeighbors_.ueEnbCost[vUEid].cqiUL.second = NOW;              // CQI UL write time stamp
    directNeighbors_.ueEnbCost[vUEid].cqiDL.first = CqiDL;             // CQI DL
    directNeighbors_.ueEnbCost[vUEid].cqiDL.second = NOW;              // CQI DL write time stamp
    directNeighbors_.ueEnbCost[vUEid].bandCqiUL.first = bandCqiUL;     // Band CQI UL
    directNeighbors_.ueEnbCost[vUEid].bandCqiUL.second = NOW;          // Band CQI UL write time stamp
    directNeighbors_.ueEnbCost[vUEid].bandCqiDL.first = bandCqiDL;     // Band CQI DL
    directNeighbors_.ueEnbCost[vUEid].bandCqiDL.second = NOW;          // Band CQI DL write time stamp
    addTableEntry(networkTopoTable_, directNeighbors_);
}

void virtualRouter::setDirectNeighborsETX(const MacNodeId vUEid, const double etxUL, const double etxDL)
{
    directNeighbors_.ueEnbCost[vUEid].etxUL.first = etxUL;            // ETX UL
    directNeighbors_.ueEnbCost[vUEid].etxUL.second = NOW;             // ETX UL write time stamp
    directNeighbors_.ueEnbCost[vUEid].etxDL.first = etxDL;            // ETX DL
    directNeighbors_.ueEnbCost[vUEid].etxDL.second = NOW;             // ETX DL write time stamp

    addTableEntry(networkTopoTable_, directNeighbors_);
}

virtualRoutingTableEntry virtualRouter::getDirectNeighbors()
{
    return directNeighbors_;
}

virtualRoutingTable virtualRouter::getNetworkTopoTable()
{
    return networkTopoTable_;
}

//void virtualRouter::printDirectNeighbors()
//{
//    EV << "virtualRouter::printDirectNeighbors(): This ENB " << directNeighbors_.first << " is connecting to the following vUEs:" << endl;
//    for (auto it: directNeighbors_.second)
//    {
//        EV << "virtualRouter::printDirectNeighbors(): vUE " << it.first
//                                         << ", Owner E2NB " << it.second.first
//                                         << ", CQI UL: " << it.second.second[0].first
//                                         << " (" << it.second.second[0].second << ")"
//                                         << ", CQI DL: " << it.second.second[1].first
//                                         << " (" << it.second.second[1].second << ")"
//                                         << ", ETX UL: " << it.second.second[2].first
//                                         << " (" << it.second.second[2].second << ")"
//                                         << ", ETX DL: " << it.second.second[3].first
//                                         << " (" << it.second.second[3].second << ")" << endl;
//    }
//}

void virtualRouter::printTable(const virtualRoutingTable table, const char* name)
{
    EV << "virtualRouter::printTable(): ***** "<< name <<" *****" << endl;
    for (auto tableEntry: table)
    {
        EV << "Master ENB " << tableEntry.masterID << endl;
        for (auto it: tableEntry.ueEnbCost)
        {
            EV << "Owner ENB " << it.second.ownerID
               << ", vUE " << it.first
               << ", CQI UL = " << it.second.cqiUL.first
               << " (" << it.second.cqiUL.second << ")"
               << ", CQI DL = " << it.second.cqiDL.first
               << " (" << it.second.cqiDL.second << ")"
               << ", ETX UL = " << it.second.etxUL.first
               << " (" << it.second.etxUL.second << ")"
               << ", ETX DL = " << it.second.etxDL.first
               << " (" << it.second.etxDL.second << ")" << endl;
            EV << "BandCQI UL {" << it.second.bandCqiUL.first.at(0);
            for (int i = 1; i < it.second.bandCqiUL.first.size(); ++i)
                EV << ", " << it.second.bandCqiUL.first.at(i);
            EV << "} (" << it.second.bandCqiUL.second << ")" << endl;
            EV << "BandCQI DL {" << it.second.bandCqiDL.first.at(0);
            for (int i = 1; i < it.second.bandCqiDL.first.size(); ++i)
                EV << ", " << it.second.bandCqiDL.first.at(i);
            EV << "} (" << it.second.bandCqiDL.second << ")" << endl;
            EV << "---------------------------------------------" << endl;
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
        if (NOW <= warmUpPeriod_)
            scheduleAt(NOW + lsaTimerWarmup_, msg);
        else
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
//    printTable(networkTopoTable_,"Network Topo Table");
    // create graph for routing, map all existing nodes to vertices
    adjmap_ = createAdjMap();
    adj_ = createAdjMatrix(adjmap_, metric_);
    delete(msg);
}

void virtualRouter::sendLSA()
{
    // LSA on DL
    Enter_Method("sendLSA");
    for (auto it: directNeighbors_.ueEnbCost)
    {
        RoutingTableMsg* lsaMsg_ = new RoutingTableMsg();

        MacNodeId srcId = nodeId_;  // ID of this e2NB
        lsaMsg_->setSourceId(srcId);
        L3Address srcAddr = binder_->getL3Address(srcId);
        lsaMsg_->setSourceAddr(srcAddr.toIPv4());

        lsaMsg_->setTable(networkTopoTable_);
        lsaMsg_->setName("LSA_DL");
        lsaMsg_->setByteLength(40); //@TODO table size?

        MacNodeId dstId = it.first; // ID of peering vUE
        lsaMsg_->setDestId(dstId);
        L3Address dstAddr = binder_->getL3Address(dstId);
        lsaMsg_->setDestAddr(dstAddr.toIPv4());

        IPv4Datagram *datagram = new IPv4Datagram();
        datagram->setSourceAddress(srcAddr);
        datagram->setDestinationAddress(dstAddr);
        datagram->setTransportProtocol(IP_PROT_NONE);
        datagram->setName("LSA_DL");
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

adjMatrix virtualRouter::createAdjMatrix(const adjMap& adjmap, routingMetric metric)
{
   // get the number of vertices for the adjacency matrix
   int V = adjmap.size();
   // initialize the adjacency matrix
   adjMatrix adj(V);
   // map value from network topo table (nodeID) to adjacency matrix (vertex ID)
   for (auto it: networkTopoTable_)
   {
       MacNodeId u = getAdjIndex(adjmap, it.masterID);              // Master ID
       for (auto jt: it.ueEnbCost)
       {
           MacNodeId v = getAdjIndex(adjmap, jt.first);             // vUE ID
           MacNodeId o = getAdjIndex(adjmap, jt.second.ownerID);    // Owner ID
           double w;                                                // symmetric edge weight
           double w_min;                                            // minimum possible edge weight
           double wU,wD;                                            // asymmetric edge weight
           switch (metric){
           case HOP:
           {
               w = 1;                                               // weighting by hop count
               w_min = 1;
               addEdgeSymmetric(adj,u,v,w);
               break;
           }
           case CQI:
           {
               wU = (double) 1/jt.second.cqiUL.first;               // weighting by CQI UL, inverse to minimize
               wD = (double) 1/jt.second.cqiDL.first;               // weighting by CQI DL
               w_min = (double) 1/15;                               // max CQI is 15 so min weight is 1/15
               addEdgeAsymmetric(adj,u,v,wU,wD);
               break;
           }
           case ETX:
           {
               wU = jt.second.etxUL.first;                          // weighting by ETX UL
               wD = jt.second.etxDL.first;                          // weighting by ETX DL
               w_min = 0;
               addEdgeAsymmetric(adj,u,v,wU,wD);
               break;
           }
           case ETT:
           {
               double etxUL = jt.second.etxUL.first;
               double etxDL = jt.second.etxDL.first;
               Cqi cqiUL = jt.second.cqiUL.first;
               Cqi cqiDL = jt.second.cqiDL.first;
               // number of antenna layers.
               unsigned int layers = 1;                     // @TODO: support MIMO?
               // get number of resource blocks for each channel
               int numRbUl = cellInfo_->getNumRbUl();
               int numRbDl = cellInfo_->getNumRbDl();
               // this is only the transmission rate per 1 millisecond (1 TTI),
               // it should be x1000 but would be unnecessary for comparison
               double transmissionRateUL = mac_->getAmc()->readTbsVect(cqiUL,layers,UL)[numRbUl];
               double transmissionRateDL = mac_->getAmc()->readTbsVect(cqiDL,layers,DL)[numRbDl];
               // ETT of each channel = ETX * packet size / transmission rate
               // Packet size is omitted because it's unnecessary for comparison
               wU = etxUL/transmissionRateUL;
               wD = etxDL/transmissionRateDL;
               // ETT is relatively small so a small number should be sufficient as min weight
               w_min = 0.00001;
               addEdgeAsymmetric(adj,u,v,wU,wD);
               break;
           }
           default:
               throw cRuntimeError("createAdjMatrix: invalid weight!");
           }
           addEdgeSymmetric(adj,o,v,w_min);
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
