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

    if (ownerType_ != PGW)
        subType_ = getNodeSubType(getAncestorPar("nodeSubType"));

    if (subType_ == E2NODEB)
    {
        // get mode
        mode_ = getE2NodeBMode(getParentModule()->par("mode").stringValue());

        // get direction vector
        const char* directions = getParentModule()->par("direction").stringValue();
        cStringTokenizer tokenizer(directions);
        while (tokenizer.hasMoreTokens())
        {
            e2NodeBDirection direction = getE2NodeBDirection(tokenizer.nextToken());
            directions_.push_back(direction);
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
        EV << "GtpUserSimplified::handleMessage - message from virtual UE" << endl;

        IPv4Datagram * datagram = check_and_cast<IPv4Datagram*>(msg);
        send(datagram,"pppGate");
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
            // @TODO: make direction determined by datagram dest id

            if (directions_.at(0) == DOWNLINK)
            {
                // if datagram comes from an UE
                if (moduleSubType == NONE)
                {
                    // test with vUe2_1
                    const char* symbolicName = binder_->getModuleNameByMacNodeId(1026); // @TODO
                    const L3Address dstAddr = L3AddressResolver().resolve(symbolicName);

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
                // if datagram comes from a vUE
                else if (moduleSubType == VUE)
                {
                    IPv4Datagram *datagram_ = check_and_cast<IPv4Datagram*>(datagram->decapsulate());
                    delete(datagram);
                    send(datagram_,"pppGate");
                }
            }
            else if (directions_.at(0) == UPLINK)
            {
                // if datagram comes from an UE
                if (moduleSubType == NONE)
                {
                    // test with vUe1_1
                    const char* symbolicName = binder_->getModuleNameByMacNodeId(2); // @TODO
                    const char* symbolicName2 = binder_->getModuleNameByMacNodeId(1025); // @TODO
                    const L3Address dstAddr = L3AddressResolver().resolve(symbolicName);
                    const L3Address srcAddr = L3AddressResolver().resolve(symbolicName2);

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
                // if datagram comes from a vUE
                else if (moduleSubType == VUE)
                {
                    IPv4Datagram *datagram_ = check_and_cast<IPv4Datagram*>(datagram->decapsulate());
                    delete(datagram);
                    send(datagram_,"pppGate");
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
            MacNodeId enbId = getAncestorPar("macNodeId");
            MacNodeId destMaster = binder_->getNextHop(destId);
            if (destMaster == enbId)
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

LteNodeSubType GtpUserSimplified::getSrcNodeSubType(IPv4Datagram* datagram)
{
    IPv4Address addr = datagram->getSourceAddress().toIPv4();
    MacNodeId nodeId = binder_->getMacNodeId(addr);
    OmnetId omnetId  = binder_->getOmnetId(nodeId);
    const char* moduleSubType_  = getSimulation()->getModule(omnetId)->par("nodeSubType");
    LteNodeSubType moduleSubType = getNodeSubType(moduleSubType_);
    return moduleSubType;
}
