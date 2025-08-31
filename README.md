# Increasing Cost Tree Search (ICTS)

Implementation of the **Increasing Cost Tree Search (ICTS)** algorithm for **Optimal Multi-Agent Pathfinding (MAPF)**, based on:

> **Sharon, G., Stern, R., Goldenberg, M., Felner, A.**  
> *The Increasing Cost Tree Search for Optimal Multi-Agent Pathfinding.*  
> *Artificial Intelligence, 195 (2013), 470–495*  
> [DOI: 10.1016/j.artint.2012.11.006](https://doi.org/10.1016/j.artint.2012.11.006)

---

## 📖 Overview

The **Multi-Agent Pathfinding (MAPF)** problem asks:  

> *Given multiple agents with individual start and goal positions on a grid, find collision-free paths that minimize the total cost.*  

ICTS addresses this with a **two-level search framework**:

- **High-Level Search (ICT):** Expands nodes representing combinations of agent path costs.  
- **Low-Level Search (MDD-based):** Builds **Multi-valued Decision Diagrams (MDDs)** to check if valid, conflict-free paths exist.  

This implementation also includes **Simple Pairwise Pruning** and **Enhanced Pairwise Pruning** optimizations.

---

## 📂 Project Structure

