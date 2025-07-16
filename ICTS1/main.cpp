
#include "Agent.h"
#include "Grid.h"
#include "HighLevel.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <fstream>
#include <cassert>
#include <tuple>
#include <filesystem>
#include <string>
#include "id.h"
#include <random>
#include "LowLevel.h"
#include "AStar.h"
#include <future>
#include <thread>
#include <chrono>



using namespace std;

tuple<Grid, vector<Agent>>loadMapWithagents(const string& path) {
	ifstream in(path);
	std::cout << path << endl;
	if (!in)throw runtime_error("Cannot open map file:" + path);
	int w, h;
	in >> w >> h;
	in.ignore();//boş satır için

	Grid grid(w, h);
	string line;
	for (int y = 0; y < h; ++y) {
		getline(in, line);
		for (int x = 0; x < w && x < line.size(); ++x) {
			if (line[x] == '1') {
				grid.setObstacle({ x,y });
			}
		}

	}

	int agentCount;
	in >> agentCount;
	vector <Agent> agents;
	for (int i = 0; i < agentCount; i++) {
		int px, py, gx, gy;
		in >> px >> py >> gx >> gy;
		Position start = { px,py };
		Position goal = { gx,gy };
		agents.push_back(Agent(i, start, goal));

	}
	return{ grid,agents };



}
tuple<Grid, vector<Agent>>loadMapWithagentsReal(const string& path) {
	ifstream in(path);
	std::cout << path << endl;
	if (!in)throw runtime_error("Cannot open map file:" + path);
	int w, h;
	in >> w >> h;
	in.ignore();

	Grid grid(w, h);
	string line;
	for (int y = 0; y < h; ++y) {
		getline(in, line);
		for (int x = 0; x < w && x < line.size(); ++x) {
			if (line[x] == '@' || line[x] == 'T') {
				grid.setObstacle({ x,y });
			}
		}

	}

	int agentCount;
	in >> agentCount;
	vector <Agent> agents;
	for (int i = 0; i < agentCount; i++) {
		int px, py, gx, gy;
		in >> px >> py >> gx >> gy;
		Position start = { px,py };
		Position goal = { gx,gy };
		agents.push_back(Agent(i, start, goal));

	}
	return{ grid,agents };



}

void runAstar(vector<Agent> agentsWindow,
	vector<vector<Position>>out_paths, Grid grid, int N, int trial) {
	double totalAStarTime = 0;
	int totalAStarCost = 0;
	int totalAStarExpanded = 0;
	int totalAStarSuccess = 0;

	int costAStar = -1, expandedAStar = 0;
	//time limit = 5 dakika
	auto start2 = std::chrono::high_resolution_clock::now();

	auto future = std::async(std::launch::async, [&]() {
		return AStar::solve(agentsWindow, grid, costAStar, expandedAStar, out_paths);
		});

	std::future_status status = future.wait_for(std::chrono::minutes(5));  // timeout 5 dk

	bool successAStar = false;
	auto end2 = std::chrono::high_resolution_clock::now();
	double durationAStar = std::chrono::duration<double, std::milli>(end2 - start2).count();

	if (status == std::future_status::ready) {
		successAStar = future.get();
	}
	else {
		std::cout << "A* TIMEOUT !\n";
		successAStar = false;
		costAStar = -1;
		expandedAStar = 0;
	}

	if (successAStar) {
		totalAStarTime += durationAStar;
		totalAStarExpanded += expandedAStar;
		totalAStarCost += costAStar;
		totalAStarSuccess++;
	}
	if (trial == 9) {
		std::cout << "AStar" << N << "," << N << ",-1,"
			<< totalAStarTime / 10.0 << "," << totalAStarExpanded / 10.0
			<< "," << totalAStarSuccess / 10.0 << ",-1,-1\n";
	}


}



