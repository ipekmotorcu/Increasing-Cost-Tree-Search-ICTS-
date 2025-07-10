
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
#include "id.cpp" 
#include "HighLevel.cpp"

using namespace std;

tuple<Grid, vector<Agent>>loadMapWithagents(const string& path) {
	ifstream in(path);
	cout << path << endl;
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
	cout << path << endl;
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

int main() {

	//Test3 Agent,Grid,Lowlevel, high level

	/*Grid grid(5, 5);
		Position p1({ 2, 2 });
		//grid.setObstacle(p1);
		//grid.setObstacle({ 2,3 });

		for (int y = 0; y < grid.getHeight(); ++y) {
			for (int x = 0; x < grid.getWidth(); ++x) {
				Position p2 = { x,y };
				cout << (grid.isFree(p2) ? 1 : 0) << " ";//1 boş,0 engel var
			}
			cout << "\n";
		}


	vector<Agent> agents = {


		Agent(1,{0,0},{4,4}),
		Agent(2,{4,0},{0,4})
	};

	bool sucess = HighLevel::solve(agents, grid);
	std::cout <<(sucess ? "Solution found":"solution not found")<<std::endl;
	*///test 3
	/*Grid grid(7, 3);
	//grid.setObstacle({ 2, 2 });
	//grid.setObstacle({ 2, 3 });

	std::vector<Agent> agents = {
	   Agent(0,{0,1}, {6,1}),
	   Agent(1,{2,1},{0,1}),
	   Agent(2,{6,1},{2,1,})
	};*/



	//CSV'ye yazdırmak için
	std::ofstream log("results1.csv", std::ios::app);
	if (!log.is_open()) {
		std::cerr << " Couldn't open results.txt for writing!\n";
	}
	else {
		log << "test_name,agent_count,total_cost,time_ms,expanded,success,ICTNodesNonGoal,depth\n";

	}
	//map okuması için
	for (int i = 0; i < 1; i++) {//test sayısına göre değiştir!!!
		//string path = "C:\\Users\\a09884\\source\\repos\\ICTS\\ICTS\\tests\\test" + to_string(i) + ".txt";
		string path = "C:\\Users\\ipekm\\source\\repos\\ICTS1\\ICTS1\\maps\\map" + to_string(i) + ".txt";
		//auto state = loadMapWithagents(path);
		auto state = loadMapWithagentsReal(path);
		Grid grid = get<0>(state);
		vector<Agent> agents = get<1>(state);

		int totalCost = -1;
		int expandedCount = 0;
		int ICTNodesNonGoal = 0;//pruningden sonra kaç ICT Node kalmış (Low level kaç defa çalıştırılmış)
		int depth = 0;

		/***************************************************************/
		auto paths = independenceDetection(agents, grid);

		if (paths.empty()) {
			cout << "Çözüm bulunamadı (ID başarısız)." << endl;
		}
		else {
			cout << "ID çözümü başarıyla bulundu. Ajan yolları:\n";
			printGridWithPaths(grid, paths);
		}
		/**************************************************************************/
		auto start = std::chrono::high_resolution_clock::now();
		bool success = HighLevel::solve(agents, grid, totalCost, expandedCount, ICTNodesNonGoal, depth,paths);
		auto end = std::chrono::high_resolution_clock::now();

		double durationMs = std::chrono::duration<double, std::milli>(end - start).count();



		log << "test" << i << "," << agents.size() << "," << (success ? totalCost : -1) << "," << durationMs << "," << expandedCount << "," << (success ? 1 : 0) << "," << (ICTNodesNonGoal - 1) << "," << depth << std::endl;



		std::cout << "\n******ICTS RESULT*****" << std::endl;
		std::cout << "Agent count                   : " << agents.size() << std::endl;
		std::cout << "Total cost                    : " << (success ? totalCost : -1) << std::endl;
		std::cout << "Time (ms)                     : " << durationMs << std::endl;
		std::cout << "Expanded nodes                : " << expandedCount << std::endl;
		std::cout << "Result                        : " << (success ? " Found" : " Not Found") << std::endl;
		std::cout << "Non-Goal ICT nodes(Not Pruned): " << ICTNodesNonGoal - 1 << std::endl;//hedefi prune edemeyiz hedefi çıkarıyorum o yüzden (0 perfect pruning)
		std::cout << "Depth:                        : " << depth;



	}
	


	return 0;
}