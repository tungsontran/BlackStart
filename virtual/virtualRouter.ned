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

package lte.virtual;

simple virtualRouter
{
    parameters:
        double warmUpPeriod;
        double lsaTimerWarmup;
        double lsaTimer;
        double lsaStart;
        string metric = default("HOP");					// see routingMetric in common.h to know the options
        
    gates:
        input ipIn @labels(RoutingTableMsg/up);
        output ipOut @labels(RoutingTableMsg/down);
}
