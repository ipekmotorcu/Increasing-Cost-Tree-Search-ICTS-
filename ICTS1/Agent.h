#include "Position.h"
#ifndef AGENT_H
#define AGENT_H




class Agent {
public:
	Agent(int id, Position start, Position goal);

	int getId() const;
	Position getStart() const;
	Position getGoal() const;

private:
	int id;
	Position start;
	Position goal;
};

#endif