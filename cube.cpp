#include <iostream>
#include "cube.hpp"

const int NOTATIONS[9] = { R, U, F, R2, U2, F2, r, u, f };
const int PERMUTATION[3][24] = { {2, 0, 3, 1, 13, 15, 6, 7, 8, 9, 10, 11, 12, 17, 14, 19, 16, 21, 18, 23, 20, 4, 22, 5}, {15, 14, 2, 3, 4, 5, 6, 7, 8, 9, 21, 20, 12, 13, 10, 11, 18, 16, 19, 17, 0, 1, 22, 23}, {18, 1, 19, 3, 0, 5, 2, 7, 4, 9, 6, 11, 12, 13, 14, 15, 16, 17, 8, 10, 22, 20, 23, 21} };
const string TURN_AXIS[3] = { "zyyzzxxzzyyzxyyxxzzxxyyx", "yzzyxzzxyzzyyxxyzxxzyxxy", "xxxxyyyyxxxxzzzzyyyyzzzz" };
const pair<int, int> COMMAND[6] = { {0, 90}, {0, 180}, {0, -90}, {1, -90}, {1, 450}, {1, 360} };
const int CHANGE_VIEW[3][24] = { {18, 16, 19, 17, 0, 1, 2, 3, 4, 5, 6, 7, 13, 15, 12, 14, 9, 11, 8, 10, 22, 20, 23, 21}, {1, 3, 0, 2, 21, 23, 20, 22, 10, 8, 11, 9, 6, 4, 7, 5, 12, 13, 14, 15, 16, 17, 18, 19}, {0, 16, 2, 17, 4, 1, 6, 3, 8, 5, 10, 7, 13, 15, 12, 14, 9, 11, 18, 19, 20, 21, 22, 23} };

Cube::Cube(const string& state, const vector<int>& solution) : state(state), solution(solution) {
}

Cube::Cube(const string& state, const bool solved) : state(state) {
	if(!solved) {
		for (int i = 0; i < 24; i++) {
			this->state[i] = (state[i] == state[7] || state[i] == state[9] || state[i] == state[12] ? state[i] : '_');
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			this->state[i] = '_';
			this->state[i + 4] = state[7];
			this->state[i + 8] = state[9];
			this->state[i + 12] = state[12];
			this->state[i + 16] = '_';
			this->state[i + 20] = '_';
		}
	}
}

const Cube Cube::turn(const int notation) const {
	const int turn_face = notation % 3;
	string temp_state, turned_state(state);
	for (int i = 0; i <= notation / 3; i++) {
		temp_state = turned_state;
		for (int j = 0; j < 24; j++) {
			turned_state[j] = temp_state[PERMUTATION[turn_face][j]];
		}
	}
	vector<int> turned_solution(solution);
	turned_solution.push_back(notation);
	return Cube(turned_state, turned_solution);
}

Solution::Solution(Cube* initial_cube) : initial_cube(initial_cube) {
}

void Solution::search() {
	vector<Cube*> forward_level{ new Cube(initial_cube->get_state(), false) }, backward_level{ new Cube(initial_cube->get_state(), true) }, temp_level;
	for (int i = 0; i < 6; i++) {
		for (const Cube* forward_cube : forward_level) {
			for (const Cube* backward_cube : backward_level) {
				if (*forward_cube == *backward_cube) {
					merge(forward_cube->get_solution(), backward_cube->get_solution());
					return;
				}
			}
		}
		update_level(forward_level, temp_level);
		for (const Cube* backward_cube : backward_level) {
			for (const Cube* forward_cube : forward_level) {
				if (*forward_cube == *backward_cube) {
					merge(forward_cube->get_solution(), backward_cube->get_solution());
					return;
				}
			}
		}
		update_level(backward_level, temp_level);
	}
	cerr << "Sensing Error: " << initial_cube->get_state() << endl;
	exit(EXIT_FAILURE);
}

const vector<pair<int, int>>& Solution::get_commands() {
	commands.clear();
	int view = 12;
	for (const int notation : initial_cube->get_solution()) {
		const char turn_axis = TURN_AXIS[notation % 3][view];
		if (turn_axis == 'x') {
			commands.push_back(COMMAND[3]);
			commands.push_back(COMMAND[0]);
			commands.push_back(COMMAND[4]);
			view = CHANGE_VIEW[0][view];
			view = CHANGE_VIEW[1][view];
		}
		else if (turn_axis == 'y') {
			commands.push_back(COMMAND[5]);
			view = CHANGE_VIEW[1][view];
		}
		commands.push_back(COMMAND[notation / 3]);
		for (int i = 0; i <= notation / 3; i++) {
			view = CHANGE_VIEW[2][view];
		}
	}
	return commands;
}

void Solution::merge(const vector<int>& forward_solution, const vector<int>& backward_solution) {
	vector<int> solution(forward_solution);
	for (auto notation = backward_solution.rbegin(); notation != backward_solution.rend(); ++notation) {
		solution.push_back((2 - *notation / 3) * 3 + *notation % 3);
	}
	initial_cube->set_solution(solution);
}

void Solution::update_level(vector<Cube*>& level, vector<Cube*>& temp_level) const {
	for (const Cube* cube : level) {
		for (const int notation : NOTATIONS) {
			if (cube->get_solution().empty() || notation % 3 != cube->get_solution().back() % 3) {
				temp_level.push_back(new Cube(cube->turn(notation)));
			}
		}
		delete cube;
	}
	level.clear();
	level.swap(temp_level);
}
