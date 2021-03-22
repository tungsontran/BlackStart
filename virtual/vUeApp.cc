
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
    EV << "localApp::selectOwnerType - setting owner type to " << type << endl;
    if(strcmp(type,"ENODEB") == 0)
        return ENODEB;
    if(strcmp(type,"UE") != 0)
        error("localApp::selectOwnerType - unknown owner type [%s]. Aborting...",type);
    return UE;
}

void vUeApp::handleMessage(cMessage *msg)
{
    if (strcmp(msg->getArrivalGate()->getFullName(), "extIO$i") == 0)
    {
        IPv4Datagram * datagram = check_and_cast<IPv4Datagram*>(msg);
        handleFromExternal(datagram);
    }
    else if(strcmp(msg->getArrivalGate()->getFullName(),"intIO$i")==0)
    {
        IPv4Datagram * datagram = check_and_cast<IPv4Datagram*>(msg);
        handleFromInternal(datagram);
    }
}

void vUeApp::handleFromExternal(IPv4Datagram *pkt)
{
    IPv4Datagram * datagram;
    // from vUE of this ENB
    if (ownerType_ == ENODEB)
    {
        datagram = check_and_cast<IPv4Datagram*>(pkt);
        EV << "localApp::handleMessage - message from virtual UE, forwarding to GtpUser" << endl;
    }
    // from ENB of this vUE
    else if (ownerType_ == UE)
    {   //@TODO
        EV << "localApp::handleMessage - message from ENB, forwarding to stack" << endl;
    }
    send(datagram,"intIO$o");
}

void vUeApp::handleFromInternal(IPv4Datagram *pkt)
{
    IPv4Datagram * datagram;
    // from GTP-U of E2NB
    if (ownerType_ == ENODEB)
    {   //@TODO
        IPv4Datagram * datagram = check_and_cast<IPv4Datagram*>(pkt);
        EV << "localApp::handleMessage - message from GtpUser, forwarding to virtual UE" << endl;
    }
    // from stack of vUE
    else if (ownerType_ == UE)
    {
        datagram = check_and_cast<IPv4Datagram*>(pkt->decapsulate());
        delete(pkt);
        EV << "localApp::handleMessage - message from stack, forwarding to ENB" << endl;
    }
    send(datagram,"extIO$o");
}

void vUeApp::sendUeList()
{
    MacNodeId masterId = getAncestorPar("masterId");
    // UeList is now a map contains <this ENB node ID, list of UEs node ID connected to it>
    std::map<MacNodeId, std::map<MacNodeId, bool>> UeList;
    UeList[masterId] = binder_->getDeployedUes(masterId,DL);

//    EV << "Connected UEs to ENB";
//    for (auto t : UeList)
//    {
//        EV << t.first <<endl;
//        std::map<MacNodeId, bool> map2 = t.second;
//        for (auto t2: map2)
//        {
//            EV << t2.first << ", ";
//            const char* moduleName = binder_->getModuleNameByMacNodeId(t2.first);
//            EV << "module: " << moduleName << ", ";
//            L3Address ip = L3AddressResolver().resolve(moduleName);
//            EV << "ip: " << ip << endl;
//        }
//    }
}
