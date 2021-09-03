//
//                           SimuLTE
//
// This file is part of a software released under the license included in file
// "license.pdf". This license can be also found at http://www.ltesimulator.com/
// The above file and the present reference are part of the software itself,
// and cannot be removed from it.
//

#include "epc/gtp/GtpUserSimplified.h"
#include "inet/networklayer/contract/ipv4/IPv4ControlInfo.h"
#include "inet/networklayer/common/L3AddressResolver.h"
#include <iostream>

Define_Module(GtpUserSimplified);

void GtpUserSimplified::initialize(int stage)
{
    cSimpleModule::initialize(stage);

    // wait until all the IP addresses are configured
    if (stage != inet::INITSTAGE_APPLICATION_LAYER)
        return;
    localPort_ = par("localPort");

    // get reference to the binder
    binder_ = getBinder();

    socket_.setOutputGate(gate("udpOut"));
    socket_.bind(localPort_);

    tunnelPeerPort_ = par("tunnelPeerPort");

    // get the address of the PGW
    pgwAddress_ = L3AddressResolver().resolve("pgw");

    ownerType_ = selectOwnerType(getAncestorPar("nodeType"));

    if (ownerType_ == ENB)
    {
        nodeId_ = getAncestorPar("macNodeId");
        subType_ = getNodeSubType(getAncestorPar("nodeSubType"));
        if (subType_ == E2NODEB)
        {
            // get mode
            mode_ = getE2NodeBMode(getParentModule()->par("mode").stringValue());
            // get reference to the virtual router
            vRouter_ = check_and_cast<virtualRouter*>(getModuleByPath("^.virtualRouter"));
        }
    }
}

EpcNodeType GtpUserSimplified::selectOwnerType(const char * type)
{
    EV << "GtpUserSimplified::selectOwnerType - setting owner type to " << type << endl;
    if(strcmp(type,"ENODEB") == 0)
        return ENB;
    if(strcmp(type,"PGW") != 0)
        error("GtpUserSimplified::selectOwnerType - unknown owner type [%s]. Aborting...",type);
    return PGW;
}

void GtpUserSimplified::handleMessage(cMessage *msg)
{
    if (strcmp(msg->getArrivalGate()->getFullName(), "trafficFlowFilterGate") == 0)
    {
        EV << "GtpUserSimplified::handleMessage - message from trafficFlowFilter" << endl;
        // obtain the encapsulated IPv4 datagram
        IPv4Datagram * datagram = check_and_cast<IPv4Datagram*>(msg);
        handleFromTrafficFlowFilter(datagram);
    }
    else if(strcmp(msg->getArrivalGate()->getFullName(),"udpIn")==0)
    {
        EV << "GtpUserSimplified::handleMessage - message from udp layer" << endl;

        GtpUserMsg * gtpMsg = check_and_cast<GtpUserMsg *>(msg);
        handleFromUdp(gtpMsg);
    }
    else if(strcmp(msg->getArrivalGate()->getFullName(),"appIn")==0)
    {
        EV << "GtpUserSimplified::handleMessage - message from virtual UE";
        if (dynamic_cast<IPv4Datagram*>(msg))
        {
            EV << " - DownlinkAirMessage - sending to stack" << endl;
            handleFromApp((IPv4Datagram*)msg);
        }
        else if (dynamic_cast<RoutingTableMsg*>(msg))
        {
            EV << " - LSA message - sending to stack" << endl;
            send(msg,"pppGate");
        }
    }
}

