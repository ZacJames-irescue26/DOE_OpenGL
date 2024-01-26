#pragma once
#include "DOE_Include.h"
#include "Panels/ScenePanel.h"
#include "Panels/ContentBrowser.h"

class EditorLayer : public Layer
{
public:
	EditorLayer();
	virtual ~EditorLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void CreateShadowMap();
	void OnUpdate(Timestep ts);
	virtual void OnImGuiRender() override;
	void OnEvent(Event& e) override;

	std::shared_ptr<OpenGLFramebuffer> m_FrameBuffer; 
	std::shared_ptr<OpenGLFramebuffer> m_ShadowFramebuffer;
	
	bool OnKeyPressed(KeyPressedEvent& e);
	bool OnMouseButtonPressed(MouseButtonEvent& e);

	void NewScene();
	void OpenScene();
	void OpenScene(const std::filesystem::path& path);
	void SaveScene();
private:
	bool m_ViewportFocused = false, m_ViewportHovered = false;
	glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
	glm::vec2 m_ViewportBounds[2];
	std::map<std::string, std::shared_ptr<Shader>> ShaderMap;
	EntityManager* m_EntityManager;
	std::shared_ptr<Entity> m_HoveredEntity;
	Camera* m_camera;
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	int m_GuizmoType = -1;
	
	
	SceneHeirarchyPanel m_SceneHierarchyPanel;
	ContentBrowser m_CotentBrowserPanel;


	//shadow map
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int depthMapFBO;
	unsigned int shadowMap;
	
	std::unique_ptr<Grid> GridMap;
	
	


};