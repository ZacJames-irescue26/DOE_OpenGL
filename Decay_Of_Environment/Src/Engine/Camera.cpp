#include "camera.h"
#include "pch.h"
#include "Engine.h"


Camera::Camera()
{
	RecalculateProjection();

}
const glm::mat4& Camera::GetProjectionMatrix() const 
{
	return projection;
}
const glm::mat4& Camera::GetViewMatrix() const 
{
	return view;
}

void Camera::updateMatrix(float FOVdeg, float nearPlane, float farPlane)
{
	view = glm::mat4(1.0f);
	projection = glm::mat4(1.0f);

	view = glm::lookAt(Position, Position + Orientation, Up);
	//view = glm::lookAt(glm::vec3(0.0f,0.0f,3.0f), glm::vec3(0.0f, 0.0f, 0.0f), Up);

	projection = glm::perspective(glm::radians(FOVdeg), (float)(width / height), nearPlane, farPlane);
	
	
}

void Camera::RecalculateProjection()
{
	width = (float)(GameEngine::Get().GetWindow().GetWidth());
	height = (float)(GameEngine::Get().GetWindow().GetHeight());
	if (m_ProjectionType == ProjectionType::Perspective)
	{
		view = glm::lookAt(Position, Position + Orientation, Up);
		if (width == 0.0f || height == 0.0f)
		{
			return;
		}
		projection = glm::perspective(m_PerspectiveFOV, (width/ height), m_PerspectiveNear, m_PerspectiveFar);
		cameraMatrix = projection * view;

	}
	else
	{
		view = glm::lookAt(Position, Position + Orientation, Up);
		float orthoLeft = -m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoRight = m_OrthographicSize * m_AspectRatio * 0.5f;
		float orthoBottom = -m_OrthographicSize * 0.5f;
		float orthoTop = m_OrthographicSize * 0.5f;

		projection = glm::ortho(orthoLeft, orthoRight,
			orthoBottom, orthoTop, m_OrthographicNear, m_OrthographicFar);
		cameraMatrix = projection * view;

	}

}




void Camera::Matrix(Shader& shader, const char* uniform)
{
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window, Timestep ts)
{
	// Handles key inputs
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		Position +=  ts.GetMiliSeconds() * speed  * Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		Position += ts.GetMiliSeconds() *  speed * -glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		Position += ts.GetMiliSeconds() *  speed * -Orientation;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		Position += ts.GetMiliSeconds() * speed * glm::normalize(glm::cross(Orientation, Up));
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		Position += ts.GetMiliSeconds() * speed * Up;
	}
	//if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	//{
	//	Position += speed * -Up;
	//}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		speed = 0.4f;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
	{
		speed = 0.01f;
	}

	// Handles mouse inputs
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{

		// Hides mouse cursor
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		// Prevents camera from jumping on the first click
		if (firstClick)
		{
			glfwSetCursorPos(window, (width / 2), (height / 2));
			firstClick = false;
		}

		// Stores the coordinates of the cursor
		double mouseX;
		double mouseY;
		// Fetches the coordinates of the cursor
		glfwGetCursorPos(window, &mouseX, &mouseY);

		// Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
		// and then "transforms" them into degrees 
		float rotX = sensitivity * (float)(mouseY - (height / 2)) / height;
		float rotY = sensitivity * (float)(mouseX - (width / 2)) / width;

		// Calculates upcoming vertical change in the Orientation
		glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));

		// Decides whether or not the next vertical Orientation is legal or not
		if (abs(glm::angle(newOrientation, Up) - glm::radians(90.0f)) <= glm::radians(85.0f))
		{
			Orientation = newOrientation;
		}

		// Rotates the Orientation left and right
		Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);

		// Sets mouse cursor to the middle of the screen so that it doesn't end up roaming around
		glfwSetCursorPos(window, (width / 2), (height / 2));

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		// Unhides cursor since camera is not looking around anymore
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// Makes sure the next time the camera looks around it doesn't jump
		firstClick = true;
	}
}