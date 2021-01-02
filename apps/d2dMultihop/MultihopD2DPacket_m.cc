//
// Generated file, do not edit! Created by nedtool 5.6 from apps/d2dMultihop/MultihopD2DPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include "MultihopD2DPacket_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i=0; i<n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp


// forward
template<typename T, typename A>
std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec);

// Template rule which fires if a struct or class doesn't have operator<<
template<typename T>
inline std::ostream& operator<<(std::ostream& out,const T&) {return out;}

// operator<< for std::vector<T>
template<typename T, typename A>
inline std::ostream& operator<<(std::ostream& out, const std::vector<T,A>& vec)
{
    out.put('{');
    for(typename std::vector<T,A>::const_iterator it = vec.begin(); it != vec.end(); ++it)
    {
        if (it != vec.begin()) {
            out.put(','); out.put(' ');
        }
        out << *it;
    }
    out.put('}');
    
    char buf[32];
    sprintf(buf, " (size=%u)", (unsigned int)vec.size());
    out.write(buf, strlen(buf));
    return out;
}

Register_Class(MultihopD2DPacket)

MultihopD2DPacket::MultihopD2DPacket(const char *name, short kind) : ::omnetpp::cPacket(name,kind)
{
    this->srcId = 0;
    this->msgid = 0;
    this->size = 0;
    this->maxRadius = 0;
    this->ttl = 0;
    this->hops = 0;
    this->lastHopSenderId = 0;
    this->timestamp = 0;
}

MultihopD2DPacket::MultihopD2DPacket(const MultihopD2DPacket& other) : ::omnetpp::cPacket(other)
{
    copy(other);
}

MultihopD2DPacket::~MultihopD2DPacket()
{
}

MultihopD2DPacket& MultihopD2DPacket::operator=(const MultihopD2DPacket& other)
{
    if (this==&other) return *this;
    ::omnetpp::cPacket::operator=(other);
    copy(other);
    return *this;
}

void MultihopD2DPacket::copy(const MultihopD2DPacket& other)
{
    this->srcId = other.srcId;
    this->msgid = other.msgid;
    this->size = other.size;
    this->srcCoord = other.srcCoord;
    this->maxRadius = other.maxRadius;
    this->ttl = other.ttl;
    this->hops = other.hops;
    this->lastHopSenderId = other.lastHopSenderId;
    this->timestamp = other.timestamp;
}

void MultihopD2DPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cPacket::parsimPack(b);
    doParsimPacking(b,this->srcId);
    doParsimPacking(b,this->msgid);
    doParsimPacking(b,this->size);
    doParsimPacking(b,this->srcCoord);
    doParsimPacking(b,this->maxRadius);
    doParsimPacking(b,this->ttl);
    doParsimPacking(b,this->hops);
    doParsimPacking(b,this->lastHopSenderId);
    doParsimPacking(b,this->timestamp);
}

void MultihopD2DPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cPacket::parsimUnpack(b);
    doParsimUnpacking(b,this->srcId);
    doParsimUnpacking(b,this->msgid);
    doParsimUnpacking(b,this->size);
    doParsimUnpacking(b,this->srcCoord);
    doParsimUnpacking(b,this->maxRadius);
    doParsimUnpacking(b,this->ttl);
    doParsimUnpacking(b,this->hops);
    doParsimUnpacking(b,this->lastHopSenderId);
    doParsimUnpacking(b,this->timestamp);
}

unsigned int MultihopD2DPacket::getSrcId() const
{
    return this->srcId;
}

void MultihopD2DPacket::setSrcId(unsigned int srcId)
{
    this->srcId = srcId;
}

uint32_t MultihopD2DPacket::getMsgid() const
{
    return this->msgid;
}

void MultihopD2DPacket::setMsgid(uint32_t msgid)
{
    this->msgid = msgid;
}

unsigned int MultihopD2DPacket::getSize() const
{
    return this->size;
}

void MultihopD2DPacket::setSize(unsigned int size)
{
    this->size = size;
}

inet::Coord& MultihopD2DPacket::getSrcCoord()
{
    return this->srcCoord;
}

void MultihopD2DPacket::setSrcCoord(const inet::Coord& srcCoord)
{
    this->srcCoord = srcCoord;
}

double MultihopD2DPacket::getMaxRadius() const
{
    return this->maxRadius;
}

void MultihopD2DPacket::setMaxRadius(double maxRadius)
{
    this->maxRadius = maxRadius;
}

int MultihopD2DPacket::getTtl() const
{
    return this->ttl;
}

void MultihopD2DPacket::setTtl(int ttl)
{
    this->ttl = ttl;
}

unsigned int MultihopD2DPacket::getHops() const
{
    return this->hops;
}

void MultihopD2DPacket::setHops(unsigned int hops)
{
    this->hops = hops;
}

unsigned int MultihopD2DPacket::getLastHopSenderId() const
{
    return this->lastHopSenderId;
}

void MultihopD2DPacket::setLastHopSenderId(unsigned int lastHopSenderId)
{
    this->lastHopSenderId = lastHopSenderId;
}

::omnetpp::simtime_t MultihopD2DPacket::getTimestamp() const
{
    return this->timestamp;
}

void MultihopD2DPacket::setTimestamp(::omnetpp::simtime_t timestamp)
{
    this->timestamp = timestamp;
}

class MultihopD2DPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertynames;
  public:
    MultihopD2DPacketDescriptor();
    virtual ~MultihopD2DPacketDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual const char *getFieldDynamicTypeString(void *object, int field, int i) const override;
    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

Register_ClassDescriptor(MultihopD2DPacketDescriptor)

