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

#include "LteMacMonitorHarq.h"

LteMacMonitorHarq::LteMacMonitorHarq() {
    this->txCnt = 0;
    this->rtxCnt = 0;
    etx = 0;

}

LteMacMonitorHarq::~LteMacMonitorHarq() {
    // TODO Auto-generated destructor stub
}

LteMacMonitorHarq::LteMacMonitorHarq(const LteMacMonitorHarq &other) {
    // TODO Auto-generated constructor stub

}

void LteMacMonitorHarq::receiveSignal(cComponent *source, simsignal_t signalID, double d, cObject *details) {
    if (d == 0)
        txCnt++;
    else if (d == 1)
        rtxCnt++;
    etx = (rtxCnt/txCnt) + 1;
    EV << "fuck: " << txCnt << " " << rtxCnt << " " << etx << endl;
}
