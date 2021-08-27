#ifndef __LOCAL_APP_H_
#define __LOCAL_APP_H_

#include <omnetpp.h>
#include "inet/networklayer/ipv4/IPv4Datagram.h"
#include "corenetwork/binder/LteBinder.h"
#include "common/LteCommon.h"
#include "epc/gtp_common.h"
#include "virtual/virtualRouter.h"
#include "virtual/packet/RoutingTableMsg.h"

class vUeApp : public cSimpleModule
{
    LteBinder* binder_;
    MacNodeId nodeId_;

    // specifies the type of the node that contains this filter (it can be ENB or UE)
    LteNodeType ownerType_;
    LteNodeType selectOwnerType(const char * type);

    /* for LSA in vUE */
    // Router reference
    virtualRouter* vRouter_;
    // enable LSA on UL
    bool enableLSA_;
    // lsa self msg
    cMessage* lsa_;
    // Timer to send LSA again
    double lsaTimer_;
    double lsaTimerWarmup_;
    double warmUpPeriod_;
    // Timer to start sending LSA
    double lsaStart_;

    protected:
        virtual int numInitStages() const { return inet::NUM_INIT_STAGES; }
        virtual void initialize(int stage);
        virtual void handleMessage(cMessage *msg);
        virtual void handleFromInternal(IPv4Datagram *pkt);
        virtual void handleFromExternal(cMessage *msg);
        void sendLSA();

    public:
        virtual ~vUeApp();
};

#endif
