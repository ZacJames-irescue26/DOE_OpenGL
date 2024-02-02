#include "Tile.h"
#include "Engine/ECS/EntityManager.h"
Tile::Tile(glm::vec2 position, EntityManager* Manager)
	: Position(position)
{
	std::shared_ptr<Entity> Tile = Manager->addEntity("Grid");
	Tile->addComponent<CTransform>(glm::vec3(position.x, 0.0f, Position.y), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	Tile->addComponent<CModel>(std::make_shared<Model>("assets/models/normalTest/normaltest.obj", Tile->id()));

}
