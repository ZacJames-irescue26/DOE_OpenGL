#pragma once
#include "Engine/Camera.h"
#include "Engine/Engine.h"
#include "Engine/ECS/Components.h"
#include "Engine/ECS/Entity.h"


#define RECURSION_COUNT 200
#define RAY_RANGE 600

class MousePicker
{
public:
	MousePicker(Camera* cam, std::shared_ptr<Entity> plane);

	
	glm::vec3 GetCurrentRay();
	glm::vec3 CalculateMouseRay(float MouseX, float MouseY, Camera* cam);
	glm::vec2 GetNormalizedDeviceCoords(float MouseX, float MouseY);

	glm::vec4 ToEyeCoords(glm::vec4 ClipCoords);
	glm::vec3 ToWorldCoords(glm::vec4 EyeCoords);
	glm::vec3 getPointOnRay(glm::vec3 ray, float distance);
	glm::vec3 binarySearch(int count, float start, float finish, glm::vec3 ray);
	bool intersectionInRange(float start, float finish, glm::vec3 ray);
	bool isUnderGround(glm::vec3 testPoint);
	glm::vec3 currentRay = glm::vec3(0.0f);
	glm::vec3 CurrentTerrainPoint = glm::vec3(0.0f);
private:
	std::shared_ptr<Entity> Plane = nullptr;
	glm::mat4 ProjectionMatrix;
	glm::mat4 ViewMatrix;
	Camera* camera;

};