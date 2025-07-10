
#include "LowLevel.h"
#include "Position.h"
#include <set>
#include <queue>
#include<functional>
#include <unordered_set>
#include <cassert>
#include <iostream>
#include <unordered_set>
#include "Position.h"
//#include "MDD.h"
#include <vector>
using namespace std;



/*Her ajan için ayrı ayrı MDD oluşturuyoruz

ajanın başlangıç noktasından başlayıp
verilen cost ile gidilebilen her yere gidiyoruz
sonuçta hedefe ulaşanları MDD'ye ekliyoruz
level[t]: ajanın zaman t'de olabileceği yerler
!!!tam olarak makaledeki gibi değil t zamanda bfs ile hangi nodelara ulaşılabilir ona bakıyor bu kısım değiştirilebilir
ama değiştirirsem dfs'deki geçerli path mi değil mi kontrolünü çıkarmayı unutma
*/
using MDD = std::vector<std::unordered_set<Position>>;
void printMDDs(const std::vector<std::vector<std::unordered_set<Position>>>& mdds) {
    for (int i = 0; i < mdds.size(); ++i) {
        std::cout << "Agent " << i << " MDD:\n";
        for (int t = 0; t < mdds[i].size(); ++t) {
            std::cout << "  t=" << t << ": ";
            for (const auto& pos : mdds[i][t]) {
                std::cout << "(" << pos.x << "," << pos.y << ") ";
            }
            std::cout << "\n";
        }
    }
}
static MDD buildMDD(const Grid& grid, const Position& start, const Position& goal, int cost, int maxCost) {
    MDD levels(maxCost + 1);//+1 olmalı mı düşün!
    std::queue<std::pair<Position, int>> q;
    q.push({ start, 0 });
    levels[0].insert(start);

    while (!q.empty()) {
        pair<Position, int> p = q.front();
        Position curr = p.first;
        int t = p.second;
        q.pop();
        if (t >= cost) continue;

        for (const auto& nb : grid.getNeighbors(curr)) {
            if (levels[t + 1].insert(nb).second)
                q.push({ nb, t + 1 });
        }

        if (levels[t + 1].insert(curr).second)
            q.push({ curr, t + 1 });
    }

    if (!levels[cost].count(goal)) return {};
    for (auto it = levels[cost].begin(); it != levels[cost].end();) {//verilen costa ulaşıldığında hedefte değilse sil hedef olmayanları
        if (*it != goal) {
            it = levels[cost].erase(it);
        }
        else
            ++it;
    }//dummy node ekledim hedefe ulaşıp da bekleme ihtimalleri için yoksa çözüm bulunamadı sanılıyor
    if (cost < maxCost) {
        for (int i = cost; i <= maxCost; ++i) {
            levels[i].insert(goal);
        }
    }
    return levels;
}
/*
hedeften başlayıp geriye doğru giderek pathleri kontrol ediyor*/
void backwardPrune(MDD& mdd, const Grid& grid, const Position& goal) {//MDD'yi geriye dönük temizlemek için hedefe gitmeyen pathleri siliyor
    int depth = mdd.size() - 1;

    for (int t = depth - 1; t >= 0; --t) {
        std::unordered_set<Position> valid;
        for (const auto& pos : mdd[t]) {
            for (const auto& nb : grid.getNeighbors(pos)) {
                if (mdd[t + 1].count(nb)) {
                    valid.insert(pos);
                    break;
                }
            }
            // wait check
            if (mdd[t + 1].count(pos)) {
                valid.insert(pos);
            }
        }
        mdd[t] = valid;
    }
}

