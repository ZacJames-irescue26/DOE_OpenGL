#include "pch.h"
#include "UnitController.h"

UnitController::UnitController()
{

}

void UnitController::MoveUnit(Timestep ts)
{
	for (auto units : SelectedEntites)
	{
		if (units->hasComponent<CTransform>() && units->hasComponent<CUnitstats>() && units->getComponent<CUnitstats>().stats.ReachedTarget == false)
		{
			glm::vec3 Velocity = units->getComponent<CTransform>().velocity;
			Velocity = { 0.0f,0.0f,0.0f };
			glm::vec3 NormalizedTarget = glm::normalize(units->getComponent<CTransform>().pos - units->getComponent<CUnitstats>().MovePosition);
			Velocity = -NormalizedTarget;
			if (glm::length(Velocity) > units->getComponent<CTransform>().MaxSpeed)
			{
				Velocity = glm::normalize(Velocity);
				Velocity = Velocity * units->getComponent<CTransform>().MaxSpeed;

			}
			if (glm::length(units->getComponent<CTransform>().pos - units->getComponent<CUnitstats>().MovePosition) <= 0.01f)
			{
				Velocity = glm::vec3(0.0f, 0.0f, 0.0f);
				units->getComponent<CUnitstats>().stats.ReachedTarget = true;
			}
			units->getComponent<CTransform>().pos += Velocity * ts.GetSeconds();
			units->getComponent<CUnitstats>().stats.ReachedTarget = false;
		}
	}
}

void UnitController::SelectUnit(std::shared_ptr<Entity> unitToAdd)
{
	SelectedEntites.push_back(unitToAdd);

}

std::shared_ptr<Entity> UnitController::CreateUnit(UnitType type, int Team, int owner, glm::vec3 Pos, glm::vec3 Scale, glm::vec3 rot, EntityManager* Manager)
{
	std::shared_ptr<Entity> createunit;
	UnitStats stats;
	switch (type)
	{
	case Worker:
		stats.Team = Team;
		stats.Owner = owner;
		stats.Currenthealth = 10.0f;
		stats.MaxHealth = 10.0f;
		stats.BuildTime = 5.0f;
		//stats.UnitModel = std::make_shared<Model>("models/nanosuit/nanosuit.obj");
		createunit = Manager->addEntity("Worker");
		createunit->addComponent<CTransform>(Pos, glm::vec3(0.0, 0.0, 0.0), Scale, rot);
		//createunit->addComponent<CModel>(stats.UnitModel);
		createunit->addComponent<CUnitstats>();
		createunit->getComponent<CUnitstats>().type = type;
		createunit->getComponent<CUnitstats>().stats = stats;


		break;
	case Trooper:
		break;
	default:
		break;
	}

	return createunit;
}

