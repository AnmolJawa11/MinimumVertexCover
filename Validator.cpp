
#include<iostream>
#include<vector>
using std::vector;
class Validator {

public:
	bool validateInputVerticesNum(int num) {
		if (num <= 0) {
			return false;
		}
		return true;
	};
	bool validateEnteredEdges(vector<vector<int>> all_edges, int max_num) {
		for (size_t i = 0; i < all_edges.size(); i++) {
			if (all_edges[i][0] > (max_num-1) || all_edges[i][1] > (max_num-1) || all_edges[i][0] < 0 || all_edges[i][1] < 0) {
				return false;
			}
		}
		return true;
	}

};