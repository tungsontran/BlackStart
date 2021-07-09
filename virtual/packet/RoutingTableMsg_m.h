//
// Generated file, do not edit! Created by nedtool 5.6 from virtual/packet/RoutingTableMsg.msg.
//

#ifndef __ROUTINGTABLEMSG_M_H
#define __ROUTINGTABLEMSG_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "common/LteCommon.h"
#include "inet/networklayer/contract/ipv4/IPv4Address.h"
#include <vector>
typedef unsigned short MacNodeId;
typedef std::map<MacNodeId,std::pair<MacNodeId,std::array<std::pair<double,simtime_t>,4>>> ueEnbCost;
typedef std::pair<MacNodeId,ueEnbCost> virtualRoutingTableEntry;
typedef std::vector<virtualRoutingTableEntry> virtualRoutingTable;
// }}

/**
 * Class generated from <tt>virtual/packet/RoutingTableMsg.msg:29</tt> by nedtool.
 * <pre>
 * packet RoutingTableMsg
 * {
 *     \@customize(true);
 *     MacNodeId sourceId;
 *     MacNodeId destId;
 *     IPv4Address sourceAddr;
 *     IPv4Address destAddr;
 *     virtualRoutingTable table;
 * }
 * </pre>
 *
 * RoutingTableMsg_Base is only useful if it gets subclassed, and RoutingTableMsg is derived from it.
 * The minimum code to be written for RoutingTableMsg is the following:
 *
 * <pre>
 * class RoutingTableMsg : public RoutingTableMsg_Base
 * {
 *   private:
 *     void copy(const RoutingTableMsg& other) { ... }

 *   public:
 *     RoutingTableMsg(const char *name=nullptr, short kind=0) : RoutingTableMsg_Base(name,kind) {}
 *     RoutingTableMsg(const RoutingTableMsg& other) : RoutingTableMsg_Base(other) {copy(other);}
 *     RoutingTableMsg& operator=(const RoutingTableMsg& other) {if (this==&other) return *this; RoutingTableMsg_Base::operator=(other); copy(other); return *this;}
 *     virtual RoutingTableMsg *dup() const override {return new RoutingTableMsg(*this);}
 *     // ADD CODE HERE to redefine and implement pure virtual functions from RoutingTableMsg_Base
 * };
 * </pre>
 *
 * The following should go into a .cc (.cpp) file:
 *
 * <pre>
 * Register_Class(RoutingTableMsg)
 * </pre>
 */
using namespace inet;
class RoutingTableMsg_Base : public ::omnetpp::cPacket
{
  protected:
    MacNodeId sourceId;
    MacNodeId destId;
    IPv4Address sourceAddr;
    IPv4Address destAddr;
    virtualRoutingTable table;

  private:
    void copy(const RoutingTableMsg_Base& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const RoutingTableMsg_Base&);
    // make constructors protected to avoid instantiation
    RoutingTableMsg_Base(const char *name=nullptr, short kind=0);
    RoutingTableMsg_Base(const RoutingTableMsg_Base& other);
    // make assignment operator protected to force the user override it
    RoutingTableMsg_Base& operator=(const RoutingTableMsg_Base& other);

  public:
    virtual ~RoutingTableMsg_Base();
    virtual RoutingTableMsg_Base *dup() const override {throw omnetpp::cRuntimeError("You forgot to manually add a dup() function to class RoutingTableMsg");}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual MacNodeId& getSourceId();
    virtual const MacNodeId& getSourceId() const {return const_cast<RoutingTableMsg_Base*>(this)->getSourceId();}
    virtual void setSourceId(const MacNodeId& sourceId);
    virtual MacNodeId& getDestId();
    virtual const MacNodeId& getDestId() const {return const_cast<RoutingTableMsg_Base*>(this)->getDestId();}
    virtual void setDestId(const MacNodeId& destId);
    virtual IPv4Address& getSourceAddr();
    virtual const IPv4Address& getSourceAddr() const {return const_cast<RoutingTableMsg_Base*>(this)->getSourceAddr();}
    virtual void setSourceAddr(const IPv4Address& sourceAddr);
    virtual IPv4Address& getDestAddr();
    virtual const IPv4Address& getDestAddr() const {return const_cast<RoutingTableMsg_Base*>(this)->getDestAddr();}
    virtual void setDestAddr(const IPv4Address& destAddr);
    virtual virtualRoutingTable& getTable();
    virtual const virtualRoutingTable& getTable() const {return const_cast<RoutingTableMsg_Base*>(this)->getTable();}
    virtual void setTable(const virtualRoutingTable& table);
};


#endif // ifndef __ROUTINGTABLEMSG_M_H

