#include "ScenePanel.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <filesystem>
#include <memory>
extern const std::filesystem::path s_AssetPath;

void SceneHeirarchyPanel::OnImGuiRender(EntityManager* manager)
{
	m_EntityManager = manager;
	manager->update();
	ImGui::Begin("Scene Hierarchy");

	for (auto& e : manager->getEntities())
	{
		DrawEntityNode(e);
	}
	if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
	{
		m_SelectionContext = {};
	}

	if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
	{
		if (ImGui::MenuItem("Create Empty Entity"))
			manager->addEntity("Empty Entity");
		ImGui::EndPopup();
	}

	ImGui::End();

	ImGui::Begin("Properties");
	if (m_SelectionContext)
	{
		DrawComponents(m_SelectionContext);
		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("Add Component");
		}
		if (ImGui::BeginPopup("Add Component"))
		{
			if (ImGui::MenuItem("Translation"))
			{
				m_SelectionContext->addComponent<CTransform>();
				ImGui::CloseCurrentPopup();
			}

			if (ImGui::MenuItem("Camera"))
			{
				m_SelectionContext->addComponent<CCamera>();
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Model"))
			{
				m_SelectionContext->addComponent<CModel>();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	ImGui::End();
}
void SceneHeirarchyPanel::SetContext()
{
	m_SelectionContext = {};
}

void SceneHeirarchyPanel::DrawEntityNode(std::shared_ptr<Entity> entity)
{
	auto& tag = entity->tag();
	ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnArrow;
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity.get(), Flags, tag.c_str());
	if (ImGui::IsItemClicked())
	{
		m_SelectionContext = entity;
	}
	bool EntityDeleted = false;
	if (ImGui::BeginPopupContextItem(0, 1))
	{
		if (ImGui::MenuItem("Delete Entity"))
			EntityDeleted = true;
		ImGui::EndPopup();
	}
	if (opened)
	{
		ImGui::TreePop();
	}
	if (EntityDeleted)
	{
		entity->destroy();
		if (m_SelectionContext == entity)
		{
			m_SelectionContext = {};
		}
	}
}
void SceneHeirarchyPanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
{
	ImGui::PushID(label.c_str());
	ImGui::Columns(2);
	ImGui::SetColumnWidth(0, columnWidth);
	ImGui::Text(label.c_str());
	ImGui::NextColumn();
	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{0, 0});

	float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2;
	ImVec2 ButtonSize = {lineHeight + 3.0f, lineHeight};
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f,0.1f,0.15f,1.0f});
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f,0.2f,0.2f,1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f,0.1f,0.15f,1.0f });

	if (ImGui::Button("X", ButtonSize))
	{
		values.x = resetValue;
	}
	ImGui::PopStyleColor(3);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &values.x, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f,0.7f,0.2f,1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f,0.8f,0.3f,1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f,0.7f,0.2f,1.0f });
	if (ImGui::Button("Y", ButtonSize))
	{
		values.y = resetValue;
	}
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##Y", &values.y, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f,0.25f,0.8f,1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f,0.35f,0.9f,1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f,0.25f,0.8f,1.0f });
	if (ImGui::Button("Z", ButtonSize))
	{
		values.z = resetValue;
	}
	ImGui::PopStyleColor(3);
	ImGui::SameLine();
	ImGui::DragFloat("##Z", &values.z, 0.1f);
	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PopStyleVar();
	
	ImGui::Columns(1);
	ImGui::PopID();
}

void SceneHeirarchyPanel::DrawComponents(std::shared_ptr<Entity> entity)
{
	auto& tag = entity->tag();
	char buffer[256];
	memset(buffer,0, sizeof(buffer));
	strcpy_s(buffer, tag.c_str());
	if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
	{
		tag = std::string(buffer);
	}
	const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap;
	if (entity->hasComponent<CTransform>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(CTransform).hash_code(), treeNodeFlags, "Transform"))
		{
			auto& transform = entity->getComponent<CTransform>();
			DrawVec3Control("Translation", transform.pos);
			DrawVec3Control("Rotation", transform.rotation);
			DrawVec3Control("Scale", transform.scale);
			ImGui::TreePop();
		}
	}
	if (entity->hasComponent<CCamera>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(CCamera).hash_code(), treeNodeFlags, "Camera"))
		{
			//ImGui::SameLine();
			if (ImGui::Button("+"))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			bool RemoveComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
				{
					RemoveComponent = true;
				}
				ImGui::EndPopup();
			}
			

			auto camera = entity->getComponent<CCamera>().camera;
			//use to make orthographic or perspective
			const char* projectionTypeStrings[] = {"Perspective", "Orthographic"};
			const char* currentProjectionTypeString = projectionTypeStrings[(int)camera->GetProjectionType()];
			if(ImGui::BeginCombo("Perspective", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						camera->SetProjectionType((Camera::ProjectionType)i);
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
			if (camera->GetProjectionType() == Camera::ProjectionType::Perspective)
			{
				float perspectiveVerticalFov = glm::degrees(camera->GetPerspectiveVerticalFOV());
				if (ImGui::DragFloat("Vertical FOV", &perspectiveVerticalFov))
					camera->SetPerspectiveVerticalFOV(glm::radians(perspectiveVerticalFov));

				float perspectiveNear = camera->GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near", &perspectiveNear))
					camera->SetPerspectiveNearClip(perspectiveNear);

				float perspectiveFar = camera->GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far", &perspectiveFar))
					camera->SetPerspectiveFarClip(perspectiveFar);
			}
			if (RemoveComponent)
			{
				entity->removeComponent<CCamera>();
			}
			ImGui::TreePop();
		}
	}
	if (entity->hasComponent<CModel>())
	{
		if (ImGui::TreeNodeEx((void*)typeid(CModel).hash_code(), treeNodeFlags, "Model"))
		{
			ImGui::SameLine();
			if (ImGui::Button("+"))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			bool RemoveComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove Component"))
				{
					RemoveComponent = true;
				}
				ImGui::EndPopup();
			}
			auto& model = entity->getComponent<CModel>();
			ImGui::Text("Path: %s", model.m_model->GetPath().c_str());
			ImGui::Button("Model", ImVec2(100.0f, 0.0f));
			if (ImGui::BeginDragDropTarget())
			{
				if(const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path ModelPath = std::filesystem::path(s_AssetPath) / path;
					model.m_model.reset();
					model.m_model = std::make_shared<Model>(ModelPath.string(), entity->id());
				}
				ImGui::EndDragDropTarget();
			}
			if (RemoveComponent)
			{
				entity->removeComponent<CModel>();
			}

			ImGui::TreePop();
		}
	}
}

void SceneHeirarchyPanel::SetSelectedEntity(std::shared_ptr<Entity> entity)
{
	m_SelectionContext = entity;
}

