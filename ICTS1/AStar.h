#pragma once
#include "Agent.h"
#include "Grid.h"
#include "Position.h"
#include <vector>

class AStar {
public:
    static bool solve(const std::vector<Agent>& agents, const Grid& grid,
        int& totalCost, int& expandedCount, vector<vector<Position>>& out_paths);
};