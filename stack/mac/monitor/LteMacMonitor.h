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

#ifndef __SIMULTE_1_1_0_LTEMACMONITOR_H_
#define __SIMULTE_1_1_0_LTEMACMONITOR_H_

#include <omnetpp.h>
#include "LteMacMonitorHarq.h"
#include "stack/mac/layer/LteMacBase.h"
#include "stack/mac/layer/LteMacUe.h"
#include "stack/mac/layer/LteMacEnb.h"

using namespace omnetpp;

/**
 * TODO - Generated class
 */
class LteMacMonitor : public cSimpleModule
{
  protected:
    LteMacBase* mac_;
    LteMacMonitorHarq* harqMonitorUL_;
    LteMacMonitorHarq* harqMonitorDL_;

  public:
    virtual void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg);
    LteMacMonitor();
    virtual ~LteMacMonitor();
};

#endif
