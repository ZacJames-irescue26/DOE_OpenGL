#pragma once

#include "Tile.h"
#include "Engine/config.h"


class Grid
{
public:
	Grid(int width, int height);

	std::vector<std::vector<Tile*>> GetGrid()
	{
		return GridMap;
	}
	glm::vec2 SnapToGrid(float x, float y);
private:
	std::vector<std::vector<Tile*>> GridMap;
	int GridWidth;
	int GridHeight;
	glm::vec2 TileSize = {1.0f,1.0f};
};