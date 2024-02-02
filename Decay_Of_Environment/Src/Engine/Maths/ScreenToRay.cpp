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

glm::vec3 MousePicker::CalculateMouseRay(float MouseX, float MouseY, Camera* cam, glm::vec2 windowsize)
{
	WindowSize = windowsize;
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
	float x = (2.0f * MouseX) / WindowSize.x - 1.0f;
	float y = (2.0f * MouseY) / WindowSize.y - 1.0f;
	
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

bool MousePicker::intersectionInRange(glm::vec3 start, float finish, glm::vec3 ray) {
	glm::vec3 startPoint = start;
	glm::vec3 endPoint = getPointOnRay(ray, finish);
	if (!isUnderGround(startPoint) && isUnderGround(endPoint)) {
		return true;
	}
	else {
		return false;
	}
}

bool MousePicker::intersectionInRange(float start, float finish, glm::vec3 ray)
{
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

void MousePicker::RayUpdate(MousePicker* Ray, glm::vec2 Pos, glm::vec2 WindowSize)
{
	//Ray->currentRay = Ray->CalculateMouseRay(Pos.x, Pos.y, camera);
	currentRay = setFromCamera(Pos, camera, WindowSize);
	if (Ray->intersectionInRange(0, RAY_RANGE, Ray->currentRay))
	{
		Ray->CurrentTerrainPoint = Ray->binarySearch(0, 0, RAY_RANGE, Ray->currentRay);
	}
	else
	{
		Ray->CurrentTerrainPoint = glm::vec3(0.0f);
	}
}

glm::vec3 MousePicker::setFromCamera(const glm::vec2 coords, Camera* camera, glm::vec2 WindowSize) {
	// 2d Viewport Coordinates
	float x = (2.0f * coords.x) / WindowSize.x - 1.0f;
	float y = 1.0f - (2.0f * coords.y) / WindowSize.y;

	// 3D Normalised Device Coordinates
	float z = 1.0f; // the camera looks on the negative z axis
	glm::vec3 rayNds = glm::vec3(x, y, z);

	// 4D Homogeneous Clip Coordinates
	glm::vec4 rayClip = glm::vec4(rayNds, 1.0);

	// 4D Eye (Camera) Coordinates
	glm::vec4 rayEye = glm::inverse(camera->GetProjectionMatrix()) * rayClip;

	// Now, we only needed to un-project the x,y part, so let's manually set the z,w part to mean "forwards, and not a point". From http://antongerdelan.net/opengl/raycasting.html
	rayEye = glm::vec4(rayEye.x, rayEye.y, -1.0, 0.0);

	// 4D World Coordinates
	glm::vec3 rayWorld = glm::vec3(glm::inverse(camera->GetViewMatrix()) * rayEye);

	rayWorld = glm::normalize(rayWorld);
	//std::cout << glm::to_string(rayWorld) << std::endl;
	return rayWorld;

};


//void MousePicker::DragTheObject(const glm::vec2 coords, glm::vec2 WindowSize)
//{
//	glm::mat4 Projection = camera->GetProjectionMatrix();
//	glm::mat4 ProjectionInv = glm::inverse(Projection);
//
//	// Step 1 - Viewport to NDC
//	float mo
// 
// 
// 
// use_x = (float)coords.x;
//	float mouse_y = (float)coords.y;
//
//	float ndc_x = (2.0f * mouse_x) / WindowSize.x - 1.0f;
//	float ndc_y = 1.0f - (2.0f * mouse_y) / WindowSize.y; // flip the Y axis
//
//	// Step 2 - NDC to view (my version)
//	float focal_length = 1.0f / tanf(glm::radians(45.0f / 2.0f));
//	float ar = (float)WindowSize.x / (float)WindowSize.y;
//	glm::vec3 ray_view(ndc_x / focal_length, (ndc_y * ar) / focal_length, 1.0f);
//
//	// Step 2 - NDC to view (Anton's version)
//	glm::vec4 ray_ndc_4d(ndc_x, ndc_y, 1.0f, 1.0f);
//	glm::vec4 ray_view_4d = ProjectionInv * ray_ndc_4d;
//
//	// Step 3 - intersect view vector with object Z plane (in view)
//	glm::vec4 view_space_intersect = glm::vec4(ray_view * m_objViewSpacePos.z, 1.0f);
//
//	// Step 4 - View to World space
//	Matrix4f View = m_pGameCamera->GetMatrix();
//	Matrix4f InvView = View.Inverse();
//	Vector4f point_world = InvView * view_space_intersect;
//	m_worldPos[m_clicked_object_id] = Vector3f(point_world);
//}