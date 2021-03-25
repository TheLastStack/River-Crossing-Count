#include "solveShoreProblem.h"

using namespace std;

bool next_combination(vector<int>& counter, int limit)
{
	/*
	Returns all possible unique combinations of length of vector counter out of sequence
	0 ... limit - 1. Similar to std::next_permutation in operation.
	Returns true on setting the vector to next valid combination.
	The combination sent in must be in lexicographical order.
	*/
	bool returning = true;
	if (counter[0] >= limit - counter.size())
	{
		for (size_t i = 0; i < counter.size(); ++i)
		{
			counter[i] = (int)i;
		}
		returning = false;
	}
	if (returning)
	{
		counter.back()++;
		if (counter.back() >= limit)
		{
			size_t i = counter.size() - 1;
			for (; i > 0 && counter[i] >= limit; --i)
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
				for (size_t j = i; j < counter.size(); ++j)
				{
					counter[j] = counter[j - 1] + 1;
				}
			}
		}
	}
	return returning;
}

void Shore::from(Shore* s, const vector<int>& a, const vector<int>& b)
{
	for (int i = 0; i < a.size(); ++i)
	{
		if (s->items_of_type[a[i]] - b[i] < 0)
		{
			throw domain_error("Setting negative number of items on one shore");
		}
		s->items_of_type[a[i]] -= b[i];
		if (s->items_of_type[a[i]] == 0)
			(s->all_crossed_type).insert(a[i]);
	}
}

void Shore::to(Shore* s, const vector<int>& a, const vector<int>& b)
{
	for (int i = 0; i < a.size(); ++i)
	{
		s->items_of_type[a[i]] += b[i];
		if (s->items_of_type[a[i]] != 0 && (s->all_crossed_type).find(a[i]) != (s->all_crossed_type).end())
			(s->all_crossed_type).erase(a[i]);
	}
}

Shore::Shore()
{}

Shore::Shore(vector<int>& a) : items_of_type(a)
{
	for (int i = 0; i < items_of_type.size(); ++i)
	{
		if (items_of_type[i] == 0)
			all_crossed_type.insert(i);
	}
}

Shore::Shore(vector<int>& a, unordered_set<int>& b) : items_of_type(a), all_crossed_type(b)
{}

Shore::Shore(const Shore& a) : items_of_type(a.items_of_type), all_crossed_type(a.all_crossed_type)
{}

void Shore::types_from(const vector<int>& a, const vector<int>& b)
{
	from(this, a, b);
}
Shore Shore::create_types_from(const vector<int>& a, const vector<int>& b)
{
	vector<int> new_items_of_type = items_of_type;
	unordered_set<int> new_all_crossed_type = all_crossed_type;
	Shore new_shore(new_items_of_type, new_all_crossed_type);
	from(&new_shore, a, b);
	return new_shore;
}

void Shore::types_to(const vector<int>& a, const vector<int>& b)
{
	to(this, a, b);
}

Shore Shore::create_types_to(const vector<int>& a, const vector<int>& b)
{
	vector<int> new_items_of_type = items_of_type;
	unordered_set<int> new_all_crossed_type = all_crossed_type;
	Shore new_shore(new_items_of_type, new_all_crossed_type);
	to(&new_shore, a, b);
	return new_shore;
}

int Shore::items_at(int a) const
{
	return items_of_type[a];
}

size_t Shore::number_of_types_crossed() const
{
	return all_crossed_type.size();
}

size_t Shore::size() const
{
	return items_of_type.size();
}

