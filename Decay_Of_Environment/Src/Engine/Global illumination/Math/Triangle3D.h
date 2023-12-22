#pragma once
#include "Triangle.h"

struct AxisAlignedBox3D;

struct Triangle3D : public Triangle<glm::vec3>
{
	using Triangle<glm::vec3>::Triangle;

	float area() const override;

	AxisAlignedBox3D boundingBox() const;

	glm::vec3 normal;

	std::array<Triangle3D, 4> split() const;
};