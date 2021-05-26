
#include "virtual/vUeApp.h"

Define_Module(vUeApp);

void vUeApp::initialize(int stage)
{
    binder_ = getBinder();
    ownerType_ = selectOwnerType(getAncestorPar("nodeType"));
    nodeId_ = getAncestorPar("macNodeId");
    if (stage == INITSTAGE_NETWORK_LAYER)
    {
        if (ownerType_ == UE)
        {
            // register the pair <id,name> to the binder
            const char* moduleName = getParentModule()->getFullName();
            binder_->registerName(nodeId_, moduleName);
            vRouter_ = check_and_cast<virtualRouter*>(getSimulation()->getModule(binder_->getOmnetId(getOwnerId(nodeId_)))->getSubmodule("virtualRouter"));
        }
        else if (ownerType_ == ENODEB)
        {
            vRouter_ = check_and_cast<virtualRouter*>(getModuleByPath("^.virtualRouter"));
        }
    }

    if (stage == INITSTAGE_LOCAL && ownerType_ == UE)
    {
        lsa_ = nullptr;
        lsaTimer_ = getAncestorPar("lsaTimer");
        lsaStart_ = getAncestorPar("lsaStart");
        lsa_ = new cMessage("linkStateAdvertisement");
        lsa_->setSchedulingPriority(1);
        scheduleAt(NOW + lsaStart_, lsa_);
    }
}

LteNodeType vUeApp::selectOwnerType(const char * type)
{
    EV << "vUeApp::selectOwnerType - setting owner type to " << type << endl;
    if(strcmp(type,"ENODEB") == 0)
        return ENODEB;
    if(strcmp(type,"UE") != 0)
        error("vUeApp::selectOwnerType - unknown owner type [%s]. Aborting...",type);
    return UE;
}

void vUeApp::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        sendLSA();
        EV << "E2NB " << nodeId_ << " is sending Network Topo Table" << endl;
        scheduleAt(NOW + lsaTimer_, msg);
        return;
    }

    if (strstr(msg->getArrivalGate()->getFullName(), "extIO$i") != nullptr)
    {
        handleFromExternal(msg);
    }
    else if(strcmp(msg->getArrivalGate()->getFullName(),"intIO$i")==0)
    {
        IPv4Datagram * datagram = check_and_cast<IPv4Datagram*>(msg);
        handleFromInternal(datagram);
    }
}

void vUeApp::handleFromExternal(cMessage *msg)
{
    // from vUE of this ENB
    if (ownerType_ == ENODEB)
    {
        EV << "vUeApp::handleMessage - message from virtual UE, forwarding to GtpUser" << endl;
    }
    // from ENB of this vUE
    else if (ownerType_ == UE)
    {
        EV << "vUeApp::handleMessage - message from ENB, forwarding to stack" << endl;
    }
    send(msg,"intIO$o");
}

void vUeApp::handleFromInternal(IPv4Datagram *pkt)
{
    L3Address srcAddr = pkt->getSourceAddress();
    cPacket* msg = nullptr;
    // from stack of vUE
    if (ownerType_ == UE)
    {
        if (!strcmp(pkt->getName(),"LSA_HELLO"))
        {
            msg = check_and_cast<RoutingTableMsg*>(pkt->decapsulate());
            delete(pkt);
            EV << "vUeApp::handleMessage - message from stack, forwarding to ENB" << endl;
        }
        else
        {
            msg = check_and_cast<IPv4Datagram*>(pkt->decapsulate());
            delete(pkt);
            EV << "vUeApp::handleMessage - message from stack, forwarding to ENB" << endl;
        }
        send(msg,"extIO$o",0);
    }
    // from GTP-U of E2NB
    else if (ownerType_ == ENODEB)
    {
        std::string vUEname = binder_->getModuleNameByMacNodeId(binder_->getMacNodeId(srcAddr.toIPv4()));
        int vUeindex = vUEname.at(vUEname.length()-2)-'0';
        EV << "vUeApp::handleMessage - message from GtpUser, forwarding to virtual UE" << endl;
        send(pkt,"extIO$o",vUeindex);
    }
}

void vUeApp::sendLSA()
{
    // LSA on UL
    if (ownerType_ == UE)
    {
        Enter_Method("sendLSA");
        ueEnbCqi::iterator it;
        virtualRoutingTableEntry directNeighbors = vRouter_->getDirectNeighbors();
        virtualRoutingTable networkTopoTable = vRouter_->getNetworkTopoTable();
        for (auto it: directNeighbors.second)
        {
            RoutingTableMsg* msg = new RoutingTableMsg();

            MacNodeId srcId = nodeId_;         // ID of vUE
            msg->setSourceId(srcId);
            L3Address srcAddr = binder_->getL3Address(srcId);
            msg->setSourceAddr(srcAddr.toIPv4());

            msg->setTable(networkTopoTable);
            msg->setName("LSA_HELLO");
            msg->setByteLength(40); //@TODO table size?

            MacNodeId dstId = it.second.first; // ID of peering eNB
            msg->setDestId(dstId);
            L3Address dstAddr = binder_->getL3Address(dstId);
            msg->setDestAddr(dstAddr.toIPv4());

            IPv4Datagram *datagram = new IPv4Datagram();
            datagram->setSourceAddress(srcAddr);
            datagram->setDestinationAddress(dstAddr);
            datagram->setTransportProtocol(IP_PROT_NONE);
            datagram->setName("LSA_HELLO");
            datagram->encapsulate(msg);

            send(datagram,"intIO$o");
        }
    }
}

vUeApp::~vUeApp()
{
//    cancelAndDelete(msg);
}
