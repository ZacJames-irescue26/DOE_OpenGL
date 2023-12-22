#include "pch.h"
#include "UIManager.h"
#include <functional>
#include "Engine/Engine.h"
#include "Platform/OpenGL/Texture.h"

UIManager::UIManager()
{
	//float top, float left, float width, float height, std::function<void()> ClickCallback, float TextureId
	//button = std::make_shared<Button>(0.4f, 0.6f, 0.2f, 0.2f, [&] {this->Test(); }, 1.0f);
	std::shared_ptr<Panel> TestPannel = std::make_shared<Panel>(-1.0f, 0.6f, 0.4f, 2.0f,0.0f);
	//button1 = std::make_shared<Button>(-1.0f,-0.6f,-0.4f,2.0f, [&] {this->Test(); });
	PannelList.push_back(TestPannel);
	ElementList.push_back(button);
	//UIList.push_back(button1);
	TestTexture = Utils::LoadTexture("Textures/space.png");
	TestTexture1 = Utils::LoadTexture("Textures/Checkerboard.png");
	//-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, //bottom left
	//0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
	//0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
	//-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, //top left

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

}/*[&] {button->Test(); }*/



void UIManager::Draw()
{
	CalculateIndices();
	//button->Draw();
	
	std::vector<float> vertices;
	MaxIndexCount = 0;
	for (int i = 0; i < PannelList.size(); i++)
	{
		PannelList[i]->Draw();
		vertices.insert(vertices.end(), PannelList[i]->verticies.begin(), PannelList[i]->verticies.end());
		MaxIndexCount += 6;
	}
	
	for (int i = 1; i < ElementList.size(); i++)
	{
		ElementList[i]->Draw();
		vertices.insert(vertices.end(), ElementList[i]->verticies.begin(), ElementList[i]->verticies.end());
		MaxIndexCount += 6;
	}
	CalculateIndices();
	Verticies = vertices;

	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, (Verticies.size() * sizeof(float)), Verticies.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Indicies.size()* sizeof(float), Indicies.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// tex coords
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// tex index
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindTextureUnit(0, TestTexture);
	glBindTextureUnit(1, TestTexture1);
}
void UIManager::CalculateIndices()
{
	//Indicies = {
	//	0,1,2,2,3,0,
	//	/*4,5,6,6,7,4*/
	//};
	Indicies.clear();
	int offset =0;
	for (int i = 0; i < MaxIndexCount; i += 6)
	{
		Indicies.push_back(0 + offset);
		Indicies.push_back(1 + offset);
		Indicies.push_back(2 + offset);

		Indicies.push_back(2 + offset);
		Indicies.push_back(3 + offset);
		Indicies.push_back(0 + offset);
		offset += 4;
	}
}

void UIManager::Test()
{
	
	std::cout << "Spawn Building" << std::endl;

}

void UIManager::SpawnBuilding()
{
	std::cout << "Spawn Building" << std::endl;
}

void UIManager::SpawnUnit()
{
	std::cout<< "Spawn Unit" << std::endl;
}
void UIManager::CreateButton(ButtonType buttonType)
{
	std::shared_ptr<Button> createButton;
	int NumberofElements = ElementList.size();
	float gridx;
	float gridyOffset;
	
	if (NumberofElements % 2 == 0)
	{
		
		gridx = 0.8f;
		gridyOffset = 0.4 - 0.2 * Row;
		Row++;

	}
	else
	{
		gridx = 0.6f;
		gridyOffset = 0.4 - 0.2 * Row;
		
	}
	switch (buttonType)
	{
	case spawnBuilding:
		createButton = std::make_shared<Button>(gridyOffset, gridx,0.2f,0.2f, [&] {this->SpawnBuilding(); }, 1.0f);
		ElementList.push_back(createButton);
		break;
	case spawnUnit:
		createButton = std::make_shared<Button>(gridyOffset, gridx, 0.2f, 0.2f, [&] {this->SpawnUnit(); }, 0.0f);
		ElementList.push_back(createButton);
		break;
	default:
		break;
	}
}
void UIManager::OnUpdate(Timestep ts, bool IsPressed)
{
	glm::vec2 NDCcoords = ToNDC(Input::GetMouseX(), Input::GetMouseY());

	if (Input::IsMouseButtonPressed(0))
	{
		if (!IsPressed)
		{
			for (int i = 1; i < ElementList.size(); i++)
			{
				ElementList[i]->CheckClick(NDCcoords.x, NDCcoords.y);

			}
			//std::cout << "First Click inside: " << isLeftMouseDown << std::endl;
		}
	}
	else
	{
		//std::cout << "First click: " << isLeftMouseDown << std::endl;

	}
}

glm::vec2 UIManager::ToNDC(float x, float y)
{
	return glm::vec2((2.0f * x) / GameEngine::Get().GetWindow().GetWidth() - 1.0f,
		1.0f - (2.0f * y) / GameEngine::Get().GetWindow().GetHeight());
}

