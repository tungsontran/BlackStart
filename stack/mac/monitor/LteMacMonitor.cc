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

#include "LteMacMonitor.h"

Define_Module(LteMacMonitor);

LteMacMonitor::LteMacMonitor()
{
    mac_ = nullptr;
    harqMonitorUL_ = nullptr;
    harqMonitorDL_ = nullptr;
}

LteMacMonitor::~LteMacMonitor()
{
    unsubscribe("harqErrorRateUl",harqMonitorUL_);
    unsubscribe("harqErrorRateDl",harqMonitorDL_);
    delete mac_;
    delete harqMonitorUL_;
    delete harqMonitorDL_;
}

void LteMacMonitor::initialize(int stage)
{
    if (stage == inet::INITSTAGE_LAST)
    {
        mac_ = check_and_cast<LteMacBase*>(getParentModule()->getSubmodule("mac"));
        harqMonitorUL_ = new LteMacMonitorHarq();
        harqMonitorDL_ = new LteMacMonitorHarq();
        subscribe("harqErrorRateUl",harqMonitorUL_);
        subscribe("harqErrorRateDl",harqMonitorDL_);
    }
}

void LteMacMonitor::handleMessage(cMessage *msg)
{

}


