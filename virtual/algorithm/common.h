/*
 * common.h
 *
 *  Created on: Apr 13, 2021
 *      Author: Tung Tran
 */

#ifndef VIRTUAL_ALGORITHM_COMMON_H_
#define VIRTUAL_ALGORITHM_COMMON_H_

#include <iostream>
#include <algorithm>
#include "common/LteCommon.h"

#define INF 0x3f3f3f3f

using namespace std;
using namespace omnetpp;

class virtualRouter;

typedef std::pair<MacNodeId,double> weight;
typedef std::vector<MacNodeId> adjMap;
typedef std::vector<std::vector<weight>> adjMatrix;

// given a nodeID, return the vertex ID
MacNodeId getAdjIndex(adjMap v, MacNodeId K);
// get a vector of nodeIDs, the indices of this vector are the vertex IDs
adjMap getAdjMap(MacNodeId enbCount, MacNodeId vueCount);
// add an symmetric edge to the graph
void addEdgeSymmetric(adjMatrix& adj, MacNodeId u, MacNodeId v, double wt);
// add an asymmetric edge to the graph
void addEdgeAsymmetric(adjMatrix& adj, MacNodeId u, MacNodeId v, double wU, double wD);
// Function to print shortest path from source to j using parent array
void printPath(std::vector<MacNodeId>& parent, MacNodeId j);
// A utility function to print the constructed distance array
void printSolution(std::vector<double> dist, int V, std::vector<MacNodeId> parent, MacNodeId src);
// Get next hop from the shortest route
MacNodeId getNextHop(std::vector<MacNodeId> parent, MacNodeId src, MacNodeId dst);

enum routingMetric {
    HOP, CQI, ETX, ETT
};

routingMetric getRoutingMetric (const char* weight);
#endif /* VIRTUAL_ALGORITHM_COMMON_H_ */
