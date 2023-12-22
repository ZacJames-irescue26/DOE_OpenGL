#include "pch.h"
#include "Panel.h"

Panel::Panel(float top, float left, float width, float height, float TextureId)
	:m_Top(top), m_Left(left), m_Width(width), m_Height(height), m_TextureID(TextureId)
{

}

void Panel::Draw()
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

