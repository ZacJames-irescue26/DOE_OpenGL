#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include"shader.h"
#include "Timestep.h"

class Camera
{
public:

	enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	// Stores the main vectors of the camera
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 2.0f);
	glm::vec3 Orientation = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::mat4 cameraMatrix = glm::mat4(1.0f);
	const float ZOOM = 45.0f;
	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = false;

	// Stores the width and height of the window
	float width;
	float height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed = 0.01f;
	float sensitivity = 10.0f;

	// Camera constructor to set up initial values
	Camera();

	const glm::mat4& GetProjectionMatrix() const;
	const glm::mat4& GetViewMatrix() const;
	// Updates and exports the camera matrix to the Vertex Shader
	void updateMatrix(float FOVdeg, float nearPlane, float farPlane);
	
	void RecalculateProjection();
	void Matrix(Shader& shader, const char* uniform);

	float GetPerspectiveVerticalFOV() const { return m_PerspectiveFOV; }
	void SetPerspectiveVerticalFOV(float verticalFov) { m_PerspectiveFOV = verticalFov; RecalculateProjection(); }
	float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
	void SetPerspectiveNearClip(float nearClip) {m_PerspectiveNear = nearClip; RecalculateProjection();}
	float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
	void SetPerspectiveFarClip(float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }


	glm::quat GetOrientation() {return glm::quat(glm::vec3(Orientation));}

	/*float GetOrthographicSize() const { return m_OrthographicSize; }
	void SetOrthographicSize(float size) { m_OrthographicSize = size; RecalculateProjection(); }
	float GetOrthographicNearClip() const { return m_OrthographicNear; }
	void SetOrthographicNearClip(float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
	float GetOrthographicFarClip() const { return m_OrthographicFar; }
	void SetOrthographicFarClip(float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }*/

	ProjectionType GetProjectionType() const { return m_ProjectionType; }
	void SetProjectionType(ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

	// Handles camera inputs
	void Inputs(GLFWwindow* window, Timestep ts);

	ProjectionType m_ProjectionType = ProjectionType::Perspective;
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection = glm::mat4(1.0f);

	float m_PerspectiveFOV = glm::radians(45.0f);
	float m_PerspectiveNear = 0.01f, m_PerspectiveFar = 1000.0f;

	float m_OrthographicSize = 10.0f;
	float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

	float m_AspectRatio = 0.0f;

};