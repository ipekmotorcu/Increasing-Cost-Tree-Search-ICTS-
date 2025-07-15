#pragma once
#include "Agent.h"
#include "Grid.h"
#include "HighLevel.h"
#include "Position.h"
#include <vector>
#include <set>
#include <queue>
#include <algorithm>
#include <iostream>


bool hasConflict(const vector<Position>& path1, const vector<Position>& path2);
bool groupsConflict(const vector<int>& g1, const vector<int>& g2, const vector<vector<Position>>& paths);
vector<vector<Position>> findPathICTS(const vector<Agent>& agents, const Grid& grid, int& totalCost, int& expanded, int& ICTNodesNonGoal, int& depth);
vector<vector<Position>> independenceDetection(const vector<Agent>& agents, const Grid& grid, int& totalCostOut, int& expandedCountOut, int& ICTNodesNonGoal, int& depth, int& maxGroupSize);