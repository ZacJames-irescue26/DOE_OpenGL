#pragma once
#include "Engine/config.h"

#include "Engine/Layer.h"
#include "Engine/Input.h"
#include "Button.h"
#include "Panel.h"


enum ButtonType
{
	spawnBuilding,
	spawnUnit
};

class UIManager
{
public:
	unsigned int VBO, EBO, VAO;
	UIManager();
	
	void OnUpdate(Timestep ts, bool IsPressed);
	void Draw();
	void CalculateIndices();
	void Test();
	void SpawnBuilding();
	void SpawnUnit();
	void CreateButton(ButtonType buttonType);
	glm::vec2 ToNDC(float x, float y);
	std::vector<float> Verticies;
	std::vector<int> Indicies;
	int MaxIndexCount = 0;
	std::vector<std::shared_ptr<UIElement>> ElementList;
	std::vector<std::shared_ptr<Panel>> PannelList;
	GLuint TestTexture;
	GLuint TestTexture1;
	
private:
	std::shared_ptr<Button> button;
	std::shared_ptr<Button> button1;
	bool FirstClick = false;
	
	int Row = 0;
	
};