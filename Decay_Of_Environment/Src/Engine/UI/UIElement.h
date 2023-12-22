#pragma once
#include "Engine/config.h"
class UIElement
{
public:
	std::vector<float> verticies;
	virtual void Draw() = 0;
	virtual void CheckClick(float x, float y) = 0;
	virtual void OnMouseEnter() = 0;
	virtual void OnMouseExit() = 0;
};