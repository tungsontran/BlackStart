//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef STACK_MAC_MONITOR_LTEMACMONITORHARQ_H_
#define STACK_MAC_MONITOR_LTEMACMONITORHARQ_H_

#include <omnetpp.h>

using namespace omnetpp;

class LteMacMonitorHarq : public cListener {
public:
    double txCnt;
    double rtxCnt;
    double etx;
public:
    LteMacMonitorHarq();
    virtual ~LteMacMonitorHarq();
    LteMacMonitorHarq(const LteMacMonitorHarq &other);
    virtual void receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details);
    double getEtx(){return etx;}

};

#endif /* STACK_MAC_MONITOR_LTEMACMONITORHARQ_H_ */
