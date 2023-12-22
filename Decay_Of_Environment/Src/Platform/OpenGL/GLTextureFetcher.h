#pragma once

#include "GLTexture.h"
//#include "GLTextureDataInterpreter.h"

class GLTextureFetcher {
protected:
	struct UnpackFormat {
		GLenum pixelFormat;
		GLenum pixelType;
	};

	Size2D mMipSize;

	GLTextureFetcher(const GLTexture& texture, int32_t mipLevel);

	UnpackFormat glUnpackFormat(GLTexture::Normalized format);

	UnpackFormat glUnpackFormat(GLTexture::Float format);

	UnpackFormat glUnpackFormat(GLTexture::Integer format);

	UnpackFormat glUnpackFormat(GLTexture::Depth format);

public:
	virtual ~GLTextureFetcher() = 0;
};