static bool dfsCrossProduct(const std::vector<MDD>& mdds,
    const std::vector<Agent>& agents,
    const Grid& grid,
    std::vector<Position>& curr,
    std::vector<std::vector<Position>>& paths,
    int time,
    int maxDepth) {
    int k = mdds.size();
    std::vector<Position> next(k);

    std::function<bool(int)> recurse = [&](int agent) -> bool {
        if (agent == k) {//bütün ajanlar için konum seçildiyse
            // Conflict check
            std::set<Position> seen;
            for (const auto& p : next)
                if (!seen.insert(p).second) return false;//aynı noktaya geliyorsa

            for (int i = 0; i < k; ++i)
                for (int j = i + 1; j < k; ++j)
                    if (curr[i] == next[j] && curr[j] == next[i])//swap kontrol
                        return false;

            // conflict yoksa path'e ekle ve devam et o konumdan
            for (int i = 0; i < k; ++i)
                paths[i].push_back(next[i]);
            std::vector<Position> prev = curr;
            curr = next;

            if (time == maxDepth) {//istenilen cost'a ulaşınca hedefteler mi ona bak yani path bulundu mu bulunmadı mı
                for (int i = 0; i < k; ++i)
                    if (curr[i] != agents[i].getGoal()) {
                        std::cout << " Agent " << i << " not at goal! curr=" << curr[i].x << "," << curr[i].y << " goal=" << agents[i].getGoal().x << std::endl;
                        return false;
                    }
                std::cout << "All agents reached goal at t=" << time << std::endl;
                return true;
            }

            if (dfsCrossProduct(mdds, agents, grid, curr, paths, time + 1, maxDepth))//bir sonraki zaman dilimine geç
                return true;

            // Rollback geçerli  değilse
            for (int i = 0; i < k; ++i)
                paths[i].pop_back();
            curr = prev;

            return false;
        }

        if (time >= mdds[agent].size()) return false;

        const auto& prev = curr[agent];
        auto neighbors = grid.getNeighbors(prev);

        for (const auto& pos : mdds[agent][time]) {//
            if (pos == prev || std::find(neighbors.begin(), neighbors.end(), pos) != neighbors.end()) {
                next[agent] = pos;
                if (recurse(agent + 1)) return true;//bir sonraki ajan için bak
            }
        }
        return false;
        };

    return recurse(0);//agent 0'dan başla
}

