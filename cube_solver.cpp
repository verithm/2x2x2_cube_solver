#include <chrono>
#include <iomanip>
#include <iostream>
#include <wiringPi.h>
#include "cube.hpp"
#include "solver.hpp"
using namespace std::chrono;

int main() {
	
	// Scan Phase
	const auto start = high_resolution_clock::now();
	wiringPiSetupGpio();
	Motor motor[2] = { {8, 7}, {14, 15} };
	Sensor sensor;
	Cube cube(sensor.scan_cube(motor));
	
	// Search Phase
	const auto lap_0 = high_resolution_clock::now();
	Solution solution(&cube);
	solution.search();
	
	// Actuation Phase
	const auto lap_1 = high_resolution_clock::now();
	for (const auto& [motor_index, angle] : solution.get_commands()) {
		cout << "motor_" << motor_index << '.'; //delete
		motor[motor_index].move(angle);
	}
	
	// Cube Solved!
	const auto finish = high_resolution_clock::now();
	
	// Print Result
	const auto scan_time = duration_cast<milliseconds>(lap_0 - start), search_time = duration_cast<milliseconds>(lap_1 - lap_0), actuation_time = duration_cast<milliseconds>(finish - lap_1), total_time = duration_cast<milliseconds>(finish - start);
	cout << right << endl;
	cout << "    2x2x2 Cube Solver" << endl;
	cout << "-------------------------" << endl;
	cout << " Move Count     :" << setw(7) << cube.get_solution().size() << endl;
	cout << endl;
	cout << " Scan Time      :" << setw(7) << fixed << setprecision(2) << scan_time.count() / 1000.0 << endl;
	cout << " Search Time    :" << setw(7) << fixed << setprecision(2) << search_time.count() / 1000.0 << endl;
	cout << " Actuation Time :" << setw(7) << fixed << setprecision(2) << actuation_time.count() / 1000.0 << endl;
	cout << "-------------------------" << endl;
	cout << " Total Time     :" << setw(7) << fixed << setprecision(2) << total_time.count() / 1000.0 << endl;
	cout << endl;
	
	return 0;
}
