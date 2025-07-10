#include "Agent.h"
#include "Position.h"



Agent::Agent(int id, Position start, Position goal) : id(id), start(start), goal(goal) {}

int Agent::getId()const {
	return id;
}
Position Agent::getStart()const {
	return start;
}
Position Agent::getGoal()const {
	return goal;
}