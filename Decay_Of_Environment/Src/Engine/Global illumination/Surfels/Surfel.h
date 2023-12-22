#pragma once
#include "Engine/config.h"

struct Surfel
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec4 albedo;
	float area = 0.0f;

	Surfel() = default;
	Surfel(const glm::vec3& position, const glm::vec3& normal, const glm::vec4& albedo, float area);
	Surfel(const glm::vec3& position, const glm::vec3& normal);

};