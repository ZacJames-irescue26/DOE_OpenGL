#pragma once
#include "../config.h"



class RectangleModel {
public:
	unsigned int VBO, VAO, EBO, vertexCount;
	std::vector<float> vertices;
	std::vector<unsigned int> cubeIndices;
	GLuint elementBuffer;
	glm::vec3 FirstClick;
	glm::vec3 SecondClick;
	RectangleModel();
	void Update();
	float l;
	float w;
	float h;
	~RectangleModel();
	glm::vec3 rotate(float ang, glm::vec3 axis, glm::vec3 p);
	void diagonal2cube();
	glm::vec3 A = glm::vec3(0.0f, 0.0f, 0.0f);    // input points
	glm::vec3 B = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec3 pnt[8];
};