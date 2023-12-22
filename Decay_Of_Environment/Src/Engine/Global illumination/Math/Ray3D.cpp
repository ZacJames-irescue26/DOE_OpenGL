#include "Ray3D.h"

#pragma mark - Lifecycle

Ray3D::Ray3D(const glm::vec3& origin, const glm::vec3& direction)
	:
	origin(origin),
	direction(glm::normalize(direction)) {
}

#pragma mark - Public

Ray3D Ray3D::transformedBy(const Transformation& t) const {
	return transformedBy(t.modelMatrix());
}

Ray3D Ray3D::transformedBy(const glm::mat4& m) const {
	glm::vec3 newOrigin = m * glm::vec4(origin, 1.0);
	glm::vec3 newDirection = m * glm::vec4(direction, 0.0);
	return Ray3D(newOrigin, newDirection);
}