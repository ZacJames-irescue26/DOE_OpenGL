#include "Grid.h"


Grid::Grid(int width, int height)
	: GridWidth(width), GridHeight(height)
{
	GridMap.reserve(width * height);
	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			Tile* newTile = new Tile(glm::vec2(x * TileSize.x, y * TileSize.y));
			GridMap[x][y] = newTile;
		}
	}
}

glm::vec2 Grid::SnapToGrid(float x, float z)
{
	int X = round((x / TileSize.x) * TileSize.x);
	int Y = round((z / TileSize.y) * TileSize.y);
	if (X < GridWidth && Y < GridHeight)
	{
		return {X, Y};
	}
	else
	{
		std::cout << "out of bounds" << std::endl;
	}
}

