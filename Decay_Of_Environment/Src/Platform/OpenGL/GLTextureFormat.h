#pragma once
#include "glad/glad.h"

struct GLTextureFormat {
	GLint internalFormat;
	GLenum inputPixelFormat;
	GLenum inputPixelType;
};