int main() {//testlerdeki ajanları random değiştiriyorum
	std::ofstream log("results3x3.csv", std::ios::app);
	if (!log.is_open()) {
		std::cerr << " Couldn't open results1.csv for writing!\n";
		return 1;
	}
	else {
		log << "test,k,k',avg_time_ms,avg_expanded,success_rate,avg_ICTNodesNonGoal,avg_depth\n";
		cout << "test, k, k', avg_time_ms, avg_expanded, success_rate, avg_ICTNodesNonGoal, avg_depth\n";
	}
	//string path = "C:\\Users\\a09884\\source\\repos\\ICTS\\ICTS\\maps\\den520d.txt";
	string path = "C:\\Users\\ipekm\\source\\repos\\ICTS1\\ICTS1\\maps\\boyarski.txt";
	auto state = loadMapWithagentsReal(path);
	Grid grid = get<0>(state);
	vector<Agent> agents = get<1>(state);
	int F = 5;
	//for (int N = 10; N <= 90; N += 10) {
	for (int N = 2; N <= F; N += 1) {
		LowLevel::mddCache.clear();

		double totalTime = 0;
		int totalCost = 0;
		int totalExpanded = 0;
		int totalICTNodesNonGoal = 0;
		int totalDepth = 0;
		int totalSuccess = 0;
		int maxGroupSize = -1;




		for (int trial = 0; trial < 10; trial++) {


			int cost = -1;
			int expanded = 0;
			int ICTNonGoal = 0;
			int depth = 0;
			int kPrime = -1;



			int offset = trial % (agents.size());
			//cout << "offset:" << offset << endl;
			vector<Agent> agentsWindow;
			vector<vector<Position>>out_paths;
			for (int j = 0; j < N; j++) {
				agentsWindow.push_back(agents[(offset + j) % agents.size()]);
			}

			//auto start = std::chrono::high_resolution_clock::now();
			//bool ICTS = false;
			//auto paths = independenceDetection(agentsWindow, grid, cost, expanded, ICTNonGoal, depth, kPrime);


			////bool success = HighLevel::solve(agentsWindow, grid, totalCost, expanded, ICTNonGoal, depth, out_paths);
			//auto end = std::chrono::high_resolution_clock::now();
			//double duration = std::chrono::duration<double, std::milli>(end - start).count();



			//if (!paths.empty()) {
			//	totalTime += duration;
			//	totalExpanded += expanded;
			//	totalICTNodesNonGoal += (ICTNonGoal);
			//	totalDepth += depth;
			//	totalSuccess++;
			//	totalCost += cost;
			//	maxGroupSize = std::max(maxGroupSize, kPrime);
			//	/*for (int i = 0; i < paths.size(); ++i) {
			//		std::cout << "Agent " << i << ": ";
			//		for (const auto& p : paths[i]) {
			//			std::cout << "(" << p.x << "," << p.y << ") ";
			//		}
			//		std::cout << "\n";
			//	}*/
			//}
			/*if (success) {
				totalTime += duration;
				totalExpanded += expanded;
				totalICTNodesNonGoal += (ICTNonGoal);
				totalDepth += depth;
				totalSuccess++;
				totalCost += cost;
				maxGroupSize = std::max(maxGroupSize, kPrime);
			}*/
			runAstar(agentsWindow, out_paths, grid,N,trial);

		}

		int agentCount = N;
		double avgCost = totalSuccess > 0 ? (double)totalCost / totalSuccess : -1;
		double avgTime = totalTime / 10.0;
		double avgExpanded = (double)totalExpanded / 10.0;
		double avgICT = (double)totalICTNodesNonGoal / 10.0;
		double avgDepth = (double)totalDepth / 10.0;
		double successRate = (double)totalSuccess / 10.0;

		/*log << "test" << N << "," << agentCount << "," << maxGroupSize << ","
			<< avgTime << "," << avgExpanded << "," << successRate
			<< "," << avgICT << "," << avgDepth << "\n";

		std::cout << "Test" << N << "," << agentCount << "," << maxGroupSize << ","
			<< avgTime << "," << avgExpanded << "," << successRate
			<< "," << avgICT << "," << avgDepth << "\n";*/




		/*std::cout << "\n==== test" << N << " sonuçları ====\n";
		std::cout << "Basarı oranı   : " << successRate << "\n";
		std::cout << "Ortalama cost  : " << avgCost << "\n";
		std::cout << "Ortalama süre  : " << avgTime << " ms\n";
		std::cout << "Ortalama depth : " << avgDepth << "\n";
		std::cout << "Ortalama expanded: " << avgExpanded << "\n";
		std::cout << "Ortalama ICTNode : " << avgICT << "\n";
		std::cout << "Ortalama k' : " << maxGroupSize << "\n";*/
	}

	return 0;
}