void GtpUserSimplified::handleFromTrafficFlowFilter(IPv4Datagram * datagram)
{
    // extract control info from the datagram
    TftControlInfo * tftInfo = check_and_cast<TftControlInfo *>(datagram->removeControlInfo());
    TrafficFlowTemplateId flowId = tftInfo->getTft();
    delete (tftInfo);

    EV << "GtpUserSimplified::handleFromTrafficFlowFilter - Received a tftMessage with flowId[" << flowId << "]" << endl;

   // If we are on the eNB and the flowId represents the ID of this eNB, forward the packet locally
    if (flowId == 0)
    {
        // intracell delivery
        send(datagram,"pppGate");
    }
    else
    {
        // intercell delivery
        if (ownerType_ == PGW ||                            // if this is a PGW
            subType_ == NONE  ||                            // if this is an ENB
           (subType_ == E2NODEB && mode_ == NORMAL))        // if this is an E2NB on normal mode
        {
            // create a new GtpUserSimplifiedMessage
            GtpUserMsg * gtpMsg = new GtpUserMsg();
            gtpMsg->setName("GtpUserMessage");

            // encapsulate the datagram within the GtpUserSimplifiedMessage
            gtpMsg->encapsulate(datagram);

            L3Address tunnelPeerAddress;
            if (flowId == -1) // send to the PGW
            {
                tunnelPeerAddress = pgwAddress_;
            }
            else
            {
                // get the symbolic IP address of the tunnel destination ID
                // then obtain the address via IPvXAddressResolver
                const char* symbolicName = binder_->getModuleNameByMacNodeId(flowId);
                tunnelPeerAddress = L3AddressResolver().resolve(symbolicName);
            }
            socket_.sendTo(gtpMsg, tunnelPeerAddress, tunnelPeerPort_);
        }
        else if (subType_ == E2NODEB && mode_ == OUTAGE)
        {
            // get the node sub type where the datagram originated from
            LteNodeSubType moduleSubType = getSrcNodeSubType(datagram);
            // if datagram comes from an UE
			// i.e receiving from an UE within the cell of this e2NB
            if (moduleSubType == NONE)
            {
                EV << "Datagram comes from an UE, routing to next hop" << endl;
                // compute next hop
                forward(datagram);
            }
            // if datagram comes from a vUE, 
			// i.e receiving UplinkAirMessage from a vUE owned by another e2NB 			
            else if (moduleSubType == VUE)
            {
                EV << "Datagram comes from a vUE, forwarding to stack" << endl;
                IPv4Datagram *datagram_ = check_and_cast<IPv4Datagram*>(datagram->decapsulate());
                delete(datagram);

                L3Address dstAddr = datagram_->getDestinationAddress();         // true dest address (UE)
                MacNodeId dstID = binder_->getMacNodeId(dstAddr.toIPv4());      // true dest ID (UE)
                MacNodeId dstMasterID = binder_->getNextHop(dstID);             // master ID of dest ID (ENB)

                if (dstMasterID == nodeId_)
                {
                    // routing reached destination eNB, proceeding intracell delivery
                    send(datagram_,"pppGate");
                }
                else
                {
                    // continue routing for the next hop
                    forward(datagram_);
                }
            }
        }
    }
}

void GtpUserSimplified::handleFromUdp(GtpUserMsg * gtpMsg)
{
    EV << "GtpUserSimplified::handleFromUdp - Decapsulating datagram from GTP tunnel" << endl;

    // obtain the original IP datagram and send it to the local network
    IPv4Datagram * datagram = check_and_cast<IPv4Datagram*>(gtpMsg->decapsulate());
    delete(gtpMsg);

    if (ownerType_ == PGW)
    {
        IPv4Address& destAddr = datagram->getDestAddress();
        MacNodeId destId = binder_->getMacNodeId(destAddr);
        if (destId != 0)
        {
             // create a new GtpUserSimplifiedMessage
             GtpUserMsg * gtpMsg = new GtpUserMsg();
             gtpMsg->setName("GtpUserMessage");

             // encapsulate the datagram within the GtpUserSimplifiedMessage
             gtpMsg->encapsulate(datagram);

             MacNodeId destMaster = binder_->getNextHop(destId);
             const char* symbolicName = binder_->getModuleNameByMacNodeId(destMaster);
             L3Address tunnelPeerAddress = L3AddressResolver().resolve(symbolicName);
             socket_.sendTo(gtpMsg, tunnelPeerAddress, tunnelPeerPort_);
             EV << "GtpUserSimplified::handleFromUdp - Destination is a MEC server. Sending GTP packet to " << symbolicName << endl;
        }
        else
        {
            // destination is outside the LTE network
            EV << "GtpUserSimplified::handleFromUdp - Deliver datagram to the internet " << endl;
            send(datagram,"pppGate");
        }
    }
    else if (ownerType_ == ENB)
    {
        IPv4Address& destAddr = datagram->getDestAddress();
        MacNodeId destId = binder_->getMacNodeId(destAddr);
        if (destId != 0)
        {
            MacNodeId destMaster = binder_->getNextHop(destId);
            if (destMaster == nodeId_)
            {
                EV << "GtpUserSimplified::handleFromUdp - Deliver datagram to the LTE NIC " << endl;
                send(datagram,"pppGate");
                return;
            }
        }
        // send the message to the correct eNB or to the internet, through the PGW
        // create a new GtpUserSimplifiedMessage
        GtpUserMsg * gtpMsg = new GtpUserMsg();
        gtpMsg->setName("GtpUserMessage");

        // encapsulate the datagram within the GtpUserSimplifiedMessage
        gtpMsg->encapsulate(datagram);

        socket_.sendTo(gtpMsg, pgwAddress_, tunnelPeerPort_);

        EV << "GtpUserSimplified::handleFromUdp - Destination is not served by this eNodeB. Sending GTP packet to the PGW"<< endl;
    }
}

// or receiving DownlinkAirMessage from a vUE owned by this e2NB
void GtpUserSimplified::handleFromApp(IPv4Datagram * datagram)
{
    L3Address dstAddr = datagram->getDestinationAddress();          // true dest address (UE)
    MacNodeId dstID = binder_->getMacNodeId(dstAddr.toIPv4());      // true dest ID (UE)
    MacNodeId dstMasterID = binder_->getNextHop(dstID);             // master ID of dest ID (ENB)

    if (dstMasterID == nodeId_)
    {
        // routing reached destination eNB, proceeding intracell delivery
        send(datagram,"pppGate");
    }
    else
    {
        // continue routing for the next hop
        forward(datagram);
    }
}

