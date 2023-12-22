#pragma once
#include "Engine/config.h"
#include "Engine/ECS/EntityManager.h"
#include "Engine/Timestep.h"
#include "Engine/Assimp/Model.h"
#include "Unit.h"
#include "Building.h"
#include "Engine/ECS/Components.h"
class UnitController
{
public:
	UnitController();
	std::shared_ptr<EntityManager> m_Manager;
	/*std::vector<std::shared_ptr<Unit>> Unitarray;
	std::vector<std::shared_ptr<Building>> BuildingArray;*/
	std::vector<std::shared_ptr<Entity>> SelectedEntites;

	void MoveUnit(Timestep ts);
	void SelectUnit(std::shared_ptr<Entity> unitToAdd);
	std::shared_ptr<Entity> CreateUnit(UnitType type, int Team, int owner, glm::vec3 Pos, glm::vec3 Scale, glm::vec3 rot, EntityManager* manager);
	//bool ReachedDestination = true;
};