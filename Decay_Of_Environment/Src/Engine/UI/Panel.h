#pragma once
#include "Engine/config.h"
#include "UIElement.h"
class Panel
{
public:
	Panel(float top, float left, float width, float height, float TextureId);
	void Draw();
	std::vector<std::shared_ptr<UIElement>> Elements;
	std::vector<float> verticies;
private:
	float m_Top, m_Left, m_Width, m_Height, m_TextureID;
};