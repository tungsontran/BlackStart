#ifndef __LOCAL_APP_H_
#define __LOCAL_APP_H_

#include <omnetpp.h>
#include "inet/networklayer/ipv4/IPv4Datagram.h"
#include "corenetwork/binder/LteBinder.h"
#include "common/LteCommon.h"
#include "epc/gtp_common.h"
#include "virtual/packet/RoutingTableMsg.h"

class vUeApp : public cSimpleModule
{
    LteBinder* binder_;
    std::map<MacNodeId, std::map<MacNodeId, bool> > UeList;

    // specifies the type of the node that contains this filter (it can be ENB or UE)
    LteNodeType ownerType_;
    LteNodeType selectOwnerType(const char * type);


    protected:
        virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual void handleFromInternal(IPv4Datagram *pkt);
        virtual void handleFromExternal(cMessage *pkt);
};

#endif
