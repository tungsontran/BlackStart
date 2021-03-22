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

#include "virtual/virtualRouter.h"

Define_Module(virtualRouter);

void virtualRouter::initialize()
{
    nodeId_ = getParentModule()->par("macNodeId");
}

void virtualRouter::setVirtualNetInfo(const ueCqi uecqi)
{
    ASSERT(&uecqi != nullptr);
    info_.first = nodeId_;
    info_.second = uecqi;
}

void virtualRouter::addToVirtualRoutingTable(const virtualNetInfo info)
{
    MacNodeId id = info.first;
    virtualRoutingTable::iterator it = table_.find(id);         // check if info of an ENB is already in table
    if (it == table_.end())                                     // not found, insert new entry
        table_.insert(info);
    else                                                        // if found, update all cqi entries for the corresponding ENB
    {
        table_[id] = info.second;
    }
}

virtualNetInfo virtualRouter::getVirtualNetInfo() const
{
    return info_;
}

virtualRoutingTable virtualRouter::getVirtualRoutingTable() const
{
    return table_;
}

void virtualRouter::printInfo() const
{
    EV << "virtualRouter::printInfo(): This ENB " << info_.first << " is connecting to the following vUEs:" << endl;
    ueCqi::iterator it;
//    for (it == info_.second.begin(); it != info_.second.end(); it++)
    for (auto it: info_.second)
    {
        EV << "virtualRouter::printInfo(): UE " << it.first << "(CQI: " << it.second << ")" << endl;
    }
}

void virtualRouter::printTable() const
{
    EV << "virtualRouter::printTable(): ***** Virtual Routing Table *****" << endl;
    virtualRoutingTable::iterator tableEntry;
    for (tableEntry == table_.begin(); tableEntry != table_.end(); tableEntry++)
    {
        EV << "virtualRouter::printTable(): ENB " << tableEntry->first << endl;
        ueCqi::iterator uecqi;
        for (uecqi == tableEntry->second.begin(); uecqi != tableEntry->second.end(); uecqi++)
        {
            EV << "virtualRouter::printTable(): UE " << uecqi->first << ", CQI = " << uecqi->second << endl;
        } //@TODO direct master
    }
}
