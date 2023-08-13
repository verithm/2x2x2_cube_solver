#ifndef SOLVER_HPP
#define SOLVER_HPP

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;

class Motor {
public:
	Motor(const int step_pin, const int dir_pin, const double step_angle = 1.8);
	void move(const int angle, const int delay = 10) const;
private:
	const int step_pin, dir_pin;
	const double step_angle;
};

class Sensor {
public:
	const string& scan_cube(const Motor* motor, const int image_size = 2464);
private:
	Mat image[6];
	vector<Point> points;
	string state;
	void set_points(const int image_size);
	const Mat capture_image(const string& image_path, const int image_width, const int image_height) const;
	const char classify_color(const Vec3b& hsv) const;
	void scan_face(const int face_index, const int image_size);
	const bool has_error() const;
};

#endif
