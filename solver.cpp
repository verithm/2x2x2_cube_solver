#include <cmath>
#include <iostream>
#include <map>
#include <wiringPi.h>
#include "solver.hpp"

const vector<pair<int, int>> COMMANDS[5] = { {{1, 360}}, {{1, 270}}, {{0, -90}, {1, 360}, {0, 90}}, {{1, 450}}, {{1, 360}} };

Motor::Motor(const int step_pin, const int dir_pin, const double step_angle) : step_pin(step_pin), dir_pin(dir_pin), step_angle(step_angle) {
	pinMode(step_pin, OUTPUT);
	pinMode(dir_pin, OUTPUT);
}

void Motor::move(const int angle, const int delay) const {
	digitalWrite(dir_pin, signbit(angle));
	for (int i = 0; i < abs(angle) / step_angle; i++) {
		digitalWrite(step_pin, HIGH);
		delayMicroseconds(delay);
		digitalWrite(step_pin, LOW);
		delayMicroseconds(delay);
	}
}

const string& Sensor::scan_cube(const Motor* motor, const int image_size) {
	state.clear();
	set_points(image_size);
	for (int i = 0; i < 5; i++) {
		scan_face(i, image_size);
		for (const auto& [motor_index, angle] : COMMANDS[i]) {
			motor[motor_index].move(angle);
		}
	}
	scan_face(5, image_size);
	if (has_error()) {
		cerr << "Sensing Error: " << state << endl;
		exit(EXIT_FAILURE);
	}
	return state;
}

void Sensor::set_points(const int image_size) {
	points.clear();
	const vector<int> coordinates = { image_size / 4, image_size * 3 / 4 };
	for (const int y : coordinates) {
		for (const int x : coordinates) {
			points.push_back(Point(x, y));
		}
	}
}

const Mat Sensor::capture_image(const string& image_path, const int image_width, const int image_height) const {
	const string command = "libcamera-still -o " + image_path + " --width " + to_string(image_width) + " --height " + to_string(image_height);
	system(command.c_str());
	return imread(image_path);
}

const char Sensor::classify_color(const Vec3b& hsv) const {
	const int h = hsv[0], s = hsv[1];
	if (s < 64) return 'W';
	else if (h < 10 || h >= 170) return 'R';
	else if (h < 25) return 'O';
	else if (h < 35) return 'Y';
	else if (h < 85) return 'G';
	else if (h >= 105 && h < 135) return 'B';
	else return '?';
}

void Sensor::scan_face(const int face_index, const int image_size) {
	image[face_index] = capture_image("/home/leonardo/Pictures/2x2x2_cube/face_" + to_string(face_index) + ".jpg", image_size, image_size);
	Mat hsv_image;
	cvtColor(image[face_index], hsv_image, COLOR_BGR2HSV);
	for (const Point& point : points) {
		Vec3b hsv = hsv_image.at<Vec3b>(point);
		state += classify_color(hsv);
	}
}

const bool Sensor::has_error() const {
	map<char, int> color_count = { {'R', 0}, {'G', 0}, {'O', 0}, {'Y', 0}, {'B', 0}, {'W', 0} };
	for (const char color : state) {
		if (color != '?') {
			color_count[color]++;
		}
	}
	for (const auto& [color, count] : color_count) {
		if (count != 4) {
			return true;
		}
	}
	return false;
}