bool hasValidPairwisePaths(const MDD& mdd1, const MDD& mdd2) {
    int depth = std::min((int)mdd1.size(), (int)mdd2.size()) - 1;

    using State = std::tuple<Position, Position, int>;
    std::queue<State> q;
    std::set<State> visited;

    // Başlangıç pozisyonlarını al 
    Position start1 = *mdd1[0].begin();
    Position start2 = *mdd2[0].begin();

    q.push({ start1, start2, 0 });
    visited.insert({ start1, start2, 0 });

    while (!q.empty()) {
        // auto [pos1, pos2, t] = q.front();
        auto state = q.front();
        Position pos1 = get<0>(state);
        Position pos2 = get<1>(state);
        int t = get<2>(state);

        q.pop();

        if (t == depth)
            return true; // geçerli yol bulundu

        const auto& next1_set = mdd1[t + 1];
        const auto& next2_set = mdd2[t + 1];

        for (const auto& next1 : next1_set) {
            for (const auto& next2 : next2_set) {
                if (next1 == next2) continue; // vertex conflict
                if (next1 == pos2 && next2 == pos1) continue; // swap conflict

                State next_state = { next1, next2, t + 1 };
                if (visited.count(next_state)) continue;
                visited.insert(next_state);
                q.push(next_state);
            }
        }
    }

    return false; // hiçbir geçerli yol yok
}
bool simplePairwisePruning(const std::vector<MDD>& mdds) {
    int k = mdds.size();

    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {

            if (!(hasValidPairwisePaths(mdds[i], mdds[j]))) {
                return true;
            }
        }
    }
    return false;
}
bool enhancedPairwisePruning(std::vector<MDD>& mdds, const std::vector<Agent>& agents, const Grid& grid) {
    int k = mdds.size();
    bool pruned_any = false;

    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            int depth = std::min((int)mdds[i].size(), (int)mdds[j].size()) - 1;//teknik olarak bütün MDD'lerin satır sayısı eşit ondan buna gerek yok ama değişiklik olursa diye bu satırı ekledim

            using State = std::tuple<Position, Position, int>;
            std::queue<State> q;
            std::set<State> visited;

            // Kaydedilecek geçerli pozisyonlar
            std::vector<std::unordered_set<Position>> valid_i(depth + 1);
            std::vector<std::unordered_set<Position>> valid_j(depth + 1);

            Position start1 = *mdds[i][0].begin();
            Position start2 = *mdds[j][0].begin();

            q.push({ start1, start2, 0 });
            visited.insert({ start1, start2, 0 });
            //başlangıç pozisyonlarını ekle
            valid_i[0].insert(start1);
            valid_j[0].insert(start2);

            bool has_solution = false;

            while (!q.empty()) {
                Position pos1;
                Position pos2;
                int t;

                auto x = q.front(); q.pop();
                pos1 = get<0>(x);
                pos2 = get<1>(x);
                t = get<2>(x);

                if (t == depth) {
                    has_solution = true;//2'li ajanlar için çözüm var ama devamına da bakmalıyız çözüm olmayan varsa o zaman pruning başarılı olacak
                    continue;
                }
                //BFS mantığı
                for (const auto& now1 : mdds[i][t + 1]) {//başka türlü çözülebilir mi bilmiyorum iç içe for dışında
                    for (const auto& now2 : mdds[j][t + 1]) {
                        //pos:current position, n:position at t+1
                        if (now1 == now2) continue; // vertex conflict
                        if (now1 == pos2 && now2 == pos1) continue; // swap

                        //conflict yoksa
                        State next = { now1, now2, t + 1 };
                        if (visited.count(next)) continue;//daha önce bakıldıysa queue'ya ekleme

                        visited.insert(next);
                        q.push(next);

                        valid_i[t + 1].insert(now1);
                        valid_j[t + 1].insert(now2);
                    }
                }
            }

            if (!has_solution) {
                return true; // iki ajan için çakışmyan path bulundu. İkili ikili baktığımızda en az bir ikili için bütün pathler çakışıyor ise pruning başarılı diğer ikililere bakmalıyız
            }

            // prune edemediysek MDD pruning: geçerli olmayan yolları çıkarıyoruz
            for (int t = 0; t <= depth; ++t) {
                auto& mi = mdds[i][t];
                auto& mj = mdds[j][t];
                for (auto it = mi.begin(); it != mi.end();) {//unordered set içinde geziyoruz
                    if (valid_i[t].count(*it)) ++it;
                    else it = mi.erase(it), pruned_any = true;
                }
                for (auto it = mj.begin(); it != mj.end();) {
                    if (valid_j[t].count(*it)) ++it;
                    else it = mj.erase(it), pruned_any = true;
                }
            }
        }
    }

    return false; // prune edilemedi ama MDD'ler daraltıldı
}

