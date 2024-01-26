#include "Grid.h"


Grid::Grid(int width, int height)
	: GridWidth(width), GridHeight(height)
{
	GridMap.reserve(width * height);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Tile* newTile = new Tile(x * TileSize.x, y * TileSize.y);
			GridMap[x][y] = newTile;
		}
	}
}

glm::vec2 Grid::SnapToGrid(float x, float z)
{
	int x = round((x / TileSize.x) * TileSize.x);
	int y = round((y / TileSize.y) * TileSize.y);
	if (x < GridWidth && y < GridHeight)
	{
		return {x, y};
	}
	else
	{
		std::cout << "out of bounds" << std::endl;
	}
}

