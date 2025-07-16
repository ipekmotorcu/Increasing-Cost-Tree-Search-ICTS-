#include "AStar.h"
#include <queue>
#include <set>
#include <map>
#include <numeric>
#include <iostream>

using State = std::vector<Position>;

struct Node {
    State positions;
    int g;
    int h;
    bool operator>(const Node& other) const {
        return g + h > other.g + other.h;
    }
};

static int manhattan(const Position& a, const Position& b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

static int computeSIC(const State& current, const std::vector<Agent>& agents, const Grid& grid) {
    int total = 0;
    for (int i = 0; i < agents.size(); ++i) {
        std::queue<std::pair<Position, int>> q;
        std::set<Position> visited;
        q.push({ current[i], 0 });
        visited.insert(current[i]);

        int dist = -1;
        while (!q.empty()) {
            auto [pos, d] = q.front(); q.pop();
            if (pos == agents[i].getGoal()) {
                dist = d;
                break;
            }

            for (const auto& nb : grid.getNeighbors(pos)) {
                if (visited.insert(nb).second)
                    q.push({ nb, d + 1 });
            }
        }

        if (dist == -1) return INT_MAX;
        total += dist;
    }
    return total;
}


static bool hasConflict(const State& a, const State& b) {
    int k = a.size();
    std::set<Position> seen;
    for (int i = 0; i < k; ++i) {
        if (!seen.insert(b[i]).second) return true;
        if (a[i] == b[i]) continue;
        for (int j = i + 1; j < k; ++j)
            if (a[i] == b[j] && a[j] == b[i]) return true;
    }
    return false;
}

bool AStar::solve(const std::vector<Agent>& agents, const Grid& grid,
    int& totalCost, int& expandedCount, vector<vector<Position>>& out_paths) {
    int k = agents.size();
    State start, goal;
    for (const auto& agent : agents) {
        start.push_back(agent.getStart());
        goal.push_back(agent.getGoal());
    }

    std::priority_queue<Node, std::vector<Node>, std::greater<>> open;
    std::set<State> closed;

    // her ajan için cameFrom map'i (geriye dönük path izleme)
    std::vector<std::unordered_map<Position, Position>> cameFrom(k);

    open.push({ start, 0, computeSIC(start, agents,grid) });
    expandedCount = 0;

    while (!open.empty()) {
        Node node = open.top();
        open.pop();

        if (closed.count(node.positions)) continue;
        closed.insert(node.positions);
        ++expandedCount;

        if (node.positions == goal) {
            totalCost = node.g;

            
            out_paths.clear();
            for (int i = 0; i < k; ++i) {
                std::vector<Position> path;
                Position current = node.positions[i];
                path.push_back(current);
                while (cameFrom[i].count(current)) {
                    current = cameFrom[i][current];
                    path.push_back(current);
                }
                std::reverse(path.begin(), path.end());
                out_paths.push_back(path);
            }

            return true;
        }


     
        std::vector<std::vector<Position>> moves(k);
        for (int i = 0; i < k; ++i) {
            moves[i] = grid.getNeighbors(node.positions[i]);
            moves[i].push_back(node.positions[i]); // wait 
        }

        /*depth= ajan say�s�
        * b�t�n olas� kombinasyonlara bakmak i�in
        * if(depth==K) t�m ajanlara pozisyon atand�ysa ve �ak��ma yoksa
        * bu node'u open listaesine ekle
        */
        std::function<void(int, State&)> recurse;
        recurse = [&](int depth, State& temp) {
            if (depth == k) {
                if (!hasConflict(node.positions, temp)) {
                    open.push({ temp, node.g + 1, computeSIC(temp, agents,grid) });
                }
                return;
            }
            for (const auto& pos : moves[depth]) {
                temp[depth] = pos;
                recurse(depth + 1, temp);
            }
            };

        State temp(k);
        recurse(0, temp);
    }

    return false;
}