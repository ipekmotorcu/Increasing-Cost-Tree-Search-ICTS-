// id.cpp
#include "Agent.h"
#include "Grid.h"
#include "HighLevel.h"
#include "Position.h"
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <iostream>

using namespace std;

bool hasConflict(const vector<Position>& path1, const vector<Position>& path2) {
    int len = max(path1.size(), path2.size());
    for (int t = 0; t < len; ++t) {
        Position p1 = (t < path1.size()) ? path1[t] : path1.back();
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

vector<vector<Position>> findPathICTS(const vector<Agent>& agents, const Grid& grid) {
    vector<vector<Position>> paths;
    int totalCost = -1, expanded = 0, ICTNodesNonGoal = 0, depth = 0;

    bool success = HighLevel::solve(agents, grid, totalCost, expanded, ICTNodesNonGoal, depth, paths);
    if (!success) return {};
    return paths;
}

vector<vector<Position>> independenceDetection(const vector<Agent>& agents, const Grid& grid) {
    int k = agents.size();
    vector<vector<int>> groups(k);
    for (int i = 0; i < k; ++i) groups[i].push_back(i);

    vector<vector<Position>> paths(k);
    for (int i = 0; i < k; ++i) {
        auto p = findPathICTS({ agents[i] }, grid);
        if (p.empty()) return {};
        paths[i] = p[0];
    }

    while (true) {
        bool merged = false;
        for (size_t i = 0; i < groups.size(); ++i) {
            for (size_t j = i + 1; j < groups.size(); ++j) {
                if (groupsConflict(groups[i], groups[j], paths)) {
                    vector<int> mergedGroup = groups[i];
                    mergedGroup.insert(mergedGroup.end(), groups[j].begin(), groups[j].end());

                    vector<Agent> mergedAgents;
                    for (int idx : mergedGroup) mergedAgents.push_back(agents[idx]);

                    auto mergedPaths = findPathICTS(mergedAgents, grid);
                    if (mergedPaths.empty()) return {};

                    groups.erase(groups.begin() + j);
                    groups.erase(groups.begin() + i);
                    groups.push_back(mergedGroup);

                    for (int a = 0; a < mergedGroup.size(); ++a)
                        paths[mergedGroup[a]] = mergedPaths[a];

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
