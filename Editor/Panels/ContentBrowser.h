#pragma once

#include <filesystem>
#include "DOE_Include.h"

class ContentBrowser
{
public:
	ContentBrowser();
	void OnImGuiRender();

private:
	std::filesystem::path m_CurrentDirectory;
	GLuint m_DirectoryIcon;
	GLuint m_FileIcon;
};