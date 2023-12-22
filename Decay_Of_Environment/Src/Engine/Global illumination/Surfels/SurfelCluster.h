#pragma once
#include "Engine/config.h"

struct SurfelCluster
{
	uint32_t surfelOffset = 0;
	uint32_t surfelCount = 0;
	glm::vec3 center;

	SurfelCluster() = default;

	SurfelCluster(size_t offset, size_t count);

};