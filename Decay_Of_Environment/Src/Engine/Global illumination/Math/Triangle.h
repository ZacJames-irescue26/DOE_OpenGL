#pragma once
#include "Engine/config.h"
//need templates so can use glm::Vec3 for each point

template<typename Point>
struct Triangle
{
	union
	{
		struct
		{
			Point a;
			Point b;
			Point c;
		};
		struct
		{
			Point p1;
			Point p2;
			Point p3;
		};
		std::array<Point, 3> points;

		std::array<float, 3 * sizeof(Point) / sizeof(float)> values;
	};

	Triangle() = default;
	Triangle(const Point& p1, const Point p2, const Point& p3)
		:p1(p1), p2(p2), p3(p3)
		{}

	virtual float area() const = 0;
};