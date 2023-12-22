#pragma once
#include "config.h"
#include "Render_Model.h"

class Sphere : public RenderModel
{

public:
	Sphere(int resolution, float radius);
	~Sphere();

	std::vector<GLfloat> vertexDataVector;
	std::vector<GLuint> indices;
	glm::vec3* normals;

};