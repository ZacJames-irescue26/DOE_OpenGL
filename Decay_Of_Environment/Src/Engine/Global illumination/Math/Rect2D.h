#pragma once
#include "Engine/config.h"
#include "Size2D.h"

struct Rect2D {
	glm::vec2 origin;
	Size2D size;

	static const Rect2D& zero();

	Rect2D() = default;

	Rect2D(const Size2D& size);

	Rect2D(const glm::vec2& origin, const Size2D& size);

	float minX() const;

	float minY() const;

	float maxX() const;

	float maxY() const;
};