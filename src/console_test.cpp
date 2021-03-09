#include <iostream>
#include <vector>
#include <stack>
#include "solveShoreProblem.h"

void main()
{
	using std::vector;
	using std::stack;
	vector<int> left_moves{ 1, 1 };
	vector<int> right_moves{ 1 };
	vector<int> left_start{ 3, 3, 3 };
	vector<int> right_start{ 0, 0, 0 };
	ShoreState start(left_start, right_start);
	ShoreState::set_moves(left_moves, right_moves);
	stack<ShoreState> a = depth_first_search(start);
}
