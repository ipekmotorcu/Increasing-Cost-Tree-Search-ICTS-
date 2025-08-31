Increasing Cost Tree Search (ICTS) for Optimal Multi-Agent Pathfinding

This project is an implementation of the Increasing Cost Tree Search (ICTS) algorithm for Optimal Multi-Agent Pathfinding (MAPF), based on the paper:

Guni Sharon, Roni Stern, Meir Goldenberg, Ariel Felner.
The Increasing Cost Tree Search for Optimal Multi-Agent Pathfinding.
Artificial Intelligence, Volume 195, 2013, Pages 470–495.
DOI: 10.1016/j.artint.2012.11.006

📖 Project Overview

The Multi-Agent Pathfinding (MAPF) problem asks:

Given multiple agents with individual start and goal positions on a grid, find collision-free paths that minimize the total cost.

The ICTS algorithm addresses this by introducing a two-level search framework:

High-Level Search (ICT): Explores combinations of path costs for agents.

Low-Level Search (MDD-based): Verifies whether conflict-free paths exist under the given cost constraints using Multi-valued Decision Diagrams (MDDs).

This implementation includes:

Core classes: Agent, Position, Grid, ICTNode

High-level search (HighLevel.cpp): Manages the ICT and invokes low-level checks

Low-level search (LowLevel.cpp): Builds MDDs, checks feasibility, and applies pruning techniques (Simple and Enhanced Pairwise Pruning)
