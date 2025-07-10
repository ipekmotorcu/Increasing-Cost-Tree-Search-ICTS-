
#include "ICTNode.h"
using namespace std;
/*
costVector: birden fazla ajanýn costu tutulduðu için
shared_ptr: yeni nesil pointer otomatik siliniyor kullanýmý bitince, bir nesneye birden fazla kiþi ayný anda sahip olabilir bir node'u hem parent hem de child olarak refere edebiliriz o yüzden bunu kullanýyoruz
makeshared new gibi yeni nesne oluþturuyor*/
ICTNode::ICTNode(const vector <int>& costs)
	: costs(costs) {
}

int ICTNode::totalCost() const {
	int sum = 0;
	for (int c : costs) {
		sum += c;
	}
	return sum;
}
bool ICTNode:: operator<(const ICTNode& other) const {
	return totalCost() > other.totalCost();
}
/*vector<shared_ptr<ICTNode>> ICTNode::generateChildren()const {
	vector<shared_ptr<ICTNode>> result;
	for (size_t i = 0; i < costs.size(); i++) {
		vector<int> newCostVector = costs;
		newCostVector[i]++;
		result.push_back(make_shared<ICTNode>(newCostVector, make_shared<ICTNode>(*this)));

	} return result;
}*/

