#include "Grid.h"

using namespace std;
Grid::Grid(int width, int height) :width(width), height(height), grid(height, vector<bool>(width, true)) {}
bool Grid::isInside(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}
bool Grid::isFree(Position pos) const {
	return isInside(pos.x, pos.y) && grid[pos.y][pos.x];//1 geçiþ var 0 engel var !x ve y sýrasý tam tersi olabilir düþün biraz

}
void Grid::setObstacle(Position pos) {
	if (isInside(pos.x, pos.y)) {
		grid[pos.y][pos.x] = false;
	}
}
vector<Position> Grid::getNeighbors(Position pos)const {//komþularýný buluyoruz burada
	vector<Position> neighbors;
	static const vector<Position> directions = { {0,1},{1,0},{0,-1},{-1,0} };//{x,y}
	for (auto& d : directions) {
		Position next{ pos.x + d.x, pos.y + d.y };
		if (isFree(next)) {
			neighbors.push_back(next);
		}
	} return neighbors;
}
int Grid::getHeight() const {

	return height;
}
int Grid::getWidth() const {
	return width;
}