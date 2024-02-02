#pragma once
#include "Engine/config.h"
#include "Engine/ECS/EntityManager.h"

class Tile
{
public:
<<<<<<< HEAD
	Tile(glm::vec2 position);
=======
	Tile(glm::vec2 position, EntityManager* Manager);
>>>>>>> e7422214c9ece909744bc837e62dcd36cc43370a
	glm::vec2 Position;
	glm::vec2 TileSize = {1.0f,1.0f};
	bool IsOccupied = false;
};