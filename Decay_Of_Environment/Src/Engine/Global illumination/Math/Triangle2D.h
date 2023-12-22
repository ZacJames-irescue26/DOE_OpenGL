#pragma once
#include "Triangle.h"
#include "Rect2D.h"

struct Triangle2D : public Triangle<glm::vec2> {
	using Triangle<glm::vec2>::Triangle;

	float area() const override;

	Rect2D boundingRect() const;

	std::array<Triangle2D, 4> split() const;
};