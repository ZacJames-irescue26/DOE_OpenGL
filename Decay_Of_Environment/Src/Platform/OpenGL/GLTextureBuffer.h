#pragma once

#include "GLBuffer.h"

template<typename DataType>
class GLTextureBuffer : public GLBuffer<DataType> {
private:
	GLenum mInternalFormat;

public:
	template<template<class...> class ContinuousContainer>
	static auto Create(const ContinuousContainer<DataType>& data, GLenum internalFormat) {
		return GLTextureBuffer(data.data(), data.size(), internalFormat);
	}

	GLTextureBuffer(const DataType* data, uint64_t count, GLenum internalFormat)
		: GLBuffer<DataType>(data, count, GL_TEXTURE_BUFFER, GL_STATIC_DRAW),
		mInternalFormat(internalFormat) {}

	void bind() const {
		GLBuffer<DataType>::bind();
		glTexBuffer(GL_TEXTURE_BUFFER, mInternalFormat, GLBuffer<DataType>::mName);
	}
};