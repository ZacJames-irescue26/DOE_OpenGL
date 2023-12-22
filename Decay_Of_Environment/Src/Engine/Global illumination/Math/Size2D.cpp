#include "Size2D.h"

const Size2D& Size2D::zero() {
	static Size2D zero = { 0, 0 };
	return zero;
}

const Size2D& Size2D::unit() {
	static Size2D unit = { 1, 1 };
	return unit;
}

Size2D::Size2D(float w, float h)
	:
	width(w),
	height(h) {
}

Size2D::Size2D(float side)
	:
	width(side),
	height(side) {
}

#pragma mark - Equality

bool Size2D::operator==(const Size2D& rhs) {
	return width == rhs.width && height == rhs.height;
}

bool Size2D::operator!=(const Size2D& rhs) {
	return !operator==(rhs);
}

#pragma mark - Utils

Size2D Size2D::transformedBy(const glm::vec2& vector) const {
	return Size2D(width * vector.x, height * vector.y);
}

Size2D Size2D::makeUnion(const Size2D& size) {
	return { std::max(width, size.width), std::max(height, size.height) };
}