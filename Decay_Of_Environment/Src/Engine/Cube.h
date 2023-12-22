#pragma once
#include "config.h"

struct CubeModelCreateInfo {
	glm::vec3 size;
};

class Cube {
public:
	unsigned int VBO, VAO, vertexCount;
	std::vector<float> vertices;

	Cube(CubeModelCreateInfo* createInfo);
	~Cube();
};