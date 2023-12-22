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
// Voxel GI
	void initVoxelization();
	void voxelize(bool clearVoxelization);
	void initVoxelVisualization(unsigned int viewportWidth, unsigned int viewportHeight);
	void renderVoxelVisualization(Camera& cam);
	void uploadRenderingSettings(const GLuint glProgram) const;
	void uploadLighting(Scene& renderingScene, const GLuint program) const;
	void Upload(GLuint program, bool useProgram = true);
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
	
	// Voxelization
	// ----------------
	GLuint voxelTextureSize = 64; // Must be set to a power of 2.
	Material* voxelizationMaterial;
	Texture3D* voxelTexture = nullptr;
	std::shared_ptr<Shader> voxelizationShader;
	std::shared_ptr<Shader> worldPosShader;
	std::shared_ptr<Shader> DebugVoxelization;
	bool automaticallyRegenerateMipmap = true;
	bool regenerateMipmapQueued = true;
	bool automaticallyVoxelize = true;
	bool voxelizationQueued = true;
	int voxelizationSparsity = 1; // Number of ticks between mipmap generation.
	FBO* vvfbo1;
	FBO* vvfbo2;
	Model* cubeModel;
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	int ticksSinceLastVoxelization = voxelizationSparsity;

	const char* diffuseColorName = "material.diffuseColor";
	const char* specularColorName = "material.specularColor";
	const char* emissivityName = "material.emissivity";
	const char* transparencyName = "material.transparency";
	const char* refractiveIndexName = "material.refractiveIndex";
	const char* specularReflectanceName = "material.specularReflectivity";
	const char* diffuseReflectanceName = "material.diffuseReflectivity";
	const char* specularDiffusionName = "material.specularDiffusion";
	glm::vec3 diffuseColor = glm::vec3(1.0, 0.8, 0.6), specularColor = glm::vec3(1.0, 0.8, 0.6);
	float specularReflectivity = 1.0f, diffuseReflectivity= 0.0f, emissivity = 0.00f, specularDiffusion = 2.2f;
	float transparency = 0.0f, refractiveIndex = 1.4f;

// ----------------
// Rendering.
// ----------------
	bool shadows = true;
	bool indirectDiffuseLight = true;
	bool indirectSpecularLight = true;
	bool directLight = true;


};