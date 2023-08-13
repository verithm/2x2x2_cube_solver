#ifndef CUBE_HPP
#define CUBE_HPP

#include <string>
#include <utility>
#include <vector>
using namespace std;

enum Notation { R, U, F, R2, U2, F2, r, u, f };

class Cube {
public:
	Cube(const string& state, const vector<int>& solution = vector<int>());
	Cube(const string& state, const bool solved);
	const bool operator==(const Cube& cube) const { return state == cube.state; }
	const string& get_state() const { return state; }
	const vector<int>& get_solution() const { return solution; }
	void set_solution(const vector<int>& solution) { this->solution = solution; }
	const Cube turn(const int notation) const;
private:
	string state;
	vector<int> solution;
};

class Solution {
public:
	Solution(Cube* initial_cube);
	void search();
	const vector<pair<int, int>>& get_commands();
private:
	Cube* initial_cube;
	vector<pair<int, int>> commands;
	void merge(const vector<int>& forward_solution, const vector<int>& backward_solution);
	void update_level(vector<Cube*>& level, vector<Cube*>& temp_level) const;
};

#endif
