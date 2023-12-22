#pragma once

#include "GLNamedObject.h"
#include "GLTextureBuffer.h"
#include "GLTexture.h"

template<typename TextureFormat, TextureFormat Format, typename BufferDataType>
class GLBufferTexture : public GLNamedObject {
private:
	GLTextureBuffer<BufferDataType> mBuffer;

public:
	GLBufferTexture(const BufferDataType* data, uint64_t count)
		: mBuffer(data, count, GLTexture::glFormat(Format).internalFormat) {
		glGenTextures(1, &mName);
	}

	virtual ~GLBufferTexture() = 0;

	void bind() const {
		glBindTexture(GL_TEXTURE_BUFFER, mName);
		mBuffer.bind();
	}

	GLTextureBuffer<BufferDataType>& buffer() {
		return mBuffer;
	}

	const GLTextureBuffer<BufferDataType>& buffer() const {
		return mBuffer;
	}
};

template<typename TextureFormat, TextureFormat Format, typename BufferDataType>
GLBufferTexture<TextureFormat, Format, BufferDataType>::~GLBufferTexture() {
	glDeleteTextures(1, &mName);
}

// Specializing by data type

template<GLTexture::Integer Format, typename BufferDataType>
class GLIntegerBufferTexture : public GLBufferTexture<GLTexture::Integer, Format, BufferDataType> {
public:
	using GLBufferTexture<GLTexture::Integer, Format, BufferDataType>::GLBufferTexture;
};

template<GLTexture::Float Format, typename BufferDataType>
class GLFloatBufferTexture : public GLBufferTexture<GLTexture::Float, Format, BufferDataType> {
public:
	using GLBufferTexture<GLTexture::Float, Format, BufferDataType>::GLBufferTexture;
};