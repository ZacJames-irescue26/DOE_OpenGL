#pragma once
#include "Engine/GraphicsContext.h"

struct GLFWwindow;

class OpenGLContext : public GraphicsContext
{
public:
	OpenGLContext(GLFWwindow* windowHandle);

	virtual void Init() override;
	virtual void SwapBuffers() override;

private:
	GLFWwindow* m_WindowHandle;
};