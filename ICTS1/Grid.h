#include <vector>
#include "Position.h"
#ifndef GRID_H
#define GRID_H
using namespace std;

class Grid {
public:
	Grid(int width, int height);
	void setObstacle(Position pos);
	bool isFree(Position pos) const;//is walkable
	bool isInside(int x, int y) const;

	vector<Position> getNeighbors(Position pos) const;
	int getWidth() const;
	int getHeight() const;

private:
	int width, height;
	vector<vector<bool>> grid;
};
#endif