#include <iostream>
#include "HighLevel.h"
#include "LowLevel.h"
#include <queue>
#include <set>
#include <numeric>
#include <chrono>
using namespace std;

static vector <int> computeSICHeuristic(const vector<Agent>& agents, const Grid& grid) {//burada hata var san�r�m!!
	vector<int> costs;
	for (const auto& agent : agents) {
		queue<pair<Position, int>> q;
		set<Position> visited;
		q.push({ agent.getStart(),0 });
		int dist = -1;
		while (!q.empty()) {
			pair<Position, int> p = q.front();
			Position curr = p.first;
			int d = p.second;
			q.pop();
			if (curr == agent.getGoal()) { dist = d; break; }
			for (const auto& n : grid.getNeighbors(curr)) {
				if (visited.insert(n).second) q.push({ n, d + 1 });
			}
		}
		if (dist == -1) {

			return {};
		}
		costs.push_back(dist);
	}

	/*cout << "SIC Heuristic(Root Costs):";
	for (int cost : costs) cout << cost << " ";
	std::cout << std::endl;*/

	return costs;
	/*//engel yoksa
	for (const auto& agent : agents) {
		costs.push_back(abs(agent.getGoal().x - agent.getStart().x) + abs(agent.getGoal().y - agent.getStart().y));

	}

	return costs;*/
}


void HighLevel::printGridWithPaths(const Grid& grid, const std::vector<std::vector<Position>>& paths) {//Grid'i yazd�rmak i�in yazd�m
	int rows = grid.getHeight();
	int cols = grid.getWidth();
	std::vector<std::vector<char>> visual(rows, std::vector<char>(cols, '.'));


	// engeller
	for (int y = 0; y < rows; ++y)
		for (int x = 0; x < cols; ++x)
			if (!grid.isFree({ x, y }))
				visual[y][x] = '#';

	// agent yollar�
	/*for (int a = 0; a < paths.size(); ++a) {
		for (int t = 0; t < paths[a].size(); ++t) {
			Position p = paths[a][t];
			char label = 'A' + a;
			visual[p.y][p.x] = label;
		}
	}*/
	for (int agentIdx = 0; agentIdx < paths.size(); agentIdx++) {
		char label;
		if (agentIdx < 10) label = '0' + agentIdx;
		else if (agentIdx < 36) label = 'A' + (agentIdx - 10);
		else label = '*';//ajan say�s� �ok fazla ise
		for (const auto& pos : paths[agentIdx]) {
			if (visual[pos.y][pos.x] == '.' || isdigit(visual[pos.y][pos.x]) || isalpha(visual[pos.y][pos.x])) {//?
				visual[pos.y][pos.x] = label;
			}
		}

	}


	for (int y = 0; y < rows; ++y) {
		for (int x = 0; x < cols; ++x)
			std::cout << visual[y][x] << ' ';
		std::cout << '\n';
	}

}
bool HighLevel::solve(const vector<Agent>& agents, const Grid& grid, int& totalCostOut, int& expandedCountOut, int& ICTNodesNonGoal, int& depth, vector<vector<Position>>& out_paths) {//? uygun path bulunamazsa high level ICT tree'ye node eklemeyi ne zaman b�rakacak
	LowLevel::mddCache.clear();

	auto start = std::chrono::high_resolution_clock::now();

	int k = agents.size();

	vector <int> rootCosts = computeSICHeuristic(agents, grid);
	if (rootCosts.empty()) return false;

	priority_queue<ICTNode> pq;
	pq.push(ICTNode(rootCosts));
	set<vector<int>> visited;//unordered set de olabilir
	//expandedCountOut=0;
	int i = 0;
	while (!pq.empty()) {
		auto end = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration<double, std::milli>(end - start).count();
		if (duration > 300000.0) {//5 dakika
			cout << "Time out" << endl;
			return false;
		}
		ICTNode node = pq.top();
		pq.pop();

		if (visited.count(node.costs)) continue;
		++expandedCountOut;
		visited.insert(node.costs);
		//for (int c : node.costs) cout << c << endl;

		//pathleri kaydetmek i�in
		std::vector<std::vector<Position>> paths;
		if (LowLevel::verify(agents, grid, node.costs, paths, ICTNodesNonGoal)) {
			out_paths = paths;
			totalCostOut = std::accumulate(node.costs.begin(), node.costs.end(), 0);//numeric(ICT Node'undaki costlar� topluyor ve toplam costu buluyor

			//cout << "Agent Paths:\n";
			//for (int i = 0; i < k; ++i) {//!!!
			//	cout << "Agent " << i << ": ";
			//	for (const auto& p : paths[i]) {
			//		std::cout << "(" << p.x << "," << p.y << ") ";
			//	}
			//	std::cout << std::endl;
			//}
			int optimalCost = std::accumulate(rootCosts.begin(), rootCosts.end(), 0);
			//cout << "optimal cost:" << optimalCost << endl;
			depth = totalCostOut - optimalCost;
			//cout << "Printing grid ..." << endl;
			//printGridWithPaths(grid, paths);//Agent pathleri yazd�rmak i�in

		   //std::cout << " Solution found with total cost: " << totalCostOut << std::endl;

		  /* for (int i = 0; i < paths.size(); ++i) {
			   std::cout << "Agent " << i << ": ";
			   for (const auto& pos : paths[i]) {
				   std::cout << "(" << pos.x << "," << pos.y << ") ";
			   }
			   std::cout << std::endl;
		   */

			return true;
		}
		for (int i = 0; i < k; ++i) {//yeni ICT nodelar�n� olu�turuyoruz burada
			vector<int> newCosts = node.costs;
			++newCosts[i];
			pq.push(ICTNode(newCosts));
		}
		//cout << i << endl;
		//i++;
	}return false;


}