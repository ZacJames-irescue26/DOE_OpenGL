#include "Grid.h"


Grid::Grid(int width, int height, EntityManager* Manager)
	: GridWidth(width), GridHeight(height)
{
	GridMap.resize(width, std::vector<Tile*>(height));
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Tile* newTile = new Tile(glm::vec2(x * TileSize.x, y * TileSize.y), Manager);
			GridMap[x][y] = newTile;
		}
	}
}

glm::vec2 Grid::SnapToGrid(float x, float z)
{
	int Gridx = round((x / TileSize.x) * TileSize.x);
	int Gridy = round((z / TileSize.y) * TileSize.y);
	if (Gridx < GridWidth && Gridy < GridHeight)
	{
		return {Gridx, Gridy};
	}
	else
	{
		std::cout << "out of bounds" << std::endl;
	}
}

glm::vec3 Grid::GetWorldPosition(int x, int z)
{
	return glm::vec3(x, 0, z) * TileSize.x;
}

glm::vec3 Grid::GetGridPosition(glm::vec2 position)
{
	return glm::vec3((int)floor((position.x / TileSize.x)), 0, (int)floor((position.y / TileSize.x)));
}

