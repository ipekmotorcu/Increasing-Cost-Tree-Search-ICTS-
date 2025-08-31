
#include "Agent.h"
#include "Grid.h"
#include "HighLevel.h"
#include "Position.h"
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include "AStar.h"
using namespace std;
std::vector<Position> findSingleAgentPathAStar(const Agent& agent, const Grid& grid) {
    std::unordered_map<Position, int> gScore;
    unordered_map<Position, Position> cameFrom;

    auto cmp = [&](const Position& a, const Position& b) {
        int fA = gScore[a] + abs(agent.getGoal().x - a.x) + abs(agent.getGoal().y - a.y);
        int fB = gScore[b] + abs(agent.getGoal().x - b.x) + abs(agent.getGoal().y - b.y);
        return fA > fB;
        };

    std::priority_queue<Position, std::vector<Position>, decltype(cmp)> openSet(cmp);
    gScore[agent.getStart()] = 0;
    openSet.push(agent.getStart());

    while (!openSet.empty()) {
        Position current = openSet.top(); openSet.pop();
        if (current == agent.getGoal()) {
            // reconstruct path
            std::vector<Position> path;
            for (Position p = current; p != agent.getStart(); p = cameFrom[p])
                path.push_back(p);
            path.push_back(agent.getStart());
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (auto& neighbor : grid.getNeighbors(current)) {
            int tentative_gScore = gScore[current] + 1;
            if (!gScore.count(neighbor) || tentative_gScore < gScore[neighbor]) {
                gScore[neighbor] = tentative_gScore;
                cameFrom[neighbor] = current;
                openSet.push(neighbor);
            }
        }
    }

    return {}; // yol yok
}
bool hasConflict(const vector<Position>& path1, const vector<Position>& path2) {
    int len = max(path1.size(), path2.size());
    for (int t = 0; t < len; ++t) {
        Position p1 = (t < path1.size()) ? path1[t] : path1.back();//path size'dan büükse sondakini döndür
        Position p2 = (t < path2.size()) ? path2[t] : path2.back();
        if (p1 == p2) return true;
        if (t > 0 && t < min(path1.size(), path2.size()) &&
            path1[t] == path2[t - 1] && path1[t - 1] == path2[t]) return true;
    }
    return false;
}

bool groupsConflict(const vector<int>& g1, const vector<int>& g2,
    const vector<vector<Position>>& paths) {
    for (int i : g1)
        for (int j : g2)
            if (hasConflict(paths[i], paths[j])) return true;
    return false;
}

vector<vector<Position>> findPathICTS(const vector<Agent>& agents, const Grid& grid, int& totalCost, int& expanded, int& ICTNodesNonGoal, int& depth, bool& ICTS) {
    vector<vector<Position>> paths;
    totalCost = -1; expanded = 0; ICTNodesNonGoal = 0; depth = 0;
    bool success = false;
    if (ICTS) {
        success = HighLevel::solve(agents, grid, totalCost, expanded, ICTNodesNonGoal, depth, paths);
    }
    else {
        //*********AStar****************
        success = AStar::solve(agents, grid, totalCost, expanded, paths);
        ICTNodesNonGoal = -1; // ICTS metrikleri yok
        depth = -1;
    }



    if (!success) return {};
    return paths;
}
//yavaş olduğunu düşündüğüm hali
vector<vector<Position>> independenceDetection(const vector<Agent>& agents, const Grid& grid, int& totalCostOut, int& expandedCountOut, int& ICTNodesNonGoal, int& depth, int& maxGroupSize, bool& ICTS) {
    int k = agents.size();
    vector<vector<int>> groups(k);
    for (int i = 0; i < k; ++i) groups[i].push_back(i);

    vector<vector<Position>> paths(k);
    for (int i = 0; i < k; ++i) {
        // auto p = findPathICTS({ agents[i] }, grid,totalCostOut,expandedCountOut,ICTNodesNonGoal,depth);
        paths[i] = findSingleAgentPathAStar(agents[i], grid);
        if (paths[i].empty()) return {};

    }
    maxGroupSize = 1;

    while (true) {
        bool merged = false;
        for (size_t i = 0; i < groups.size(); ++i) {
            for (size_t j = i + 1; j < groups.size(); ++j) {
                if (groupsConflict(groups[i], groups[j], paths)) {
                    vector<int> mergedGroup = groups[i];
                    mergedGroup.insert(mergedGroup.end(), groups[j].begin(), groups[j].end());

                    vector<Agent> mergedAgents;
                    for (int idx : mergedGroup) mergedAgents.push_back(agents[idx]);

                    auto mergedPaths = findPathICTS(mergedAgents, grid, totalCostOut, expandedCountOut, ICTNodesNonGoal, depth, ICTS);
                    if (mergedPaths.empty()) return {};

                    groups.erase(groups.begin() + j);//
                    groups.erase(groups.begin() + i);
                    groups.push_back(mergedGroup);

                    for (int a = 0; a < mergedGroup.size(); ++a)
                        paths[mergedGroup[a]] = mergedPaths[a];
                    maxGroupSize = max(maxGroupSize, (int)mergedGroup.size());
                    merged = true;
                    break;
                }
            }
            if (merged) break;
        }
        if (!merged) break;
    }
    return paths;
}

