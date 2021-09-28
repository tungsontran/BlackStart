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
#include <omnetpp/cenvir.h>

#define INF 0x3f3f3f3f

using namespace std;
using namespace omnetpp;

class virtualRouter;

template<typename T, typename S>
struct Weight
{
    /// Dest node identifier.
    T weight_;
    /// weight value.
    S dest_;

    /// Comparison operator to enable sorting.
    bool operator<(const Weight& y) const
    {
        if (weight_ < y.weight_)
            return true;
        if (weight_ == y.weight_)
            return uniform(getEnvir()->getRNG(0),0,1) < 0.5;
        return false;
    }

    bool operator>(const Weight& y) const
    {
        if (weight_ > y.weight_)
            return true;
        if (weight_ == y.weight_)
            return uniform(getEnvir()->getRNG(0),0,1) < 0.5;
        return false;
    }

  public:
    Weight()
    {

    }
    Weight(const T w, const S d)
    {
        weight_ = w;
        dest_ = d;
    }
};
typedef Weight<double,MacNodeId> weight;
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
    HOP, CQI, ETX, ETT,
    ADD_HOP_ETX, ADD_HOP_ETT, ADD_ETX_HOP, ADD_ETT_HOP,
};

routingMetric getRoutingMetric (const char* weight);
#endif /* VIRTUAL_ALGORITHM_COMMON_H_ */
