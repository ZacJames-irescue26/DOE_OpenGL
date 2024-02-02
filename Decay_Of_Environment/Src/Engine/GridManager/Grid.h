#pragma once

#include "Tile.h"
#include "Engine/config.h"
#include "Engine/ECS/EntityManager.h"

class Grid
{
public:
	Grid(int width, int height, EntityManager* Manager);

	std::vector<std::vector<Tile*>> GetGrid()
	{
		return GridMap;
	}
	glm::vec2 SnapToGrid(float x, float y);
	glm::vec3 GetWorldPosition(int x, int z);
	glm::vec3 GetGridPosition(glm::vec2 position);
private:
	std::vector<std::vector<Tile*>> GridMap;
	int GridWidth;
	int GridHeight;
	glm::vec2 TileSize = {2.0f,2.0f};
};