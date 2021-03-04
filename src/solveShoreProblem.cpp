#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <stdexcept>
#include <algorithm>
#include <iostream>

using namespace std;

bool next_combination(vector<int>& counter, int limit)
{
	/*
	Returns all possible unique combinations of length of vector counter out of sequence 
	0 ... limit - 1. Similar to std::next_permutation in operation. 
	Returns true on setting the vector to next valid combination.
	The combination sent in must be in lexicographical order.
	*/
	bool returning = counter.size() <= limit;
	if (returning)
	{
		counter.back()++;
		if (counter.back() >= limit)
		{
			int i = counter.size() - 1;
			for (;i > 0 && counter[i] >= limit; --i)
			{
				counter[i - 1] += 1;
				counter[i] = counter[i - 1] + 1;
			}
			while (counter.back() >= limit)
			{
				i = counter.size() - 1;
				while (i > 0 && counter[i] - counter[i - 1] == 1)
					--i;
				counter[i - 1] += 1;
				for (int j = i; j < counter.size(); ++j)
				{
					counter[j] = counter[j - 1] + 1;
				}
			}
		}
		if (counter[0] >= limit - counter.size())
		{
			for (int i = 0; i < counter.size(); ++i)
			{
				counter[i] = i;
			}
			returning = false;
		}
	}
	return returning;
}
class Shore {
private:
	//Saving an array. Each index represents a particular type. 
	//The value in each index represents the number of items in that type.
	vector<int> items_of_type;
	//Storing index of the types which have completely crossed over to the other shore
	unordered_set<int> all_crossed_type;
	void from(Shore* s, const vector<int>& a, const vector<int>& b)
	{
		for (int i = 0; i < a.size(); ++i)
		{
			if (s->items_of_type[a[i]] - b[i] < 0)
			{
				throw domain_error("Setting negative number of items on one shore");
			}
			s->items_of_type[a[i]] -= b[i];
			if (s->items_of_type[a[i]] == 0)
				(s->all_crossed_type).insert(i);
		}
	}
	void to(Shore* s, const vector<int>& a, const vector<int>& b)
	{
		for (int i = 0; i < a.size(); ++i)
		{
			s->items_of_type[a[i]] += b[i];
			if (s->items_of_type[a[i]] != 0 && (s->all_crossed_type).find(a[i]) != (s->all_crossed_type).end())
				(s->all_crossed_type).erase(a[i]);
		}
	}
public:
	Shore()
	{
		;
	}
	Shore(vector<int>& a):items_of_type(0)
	{
		items_of_type = a;
		for (int i = 0; i < items_of_type.size(); ++i)
		{
			if (items_of_type[i] == 0)
				all_crossed_type.insert(i);
		}
	}
	Shore(vector<int>& a, unordered_set<int>& b):items_of_type(0)
	{
		items_of_type = a;
		all_crossed_type = b;
	}
	Shore(const Shore& a)
	{
		this->items_of_type = a.items_of_type;
		this->all_crossed_type = a.all_crossed_type;
	}
	void types_from(const vector<int>& a, const vector<int>& b)
	{
		from(this, a, b);
	}
	Shore create_types_from(const vector<int>& a, const vector<int>& b)
	{
		vector<int> new_items_of_type = items_of_type;
		unordered_set<int> new_all_crossed_type = all_crossed_type;
		Shore new_shore(new_items_of_type, new_all_crossed_type);
		from(&new_shore, a, b);
		return new_shore;
	}
	void types_to(const vector<int>& a, const vector<int>& b)
	{
		to(this, a, b);
	}
	Shore create_types_to(const vector<int>& a, const vector<int>& b)
	{
		vector<int> new_items_of_type = items_of_type;
		unordered_set<int> new_all_crossed_type = all_crossed_type;
		Shore new_shore(new_items_of_type, new_all_crossed_type);
		to(&new_shore, a, b);
		return new_shore;
	}
	int items_at(int a) const
	{
		return items_of_type[a];
	}
	int number_of_types_crossed() const
	{
		return all_crossed_type.size();
	}
	int size() const
	{
		return items_of_type.size();
	}
};

