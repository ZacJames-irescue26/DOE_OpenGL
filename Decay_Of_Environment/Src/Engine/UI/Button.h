#pragma once
#include "UIElement.h"
#include <functional>
class Button : public UIElement
{
public:
	Button(float top, float left, float width, float height, std::function<void()> ClickCallback, float TextureId);

public:
	virtual void CheckClick(float x, float y) override;
	virtual void Draw() override;
	virtual void OnMouseEnter() override;
	virtual void OnMouseExit() override;
	void Test();
	std::function<void()> OnClickCallback;
private:
	float m_Top, m_Left, m_Width, m_Height, m_TextureID;
};