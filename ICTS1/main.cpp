
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

//int main() {
//	
//		
//
//
//	//CSV'ye yazdırmak için
//	std::ofstream log("results1.csv", std::ios::app);
//	if (!log.is_open()) {
//		std::cerr << " Couldn't open results.txt for writing!\n";
//	}
//	else {
//		log << "8x8_ICT_ID,k,k',total_cost,time_ms,expanded,success,ICTNodesNonGoal,depth\n";
//		
//	}
//	//map okuması için
//	for (int i = 1; i < 2;i++) {//test sayısına göre değiştir!!!
//		
//		//string path = "C:\\Users\\a09884\\source\\repos\\ICTS\\ICTS\\tests\\test" + to_string(i) + ".txt";
//		string path = "C:\\Users\\a09884\\source\\repos\\ICTS\\ICTS\\maps\\map" + to_string(i) + ".txt";
//    //auto state = loadMapWithagents(path);
//	auto state = loadMapWithagentsReal(path);
//	Grid grid = get<0>(state);
//	vector<Agent> agents = get<1>(state);
//
//	int totalCost = -1;
//	int expandedCount = 0;
//	int ICTNodesNonGoal = 0;//pruningden sonra kaç ICT Node kalmış (Low level kaç defa çalıştırılmış)
//	int depth = 0;
//	int maxGroupSize = -1;
//
//	auto start = std::chrono::high_resolution_clock::now();
//	//ID olmadan
//	/*vector<vector<Position>>out_paths;
//	bool success = HighLevel::solve(agents, grid, totalCost, expandedCount,ICTNodesNonGoal,depth,out_paths);*/
//	//ID ile
//	auto paths = independenceDetection(agents, grid,totalCost,expandedCount,ICTNodesNonGoal,depth,maxGroupSize);//id kullanmak için
//	auto end = std::chrono::high_resolution_clock::now();
//
//	double durationMs = std::chrono::duration<double, std::milli>(end - start).count();
//	
//	//bu kısım id için lazım
//	bool success = false;
//	if (paths.empty()) {
//		cout << "Çözüm bulunamadı (ID başarısız)." << endl;
//	}
//	else {
//		success = true;
//		cout << "Çözüm bulundu. Agent paths:\n";
//		HighLevel::printGridWithPaths(grid, paths);
//	}
//		
//	log << "test" << i << "," << agents.size() << "," << maxGroupSize<<","<<(success ? totalCost : -1) << ","<< durationMs << "," << expandedCount << "," << (success ? 1 : 0) <<","<<(ICTNodesNonGoal-1)<<","<<depth<< std::endl;
//
//	
//	
//	std::cout << "\n******ICTS RESULT*****" << std::endl;
//	std::cout << "Agent count (k)               : " << agents.size() << std::endl;
//	std::cout << "Max Agent Count ID (k')       : " << maxGroupSize << endl;
//	std::cout << "Total cost                    : " << (success ? totalCost : -1) << std::endl;
//	std::cout << "Time (ms)                     : " << durationMs << std::endl;
//	std::cout << "Expanded nodes                : " << expandedCount << std::endl;
//	std::cout << "Result                        : " << (success ? " Found" : " Not Found") << std::endl;
//	std::cout << "Non-Goal ICT nodes(Not Pruned): " << ICTNodesNonGoal-1 << std::endl;//hedefi prune edemeyiz hedefi çıkarıyorum o yüzden (0 perfect pruning)
//	std::cout << "Depth:                        : " << depth;
//	
//
//
//	}
//	
//	return 0;
//}
//int main() {
//	std::ofstream log("results1.csv", std::ios::app);
//	if (!log.is_open()) {
//		std::cerr << " Couldn't open results1.csv for writing!\n";
//		return 1;
//	}
//	else {
//		log << "test,k,k',avg_cost,avg_time_ms,avg_expanded,success_rate,avg_ICTNodesNonGoal,avg_depth\n";
//	}
//
//	for (int i = 3; i < 9; i++) { // test3.txt ~ test9.txt
//		string path = "C:\\Users\\a09884\\source\\repos\\ICTS\\ICTS\\maps\\map" + to_string(i) + ".txt";
//
//		double totalTime = 0;
//		int totalCost = 0;
//		int totalExpanded = 0;
//		int totalICTNodesNonGoal = 0;
//		int totalDepth = 0;
//		int totalSuccess = 0;
//		int maxGroupSize = -1;
//
//		for (int trial = 0; trial < 100; trial++) {
//			
//				auto state = loadMapWithagentsReal(path);
//				Grid grid = get<0>(state);
//				vector<Agent> agents = get<1>(state);
//
//			int cost = -1;
//			int expanded = 0;
//			int ICTNonGoal = 0;
//			int depth = 0;
//			int kPrime = -1;
//
//			auto start = std::chrono::high_resolution_clock::now();
//			auto paths = independenceDetection(agents, grid, cost, expanded, ICTNonGoal, depth, kPrime);
//			auto end = std::chrono::high_resolution_clock::now();
//			double duration = std::chrono::duration<double, std::milli>(end - start).count();
//
//			totalTime += duration;
//			totalExpanded += expanded;
//			totalICTNodesNonGoal += (ICTNonGoal - 1); // root hariç
//			totalDepth += depth;
//
//			if (!paths.empty()) {
//				totalSuccess++;
//				totalCost += cost;
//				maxGroupSize = std::max(maxGroupSize, kPrime);
//			}
//		}
//
//		int agentCount = get<1>(loadMapWithagentsReal(path)).size();
//		double avgCost = totalSuccess > 0 ? (double)totalCost / totalSuccess : -1;
//		double avgTime = totalTime / 100.0;
//		double avgExpanded = (double)totalExpanded / 100.0;
//		double avgICT = (double)totalICTNodesNonGoal / 100.0;
//		double avgDepth = (double)totalDepth / 100.0;
//		double successRate = (double)totalSuccess / 100.0;
//
//		log << "test" << i << "," << agentCount << "," << maxGroupSize << ","
//			<< avgCost << "," << avgTime << "," << avgExpanded << "," << successRate
//			<< "," << avgICT << "," << avgDepth << "\n";
//
//		std::cout << "\n==== test" << i << " sonuçları ====\n";
//		std::cout << "Basarı oranı   : " << successRate << "\n";
//		std::cout << "Ortalama cost  : " << avgCost << "\n";
//		std::cout << "Ortalama süre  : " << avgTime << " ms\n";
//		std::cout << "Ortalama depth : " << avgDepth << "\n";
//		std::cout << "Ortalama expanded: " << avgExpanded << "\n";
//	}
//
//	return 0;
//}

