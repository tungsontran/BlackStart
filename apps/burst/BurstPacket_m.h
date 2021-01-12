//
// Generated file, do not edit! Created by nedtool 5.6 from apps/burst/BurstPacket.msg.
//

#ifndef __INET_BURSTPACKET_M_H
#define __INET_BURSTPACKET_M_H

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0506
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif


namespace inet {

/**
 * Class generated from <tt>apps/burst/BurstPacket.msg:16</tt> by nedtool.
 * <pre>
 * //
 * // Packet used by the ~BurstSender and ~BurstReceiver modules.
 * //
 * packet BurstPacket
 * {
 *     unsigned int msgId;
 *     simtime_t timestamp;
 *     simtime_t arrivalTime;
 *     unsigned int size;
 * }
 * </pre>
 */
class BurstPacket : public ::omnetpp::cPacket
{
  protected:
    unsigned int msgId;
    ::omnetpp::simtime_t timestamp;
    ::omnetpp::simtime_t arrivalTime;
    unsigned int size;

  private:
    void copy(const BurstPacket& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const BurstPacket&);

  public:
    BurstPacket(const char *name=nullptr, short kind=0);
    BurstPacket(const BurstPacket& other);
    virtual ~BurstPacket();
    BurstPacket& operator=(const BurstPacket& other);
    virtual BurstPacket *dup() const override {return new BurstPacket(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual unsigned int getMsgId() const;
    virtual void setMsgId(unsigned int msgId);
    virtual ::omnetpp::simtime_t getTimestamp() const;
    virtual void setTimestamp(::omnetpp::simtime_t timestamp);
    virtual ::omnetpp::simtime_t getArrivalTime() const;
    virtual void setArrivalTime(::omnetpp::simtime_t arrivalTime);
    virtual unsigned int getSize() const;
    virtual void setSize(unsigned int size);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const BurstPacket& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, BurstPacket& obj) {obj.parsimUnpack(b);}

} // namespace inet

#endif // ifndef __INET_BURSTPACKET_M_H
