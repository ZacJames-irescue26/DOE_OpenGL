#pragma once
#include "Engine/config.h"
#include "Engine/ECS/EntityManager.h"

class Tile
{
public:
	Tile(glm::vec2 position, EntityManager* Manager);
	glm::vec2 Position;
	glm::vec2 TileSize = {1.0f,1.0f};
	bool IsOccupied = false;
};