int main() {//testlerdeki ajanları random değiştiriyorum
	std::ofstream log("results3x3.csv", std::ios::app);
	if (!log.is_open()) {
		std::cerr << " Couldn't open results1.csv for writing!\n";
		return 1;
	}
	else {
		log << "test,k,k',avg_time_ms,avg_expanded,success_rate,avg_ICTNodesNonGoal,avg_depth\n";
	}
	//string path = "C:\\Users\\a09884\\source\\repos\\ICTS\\ICTS\\maps\\mapN.txt";
	string path = "C:\\Users\\ipekm\\source\\repos\\ICTS1\\ICTS1\\maps\\mapN.txt";
	auto state = loadMapWithagentsReal(path);
	Grid grid = get<0>(state);
	vector<Agent> agents = get<1>(state);
	//for (int N = 10; N <= 90; N += 10) {
	for (int N = 3; N <= 8; N += 1) {
		LowLevel::mddCache.clear();

		double totalTime = 0;
		int totalCost = 0;
		int totalExpanded = 0;
		int totalICTNodesNonGoal = 0;
		int totalDepth = 0;
		int totalSuccess = 0;
		int maxGroupSize = -1;

		// A* için toplamlar
		double totalAStarTime = 0;
		int totalAStarCost = 0;
		int totalAStarExpanded = 0;
		int totalAStarSuccess = 0;


		for (int trial = 0; trial < 10; trial++) {//10


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

			auto start = std::chrono::high_resolution_clock::now();

			auto paths = independenceDetection(agentsWindow, grid, cost, expanded, ICTNonGoal, depth, kPrime);//Astar şu anda değişttirmeyi unutma


			//bool success = HighLevel::solve(agentsWindow, grid, totalCost, expanded, ICTNonGoal, depth, out_paths);
			auto end = std::chrono::high_resolution_clock::now();
			double duration = std::chrono::duration<double, std::milli>(end - start).count();



			if (!paths.empty()) {
			   totalTime += duration;
			   totalExpanded += expanded;
			   totalICTNodesNonGoal += (ICTNonGoal);
				totalDepth += depth;
				totalSuccess++;
				totalCost += cost;
				maxGroupSize = std::max(maxGroupSize, kPrime);
				for (int i = 0; i < paths.size(); ++i) {
					std::cout << "Agent " << i << ": ";
					for (const auto& p : paths[i]) {
						std::cout << "(" << p.x << "," << p.y << ") ";
					}
					std::cout << "\n";
				}
			}
			/*if (success) {
				totalTime += duration;
				totalExpanded += expanded;
				totalICTNodesNonGoal += (ICTNonGoal);
				totalDepth += depth;
				totalSuccess++;
				totalCost += cost;
				maxGroupSize = std::max(maxGroupSize, kPrime);
			}*/

			//// === A* ===
			//int costAStar = -1, expandedAStar = 0;
			//// === A* ZAMAN KONTROLLÜ ÇALIŞTIR ===
			//auto start2 = std::chrono::high_resolution_clock::now();

			//auto future = std::async(std::launch::async, [&]() {
			//	return AStar::solve(agentsWindow, grid, costAStar, expandedAStar);
			//	});

			//std::future_status status = future.wait_for(std::chrono::minutes(5));  // timeout 5 dk

			//bool successAStar = false;
			//auto end2 = std::chrono::high_resolution_clock::now();
			//double durationAStar = std::chrono::duration<double, std::milli>(end2 - start2).count();

			//if (status == std::future_status::ready) {
			//	successAStar = future.get(); // A* tamamlandı
			//}
			//else {
			//	std::cout << "A* TIMEOUT (5 dakika)!\n";
			//	successAStar = false;
			//	costAStar = -1;
			//	expandedAStar = 0;
			//}

			//if (successAStar) {
			//	totalAStarTime += durationAStar;
			//	totalAStarExpanded += expandedAStar;
			//	totalAStarCost += costAStar;
			//	totalAStarSuccess++;
			//}


		}

		int agentCount = N;
		double avgCost = totalSuccess > 0 ? (double)totalCost / totalSuccess : -1;
		double avgTime = totalTime / 10.0;
		double avgExpanded = (double)totalExpanded / 10.0;
		double avgICT = (double)totalICTNodesNonGoal / 10.0;
		double avgDepth = (double)totalDepth / 10.0;
		double successRate = (double)totalSuccess / 10.0;

		log << "test" << N << "," << agentCount << "," << maxGroupSize << ","
			<< avgTime << "," << avgExpanded << "," << successRate
			<< "," << avgICT << "," << avgDepth << "\n";

		std::cout << "" << N << "," << agentCount << "," << maxGroupSize << ","
			<< avgTime << "," << avgExpanded << "," << successRate
			<< "," << avgICT << "," << avgDepth << "\n";


		/*std::cout << "AStar" << N << "," << agentCount << ",-1,"
			<< totalAStarTime / 10.0 << "," << totalAStarExpanded / 10.0
			<< "," << totalAStarSuccess / 10.0 << ",-1,-1\n";*/

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