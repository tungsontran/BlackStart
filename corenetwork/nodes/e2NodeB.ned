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

package lte.corenetwork.nodes;

import lte.corenetwork.nodes.eNodeB;
import lte.virtual.virtualRouter;
import lte.virtual.vUeApp;

module e2NodeB extends eNodeB
{
    parameters:
        int numvUe = default(6);
        string mode = default("NORMAL");
        string direction = default("DOWNLINK DOWNLINK DOWNLINK DOWNLINK DOWNLINK DOWNLINK");
        nodeSubType = "E2NODEB";
    gates:
        inout app[];
    submodules:
        virtualRouter: virtualRouter {
            @display("p=50,41.538464;i=block/cogwheel_s");
        }
        vUeApp[numvUe]: vUeApp {
            @display("p=682.5,268.8");
        }
    connections allowunconnected:
        for i=0..numvUe-1 {
            vUeApp[i].intIO$o --> gtpUser.appIn;
            vUeApp[i].intIO$i <-- gtpUser.appOut;
        }

        for i=0..sizeof(app)-1 {
            vUeApp[i].extIO <--> app++;
        }
}
