#include <iostream>
#ifndef ICTNODE_H
#define ICTNode_H
#include<vector>
#include <memory>//shared_ptr ve make_shared için
using namespace std;


class ICTNode {
public:
	vector<int> costs;//double da olabilir
	shared_ptr<ICTNode> parent;
	vector<shared_ptr<ICTNode>> children;
	//ICTNode(const vector <int>& costVector, shared_ptr<ICTNode> parent = nullptr);
	ICTNode(const vector <int>& costs);


	int totalCost() const;

	bool operator<(const ICTNode& other) const;

	//vector<shared_ptr<ICTNode>>generateChildren()const;
};
#endif