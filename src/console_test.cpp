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
	using std::cin;
	vector<int> left_start;
	int n;
	cout << "Enter the number of types of soldiers" << endl;
	cin >> n;
	cout << "Enter the number of soldiers of each type on the left shore." << endl;
	for (int i = 0; i < n; ++i)
	{
		int a;
		cin >> a;
		left_start.push_back(a);
	}
	cout << "Enter the number of soldiers of each type on the right shore." << endl;
	vector<int> right_start;
	for (int i = 0; i < n; ++i)
	{
		int a;
		cin >> a;
		right_start.push_back(a);
	}
	int k;
	cout << "Enter the number of types of soldiers who can move from the left shore to the right at a time" << endl;
	cin >> k;
	if (k > n)
	{
		cout << "Number of types of soldiers crossong banks cannot exceed number of types of soldiers on each bank" << endl;
		return 1;
	}
	cout << "Enter the number of soldiers of each type who can move from left shore to right shore" << endl;
	vector<int> left_moves;
	for (int i = 0; i < k; ++i)
	{
		int a;
		cin >> a;
		left_moves.push_back(a);
	}
	cout << "Enter the number of types of soldiers who can move from the right shore to the left at a time" << endl;
	cin >> k;
	if (k > n)
	{
		cout << "Number of types of soldiers crossong banks cannot exceed number of types of soldiers on each bank" << endl;
		return 1;
	}
	cout << "Enter the number of soldiers of each type who can move from right shore to left shore" << endl;
	vector<int> right_moves;
	for (int i = 0; i < k; ++i)
	{
		int a;
		cin >> a;
		right_moves.push_back(a);
	}
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
