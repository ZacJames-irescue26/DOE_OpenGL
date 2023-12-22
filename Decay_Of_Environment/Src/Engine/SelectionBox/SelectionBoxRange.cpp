#include "pch.h"
#include "SelectionBoxRange.h"

RectangleModel::RectangleModel() {


	diagonal2cube();
	float vertices[] = {
		// positions          // texture coords
		pnt[0].x,pnt[0].y,pnt[0].z, 1.0f, 1.0f, // bottom
		pnt[1].x,pnt[1].y,pnt[1].z, 1.0f, 0.0f,
		pnt[2].x,pnt[2].y,pnt[2].z, 0.0f, 0.0f,
		pnt[3].x,pnt[3].y,pnt[3].z, 0.0f, 0.0f,
		pnt[4].x,pnt[4].y,pnt[4].z, 1.0f, 1.0f,
		pnt[5].x,pnt[5].y,pnt[5].z, 1.0f, 0.0f,
		pnt[6].x,pnt[6].y,pnt[6].z, 0.0f, 0.0f, //top
		pnt[7].x,pnt[7].y,pnt[7].z, 0.0f, 1.0f,
		/*Point 0: vec3(0.000000, 0.000000, 0.000000)
		Point 1 : vec3(-0.333333, 0.666667, 0.666667)
		Point 2 : vec3(0.333333, 0.388889, 1.277778)
		Point 3 : vec3(0.666667, -0.277778, 0.611111)
		Point 4 : vec3(0.666667, 0.611111, -0.277778)
		Point 5 : vec3(0.333333, 1.277778, 0.388889)
		Point 6 : vec3(1.000000, 1.000000, 1.000000)
		Point 7 : vec3(1.333333, 0.333333, 0.333333)*/
	};


	unsigned int indices[] = {
		// front
	   0, 1, 3,
	   1, 2, 3,
	   // back
	   4, 5, 7,
	   5, 6, 7,
	   // right
	   0, 1, 4,
	   1, 4, 5,
	   // left
	   2, 3, 7,
	   2, 6, 7,
	   // top
	   0, 3, 4,
	   3, 4, 7,
	   // bottom
	   1, 2, 5,
	   2, 5, 6

	};
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// tex coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	

	
}
void RectangleModel::Update()
{
	diagonal2cube();
	std::vector<float> vertices = {
		// positions          // texture coords
		pnt[0].x,pnt[0].y,pnt[0].z, 1.0f, 1.0f, // bottom
		pnt[1].x,pnt[1].y,pnt[1].z, 1.0f, 0.0f,
		pnt[2].x,pnt[2].y,pnt[2].z, 0.0f, 0.0f,
		pnt[3].x,pnt[3].y,pnt[3].z, 0.0f, 0.0f,
		pnt[4].x,pnt[4].y,pnt[4].z, 1.0f, 1.0f,
		pnt[5].x,pnt[5].y,pnt[5].z, 1.0f, 0.0f,
		pnt[6].x,pnt[6].y,pnt[6].z, 0.0f, 0.0f, //top
		pnt[7].x,pnt[7].y,pnt[7].z, 0.0f, 1.0f,
		/*Point 0: vec3(0.000000, 0.000000, 0.000000)
		Point 1 : vec3(-0.333333, 0.666667, 0.666667)
		Point 2 : vec3(0.333333, 0.388889, 1.277778)
		Point 3 : vec3(0.666667, -0.277778, 0.611111)
		Point 4 : vec3(0.666667, 0.611111, -0.277778)
		Point 5 : vec3(0.333333, 1.277778, 0.388889)
		Point 6 : vec3(1.000000, 1.000000, 1.000000)
		Point 7 : vec3(1.333333, 0.333333, 0.333333)*/
	};


	unsigned int indices[] = {
		// front
	   0, 1, 3,
	   1, 2, 3,
	   // back
	   4, 5, 7,
	   5, 6, 7,
	   // right
	   0, 1, 4,
	   1, 4, 5,
	   // left
	   2, 3, 7,
	   2, 6, 7,
	   // top
	   0, 3, 4,
	   3, 4, 7,
	   // bottom
	   1, 2, 5,
	   2, 5, 6

	};
	// world space positions of our cubes

	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), 0, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
}
RectangleModel::~RectangleModel() {
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &EBO);
}


//---------------------------------------------------------------------------
                    // output cube points
//---------------------------------------------------------------------------
// return rotated p around axis (center is (0,0,0))
glm::vec3 RectangleModel::rotate(float ang, glm::vec3 axis, glm::vec3 p)
{
	float c = glm::cos(ang), s = glm::sin(ang);// precompute goniometrics
	glm::vec3 u, v, w;                 // bazis vectors
	float x, y, z, xx, yy;
	w = glm::normalize(axis);          // w is rotation axis
	u = glm::vec3(1.0, 0.0, 0.0);        // u = any unit align vector not parallel to w
	if (fabs(glm::dot(u, w)) > 0.75) u = glm::vec3(0.0, 1.0, 0.0);
	u = glm::cross(u, w);               // make u paralel to w and align vector
	v = glm::cross(u, w);               // make v paralel to u and w
	// convert to basis vectors local coordinates
	xx = glm::dot(p, u);
	yy = glm::dot(p, v);
	z = glm::dot(p, w);
	// rotate XY plane
	x = xx * c - yy * s;
	y = xx * s + yy * c;
	// convert back to global coordinates
	p = (x * u) + (y * v) + (z * w);
	return p;
}
//---------------------------------------------------------------------------
// compute pnt[] from A,B
void RectangleModel::diagonal2cube()
{
	const float a0 = 1.230959417340774682134929178248;    // 70.5288 deg
	const float a1 = 1.5707963267948966192313216916398;   // 90.0000 deg
	float a;
	glm::vec3 p, u, v, U, V, n;
	p = glm::vec3(0.5) * (A + B);        // center of cube
	u = A - p;              // half diagonal from center to pnt[0]
	a = glm::length(u);        // half size of cube's diagonal
	u = glm::normalize(u);     // normalize u
	n = glm::vec3(1.0, 0.0, 0.0);// n = any unit align vector not parallel to u
	if (fabs(dot(u, n)) > 0.75) n = glm::vec3(0.0, 1.0, 0.0);
	n = glm::cross(u, n);       // make n paralel to u and align vector
	u *= a;               // rescale u back to original size
	v = rotate(a0, n, u);   // v is u rotated by 70.5288 deg around n
	n = u - v;              // n is axis perpendicular to n and in the same plane as u,v
	U = rotate(a1, n, u);   // U is u rotated by 90.0 deg around n
	V = rotate(a1, n, v);   // V is v rotated by 90.0 deg around n
	pnt[0] = p + u;         // construct the cube vertexes
	pnt[6] = p - u;
	pnt[1] = p + v;
	pnt[7] = p - v;
	pnt[3] = p + U;
	pnt[5] = p - U;
	pnt[2] = p + V;
	pnt[4] = p - V;
}
//---------------------------------------------------------------------------