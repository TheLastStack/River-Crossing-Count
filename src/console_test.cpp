#include <iostream>
#include <vector>
#include <stack>
#include "solveShoreProblem.h"

int main()
{
	using std::vector;
	using std::queue;
	using std::cout;
	using std::endl;
	using std::get;
	using std::pair;
	vector<int> left_moves{ 1, 1 };
	vector<int> right_moves{ 1 };
	vector<int> left_start{ 3, 3, 3 };
	vector<int> right_start{ 0, 0, 0 };
	ShoreState start(left_start, right_start);
	ShoreState::set_moves(left_moves, right_moves);
	queue<ShoreState> a = depth_first_search(start);
	pair<Shore, Shore> sh;
	for (int i = 0; !a.empty(); ++i)
	{
		sh = a.front().get_state();
		cout << "Left Shore: ";
		for (auto i : get<0>(sh).to_vector())
		{
			cout << i;
		}
		cout << " Right Shore: ";
		for (auto i : get<1>(sh).to_vector())
		{
			cout << i;
		}
		cout << endl;
		a.pop();
	}
	return 0;
}
