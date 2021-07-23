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

#include "dijkstra.h"

template<typename T>
void print_queue(T q) { // NB: pass by value so the print uses a copy
    EV << "pq" << endl;
    while(!q.empty()) {
        EV << q.top().first << " " << q.top().second << endl;
        q.pop();
    }
}

// Prints shortest paths from src to all other vertices
std::vector<MacNodeId> dijkstra(adjMatrix adj, int V, MacNodeId src, adjMap adjmap)
{
    // create queue of ascending order
    std::priority_queue<weight, std::vector<weight> , std::greater<weight>> pq;

    // Create a vector for distances and initialize all distances as infinite
    std::vector<double> dist(V, INF);
    dist[src] = 0;

    // Parent array to store shortest path tree
    std::vector<MacNodeId> parent(V);
    parent[src] = -1;

    // Insert source itself in priority queue and initialize its distance as 0.
    pq.push(std::make_pair(0, src));

    // Looping till priority queue becomes empty (or all distances are not finalized)
    while (!pq.empty())
    {
        // The first vertex in pair is the minimum distance vertex, extract it from priority queue.
        // vertex label is stored in second of pair (it has to be done this way to keep the vertices
        // sorted distance (distance must be first item in pair)
        int u = pq.top().second;

        pq.pop();

        // Get all adjacent of u.
        for (auto x : adj[u])
        {
            // Get vertex label and weight of current adjacent of u.
            MacNodeId v = x.first;
            double weight = x.second;
            // If there is shorter path to v through u.
            if (dist[v] > dist[u] + weight)
            {
                // Updating distance of v
                dist[v] = dist[u] + weight;
                pq.push(std::make_pair(dist[v], v));
                parent[v] = u;
            }
        }
    }
    printSolution(dist, V, parent, src);
    return parent;
}
