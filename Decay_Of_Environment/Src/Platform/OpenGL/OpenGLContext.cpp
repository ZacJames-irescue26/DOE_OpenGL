#include "pch.h"

#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
	: m_WindowHandle(windowHandle)
{
}

void OpenGLContext::Init()
{
	glfwMakeContextCurrent(m_WindowHandle);
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	//DOE_CORE_ASSERT(status, "Failed to initialze glad");

	/*DOE_INFO("OpenGL Info:");
	DOE_INFO("Vendor: {0}", glGetString(GL_VENDOR));
	DOE_INFO("Renderer: {0}", glGetString(GL_RENDERER));
	DOE_INFO("Version: {0}", glGetString(GL_VERSION));*/
}

void OpenGLContext::SwapBuffers()
{
	glfwSwapBuffers(m_WindowHandle);
}