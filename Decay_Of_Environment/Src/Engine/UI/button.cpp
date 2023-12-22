#include "pch.h"
#include "Button.h"

Button::Button(float top, float left, float width, float height, std::function<void()> ClickCallback, float TextureId)
	:m_Top(top), m_Left(left), m_Width(width), m_Height(height), OnClickCallback(ClickCallback), m_TextureID(TextureId)
{
	
}

void Button::CheckClick(float x, float y)
{
	
	if (x > m_Left && x < m_Left + m_Width && y > m_Top && y < m_Top + m_Height)
	{
		OnClickCallback();
	}
}

void Button::Draw()
{

	verticies = {
		m_Left, m_Top + m_Height,           0.0f, 0.0f, 0.0f,m_TextureID, // bottom left
		m_Left + m_Width, m_Top + m_Height, 0.0f, 1.0f, 0.0f,m_TextureID, //bottom right
		m_Left + m_Width,m_Top,             0.0f, 1.0f, 1.0f,m_TextureID, //top right
		m_Left, m_Top,                      0.0f, 0.0f, 1.0f,m_TextureID //top left
		
		//-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, //bottom left
		//0.5f, -0.5f, 0.0f, 0.0f, 1.0f,
		//0.5f, 0.5f, 0.0f, 0.0f, 0.0f,
		//-0.5f, 0.5f, 0.0f, 0.0f, 1.0f, //top left
	};
}

void Button::OnMouseEnter()
{

}

void Button::OnMouseExit()
{

}

