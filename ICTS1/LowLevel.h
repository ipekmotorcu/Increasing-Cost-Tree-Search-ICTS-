#pragma once
#include "Grid.h"
#include "Agent.h"

class LowLevel {
public:
	//static bool verify(const vector<Agent>& agents, const Grid& grid, const vector<int>& costs);
	static bool verify(const std::vector<Agent>& agents, const Grid& grid,
		const std::vector<int>& costs,
		std::vector<std::vector<Position>>& out_paths, int& ICTNodesNonGoal);

};