LteNodeSubType GtpUserSimplified::getSrcNodeSubType(IPv4Datagram* datagram)
{
    IPv4Address addr = datagram->getSourceAddress().toIPv4();
    MacNodeId nodeId = getBinder()->getMacNodeId(addr);
    OmnetId omnetId  = getBinder()->getOmnetId(nodeId);
    const char* moduleSubType_  = getSimulation()->getModule(omnetId)->par("nodeSubType");
    LteNodeSubType moduleSubType = getNodeSubType(moduleSubType_);
    return moduleSubType;
}

LteNodeSubType GtpUserSimplified::getDstNodeSubType(IPv4Datagram* datagram)
{
    IPv4Address addr = datagram->getDestinationAddress().toIPv4();
    MacNodeId nodeId = getBinder()->getMacNodeId(addr);
    OmnetId omnetId  = getBinder()->getOmnetId(nodeId);
    const char* moduleSubType_  = getSimulation()->getModule(omnetId)->par("nodeSubType");
    LteNodeSubType moduleSubType = getNodeSubType(moduleSubType_);
    return moduleSubType;
}

void GtpUserSimplified::forward(IPv4Datagram* datagram)
{
    adjMap adjMap = vRouter_->createAdjMap();

    L3Address dstAddr_o = datagram->getDestinationAddress();                // true dest address (UE)
    MacNodeId dstID = binder_->getMacNodeId(dstAddr_o.toIPv4());            // true dest ID (UE)
    EV << "DestID of this datagram: " << dstID << endl;

    MacNodeId dstMasterID = binder_->getNextHop(dstID);                     // ID of the master ENB which the UE attaches to
    MacNodeId dstVertex = getAdjIndex(adjMap,dstMasterID);                  // vertex ID of the master ENB which the UE attaches to
    EV << "MasterID of this datagram: " << dstVertex << "(" << dstMasterID << ")" << endl;

    MacNodeId nextHopID = vRouter_->computeRoute(dstMasterID);              // ID of the vUE
    MacNodeId nextVertex = getAdjIndex(adjMap,nextHopID);                   // vertex ID of the vUE
    EV << "Forwarding this datagram to next hop: " << nextVertex;
    const char* nextHopName = binder_->getModuleNameByMacNodeId(nextHopID); // Module name of the vUE
    EV << "(" << nextHopID << " | " << nextHopName << ")" << endl;
    EV << "Next hop is " << nextHopName << " owned by "
    << binder_->getModuleNameByMacNodeId(getOwnerId(nextHopID)) << " that serves "
    << binder_->getModuleNameByMacNodeId(binder_->getNextHop(nextHopID));

    if (!isOwner(nextHopID,nodeId_))                    // if this eNB is the master of next hop vUE aka DOWNLINK
    {
        EV << " i.e DOWNLINK" << endl;
        L3Address dstAddr = L3AddressResolver().resolve(nextHopName);

        // double datagram encapsulate, keep old srcAddr, new dstAddr is the vUE of next hop ENB
        IPv4Datagram *datagram_ = new IPv4Datagram();
        datagram_->setSourceAddress(datagram->getSourceAddress());
        datagram_->setDestinationAddress(dstAddr);
        datagram_->setName("DownlinkAirMessage");
        datagram_->setTransportProtocol(datagram->getTransportProtocol());
        datagram_->setIdentification(datagram->getIdentification());
        datagram_->setTimeToLive(datagram->getTimeToLive());
        datagram_->encapsulate(datagram);

        send(datagram_,"pppGate");
        EV << "Outage mode, downlink direction - Sending packet to stack" << endl;
    }
    else if (isOwner(nextHopID,nodeId_))        // if this eNB owns this next hop vUE aka UPLINK
    {
        EV << " i.e UPLINK" << endl;
        MacNodeId dstID = binder_->getNextHop(nextHopID);                         // master ID of the vUE i.e dest eNB of the UL
        const char* dstName = binder_->getModuleNameByMacNodeId(dstID);

        L3Address srcAddr = L3AddressResolver().resolve(nextHopName);
        L3Address dstAddr = L3AddressResolver().resolve(dstName);

        // double datagram encapsulate, new srcAddr is the vUE of this ENB, new dstAddr is the next hop ENB
        IPv4Datagram *datagram_ = new IPv4Datagram();
        datagram_->setSourceAddress(srcAddr);
        datagram_->setDestinationAddress(dstAddr);
        datagram_->setName("UplinkAirMessage");
        datagram_->setTransportProtocol(datagram->getTransportProtocol());
        datagram_->setIdentification(datagram->getIdentification());
        datagram_->setTimeToLive(datagram->getTimeToLive());
        datagram_->encapsulate(datagram);

        send(datagram_,"appOut");
        EV << "Outage mode, uplink direction - Sending packet to local app" << endl;
    }
}
