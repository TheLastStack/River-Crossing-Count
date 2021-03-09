#ifndef SOLVE_SHORE_PROBLEM_H
#define SOLVE_SHORE_PROBLEM_H

#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <utility>
#include <stdexcept>
#include <cstddef>
#include <limits>

template <class T>
inline void hash_combine(std::size_t& seed, T const& v)
{
	seed ^= std::hash<T>()(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

class Shore {
private:
	//Saving an array. Each index represents a particular type. 
	//The value in each index represents the number of items in that type.
	std::vector<int> items_of_type;
	//Storing index of the types which have completely crossed over to the other shore
	std::unordered_set<int> all_crossed_type;
	void from(Shore* s, const std::vector<int>& a, const std::vector<int>& b);
	void to(Shore* s, const std::vector<int>& a, const std::vector<int>& b);
public:
	Shore();
	Shore(std::vector<int>& a);
	Shore(std::vector<int>& a, std::unordered_set<int>& b);
	Shore(const Shore& a);
	void types_from(const std::vector<int>& a, const std::vector<int>& b);
	Shore create_types_from(const std::vector<int>& a, const std::vector<int>& b);
	void types_to(const std::vector<int>& a, const std::vector<int>& b);
	Shore create_types_to(const std::vector<int>& a, const std::vector<int>& b);
	int items_at(int a) const;
	std::size_t number_of_types_crossed() const;
	std::size_t size() const;
	bool operator==(const Shore& a) const;
	std::vector<int> to_vector() const;
};

class ShoreState {
	/* Represents a particular state of two shores.*/
private:
	Shore left_shore;
	Shore right_shore;
	static std::vector<int> left_shore_moves;
	static std::vector<int> right_shore_moves;
	std::queue<ShoreState*> children;
	int children_extracted;
public:
	std::pair<int, ShoreState*> best_soln;
	static void set_moves(const std::vector<int>& a, const std::vector<int>& b);
	ShoreState();
	ShoreState(std::vector<int>& a, std::vector<int>& b);
	ShoreState(const Shore& a, const Shore& b);
	ShoreState(const ShoreState& a);
	void closeNodes();
	bool operator==(const ShoreState& a) const;
	void expandNode();
	ShoreState get_child() const;
	bool has_children() const;
	ShoreState pop_child();
	ShoreState cycle_child();
	bool is_shorestate_soln() const;
	std::size_t hash_code() const;
	std::pair<Shore, Shore> get_state() const;
	bool examined_children() const;
};

struct ShoreStateHasher {
	std::size_t operator()(const ShoreState& S) const;
};

bool next_combination(std::vector<int>& counter, int limit);
std::stack<ShoreState> depth_first_search(const ShoreState& a);

#endif