bool Shore::operator==(const Shore& a) const
{
	if (a.size() == this->items_of_type.size())
	{
		for (int i = 0; i < this->size(); ++i)
		{
			if (a.items_at(i) != this->items_of_type[i])
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

vector<int> Shore::to_vector() const
{
	return items_of_type;
}

size_t ShoreStateHasher::operator()(const ShoreState& S) const
{
	return S.hash_code();
}

vector<int> ShoreState::right_shore_moves;
vector<int> ShoreState::left_shore_moves;

void ShoreState::set_moves(const vector<int>& a, const vector<int>& b)
{
	left_shore_moves = a;
	right_shore_moves = b;
}

ShoreState::ShoreState() : children_extracted(0)
{
	best_soln = make_pair(-1, this);
}

ShoreState::ShoreState(vector<int>& a, vector<int>& b) : left_shore(a), right_shore(b), children_extracted(0)
{
	best_soln = make_pair(-1, this);
}

ShoreState::ShoreState(const Shore& a, const Shore& b) : left_shore(a), right_shore(b), children_extracted(0)
{
	best_soln = make_pair(-1, this);
}

ShoreState::ShoreState(const ShoreState& a) : left_shore(a.left_shore), right_shore(a.right_shore), children(a.children), children_extracted(a.children_extracted)
{
	if (get<0>(a.best_soln) < 1)
	{
		this->best_soln = make_pair(get<0>(a.best_soln), this);
	}
	else
	{
		this->best_soln = a.best_soln;
	}
}

void ShoreState::closeNodes()
{
	while (!children.empty())
	{
		delete children.front();
		children.pop();
	}
	if (get<1>(best_soln) != this)
	{
		delete get<1>(best_soln);
	}
}

bool ShoreState::operator==(const ShoreState& a) const
{
	return this->left_shore == a.left_shore && this->right_shore == a.right_shore;
}

void ShoreState::expandNode()
{
	vector<int> left_shore_counter(left_shore_moves.size(), 0);
	vector<int> right_shore_counter(right_shore_moves.size(), 0);
	unordered_set<ShoreState, ShoreStateHasher> already_done;
	bool valid_state = true, outer_loop = true, inner_loop = true;
	vector<int> local_lsm(left_shore_moves.size(), 0);
	vector<int> local_rsm(right_shore_moves.size(), 0);
	ShoreState local_check = *this;
	local_lsm.back() += 1;
	while (local_lsm[0] <= left_shore_moves[0])
	{
		while (local_rsm[0] <= right_shore_moves[0] && accumulate(local_lsm.cbegin(), local_lsm.cend(), 0) > accumulate(local_rsm.cbegin(), local_rsm.cend(), 0))
		{
			for (int i = 0; i < left_shore_counter.size(); ++i)
				left_shore_counter[i] = i;
			for (int i = 0; i < right_shore_counter.size(); ++i)
				right_shore_counter[i] = i;
			while (outer_loop)
			{
				for (int i = 0; i < left_shore_counter.size(); ++i)
				{
					if (left_shore.items_at(left_shore_counter[i]) - local_lsm[i] < 0)
						valid_state = false;
				}
				if (valid_state)
				{
					while (inner_loop)
					{
						left_shore.types_from(left_shore_counter, local_lsm);
						right_shore.types_to(left_shore_counter, local_lsm);
						for (int i = 0; i < right_shore_counter.size(); ++i)
						{
							if (right_shore.items_at(right_shore_counter[i]) - local_rsm[i] < 0)
								valid_state = false;
						}
						if (valid_state)
						{
							right_shore.types_from(right_shore_counter, local_rsm);
							left_shore.types_to(right_shore_counter, local_rsm);
							if (already_done.find(*this) == already_done.end() && !(*this == local_check))
							{
								children.push(new ShoreState(this->left_shore, this->right_shore));
								already_done.insert(ShoreState(this->left_shore, this->right_shore));
							}
							left_shore.types_from(right_shore_counter, local_rsm);
							right_shore.types_to(right_shore_counter, local_rsm);
						}
						else
						{
							valid_state = true;
						}
						left_shore.types_to(left_shore_counter, local_lsm);
						right_shore.types_from(left_shore_counter, local_lsm);
						if (!next_permutation(right_shore_counter.begin(), right_shore_counter.end()))
						{
							inner_loop = next_combination(right_shore_counter, (int)right_shore.size());
						}
					}
				}
				else
				{
					valid_state = true;
				}
				if (!next_permutation(left_shore_counter.begin(), left_shore_counter.end()))
				{
					outer_loop = next_combination(left_shore_counter, (int)left_shore.size());
				}
				inner_loop = true;
			}
			local_rsm.back() += 1;
			outer_loop = true;
			for (size_t i = local_rsm.size() - 1; i > 0; --i)
			{
				if (local_rsm[i] > right_shore_moves[i])
				{
					local_rsm[i] = 0;
					local_rsm[i - 1] += 1;
				}
			}
		}
		local_lsm.back() += 1;
		local_rsm.assign(local_rsm.size(), 0);
		for (size_t i = local_lsm.size() - 1; i > 0; --i)
		{
			if (local_lsm[i] > left_shore_moves[i])
			{
				local_lsm[i] = 0;
				local_lsm[i - 1] += 1;
			}
		}
	}
}

ShoreState ShoreState::get_child() const
{
	// Ensuring no dynamically created copies are visible outside the class.
	ShoreState a;
	if (!children.empty()) // Checking for queue
	{
		a = *children.front();
	}
	return a;
}

bool ShoreState::has_children() const
{
	return !children.empty();
}

ShoreState ShoreState::pop_child()
{
	// Ensuring no dynamically created copies are visible outside the class.
	ShoreState a;
	if (!children.empty())
	{
		a = *(children.front());
		delete children.front();
		children.pop();
		++children_extracted;
	}
	return a;
}

ShoreState ShoreState::cycle_child()
{
	ShoreState a;
	if (!children.empty())
	{
		a = *(children.front());
		children.push(children.front());
		children.pop();
		++children_extracted;
	}
	return a;
}

bool ShoreState::is_shorestate_soln() const
{
	return left_shore.number_of_types_crossed() == left_shore.size();
}

size_t ShoreState::hash_code() const
{
	size_t seed = hash<size_t>()(left_shore.size() + right_shore.size());
	for (int i = 0; i < left_shore.size(); ++i)
	{
		hash_combine(seed, left_shore.items_at(i));
	}
	for (int i = 0; i < right_shore.size(); ++i)
	{
		hash_combine(seed, right_shore.items_at(i));
	}
	return seed;
}

pair<Shore, Shore> ShoreState::get_state() const
{
	return make_pair(left_shore, right_shore);
}

bool ShoreState::examined_children() const
{
	return children_extracted >= children.size();
}

queue<ShoreState> depth_first_search(const ShoreState& a)
{
	stack<ShoreState> states_under_examination;
	queue<ShoreState> answer;
	unordered_map<ShoreState, pair<int, ShoreState>, ShoreStateHasher> states_already_examined;
	ShoreState root = a;
	unordered_map<ShoreState, pair<int, ShoreState>, ShoreStateHasher>::const_iterator got;
	ShoreState& check = root;
	ShoreState* dyn = NULL;
	states_under_examination.push(root);
	while (!states_under_examination.empty())
	{
		if (states_under_examination.top().is_shorestate_soln())
		{
			states_already_examined.insert(make_pair(states_under_examination.top(), make_pair(0, states_under_examination.top())));
			dyn = new ShoreState(states_under_examination.top());
			states_under_examination.pop();
			states_under_examination.top().best_soln = make_pair(1, dyn);
		}
		else if ((got = states_already_examined.find(states_under_examination.top())) != states_already_examined.end())
		{
			dyn = new ShoreState(states_under_examination.top());
			states_under_examination.pop();
			if (get<1>(states_under_examination.top().best_soln) == &states_under_examination.top() && get<0>(dyn->best_soln) == -1)
			{
				states_under_examination.top().best_soln = make_pair(-1, dyn);
			}
			if (get<0>(dyn->best_soln) != -1 && get<0>(states_under_examination.top().best_soln) > get<0>(dyn->best_soln))
			{
				if (get<1>(states_under_examination.top().best_soln) != &states_under_examination.top())
					delete get<1>(states_under_examination.top().best_soln);
				states_under_examination.top().best_soln = make_pair(get<0>(dyn->best_soln) + 1, dyn);
			}
		}
		else
		{
			if (states_under_examination.top().has_children())
			{
				if (states_under_examination.top().examined_children())
				{
					states_under_examination.push(states_under_examination.top().pop_child());
				}
				else
				{
					states_under_examination.push(states_under_examination.top().cycle_child());
				}
			}
			else
			{
				if (get<1>(states_under_examination.top().best_soln) == &states_under_examination.top())
				{
					states_under_examination.top().expandNode();
					if (states_under_examination.top().has_children())
					{
						states_under_examination.push(states_under_examination.top().cycle_child());
					}
					else
					{
						states_under_examination.top().closeNodes();
						states_already_examined.insert(make_pair(states_under_examination.top(), make_pair(-1, states_under_examination.top())));
						states_under_examination.pop();
					}
				}
				else
				{
					states_already_examined.insert(make_pair(states_under_examination.top(), make_pair(get<0>(states_under_examination.top().best_soln), ShoreState(*get<1>(states_under_examination.top().best_soln)))));
					dyn = new ShoreState(states_under_examination.top());
					int solution = get<0>(states_under_examination.top().best_soln);
					if (solution == -1)
					{
						states_under_examination.top().closeNodes();
					}
					states_under_examination.pop();
					if (!states_under_examination.empty() && solution != -1)
					{
						if (get<1>(states_under_examination.top().best_soln) != &states_under_examination.top())
							delete get<1>(states_under_examination.top().best_soln);
						states_under_examination.top().best_soln = make_pair(solution + 1, dyn);
					}
				}
			}
		}
	}
	check = a;
	while (!(get<0>(states_already_examined.at(check)) <= 0))
	{
		answer.push(check);
		check = get<1>(states_already_examined.at(check));
	}
	answer.push(root);
	for (auto& it: states_already_examined)
	{
		get<1>(it.second).closeNodes();
	}
	return answer;
}
