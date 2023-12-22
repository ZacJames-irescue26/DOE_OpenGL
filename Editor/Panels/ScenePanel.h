#pragma once
#include "DOE_Include.h"

class SceneHeirarchyPanel
{
public:
	void OnImGuiRender(EntityManager* manager);
	void SetContext();
	void DrawEntityNode(std::shared_ptr<Entity> entity);
	void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f);
	void DrawComponents(std::shared_ptr<Entity> entity);
	void SetSelectedEntity(std::shared_ptr<Entity> entity);
	std::shared_ptr<Entity> GetSelectedEntity() const { return m_SelectionContext; }
private:
	std::shared_ptr<Entity> m_SelectionContext;
	EntityManager* m_EntityManager;
};