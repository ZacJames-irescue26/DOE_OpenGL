#pragma once
#include "DOE_Include.h"

class MousePicker;

class Scene : public Layer
{
public:

	Scene();
	~Scene();
	void CreateShaders();
	void CreateShader(std::string name, const std::string& VertPath, const std::string& FragPath);
	void CreateShader(std::string name, const std::string& VertPath, const std::string& FragPath, const std::string& GeometryShaderPath);
	void CreateEnvCubeMap();
	void CreateIrradianceMap();
	void CreateCubeShadowMap();
	void CreateShadowMap();
	void CreateHDRTexture();
	virtual void OnUpdate(Timestep ts) override;
	void Movement(Timestep ts);
	virtual void OnImGuiRender() override;
	/*virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnEvent(Event& event) override;*/

	void Render();

	void RenderEntities(Camera& CamRef, std::shared_ptr<Shader> shader);

	void RayUpdate(MousePicker* Ray, glm::vec2 Pos);

	void UpdateObject(std::shared_ptr<Shader> shader, glm::vec3 pos);
	std::vector<glm::vec2> GetBoundingBox(glm::vec2 p1, glm::vec2 p2);
private:
	//Shaders
	std::map<std::string, std::shared_ptr<Shader>> ShaderMap;
	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Shader> SelectionBoxShader;
	std::shared_ptr<Shader> UIShader;
	std::shared_ptr<Shader> LightShader;
	std::shared_ptr<Shader> Lighting;
	std::shared_ptr<Shader> simpleDepthShader;
	std::shared_ptr<Shader> DebugShadowMapShader;
	std::shared_ptr<Shader> CubeMapShader;
	std::shared_ptr<Shader> HDRShader;
	std::shared_ptr<Shader> ShadowCubeMap;
	std::shared_ptr<Shader> PBRShader;
	std::shared_ptr<Shader> IBLCubeMap;
	std::shared_ptr<Shader> IrradianceShader;
	std::shared_ptr<Shader> Anim_Model;
	//DeltaTime
	float deltaTime;
	float lastFrame;
	unsigned int counter;
	std::unique_ptr<Renderer> m_Renderer;


	std::shared_ptr<Sphere> m_sphere;
	Model* NormalModel;
	Cube* cubeModel;
	EntityManager* m_entityManager;
	Camera* m_camera;
	MousePicker* Ray;
	MousePicker* selectionBoxFirstClick;
	MousePicker* selectionBoxSecondCLick;
	UIManager* uiManager;
	//std::shared_ptr<Entity> plane;
	bool isLeftMouseDown;
	bool released = false;
	glm::vec2 FirstClick;
	glm::vec2 Lastclick;
	glm::vec2 NormalizedLastCLick;
	glm::vec2 NormalizedFirstClick;
	SquareModel* selectionBox = nullptr;
	std::vector<glm::vec2> corners;
	RectangleModel* Rect;
	SquareModel* ShadowMap;
	std::vector<glm::vec3> WorldSpaceConers;
	std::vector<std::shared_ptr<Entity>> SelctedUnits;
	glm::vec3 MousePos = { 0.0f,0.0f,0.0f };
	std::shared_ptr<Entity> Player;
	std::shared_ptr<UnitController> m_UnitController;
	Model* AnimatedModel;

	//shadow map
	const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
	unsigned int depthMapFBO;
	unsigned int shadowMap;
	unsigned int skyboxVAO, skyboxVBO;
	uint32_t CubeMapId;
	glm::vec3 lightPos = glm::vec3(0.0f);
	unsigned int depthCubemap;
	unsigned int pointShadowMapFBO;
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	//HDR
	unsigned int colorBuffer;
	/*std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;*/
	unsigned int hdrFBO;
	float exposure = 0.1f;
	bool hdr = false;
	unsigned int quadVAO = 0;
	unsigned int quadVBO;
	// env cube map
	unsigned int envCubemap;
	unsigned int captureFBO, captureRBO;
	//irradiance map
	unsigned int irradianceMap;

	//Animation test
	bool PauseAnimation = false;
	float CurrentFrame = 0;
	float MaxDuration = 0;

};