/*bool enhancedMAgentPruning(std::vector<MDD>& mdds, const std::vector<Agent>& agents, const Grid& grid, int M = 3) {
    int k = mdds.size();
    bool pruned_any = false;

    std::vector<std::vector<int>> groups;
    generateCombinations(k, M, groups);  // tüm M’li kombinasyonlar

    for (const auto& group : groups) {
        int depth = mdds[group[0]].size() - 1;

        using State = std::pair<std::vector<Position>, int>;
        std::queue<State> q;
        std::set<State> visited;

        // Geçerli pozisyonları tut
        std::vector<std::unordered_set<Position>> valid[M];
        for (int i = 0; i < M; ++i)
            valid[i].resize(depth + 1);

        std::vector<Position> start;
        for (int i = 0; i < M; ++i)
            start.push_back(*mdds[group[i]][0].begin());

        q.push({ start, 0 });
        visited.insert({ start, 0 });

        for (int i = 0; i < M; ++i)
            valid[i][0].insert(start[i]);

        bool has_solution = false;

        while (!q.empty()) {
            auto state = q.front();
            vector<Position> positions = get<0>(state);
            int t = get<1>(state);

            q.pop();

            if (t == depth) {
                has_solution = true;
                continue;
            }

            std::vector<std::vector<Position>> next_candidates(M);
            for (int i = 0; i < M; ++i)
                next_candidates[i].assign(mdds[group[i]][t + 1].begin(), mdds[group[i]][t + 1].end());

            std::vector<int> indices(M, 0);
            while (true) {
                std::vector<Position> next(M);
                for (int i = 0; i < M; ++i)
                    next[i] = next_candidates[i][indices[i]];

                // vertex conflict
                std::set<Position> seen(next.begin(), next.end());
                if (seen.size() < M) goto advance;//

                // swap conflict
                bool conflict = false;
                for (int i = 0; i < M; ++i)
                    for (int j = i + 1; j < M; ++j)
                        if (positions[i] == next[j] && positions[j] == next[i])
                            conflict = true;
                if (conflict) goto advance;

                State new_state = { next, t + 1 };
                if (!visited.count(new_state)) {
                    visited.insert(new_state);
                    q.push(new_state);
                    for (int i = 0; i < M; ++i)
                        valid[i][t + 1].insert(next[i]);
                }

            advance:
                int i = M - 1;
                while (i >= 0) {
                    if (++indices[i] < next_candidates[i].size()) break;
                    indices[i] = 0;
                    --i;
                }
                if (i < 0) break;
            }
        }

        if (!has_solution)
            return true;  // prune başarılı → bu grubun çözümü yok

        // MDD prune: geçersiz node’ları çıkar
        for (int t = 0; t <= depth; ++t) {
            for (int i = 0; i < M; ++i) {
                int agent = group[i];
                auto& level = mdds[agent][t];
                for (auto it = level.begin(); it != level.end();) {
                    if (valid[i][t].count(*it)) ++it;
                    else it = level.erase(it), pruned_any = true;
                }
            }
        }
    }

    return false;  // çözüm var ama MDD'ler daraltıldı
}*/
bool LowLevel::verify(const std::vector<Agent>& agents, const Grid& grid,
    const std::vector<int>& costs,
    std::vector<std::vector<Position>>& out_paths, int& ICTNodesNonGoal) {

    int k = agents.size();
    int maxCost = *std::max_element(costs.begin(), costs.end());
    int maxDepth = maxCost;


    std::vector<MDD> mdds(k);
    for (int i = 0; i < k; ++i) {
        mdds[i] = buildMDD(grid, agents[i].getStart(), agents[i].getGoal(), costs[i], maxCost);
        if (mdds[i].empty()) return false;
        backwardPrune(mdds[i], grid, agents[i].getGoal());


    }//printMDDs(mdds);

//pruning
   //simple
    /*if (simplePairwisePruning(mdds) ){//pruning başarılı ise low level'ı çalıştırmaya gerek yok false döndür
       // cout << "Pruning is succesful." << endl;
        return false;
    }
    else {
       // cout << "Pruning is not succesful" << endl;
        ICTNodesNonGoal++;//prune edilemeyen node sayısını 1 artır
    }*/
    if (enhancedPairwisePruning(mdds, agents, grid)) {//pruning başarılı ise low level'ı çalıştırmaya gerek yok false döndür
        // cout << "Pruning is succesful." << endl;
        return false;
    }
    else {
        // cout << "Pruning is not succesful" << endl;
        ICTNodesNonGoal++;//prune edilemeyen node sayısını 1 artır
    }

    // cout << "After Enhanced Pruning................" << endl;
    // printMDDs(mdds);


    std::vector<Position> curr(k);
    for (int i = 0; i < k; ++i)
        curr[i] = agents[i].getStart();

    out_paths = std::vector<std::vector<Position>>(k);
    for (int i = 0; i < k; ++i)
        out_paths[i].push_back(curr[i]);



    return dfsCrossProduct(mdds, agents, grid, curr, out_paths, 1, maxDepth);
}