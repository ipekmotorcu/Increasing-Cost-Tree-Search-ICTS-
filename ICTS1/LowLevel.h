#pragma once
#include "Grid.h"
#include "Agent.h"
#include <unordered_map>
#include <tuple>
#include <unordered_set>
#include <vector>

class LowLevel {
public:
    static bool verify(const std::vector<Agent>& agents, const Grid& grid,
        const std::vector<int>& costs,
        std::vector<std::vector<Position>>& out_paths, int& ICTNodesNonGoal);

    using MDD = std::vector<std::unordered_set<Position>>;
    using CacheKey = std::tuple<Position, Position, int>;

    struct CacheKeyHash {
        size_t operator()(const CacheKey& key) const {
            auto& start = std::get<0>(key);
            auto& goal = std::get<1>(key);
            auto& cost = std::get<2>(key);
            size_t h1 = std::hash<int>()(start.x) ^ (std::hash<int>()(start.y) << 1);
            size_t h2 = std::hash<int>()(goal.x) ^ (std::hash<int>()(goal.y) << 1);
            size_t h3 = std::hash<int>()(cost);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    //  Her ajan i�in verilen cost'ta hedefe ula�an MDD'yi cache'ler
    static std::unordered_map<CacheKey, MDD, CacheKeyHash> mddCache;
};