#pragma once
#include "Triangle3D.h"

struct Plane {
	float distance;
	glm::vec3 normal;

	Plane();

	Plane(float d, const glm::vec3& n);

	Plane(const Triangle3D& triangle);
};