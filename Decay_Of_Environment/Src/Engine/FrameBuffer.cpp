#include "FrameBuffer.h"
#include <glad/glad.h>
#include "shader.h"

static const uint32_t s_MaxFramebufferSize = 8192;

namespace Utils {

	static GLenum TextureTarget(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static void CreateTextures(bool multisampled, GLuint* outID, uint32_t count)
	{
		glCreateTextures(TextureTarget(multisampled), count, outID);
	}

	static void BindTexture(bool multisampled, uint32_t id)
	{
		glBindTexture(TextureTarget(multisampled), id);
	}

	static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, width, height, GL_FALSE);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
	}


	static void AttachDepthTexture(FramebufferTextureSpecification TextureFormat, uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
	{
		bool multisampled = samples > 1;
		if (multisampled)
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
		}
		else
		{
			
			if (TextureFormat.TextureFormat == FramebufferTextureFormat::Depth)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			}
			else
			{
				glTexStorage2D(GL_TEXTURE_2D, 1, format, width, height);
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
	}

	static void AttachShadowDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, attachmentType, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, id, 0);
	}

	static bool IsDepthFormat(FramebufferTextureFormat format)
	{
		switch (format)
		{
			case FramebufferTextureFormat::DEPTH24STENCIL8:  return true;
			case FramebufferTextureFormat::Depth: return true;
		}

		return false;
	}

	static GLenum HazelFBTextureFormatToGL(FramebufferTextureFormat format)
	{
		switch (format)
		{
			case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
			case FramebufferTextureFormat::RED_INTEGER: return GL_RED_INTEGER;
			case FramebufferTextureFormat::RGB32F: return GL_RGB32F;
		}

		assert(false);
		return 0;
	}

}

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec)
	: m_Specification(spec)
{
	for (auto spec : m_Specification.Attachments.Attachments)
	{
		if (!Utils::IsDepthFormat(spec.TextureFormat))
			m_ColorAttachmentSpecifications.emplace_back(spec);
		else
			m_DepthAttachmentSpecification = spec;
	}

	Invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
	glDeleteFramebuffers(1, &m_RendererID);
	glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
	glDeleteTextures(1, &m_DepthAttachment);
}

void OpenGLFramebuffer::Invalidate()
{
	if (m_RendererID)
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(m_ColorAttachments.size(), m_ColorAttachments.data());
		glDeleteTextures(1, &m_DepthAttachment);
		
		m_ColorAttachments.clear();
		m_DepthAttachment = 0;
	}

	glCreateFramebuffers(1, &m_RendererID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

	bool multisample = m_Specification.Samples > 1;

	// Attachments
	if (m_ColorAttachmentSpecifications.size())
	{
		m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
		Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

		for (size_t i = 0; i < m_ColorAttachments.size(); i++)
		{
			Utils::BindTexture(multisample, m_ColorAttachments[i]);
			switch (m_ColorAttachmentSpecifications[i].TextureFormat)
			{
				case FramebufferTextureFormat::RGBA8:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_RGBA8, GL_RGBA, m_Specification.Width, m_Specification.Height, i);
					break;
				case FramebufferTextureFormat::RED_INTEGER:
					Utils::AttachColorTexture(m_ColorAttachments[i], m_Specification.Samples, GL_R32I, GL_RED_INTEGER, m_Specification.Width, m_Specification.Height, i);
					break;
			}
		}
	}

	if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
	{
		Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
		Utils::BindTexture(multisample, m_DepthAttachment);
		switch (m_DepthAttachmentSpecification.TextureFormat)
		{
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				Utils::AttachDepthTexture(m_DepthAttachmentSpecification.TextureFormat, m_DepthAttachment, m_Specification.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specification.Width, m_Specification.Height);
				break;
			case FramebufferTextureFormat::Depth:
				//Utils::AttachDepthTexture(m_DepthAttachmentSpecification.TextureFormat, m_DepthAttachment, m_Specification.Samples, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, m_Specification.Width, m_Specification.Height);
				Utils::AttachShadowDepthTexture(m_DepthAttachment, m_Specification.Samples, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, m_Specification.Width, m_Specification.Height);
				break;
		}
	}

	if (m_ColorAttachments.size() > 1)
	{
		assert(m_ColorAttachments.size() <= 4);
		GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(m_ColorAttachments.size(), buffers);
	}
	else if (m_ColorAttachments.empty())
	{
		// Only depth-pass
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
	}

	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Framebuffer is incomplete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
	glViewport(0, 0, m_Specification.Width, m_Specification.Height);
}

void OpenGLFramebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height)
{
	if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize)
	{
		std::cout << "Attempted to rezize framebuffer to " << width << ", " << height << std::endl;
		return;
	}
	m_Specification.Width = width;
	m_Specification.Height = height;
	
	Invalidate();
}

int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
{
	assert(attachmentIndex < m_ColorAttachments.size());

	glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
	int pixelData;
	glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
	return pixelData;

}

void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
{
	assert(attachmentIndex < m_ColorAttachments.size());

	auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
	glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
		Utils::HazelFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
}

void OpenGLFramebuffer::ActivateAsTexture(std::shared_ptr<Shader> shader, const std::string SamplerName, const int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, GetColorAttachmentRendererID());
	shader->setInt(SamplerName, textureUnit);
}

FBO::FBO(GLuint w, GLuint h, GLenum magFilter, GLenum minFilter, GLint internalFormat, GLint format, GLint wrap)
	: width(w), height(h)
{
	GLint previousFrameBuffer;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFrameBuffer);

	// Init framebuffer.
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenTextures(1, &textureColorBuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);

	// Texture parameters.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, GL_RGBA, format, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h); // Use a single rbo for both depth and stencil buffer.
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, previousFrameBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { std::cerr << "FBO failed to initialize correctly." << std::endl; }
}

GLuint FBO::generateAttachment(GLuint w, GLuint h, GLboolean depth, GLboolean stencil, GLenum magFilter, GLenum minFilter, GLenum wrap)
{
	GLenum attachment_type;
	if (!depth && !stencil) {
		attachment_type = GL_RGB;
	}
	else if (depth && !stencil) {
		attachment_type = GL_DEPTH_COMPONENT;
	}
	else if (!depth && stencil) {
		attachment_type = GL_STENCIL_INDEX;
	}
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	if (!depth && !stencil) {
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, w, h, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

void FBO::ActivateAsTexture(const int shaderProgram, const std::string glSamplerName, const int textureUnit)
{
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
	glUniform1i(glGetUniformLocation(shaderProgram, glSamplerName.c_str()), textureUnit);
}

FBO::~FBO()
{
	glDeleteTextures(1, &textureColorBuffer);
	glDeleteFramebuffers(1, &frameBuffer);
}


