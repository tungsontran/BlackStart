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

#ifndef __EVALUATION_FOR_THE_ICT_SYSTEM_RESTORATION_METHODS_VIRTUALROUTER_H_
#define __EVALUATION_FOR_THE_ICT_SYSTEM_RESTORATION_METHODS_VIRTUALROUTER_H_

#include <omnetpp.h>
#include "common/LteCommon.h"

using namespace omnetpp;

class virtualRouter : public cSimpleModule
{
  protected:
    virtualNetInfo info_;
    MacNodeId nodeId_;
    virtualRoutingTable table_;
    virtual void initialize();
  public:
    void setVirtualNetInfo(const ueCqi uecqi);
    virtualNetInfo getVirtualNetInfo() const;
    void addToVirtualRoutingTable(const virtualNetInfo info);
    virtualRoutingTable getVirtualRoutingTable() const;
    void printInfo() const;
    void printTable() const;
};

#endif
