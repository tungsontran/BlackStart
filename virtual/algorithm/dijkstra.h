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
#include "virtual/algorithm/common.h"

#ifndef VIRTUAL_ALGORITHM_DIJKSTRA_H_
#define VIRTUAL_ALGORITHM_DIJKSTRA_H_

//class dijkstra {
//public:
//    dijkstra();
//    virtual ~dijkstra();
//    dijkstra(const dijkstra &other);
//
//    void shortestPath(vector<cqiWeight> adj[], int V, int src);
//};

std::vector<MacNodeId> dijkstra(adjMatrix adj, int V, MacNodeId src, adjMap adjmap);
#endif /* VIRTUAL_ALGORITHM_DIJKSTRA_H_ */
