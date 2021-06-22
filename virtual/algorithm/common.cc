#include "common.h"

MacNodeId getAdjIndex(adjMap v, MacNodeId K)
{
    MacNodeId index;
    auto it = std::find(v.begin(), v.end(), K);
    if (it != v.end())
    {
        index = it - v.begin();
//        EV << "getAdjIndex: given nodeID " << K << ", got vertexID " << index << endl;
    }
    else
        throw cRuntimeError("getAdjIndex: nodeID is non-existent!");
    return index;
}

adjMap getAdjMap(MacNodeId enbCount, MacNodeId vueCount)
{
    adjMap result;
    for (auto i = ENB_MIN_ID; i < enbCount; i++)
        result.push_back(i);
    for (auto i = VUE_MIN_ID; i < vueCount; i++)
        result.push_back(i);
    return result;
}

void addEdgeAsymmetric(adjMatrix& adj, MacNodeId u, MacNodeId v, double wU, double wD)
{
    adj[u].push_back(std::make_pair(v, wD));
    adj[v].push_back(std::make_pair(u, wU));
}

void addEdgeSymmetric(adjMatrix& adj, MacNodeId u, MacNodeId v, double wt)
{
    adj[u].push_back(std::make_pair(v, wt));
    adj[v].push_back(std::make_pair(u, wt));
}

routingMetric getRoutingMetric (const char* metric)
{
    if (!strcmp(metric,"CQI"))
        return CQI;
    else if (!strcmp(metric,"ETX"))
        return ETX;
    else if (!strcmp(metric,"ETT"))
            return ETT;
    return HOP;
}

void printPath(std::vector<MacNodeId>& parent, MacNodeId j)
{
    // Base Case : If j is source
    if (parent[j] == (MacNodeId)-1)
        return;
    printPath(parent, parent[j]);
    EV << j << " ";
}

void printSolution(std::vector<double> dist, int V, std::vector<MacNodeId> parent, MacNodeId src)
{
    EV << "Vertex\t Distance\t Path" << endl;
    for (int i = 0; i < V; i++)
    {
        EV << src << " -> " << i <<"\t\t" << dist[i] << "\t\t" << src << " ";
        printPath(parent, i);
        EV << endl;
    }
}

MacNodeId getNextHop(std::vector<MacNodeId> parent, MacNodeId src, MacNodeId dst)
{
    MacNodeId nextHop = parent[dst];
    while (parent[nextHop] != src && nextHop != src)
        nextHop = getNextHop(parent, src, nextHop);
    return nextHop;
}
