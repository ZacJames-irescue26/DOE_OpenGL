#pragma once

#include "GLTextureFetcher.h"
#include "GLTextureDataInterpreter.h"

template<class T, T U>
class GLTexture2D;

template<class TextureFormat, TextureFormat Format>
class GLTexture2DSampler : public GLTextureFetcher {
private:
	friend GLTexture2D<TextureFormat, Format>;

	std::unique_ptr<GLubyte> mPixelBuffer;

	GLTexture2DSampler(const GLTexture2D<TextureFormat, Format>& texture, uint8_t mipLevel)
		: GLTextureFetcher(texture, mipLevel),
		mPixelBuffer(new GLubyte[mMipSize.width * mMipSize.height * 4 * GLTextureDataInterpreter<TextureFormat, Format>::BytesPerChannel]) {

		auto format = glUnpackFormat(Format);
		glGetTexImage(GL_TEXTURE_2D, mipLevel, format.pixelFormat, format.pixelType, mPixelBuffer.get());
	}

protected:
	size_t bufferOffset(int32_t x, int32_t y) const {
		return (y * mMipSize.width + x) * 4;
	}

	size_t bufferOffset(const glm::vec2& normalizedCoords) const {
		int32_t x = normalizedCoords.x * (mMipSize.width - 1);
		int32_t y = normalizedCoords.y * (mMipSize.height - 1);
		return (y * mMipSize.width + x) * 4;
	}

public:
	auto sample(int32_t x, int32_t y) const {
		size_t offset = bufferOffset(x, y);
		void* buffer = mPixelBuffer.get();
		return GLTextureDataInterpreter<TextureFormat, Format>::DecodeData(buffer, offset);
	}

	auto sample(const glm::vec2& normalizedCoords) const {
		size_t offset = bufferOffset(normalizedCoords);
		void* buffer = mPixelBuffer.get();
		return GLTextureDataInterpreter<TextureFormat, Format>::DecodeData(buffer, offset);
	};
};