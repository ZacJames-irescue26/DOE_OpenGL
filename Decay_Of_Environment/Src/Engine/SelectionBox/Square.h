#pragma once

#include "../config.h"
struct SquareModelCreateInfo {
	glm::vec3 size;
};
class SquareModel {
public:
	unsigned int m_VBO, m_VAO, EBO, vertexCount;
	std::vector<float> vertices;
	glm::vec2 m_firstpoint = glm::vec2{ -0.5f,  0.5f };
	glm::vec2 m_secondpoint = glm::vec2{ 0.5f, -0.5f };
	SquareModel();
	void update();
	~SquareModel();
	bool ShowSelectionBox = false;
};