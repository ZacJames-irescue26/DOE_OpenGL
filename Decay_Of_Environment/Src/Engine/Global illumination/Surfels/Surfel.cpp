#include "Surfel.h"

Surfel::Surfel(const glm::vec3& position, const glm::vec3& normal, const glm::vec4& albedo, float area)
	:position(position), normal(normal), albedo(albedo), area(area)
{

}

Surfel::Surfel(const glm::vec3& position, const glm::vec3& normal)
	:Surfel(position, normal, glm::vec4(1.0f,1.0f,1.0f,1.0f), 0.0f)
{

}

