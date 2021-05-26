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

import inet.mobility.static.StationaryMobility;
import inet.networklayer.common.InterfaceTable;
import inet.networklayer.contract.IRoutingTable;
import inet.networklayer.contract.INetworkLayer;

import lte.stack.ILteNic;
import lte.virtual.vUeApp;

module vUe
{
    parameters:
        string mode;

        @networkNode();
        @display("i=device/pocketpc;bgb=330.332,507.872");

		//# Mobility
        string mobilityType = default("StationaryMobility");

        //# Node specs
        string nodeType = "UE";  // DO NOT CHANGE
        string nodeSubType = "VUE";
        int masterId;
        int ownerId;
        int macNodeId = default(0); // TODO: this is not a real parameter
        int macCellId = default(0); // TODO: this is not a real parameter

		//# LSA
		double lsaTimer;
        double lsaStart;

        //# D2D capability
        string nicType = default("LteNicUe");

		//# Network Layer specs
        string networkLayerType = default("IPv4NetworkLayer");
        string routingTableType = default("IPv4RoutingTable");
        *.interfaceTableModule = default(absPath(".interfaceTable"));
        *.routingTableModule = default(absPath(".routingTable"));
    gates:
        input radioIn @directIn;     // connection to master
        inout app;
    submodules:
        interfaceTable: InterfaceTable {
            @display("p=55.952,29.052;is=s");
        }
        routingTable: <routingTableType> like IRoutingTable if routingTableType != "" {
            parameters:
                @display("p=55.952,139.88;is=s");
        }
        mobility: StationaryMobility {
            @display("p=55.952,85.004;is=s");
        }
        lteNic: <nicType> like ILteNic {
            nodeType = nodeType;
            //d2dCapable = d2dCapable;
            @display("p=163.552,407.80402");
        }
        vUeApp: vUeApp {
            @display("p=163.552,220.58");
        }
        networkLayer: <networkLayerType> like INetworkLayer {
            parameters:
                @display("p=273.30402,29.052");
        }
    connections allowunconnected:
        vUeApp.extIO++ <--> app;

        vUeApp.intIO$o --> lteNic.upperLayerIn;
        vUeApp.intIO$i <-- lteNic.upperLayerOut;

        lteNic.radioIn <-- radioIn;
}