MultihopD2DPacketDescriptor::MultihopD2DPacketDescriptor() : omnetpp::cClassDescriptor("MultihopD2DPacket", "omnetpp::cPacket")
{
    propertynames = nullptr;
}

MultihopD2DPacketDescriptor::~MultihopD2DPacketDescriptor()
{
    delete[] propertynames;
}

bool MultihopD2DPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<MultihopD2DPacket *>(obj)!=nullptr;
}

const char **MultihopD2DPacketDescriptor::getPropertyNames() const
{
    if (!propertynames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
        const char **basenames = basedesc ? basedesc->getPropertyNames() : nullptr;
        propertynames = mergeLists(basenames, names);
    }
    return propertynames;
}

const char *MultihopD2DPacketDescriptor::getProperty(const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? basedesc->getProperty(propertyname) : nullptr;
}

int MultihopD2DPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    return basedesc ? 9+basedesc->getFieldCount() : 9;
}

unsigned int MultihopD2DPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeFlags(field);
        field -= basedesc->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISCOMPOUND,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
        FD_ISEDITABLE,
    };
    return (field>=0 && field<9) ? fieldTypeFlags[field] : 0;
}

const char *MultihopD2DPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldName(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldNames[] = {
        "srcId",
        "msgid",
        "size",
        "srcCoord",
        "maxRadius",
        "ttl",
        "hops",
        "lastHopSenderId",
        "timestamp",
    };
    return (field>=0 && field<9) ? fieldNames[field] : nullptr;
}

int MultihopD2DPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    int base = basedesc ? basedesc->getFieldCount() : 0;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcId")==0) return base+0;
    if (fieldName[0]=='m' && strcmp(fieldName, "msgid")==0) return base+1;
    if (fieldName[0]=='s' && strcmp(fieldName, "size")==0) return base+2;
    if (fieldName[0]=='s' && strcmp(fieldName, "srcCoord")==0) return base+3;
    if (fieldName[0]=='m' && strcmp(fieldName, "maxRadius")==0) return base+4;
    if (fieldName[0]=='t' && strcmp(fieldName, "ttl")==0) return base+5;
    if (fieldName[0]=='h' && strcmp(fieldName, "hops")==0) return base+6;
    if (fieldName[0]=='l' && strcmp(fieldName, "lastHopSenderId")==0) return base+7;
    if (fieldName[0]=='t' && strcmp(fieldName, "timestamp")==0) return base+8;
    return basedesc ? basedesc->findField(fieldName) : -1;
}

const char *MultihopD2DPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldTypeString(field);
        field -= basedesc->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "unsigned int",
        "uint32_t",
        "unsigned int",
        "inet::Coord",
        "double",
        "int",
        "unsigned int",
        "unsigned int",
        "simtime_t",
    };
    return (field>=0 && field<9) ? fieldTypeStrings[field] : nullptr;
}

const char **MultihopD2DPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldPropertyNames(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *MultihopD2DPacketDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldProperty(field, propertyname);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int MultihopD2DPacketDescriptor::getFieldArraySize(void *object, int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldArraySize(object, field);
        field -= basedesc->getFieldCount();
    }
    MultihopD2DPacket *pp = (MultihopD2DPacket *)object; (void)pp;
    switch (field) {
        default: return 0;
    }
}

const char *MultihopD2DPacketDescriptor::getFieldDynamicTypeString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldDynamicTypeString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MultihopD2DPacket *pp = (MultihopD2DPacket *)object; (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string MultihopD2DPacketDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldValueAsString(object,field,i);
        field -= basedesc->getFieldCount();
    }
    MultihopD2DPacket *pp = (MultihopD2DPacket *)object; (void)pp;
    switch (field) {
        case 0: return ulong2string(pp->getSrcId());
        case 1: return ulong2string(pp->getMsgid());
        case 2: return ulong2string(pp->getSize());
        case 3: {std::stringstream out; out << pp->getSrcCoord(); return out.str();}
        case 4: return double2string(pp->getMaxRadius());
        case 5: return long2string(pp->getTtl());
        case 6: return ulong2string(pp->getHops());
        case 7: return ulong2string(pp->getLastHopSenderId());
        case 8: return simtime2string(pp->getTimestamp());
        default: return "";
    }
}

bool MultihopD2DPacketDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->setFieldValueAsString(object,field,i,value);
        field -= basedesc->getFieldCount();
    }
    MultihopD2DPacket *pp = (MultihopD2DPacket *)object; (void)pp;
    switch (field) {
        case 0: pp->setSrcId(string2ulong(value)); return true;
        case 1: pp->setMsgid(string2ulong(value)); return true;
        case 2: pp->setSize(string2ulong(value)); return true;
        case 4: pp->setMaxRadius(string2double(value)); return true;
        case 5: pp->setTtl(string2long(value)); return true;
        case 6: pp->setHops(string2ulong(value)); return true;
        case 7: pp->setLastHopSenderId(string2ulong(value)); return true;
        case 8: pp->setTimestamp(string2simtime(value)); return true;
        default: return false;
    }
}

const char *MultihopD2DPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructName(field);
        field -= basedesc->getFieldCount();
    }
    switch (field) {
        case 3: return omnetpp::opp_typename(typeid(inet::Coord));
        default: return nullptr;
    };
}

void *MultihopD2DPacketDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    omnetpp::cClassDescriptor *basedesc = getBaseClassDescriptor();
    if (basedesc) {
        if (field < basedesc->getFieldCount())
            return basedesc->getFieldStructValuePointer(object, field, i);
        field -= basedesc->getFieldCount();
    }
    MultihopD2DPacket *pp = (MultihopD2DPacket *)object; (void)pp;
    switch (field) {
        case 3: return (void *)(&pp->getSrcCoord()); break;
        default: return nullptr;
    }
}


