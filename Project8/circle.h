#pragma once
#include "point.h"
#include <string>

class Circle {
public:
	int radius, z;
	Point center;
	std::string id;
	Circle(Point c, int r, int z, std::string id) : center(c), radius(r), z(z), id(id)  {}
};