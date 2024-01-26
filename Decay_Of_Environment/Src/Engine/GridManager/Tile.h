#pragma once
#include "Engine/config.h"

class Tile
{
public:
	Tile(glm::vec2 position);
	float glm::vec2 Position;
	glm::vec2 TileSize = {1.0f,1.0f};
	bool IsOccupied = false;
};