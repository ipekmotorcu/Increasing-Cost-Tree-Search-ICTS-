#pragma once
#include "Grid.h"
#include "Agent.h"
#include "ICTNode.h"
#include <vector>

class HighLevel {
public:
    static bool solve(const std::vector<Agent>& agents, const Grid& grid,
        int& totalCostOut, int& expandedCountOut,
        int& ICTNodesNonGoal, int& depth,
        std::vector<std::vector<Position>>& out_paths);
	static bool solveID(const std::vector<Agent>& agents, const Grid& grid, int& totalCostOut, int& expandedCountOut);

};
