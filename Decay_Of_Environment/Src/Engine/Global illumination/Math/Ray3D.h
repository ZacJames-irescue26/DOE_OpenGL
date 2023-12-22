#pragma once

#include "AxisAlignedBox3D.h"
#include "Parallelogram3D.h"

struct Ray3D {
	glm::vec3 origin;
	glm::vec3 direction;

	Ray3D(const glm::vec3& origin, const glm::vec3& direction);

	Ray3D transformedBy(const Transformation& t) const;

	Ray3D transformedBy(const glm::mat4& m) const;
};