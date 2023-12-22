#include "pch.h"
#include "ScreenToRay.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/string_cast.hpp"




MousePicker::MousePicker(Camera* cam, std::shared_ptr<Entity> plane)
	:camera(cam), Plane(plane)
{

}


glm::vec3 MousePicker::GetCurrentRay()
{
	return currentRay;
}

glm::vec3 MousePicker::CalculateMouseRay(float MouseX, float MouseY, Camera* cam)
{
	camera = cam;
	glm::vec2 Normalizedcoords = GetNormalizedDeviceCoords(MouseX, MouseY);
	glm::vec4 ClipCoords = glm::vec4(Normalizedcoords.x, Normalizedcoords.y, -1.0f, 1.0f);
	glm::vec3 coords = glm::vec3(MouseX, GameEngine::Get().GetWindow().GetHeight() - MouseY, 1.0f);
	glm::vec4 EyeCoords = ToEyeCoords(ClipCoords);
	glm::vec3 WorldRay = ToWorldCoords(EyeCoords);
	
	
	return glm::vec3(WorldRay);

}

glm::vec2 MousePicker::GetNormalizedDeviceCoords(float MouseX, float MouseY)
{
	// in opengl coordinates between 1 and -1
	float x = (2.0f * MouseX) / GameEngine::Get().GetWindow().GetWidth() - 1.0f;
	float y = 1.0f - (2.0f * MouseY) / GameEngine::Get().GetWindow().GetHeight();
	return glm::vec2(x,y);
}

glm::vec4 MousePicker::ToEyeCoords(glm::vec4 ClipCoords)
{
	glm::mat4x4 InvertedProjection = glm::inverse(camera->projection);
	//transform matrix
	glm::vec4 EyeCoords = InvertedProjection * ClipCoords;
	return glm::vec4(EyeCoords.x, EyeCoords.y, -1.0f, 0.0f);

}

glm::vec3 MousePicker::ToWorldCoords(glm::vec4 EyeCoords)
{
	glm::mat4 InvertedView = glm::inverse(camera->view);
	
	//transform matrix
	glm::vec4 RayWorld = InvertedView * EyeCoords;
	glm::vec3 MouseRay = glm::vec3(RayWorld.x,RayWorld.y,RayWorld.z);
	glm::vec3 NormalizedMouseRay = glm::normalize(MouseRay);
	return NormalizedMouseRay;
}

glm::vec3 MousePicker::getPointOnRay(glm::vec3 ray, float distance) {
	glm::vec3 camPos = camera->Position;
	glm::vec3 start = glm::vec3(camPos.x, camPos.y, camPos.z);
	glm::vec3 scaledRay = glm::vec3(ray.x * distance, ray.y * distance, ray.z * distance);
	glm::vec3 add = start + scaledRay;
	return add;
}

glm::vec3 MousePicker::binarySearch(int count, float start, float finish, glm::vec3 ray) {
	float half = start + ((finish - start) / 2.0f);
	if (count >= RECURSION_COUNT) {
		glm::vec3 endPoint = getPointOnRay(ray, half);
		//Terrain terrain = getTerrain(endPoint.getX(), endPoint.getZ());
		auto Terrain = Plane;

		if (Terrain != nullptr) {
			return endPoint;
		}
		else {
			return glm::vec3(0.0f);
		}
	}
	if (intersectionInRange(start, half, ray)) {
		return binarySearch(count + 1, start, half, ray);
	}
	else {
		return binarySearch(count + 1, half, finish, ray);
	}
}

bool MousePicker::intersectionInRange(float start, float finish, glm::vec3 ray) {
	glm::vec3 startPoint = getPointOnRay(ray, start);
	glm::vec3 endPoint = getPointOnRay(ray, finish);
	if (!isUnderGround(startPoint) && isUnderGround(endPoint)) {
		return true;
	}
	else {
		return false;
	}
}

bool MousePicker::isUnderGround(glm::vec3 testPoint)
{

	//Terrain terrain = getTerrain(testPoint.getX(), testPoint.getZ());
	float height = 0;
	if (Plane != nullptr) {
		height = Plane->getComponent<CTransform>().pos.z;
	}
	if (testPoint.y < height) {
		return true;
	}
	else {
		return false;
	}
}

