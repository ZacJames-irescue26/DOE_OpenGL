#pragma once
#include "config.h"
#include "shader.h"

enum class FramebufferTextureFormat
{
	None = 0,

	// Color
	RGBA8,
	RED_INTEGER,
	RGB32F,
	// Depth/stencil
	DEPTH24STENCIL8,

	// Defaults
	Depth
};

struct FramebufferTextureSpecification
{
	FramebufferTextureSpecification() = default;
	FramebufferTextureSpecification(FramebufferTextureFormat format)
		: TextureFormat(format) {}

	FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
	// TODO: filtering/wrap
};

struct FramebufferAttachmentSpecification
{
	FramebufferAttachmentSpecification() = default;
	FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments)
		: Attachments(attachments) {}

	std::vector<FramebufferTextureSpecification> Attachments;
};

struct FramebufferSpecification
{
	uint32_t Width = 0, Height = 0;
	FramebufferAttachmentSpecification Attachments;
	uint32_t Samples = 1;

	bool SwapChainTarget = false;
};

class OpenGLFramebuffer
{
public:
	OpenGLFramebuffer(const FramebufferSpecification& spec);
	virtual ~OpenGLFramebuffer();

	void Invalidate();

	void Bind();
	void Unbind();

	void Resize(uint32_t width, uint32_t height);
	int ReadPixel(uint32_t attachmentIndex, int x, int y);

	void ClearAttachment(uint32_t attachmentIndex, int value);


	void ActivateAsTexture(std::shared_ptr<Shader> shader, const std::string SamplerName, const int textureUnit);
	uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const { return m_ColorAttachments[index]; }
	uint32_t GetDepthAttachment() const { return m_DepthAttachment; }
	const FramebufferSpecification& GetSpecification() const { return m_Specification; }
private:
	uint32_t m_RendererID = 0;
	FramebufferSpecification m_Specification;

	std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
	FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

	std::vector<uint32_t> m_ColorAttachments;
	uint32_t m_DepthAttachment = 0;
};

class FBO {
public:
	GLuint width, height, frameBuffer, textureColorBuffer, attachment, rbo;
	void ActivateAsTexture(const int shaderProgram, const std::string glSamplerName, const int textureUnit = GL_TEXTURE0);
	FBO(
		GLuint w, GLuint h, GLenum magFilter = GL_NEAREST, GLenum minFilter = GL_NEAREST,
		GLint internalFormat = GL_RGB16F, GLint format = GL_FLOAT, GLint wrap = GL_REPEAT);
	~FBO();
private:
	GLuint generateAttachment(GLuint w, GLuint h, GLboolean depth, GLboolean stencil, GLenum magFilter, GLenum minFilter, GLenum wrap);
};