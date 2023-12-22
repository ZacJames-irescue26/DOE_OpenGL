#include "ContentBrowser.h"

#include "imgui.h"

extern const std::filesystem::path s_AssetPath = "assets";

ContentBrowser::ContentBrowser()
	: m_CurrentDirectory(s_AssetPath)
{
	m_DirectoryIcon = Utils::LoadTexture("assets/ContentBrowser/DirectoryIcon.png");
	m_FileIcon = Utils::LoadTexture("assets/ContentBrowser/FileIcon.png");

}

void ContentBrowser::OnImGuiRender()
{
	ImGui::Begin("Content Browser");

	if (m_CurrentDirectory != std::filesystem::path(s_AssetPath))
	{
		if (ImGui::Button("<-"))
		{
			m_CurrentDirectory = m_CurrentDirectory.parent_path();
		}
	}

	static float padding = 16.0f;
	static float thumbnailSize = 128.0f;
	float cellSize = thumbnailSize + padding;

	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = (int)(panelWidth / cellSize);
	if (columnCount < 1)
		columnCount = 1;

	ImGui::Columns(columnCount, 0, false);

	for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory))
	{
		const auto& path = directoryEntry.path();
		auto relativePath = std::filesystem::relative(path, s_AssetPath);
		std::string filenameString = relativePath.string();

		ImGui::PushID(filenameString.c_str());
		GLuint icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::ImageButton((ImTextureID)icon, { thumbnailSize, thumbnailSize }, { 0, 1 }, { 1, 0 });

		if (ImGui::BeginDragDropSource())
		{
			const wchar_t* itemPath = relativePath.c_str();
			ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath)+1) * sizeof(wchar_t));
			ImGui::EndDragDropSource();
		}

		ImGui::PopStyleColor();
		if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
		{
			if (directoryEntry.is_directory())
				m_CurrentDirectory /= path.filename();

		}
		ImGui::TextWrapped(filenameString.c_str());

		ImGui::NextColumn();

		ImGui::PopID();
	}

	ImGui::End();
}
