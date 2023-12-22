#include "pch.h"
#include "Square.h"

SquareModel::SquareModel() {

	/*float l = createInfo->size.x / 2;
	float w = createInfo->size.y / 2;
	float h = createInfo->size.z / 2;*/

	
	vertices = { {
			m_firstpoint.x, m_firstpoint.y, 0.0f, 0.0f, 1.0f, //top left
			m_secondpoint.x, m_firstpoint.y, 0.0f, 0.0f, 0.0f, //top right
			m_secondpoint.x, m_secondpoint.y, 0.0f, 1.0f, 0.0f, // bottom right
			m_firstpoint.x, m_firstpoint.y, 0.0f, 0.0f, 1.0f, //top left
			m_secondpoint.x, m_secondpoint.y, 0.0f, 1.0f, 0.0f, // bottom right
			m_firstpoint.x, m_secondpoint.y, 0.0f, 1.0f, 1.0f

		} };

	vertexCount = vertices.size() / 5;
	glCreateBuffers(1, &m_VBO);
	glCreateVertexArrays(1, &m_VAO);
	glVertexArrayVertexBuffer(m_VAO, 0, m_VBO, 0, 5 * sizeof(float));
	glNamedBufferStorage(m_VBO, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_STORAGE_BIT);
	glEnableVertexArrayAttrib(m_VAO, 0);
	glEnableVertexArrayAttrib(m_VAO, 1);
	glVertexArrayAttribFormat(m_VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(m_VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
	glVertexArrayAttribBinding(m_VAO, 0, 0);
	glVertexArrayAttribBinding(m_VAO, 1, 0);

}
void SquareModel::update()
{
	glm::vec2 BottomLeft = glm::min(m_firstpoint, m_secondpoint);
	glm::vec2 TopRight = glm::max(m_firstpoint, m_secondpoint);

	vertices =
	{
		BottomLeft.x, TopRight.y, 0.0f, 0.0f, 1.0f,             //top left
		TopRight.x, TopRight.y, 0.0f, 0.0f, 0.0f,              //top right
		TopRight.x, BottomLeft.y, 0.0f, 1.0f, 0.0f,           // bottom right
		BottomLeft.x, TopRight.y, 0.0f, 0.0f, 1.0f,          //top left
		TopRight.x, BottomLeft.y, 0.0f, 1.0f, 0.0f,         // bottom right
		BottomLeft.x, BottomLeft.y, 0.0f, 1.0f, 1.0f       // bottom left
	};

	vertexCount = vertices.size() / 5;
	
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),0,GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
	

	
}

SquareModel::~SquareModel() {
	glDeleteBuffers(1, &m_VBO);
	glDeleteVertexArrays(1, &m_VAO);
}