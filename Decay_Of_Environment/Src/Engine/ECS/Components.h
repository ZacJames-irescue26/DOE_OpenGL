#pragma once

#include "Engine/config.h"
#include "Engine/Assimp/Model.h"
#include "../animation.h"
#include "../animator.h"
#include "../Camera.h"
#include "Jolt/Jolt.h"

enum UnitType
{
	Worker,
	Trooper
};

struct UnitStats
{
	std::shared_ptr<Model> UnitModel = nullptr;
	int Team = 0;
	int Owner = 0;
	float MaxHealth = 10.0f;
	float Currenthealth = 10.0f;
	float AttackDamage = 2.0f;
	float AttackRange = 1.0f;
	float BuildTime = 10.0f;
	bool ReachedTarget = true;
};

enum BuildingType
{
	MCVBuilding,
	Barracks,
	Factory
};
struct BuildingStats
{
public:
	int Team = 0;
	int Owner = 0;
	float MaxHealth = 10.0f;
	float Currenthealth = 10.0f;
	BuildingType Type;
	float BuildTime = 5.0f;

};

class Component
{
public:
	bool has = false;
};
class CTransform : public Component
{
public:
	glm::vec3 pos = { 0.0, 0.0, 0.0 };
	glm::vec3 prevPos = { 0.0, 0.0, 0.0 };
	glm::vec3 scale = { 1.0, 1.0, 1.0 };
	glm::vec3 velocity = { 0.0, 0.0,0.0 };
	glm::vec3 rotation = {0.0f, 0.0f, 0.0f};
	float MaxSpeed = 10.0f;
	glm::mat4 modelTransform = glm::mat4(1.0f);

	CTransform() {}
	CTransform(const glm::vec3 p)
		:pos(pos) {}

	CTransform(const glm::vec3& pos, const glm::vec3& velocity, const glm::vec3& scale, glm::vec3 rot)
		:pos(pos), prevPos(pos), velocity(velocity), scale(scale), rotation(rot) {}
	glm::mat4 GetTransform()
	{

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos)
		*glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0)) //rotation x = 0.0 degrees
		*glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0)) //rotation y = 0.0 degrees
		*glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1)) //rotation z = 0.0 degrees
		*glm::scale(glm::mat4(1.0f), scale);
		return transform;
	}

	glm::mat4 normalMatrix()
	{
		return glm::transpose(glm::inverse(GetTransform()));
	}

};

class CModel : public Component
{
public:
	CModel() { m_model = std::make_shared<Model>();}
	std::shared_ptr<Model> m_model;
	int RADIOSITY_TEXTURE_SIZE = 32;
	CModel(std::shared_ptr <Model> model)
		:m_model(model) 
	{	
	}
	
};

class CInput : public Component
{
public:
	bool up = false;
	bool left = false;
	bool right = false;
	bool down = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput() {}
};

class CUnitstats : public Component
{

	
public:
	/*int team = 0;*/
	glm::vec3 MovePosition = {1.0f,1.0f,1.0f};
	UnitType type;
	UnitStats stats;
};

class CBuildingStats : public Component
{


public:
	BuildingType type;
	BuildingStats stats;
};

class CBoxCollider : public Component
{
public:
	

};

class CLight : public Component
{
public:
	
};

class CRadiosity : public Component
{
public:
	
};

class CTest : public Component
{
public:
};

class CAnimation : public Component
{
public:
	CAnimation() {}
	CAnimation(std::shared_ptr<Animation> a)
		: animation(a)/*, animator(&a)*/ 
	{
		animator = std::make_shared<Animator>();
	}
	std::shared_ptr<Animation> animation;
	std::shared_ptr<Animator> animator;
};

class CStaticMesh : public Component
{
	
};

class CCamera : public Component
{
public:
	CCamera() {camera = new Camera(); }
	Camera* camera;
	//void CreateCamera() {camera = new Camera(); }
};