class ShoreState {
private:
	Shore left_shore;
	Shore right_shore;
	static vector<int> left_shore_moves;
	static vector<int> right_shore_moves;
	deque<ShoreState*> children;
	ShoreState* parent;
public:
	static void set_moves(const vector<int>& a, const vector<int>& b)
	{
		left_shore_moves = a;
		right_shore_moves = b;
	}
	ShoreState()
	{
		parent = NULL;
	}
	ShoreState(vector<int>& a, vector<int>& b, ShoreState* c):left_shore(a), right_shore(b), parent(c)
	{
		;
	}
	ShoreState(const Shore& a, const Shore& b, ShoreState* c)
	{
		left_shore = a;
		right_shore = b;
		parent = c;
	}
	~ShoreState()
	{
		for (auto i : children)
		{
			delete i;
		}
	}
	void expandNode()
	{
		vector<int> left_shore_counter(left_shore_moves.size(), 0);
		vector<int> right_shore_counter(right_shore_moves.size(), 0);
		bool valid_state = true, outer_loop = true, inner_loop = true;
		for (int i = 1; i < left_shore_counter.size(); ++i)
			left_shore_counter[i] = i;
		for (int i = 1; i < left_shore_counter.size(); ++i)
			right_shore_counter[i] = i;
		while (outer_loop)
		{
			for (int i = 0; i < left_shore_counter.size(); ++i)
			{
				if (left_shore.items_at(left_shore_counter[i]) - left_shore_moves[i] < 0)
					valid_state = false;
			}
			if (valid_state)
			{
				while (inner_loop)
				{
					for (int i = 0; i < right_shore_counter.size(); ++i)
					{
						if (right_shore.items_at(right_shore_counter[i]) - right_shore_moves[i] < 0)
							valid_state = false;
					}
					if (valid_state)
					{
						left_shore.types_from(left_shore_counter, left_shore_moves);
						right_shore.types_to(right_shore_counter, right_shore_moves);
						children.push_front(new ShoreState(this->left_shore, this->right_shore, this));
						left_shore.types_to(left_shore_counter, left_shore_moves);
						right_shore.types_from(right_shore_counter, right_shore_moves);
					}
					else
					{
						valid_state = true;
					}
					if (!next_permutation(right_shore_counter.begin(), right_shore_counter.end()))
					{
						inner_loop = next_combination(right_shore_counter, right_shore.size());
					}
				}
			}
			else
			{
				valid_state = true;
			}
			if (!next_permutation(left_shore_counter.begin(), left_shore_counter.end()))
			{
				outer_loop = next_combination(left_shore_counter, left_shore.size());
			}
		}
	}
	deque<ShoreState*> tracert()
	{
		deque<ShoreState*> root_to_soln;
		ShoreState* s = this;
		while (s->parent != NULL)
		{
			root_to_soln.push_front(s);
			s = s->parent;
		}
		return root_to_soln;
	}
	ShoreState get_child()
	{
		return *children.front();
	}
	bool has_children()
	{
		return !children.empty();
	}
	ShoreState pop_child()
	{
		ShoreState a = *(children.front());
		delete children.front();
		children.pop_front();
		return a;
	}
	bool is_shorestate_soln()
	{
		return left_shore.number_of_types_crossed() == left_shore.size();
	}
};
deque<ShoreState> depth_first_search(const ShoreState& a)
{
	deque<ShoreState> states_under_examination;
	deque<ShoreState> solution;
	int number_of_steps = 0;
	unordered_map<ShoreState, tuple<int, ShoreState*>> states_already_examined;
	ShoreState root = a;
	states_under_examination.push_front(root);
	while (!states_under_examination.empty())
	{
		if (states_under_examination.front().has_children())
		{
			unordered_map<ShoreState, tuple<int, deque<ShoreState>>>::const_iterator got = states_already_examined.find(states_under_examination.front().get_child());
			if (got == states_already_examined.end())
			{
				states_under_examination.push_front(states_under_examination.front().pop_child());
				states_under_examination.front().expandNode();
			}
			else
			{

			}
		}
		else
		{

		}
	}
}
void main()
{

}