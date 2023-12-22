#include "Plane.h"
#include <glm/geometric.hpp>

#pragma mark - Lifecycle

Plane::Plane()
	:
	distance(0),
	normal({ 1.0, 0.0, 0.0 }) {
}

Plane::Plane(float d, const glm::vec3& n)
	:
	distance(d),
	normal(n) {
}

Plane::Plane(const Triangle3D& triangle) {
	normal = triangle.normal;
	distance = glm::dot(triangle.a, normal);
}