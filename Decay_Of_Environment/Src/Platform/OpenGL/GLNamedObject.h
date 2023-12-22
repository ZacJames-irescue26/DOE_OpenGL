#pragma once
#include "glad/glad.h"

#include "Engine/config.h"

class GLNamedObject {
protected:
	GLuint mName = -1;

public:
	GLNamedObject() = default;

	GLNamedObject(GLuint);

	GLNamedObject(GLNamedObject&&);

	GLNamedObject& operator=(GLNamedObject&&);

	GLNamedObject(const GLNamedObject&) = delete;

	GLNamedObject& operator=(const GLNamedObject&) = delete;

	virtual ~GLNamedObject() = 0;

	void swap(GLNamedObject&);

	GLuint name() const;
};

void swap(GLNamedObject&, GLNamedObject&);