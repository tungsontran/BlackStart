/*
 * RoutingTableMsg.h
 *
 *  Created on: Mar 26, 2021
 *      Author: Derp Boiz
 */

#ifndef VIRTUAL_PACKET_ROUTINGTABLEMSG_H_
#define VIRTUAL_PACKET_ROUTINGTABLEMSG_H_

#include "RoutingTableMsg_m.h"
#include "inet/networklayer/contract/ipv4/IPv4Address.h"

using namespace inet;

class RoutingTableMsg: public RoutingTableMsg_Base {

public:
    RoutingTableMsg(const char *name=nullptr, short kind=0) : RoutingTableMsg_Base(name,kind) {}
    RoutingTableMsg(const RoutingTableMsg &other): RoutingTableMsg_Base(other.getName())
    {
        operator=(other);
    }
    RoutingTableMsg& operator=(const RoutingTableMsg &other)
    {
        RoutingTableMsg_Base::operator=(other);
        return *this;
    }
    virtual RoutingTableMsg *dup() const override {return new RoutingTableMsg(*this);}
};

#endif /* VIRTUAL_PACKET_ROUTINGTABLEMSG_H_ */
