
#include "virtual/vUeApp.h"

Define_Module(vUeApp);

void vUeApp::initialize(int stage)
{
    binder_ = getBinder();
    ownerType_ = selectOwnerType(getAncestorPar("nodeType"));
    MacNodeId nodeId_ = getAncestorPar("macNodeId");
    if (stage == INITSTAGE_NETWORK_LAYER)
    {
        if (ownerType_ == UE)
        {
            // register the pair <id,name> to the binder
            const char* moduleName = getParentModule()->getFullName();
            binder_->registerName(nodeId_, moduleName);
        }
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

void vUeApp::handleFromExternal(cMessage *pkt)
{
    // from vUE of this ENB
    if (ownerType_ == ENODEB)
    {
        EV << "vUeApp::handleMessage - message from virtual UE, forwarding to GtpUser" << endl;
    }
    // from ENB of this vUE
    else if (ownerType_ == UE)
    {   //@TODO
        EV << "vUeApp::handleMessage - message from ENB, forwarding to stack" << endl;
    }
    send(pkt,"intIO$o");
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

