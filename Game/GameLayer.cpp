#include "GameLayer.h"
#include "imgui.h"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"


Scene::Scene()
	:Layer("Example")
{
	CreateShaders();
	m_shader = ShaderMap.find("BaseShader")->second;
	SelectionBoxShader = ShaderMap.find("selectionBoxShader")->second;
	UIShader = ShaderMap.find("UIShader")->second;
	LightShader = ShaderMap.find("LightShader")->second;
	Lighting = ShaderMap.find("Lighting")->second;
	simpleDepthShader = ShaderMap.find("SimpleDepthShader")->second;

	DebugShadowMapShader = ShaderMap.find("DebugDepthShader")->second;
	CubeMapShader = ShaderMap.find("CubeMapShader")->second;
	HDRShader = ShaderMap.find("HDR")->second;
	ShadowCubeMap = ShaderMap.find("ShadowCubeMap")->second;
	PBRShader = ShaderMap.find("PBRShader")->second;
	IBLCubeMap = ShaderMap.find("IBLCubeMap")->second;
	IrradianceShader = ShaderMap.find("IrradianceShader")->second;
	Anim_Model = ShaderMap.find("Anim_Model")->second;
	m_Renderer = std::make_unique<Renderer>(); 
	CreateShadowMap();
	// To put into a vertex buffer class
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	std::vector<std::string> faces = {
									"Textures/SkyBox/right.jpg",
									"Textures/SkyBox/left.jpg",
									"Textures/SkyBox/top.jpg", 
									"Textures/SkyBox/bottom.jpg",
									"Textures/SkyBox/front.jpg",
									"Textures/SkyBox/back.jpg"
	};
	CubeMapId = Renderer::LoadCubeMap(faces);
	Lighting->use();
	Lighting->setInt("skybox", 0);

	CubeMapShader->use();
	CubeMapShader->setInt("skybox", 0);

	

	m_camera = GameEngine::Get().GetCamera();
	m_entityManager = new EntityManager();
	CubeModelCreateInfo cubeInfo;
	cubeInfo.size = { 0.5f, 0.5f, 0.5f };
	cubeModel = new Cube(&cubeInfo);
	/*auto nanosuit = m_entityManager->addEntity("nanosuit");
	nanosuit->addComponent<CTransform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
	nanosuit->addComponent<CModel>(std::make_shared<Model>("models/nanosuit/nanosuit.obj"));
	NormalModel = new Model("models/nanosuit/nanosuit.obj");*/
	auto plane = m_entityManager->addEntity("plane");
	plane->addComponent<CTransform>(glm::vec3(1.0f, -0.5f, 0.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-90.0f,0.0f,0.0f));
	plane->addComponent<CModel>(std::make_shared<Model>("models/TestGround.obj"));
	auto Light = m_entityManager->addEntity("Light");
	Light->addComponent<CLight>();
	lightPos = glm::vec3(1.0f, 5.0f, 1.0f);
	Light->addComponent<CTransform>(glm::vec3(lightPos), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3{0.0f});
	Light->addComponent<CModel>(std::make_shared<Model>("models/normalTest/normalTest.obj"));

	auto LightingTest = m_entityManager->addEntity("Light");
	LightingTest->addComponent<CTransform>(glm::vec3(5.0f, 2.5f, 0.5f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3{ 0.0f });
	LightingTest->addComponent<CModel>(std::make_shared<Model>("models/normalTest/normalTest.obj"));
	LightingTest->addComponent<CTest>();
	auto HDRTest = m_entityManager->addEntity("HDRTest");
	HDRTest->addComponent<CTransform>(glm::vec3(1.0f, -0.5f, 0.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3{ 0.0f });
	HDRTest->addComponent<CModel>(std::make_shared<Model>("models/HDRTest.obj"));
	
	auto PBRTest = m_entityManager->addEntity("PBRTest");
	PBRTest->addComponent<CTransform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3{ 0.0f });
	PBRTest->addComponent<CModel>(std::make_shared<Model>("models/TestSphere.obj"));

	Ray = new MousePicker(m_camera, plane);
	selectionBoxFirstClick = new MousePicker(m_camera, plane);
	selectionBoxSecondCLick = new MousePicker(m_camera, plane);
	selectionBox = new SquareModel();
	Rect = new RectangleModel();
	uiManager = new UIManager();
	//Sphere* TestSphere = new Sphere(100, 1);
	m_UnitController = std::make_shared<UnitController>();
	m_UnitController->CreateUnit(Worker,0,0, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.2f, 0.2f, 0.2f),0.0f, m_entityManager);
	m_UnitController->CreateUnit(Worker, 0, 0, glm::vec3(-1.5f, 0.0f,0.5f), glm::vec3(0.2f, 0.2f, 0.2f), 0.0f, m_entityManager);
	ShadowMap = new SquareModel();
	
	auto AnimatedTest = m_entityManager->addEntity("AnimatedTest");
	AnimatedTest->addComponent<CTransform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), 0.0f);
	AnimatedTest->addComponent<CModel>(std::make_shared<Model>("models/AnimatedModels/dancing_vampire.dae"));
	//Animation RunAnimation("models/AnimatedModels/dancing_vampire.dae", AnimatedTest->getComponent<CModel>().m_model.get());
	std::shared_ptr<Animation> animation = std::make_shared<Animation>("models/AnimatedModels/dancing_vampire.dae", AnimatedTest->getComponent<CModel>().m_model.get());
	AnimatedTest->addComponent<CAnimation>(animation);
	AnimatedTest->getComponent<CAnimation>().animator->Init(animation);

	/*auto LevelTest = m_entityManager->addEntity("LevelTest");
	LevelTest->addComponent<CTransform>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.1f, 0.1f, 0.1f), 0.0f);
	LevelTest->addComponent<CModel>(std::make_shared<Model>("models/levels/TreeLevelSeparate.gltf"));
	std::cout << "NumMeshes: " << LevelTest->getComponent<CModel>().m_model->scene->mNumMeshes << std::endl;*/
	// positions
	
	//lightPositions.push_back(glm::vec3(lightPos)); // back light
	//lightPositions.push_back(glm::vec3(-1.4f, 1.9f, 9.0f));
	//lightPositions.push_back(glm::vec3(0.0f, 1.8f, 4.0f));
	//lightPositions.push_back(glm::vec3(0.8f, 1.7f, 6.0f));
	//// colors

	//lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
	//lightColors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	//lightColors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	//lightColors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	CreateHDRTexture();
	CreateEnvCubeMap();
	//CreateCubeShadowMap();
}


Scene::~Scene()
{
	delete Rect;
	delete selectionBox;
	delete selectionBoxFirstClick;
	delete selectionBoxSecondCLick;
}

void Scene::CreateShaders()
{
	CreateShader("BaseShader", "shaders/Vert.txt", "shaders/frag.txt");
	CreateShader("selectionBoxShader", "shaders/SelectionBoxVert.txt", "shaders/SelectionBoxFrag.txt");
	CreateShader("UIShader", "shaders/UIVert.txt", "shaders/UIFrag.txt");
	CreateShader("LightShader", "shaders/Light.vert", "shaders/Light.frag");

	CreateShader("Lighting", "shaders/Lighting.vert", "shaders/Lighting.frag");
	CreateShader("SimpleDepthShader", "shaders/SimpleDepthShader.vert", "shaders/SimpleDepthShader.frag");
	CreateShader("DebugDepthShader", "shaders/debug_quad_depth.vert", "shaders/debug_quad_depth.frag");
	CreateShader("CubeMapShader", "shaders/CubeMap.vert", "shaders/CubeMap.frag");
	CreateShader("HDR", "shaders/HDR.vert", "shaders/HDR.frag");
	CreateShader("ShadowCubeMap", "shaders/ShadowCubeMap.vert", "shaders/ShadowCubeMap.frag", "shaders/ShadowCubeMap.geom");
	CreateShader("PBRShader", "shaders/PBR.vert", "shaders/PBR.frag");
	CreateShader("IBLCubeMap", "shaders/IBLCubemap.vert", "shaders/IBLEquirectangular_to_cubemap.frag");
	CreateShader("IrradianceShader", "shaders/IBLCubemap.vert", "shaders/irradiance_convolution.frag");
	CreateShader("Anim_Model", "shaders/anim_model.vert", "shaders/anim_model.frag");
}
void Scene::CreateShader(std::string name, const std::string& VertPath, const std::string& FragPath)
{
	ShaderMap.insert({ name, make_shared<Shader>(VertPath, FragPath) });
}
void Scene::CreateShader(std::string name, const std::string& VertPath, const std::string& FragPath, const std::string& GeometryShaderPath)
{
	ShaderMap.insert({ name, make_shared<Shader>(VertPath, FragPath, GeometryShaderPath) });
}

void Scene::CreateEnvCubeMap()
{
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);
	// pbr: setup cubemap to render to and attach to framebuffer
	// ---------------------------------------------------------
	
	glGenTextures(1, &envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
	// ----------------------------------------------------------------------------------------------
	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// pbr: convert HDR equirectangular environment map to cubemap equivalent
	// ----------------------------------------------------------------------
	IBLCubeMap->use();
	IBLCubeMap->setInt("equirectangularMap", 0);
	IBLCubeMap->setMat4("projection", captureProjection);
	unsigned int hdrTexture = Renderer::LoadHDR("Textures/newport_loft.hdr");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, 512, 512); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		IBLCubeMap->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Renderer->DrawCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// pbr: create an irradiance cubemap, and re-scale capture FBO to irradiance scale.
	// --------------------------------------------------------------------------------
	
	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.
	// -----------------------------------------------------------------------------
	IrradianceShader->use();
	IrradianceShader->setInt("environmentMap", 0);
	IrradianceShader->setMat4("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		IrradianceShader->setMat4("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_Renderer->DrawCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::CreateIrradianceMap()
{
	
	
}

void Scene::CreateCubeShadowMap()
{
	
	// Framebuffer for Cubemap Shadow Map
	
	glGenFramebuffers(1, &pointShadowMapFBO);

	// Texture for Cubemap Shadow Map FBO
	
	glGenTextures(1, &depthCubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindFramebuffer(GL_FRAMEBUFFER, pointShadowMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	
}

void Scene::CreateShadowMap()
{
	
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	DebugShadowMapShader->use();
	DebugShadowMapShader->setInt("depthMap", 0);

}

void Scene::CreateHDRTexture()
{
	float scr_width = GameEngine::Get().GetWindow().GetWidth();
	float scr_height = GameEngine::Get().GetWindow().GetHeight();
	glGenFramebuffers(1, &hdrFBO);
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scr_width, scr_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// create depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scr_width, scr_height);
	// attach buffers
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	HDRShader->use();
	HDRShader->setInt("screenTexture", 0);
}

void Scene::Render()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	
	counter++;
	if (deltaTime >= 1.0f / 30.0f)
	{
		std::string FPS = std::to_string((1.0f / deltaTime) * counter);
		std::string ms = std::to_string((deltaTime/ counter) * 1000);
		std::string newTitle = "FPS: " + FPS + " / " + ms + "ms";
		glfwSetWindowTitle(static_cast<GLFWwindow*>(GameEngine::Get().GetWindow().GetNativeWindow()), newTitle.c_str());
		lastFrame = currentFrame;
		counter = 0;
	}
	Camera& CamRef = *m_camera;
	CamRef.updateMatrix(45.0f, 0.1f, 1000.0f);
	
	glm::mat4 lightProjection, lightView;
	//float near_plane = 1.0f, far_plane = 7.5f;
	//lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	//lightProjection = glm::perspective(90.0f, 1.0f, 0.1f, farPlane);
	//lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//lightSpaceMatrix = lightProjection * lightView;
	//Lighting->use();
	//Lighting->setMat4("lightSpaceMatrix", lightSpaceMatrix);

	// render scene from light's point of view
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glm::mat4 lightSpaceMatrix;
	Renderer::StartDirectionalLightShadow(lightPos, SHADOW_WIDTH, SHADOW_HEIGHT, depthMapFBO, lightSpaceMatrix);
	Renderer::BeginScene(m_camera, simpleDepthShader);
	for (auto& e : m_entityManager->getEntities())
	{
		simpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		if (e->hasComponent<CLight>() || e->hasComponent<CAnimation>())
		{
			continue;
		}
		Renderer::DrawEntityModel(simpleDepthShader, e);
	}

	Renderer::EndScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// 2. then render scene as normal with shadow mapping (using depth map)
	glViewport(0, 0, GameEngine::Get().GetWindow().GetWidth(), GameEngine::Get().GetWindow().GetHeight());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	m_entityManager->update();
	//TestAnimator->UpdateAnimation(deltaTime);

	RayUpdate(Ray, glm::vec2(Input::GetMouseX(), Input::GetMouseY()));


	//glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(Ray->CurrentTerrainPoint.x, Ray->CurrentTerrainPoint.y, Ray->CurrentTerrainPoint.z)); //x left right y up down z forward and backwards
	//model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	//m_shader->setMat4("model", model);
	//NormalModel->Draw(*m_shader);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Renderer::BeginScene(m_camera, Lighting);
	for (auto& e : m_entityManager->getEntities())
	{
		if (e->hasComponent<CLight>() || e->hasComponent<CAnimation>())
		{
			continue;
		}
		
		
		bool blinn = true;

		/*for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			Lighting->setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			Lighting->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		}
		lightPositions[0] = lightPos;
		lightColors[0] = lightColor;*/
		Lighting->setVec3("camPos", CamRef.Position);
		Lighting->setMat4("lightSpaceMatrix", lightSpaceMatrix);

		Lighting->setVec3("lightPos", lightPos);
		Lighting->setVec4("lightColor", lightColor);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		Lighting->setInt("shadowMap", 1);
		Renderer::DrawEntityModel(Lighting, e);
	}



	Renderer::EndScene();
	//Renderer::BeginScene(m_camera, Anim_Model);
	//std::vector<glm::mat4> Transforms;
	//#define MAX_BONES 100
	//glm::mat4 projection = CamRef.GetProjectionMatrix();
	//glm::mat4 view = CamRef.GetViewMatrix();
	//Anim_Model->setMat4("projection", projection);
	//Anim_Model->setMat4("view", view);
	//for (auto& e : m_entityManager->getEntities("AnimatedTest"))
	//{
	//	e->getComponent<CAnimation>().animator->UpdateAnimation(deltaTime);
	//	auto transforms = e->getComponent<CAnimation>().animator->GetFinalBoneMatrices();

	//	for (int i = 0; i < transforms.size(); ++i)
	//		Anim_Model->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
	//	glm::mat4 model = glm::mat4(1.0f);
	//	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
	//	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
	//	Anim_Model->setMat4("model", model);
	//	Renderer::JustDraw(Anim_Model, e);

	//}
	Renderer::EndScene();

	Renderer::BeginScene(m_camera, PBRShader);
	
	// bind pre-computed IBL data
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;
	PBRShader->setVec3("albedo", 0.5f, 0.0f, 0.0f);
	PBRShader->setFloat("ao", 1.0f);
	PBRShader->setVec3("camPos", CamRef.Position);
	
	for (auto& e : m_entityManager->getEntities("AnimatedTest"))
	{
		auto animation = e->getComponent<CAnimation>().animator;
		animation->UpdateAnimation(deltaTime);
		auto transforms = animation->GetFinalBoneMatrices();
		animation->SetPause(PauseAnimation);
		if (!PauseAnimation)
		{
			CurrentFrame = animation->GetCurrentTime();
		}
		if (CurrentFrame < animation->GetDuration())
		{
			animation->SetCurrentTime(CurrentFrame);
		}
		PauseAnimation = animation->GetPause();
		MaxDuration = animation->GetDuration();
		for (int i = 0; i < transforms.size(); ++i)
			PBRShader->setMat4("finalBonesMatrices[" + std::to_string(i) + "]", transforms[i]);
		PBRShader->setBool("IsAnimated", true);
		//Renderer::DrawPPBRModel(PBRShader, e);
	}
	for (auto& e : m_entityManager->getEntities("PBRTest"))
	{
		if (e->hasComponent<CAnimation>())
		{
			continue;
		}
		PBRShader->setBool("IsAnimated", false);
		for (int row = 0; row < nrRows; ++row)
		{
			

			PBRShader->setFloat("metallic", (float)row / (float)nrRows);
			for (int col = 0; col < nrColumns; ++col)
			{
				// we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
				// on direct lighting.
				PBRShader->setFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(
					(col - (nrColumns / 2)) * spacing,
					(row - (nrRows / 2)) * spacing,
					0.0f
				));
				PBRShader->setMat4("model", model);
				PBRShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
				
				
				//Renderer::JustDraw(PBRShader, e);
			}
		}
		glm::vec3 lightPositions[] = {
			glm::vec3(-10.0f,  10.0f, 10.0f),
			glm::vec3(10.0f,  10.0f, 10.0f),
			glm::vec3(-10.0f, -10.0f, 10.0f),
			glm::vec3(10.0f, -10.0f, 10.0f),
		};
		glm::vec3 lightColors[] = {
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f)
		};
		for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
		{
			
			glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
			newPos = lightPositions[i];
			PBRShader->setVec3("lightPositions[" + std::to_string(i) + "]", newPos);
			PBRShader->setVec3("lightColors[" + std::to_string(i) + "]", lightColors[i]);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, newPos);
			model = glm::scale(model, glm::vec3(0.5f));
			PBRShader->setMat4("model", model);
			PBRShader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
			
		}
	}
	

	Renderer::EndScene();
	CubeMapShader->use();
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	CubeMapShader->setInt("skybox", CubeMapId);*/
	Renderer::DrawCubeMap(envCubemap, CubeMapShader, m_camera, skyboxVAO, true);
	//Renderer::DrawCubeMap(irradianceMap, CubeMapShader, m_camera, skyboxVAO, true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Renderer::BeginScene(m_camera, LightShader);

	for (auto& e : m_entityManager->getEntities())
	{
		if (e->hasComponent<CLight>())
		{
			LightShader->setVec3("objectColor", 1.0f, 0.5f, 0.31f);
			LightShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
			Renderer::DrawEntityModel(LightShader, e);
		}
	}

	Renderer::EndScene();

	//DebugShadowMapShader->use();
	ShadowMap->m_firstpoint = glm::vec2{ -1.0f, 1.0f };
	ShadowMap->m_secondpoint = glm::vec2{ 1.0f, -1.0f };
	float near_plane = 0.1f, far_plane = 75.0f;
	DebugShadowMapShader->setFloat("near_plane", near_plane);
	DebugShadowMapShader->setFloat("far_plane", far_plane);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	/*glBindVertexArray(ShadowMap->m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, ShadowMap->vertexCount);*/

	/*glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Renderer::BeginScene(m_camera, m_shader);
	glm::mat4 projection = glm::perspective(glm::radians(CamRef.ZOOM), (GLfloat)SCR_WIDTH / (GLfloat)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = CamRef.GetViewMatrix();
	m_shader->setMat4("projection", projection);
	m_shader->setMat4("view", view);

	for (auto& e : m_entityManager->getEntities())
	{
		if (e->hasComponent<CLight>())
		{
			continue;
		}
		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			m_shader->setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			m_shader->setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
		}
		lightPositions[0] = lightPos;
		m_shader->setVec3("viewPos", CamRef.Position);
		m_shader->setInt("inverse_normals", true);
		Renderer::DrawEntityModel(m_shader, e);
	}

	Renderer::EndScene();*/
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Renderer::BeginScene(m_camera, HDRShader);
	
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);

	HDRShader->setInt("hdr", hdr);
	HDRShader->setFloat("exposure", exposure);
	HDRShader->setInt("hdrBuffer", 0);
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	Renderer::RenderQuad(quadVAO);

	//std::cout << "hdr: " << (hdr ? "on" : "off") << "| exposure: " << exposure << std::endl;
	Renderer::EndScene();
	

	RayUpdate(selectionBoxFirstClick, FirstClick);
	Rect->A = glm::vec3(selectionBoxFirstClick->CurrentTerrainPoint);
	Rect->B = Ray->CurrentTerrainPoint;
	Rect->Update();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f)); //x left right y up down z forward and backwards
	model = glm::scale(model, glm::vec3(1.0f,1.0f,1.0f));
	model = model * glm::eulerAngleXYZ(glm::radians(0.0f), 0.0f, 0.0f);
	m_shader->setMat4("model", model);

	

	glBindVertexArray(Rect->VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_DEPTH_TEST);
	UIShader->use();
	uiManager->Draw();
	auto loc = glGetUniformLocation(UIShader->ID, "u_Textures");
	int samplers[2] = { 0, 1 };
	glUniform1iv(loc,2,samplers);
	
	glBindVertexArray(uiManager->VAO);
	glDrawElements(GL_TRIANGLES, uiManager->Indicies.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_TRIANGLES, 0, uiManager->Verticies.size());
	glEnable(GL_DEPTH_TEST);
	if (selectionBox != nullptr && selectionBox->ShowSelectionBox)
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);

		SelectionBoxShader->use();
		selectionBox->update();
		glBindVertexArray(selectionBox->m_VAO);
		glDrawArrays(GL_TRIANGLES, 0, selectionBox->vertexCount);

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		
	}
	
	
}



void Scene::RayUpdate(MousePicker* Ray, glm::vec2 Pos)
{
	Ray->currentRay = Ray->CalculateMouseRay(Pos.x, Pos.y, m_camera);
	if (Ray->intersectionInRange(0, RAY_RANGE, Ray->currentRay))
	{
		Ray->CurrentTerrainPoint = Ray->binarySearch(0, 0, RAY_RANGE, Ray->currentRay);
	}
	else
	{
		Ray->CurrentTerrainPoint = glm::vec3(0.0f);
	}
}

void Scene::OnUpdate(Timestep ts) 
{
	
	Render();
	Movement(ts);
	MousePos = Ray->CurrentTerrainPoint;
	uiManager->OnUpdate(ts, isLeftMouseDown);

	if (Input::IsKeyPressed(Key::X))
	{
		uiManager->CreateButton(ButtonType::spawnBuilding);
	}
	if (Input::IsKeyPressed(Key::C))
	{
		uiManager->CreateButton(ButtonType::spawnUnit);
	}

	if (Input::IsMouseButtonPressed(0))
	{
		if (!isLeftMouseDown)
		{

			FirstClick = glm::vec2(Input::GetMouseX(), Input::GetMouseY());
			selectionBox->m_firstpoint.x = (2.0f * FirstClick.x) / GameEngine::Get().GetWindow().GetWidth() - 1.0f;
			selectionBox->m_firstpoint.y = 1.0f - (2.0f * FirstClick.y) / GameEngine::Get().GetWindow().GetHeight();
			NormalizedFirstClick.x = (2.0f * FirstClick.x) / GameEngine::Get().GetWindow().GetWidth() - 1.0f;
			NormalizedFirstClick.y = 1.0f - (2.0f * FirstClick.y) / GameEngine::Get().GetWindow().GetHeight();
			isLeftMouseDown = true;
		}
		selectionBox->ShowSelectionBox = true;
		Lastclick = glm::vec2(Input::GetMouseX(), Input::GetMouseY());
		NormalizedLastCLick.x = (2.0f * Lastclick.x) / GameEngine::Get().GetWindow().GetWidth() - 1.0f;
		NormalizedLastCLick.y = 1.0f - (2.0f * Lastclick.y) / GameEngine::Get().GetWindow().GetHeight();

		selectionBox->m_secondpoint.x = (2.0f * Lastclick.x) / GameEngine::Get().GetWindow().GetWidth() - 1.0f;
		selectionBox->m_secondpoint.y = 1.0f - (2.0f * Lastclick.y) / GameEngine::Get().GetWindow().GetHeight();
		//check for entities in bounding box

		RayUpdate(selectionBoxFirstClick, FirstClick);
		RayUpdate(selectionBoxSecondCLick, Lastclick);
		released = true;



	}
	else
	{
		if (selectionBox != nullptr && released)
		{
			
			corners = GetBoundingBox(FirstClick, Lastclick);
			for (auto coner : corners)
			{
				RayUpdate(selectionBoxFirstClick, coner);
				WorldSpaceConers.push_back(selectionBoxFirstClick->CurrentTerrainPoint);
			}
			for (auto e : m_entityManager->getEntities())
			{
				if (e != nullptr && e->hasComponent<CTransform>())
				{
					if(!e->hasComponent<CUnitstats>()) continue;
					glm::vec3 position = e->getComponent<CTransform>().pos;
					if (position.x - 0.5 > WorldSpaceConers[0].x && position.x + 0.5 < WorldSpaceConers[3].x
						&& position.z + 0.5 < WorldSpaceConers[0].z && position.z - 0.5 > WorldSpaceConers[3].z
						)
					{
						std::cout << "Inside the box" << std::endl;
						m_UnitController->SelectUnit(e);
					}
				}
				
			}
			WorldSpaceConers.clear();
			selectionBox->ShowSelectionBox = false;
		}
		isLeftMouseDown = false;
		released = false;
	}
	if (Input::IsMouseButtonPressed(1))
	{
		for (auto units : m_UnitController->SelectedEntites)
		{
			units->getComponent<CUnitstats>().MovePosition = MousePos;
			units->getComponent<CTransform>().velocity = { 0.0f,0.0f,0.0f };
			units->getComponent<CUnitstats>().stats.ReachedTarget = false;
		}
	}
	
}

void Scene::Movement(Timestep ts)
{
	
	m_UnitController->MoveUnit(ts);

	
}

void Scene::OnImGuiRender()
{
	/*ImGui::Text(glm::to_string(m_camera->view).c_str());*/
	ImGui::Text(glm::to_string(Ray->CurrentTerrainPoint).c_str());
	/*ImGui::Text("Player pos: %s", glm::to_string(Player->getComponent<CTransform>().pos).c_str());
	ImGui::Text("Player vel: %s", glm::to_string(Player->getComponent<CTransform>().velocity).c_str());
	ImGui::Text("Player moveTo: %s", glm::to_string(Player->getComponent<CUnitstats>().MovePosition).c_str());*/
	
	ImGui::Image((ImTextureID)(shadowMap), ImVec2(100, 100));
	ImGui::SliderFloat("Light Position: x", &lightPos.x, -10.0f, 50.0f);
	ImGui::SliderFloat("Light Position: y", &lightPos.y, -10.0f, 50.0f);
	ImGui::SliderFloat("Light Position: z", &lightPos.z, -10.0f, 50.0f);
	ImGui::SliderFloat("Light color: r", &lightColor.x,  0.0f, 500.0f);
	ImGui::SliderFloat("Light color: g", &lightColor.y,  0.0f, 500.0f);
	ImGui::SliderFloat("Light color: b", &lightColor.z,  0.0f, 500.0f);
	ImGui::SliderFloat("exposure: b", &exposure, 0.0f, 100.0f);
	ImGui::Checkbox("Hdr: ", &hdr);
	ImGui::Image((ImTextureID)(colorBuffer), ImVec2(100, 100));
	ImGui::Checkbox("Pause: ", &PauseAnimation);
	ImGui::SliderFloat("Frame: x", &CurrentFrame, 0.0f, MaxDuration);
	//ImGui::Image((ImTextureID)(depthCubemap), ImVec2(100, 100));

}

//void Scene::OnEvent(Event& event)
//{
//	
//}

void Scene::UpdateObject(std::shared_ptr<Shader> shader,glm::vec3 pos)
{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::mat4(1.0f);
		
		model = glm::translate(model, glm::vec3(pos.x,pos.y,pos.z)); //x left right y up down z forward and backwards
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		model = model * glm::eulerAngleXYZ(glm::radians(90.0f), 0.0f, 0.0f);

		glUniformMatrix4fv(glGetUniformLocation(m_shader->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		/*glBindVertexArray(cubeModel->VAO);
		glDrawArrays(GL_TRIANGLES, 0, cubeModel->vertexCount);*/

		glBindVertexArray(Rect->VAO);
		glDrawArrays(GL_TRIANGLES, 0, Rect->vertexCount);
}

std::vector<glm::vec2> Scene::GetBoundingBox(glm::vec2 p1, glm::vec2 p2)
{
	glm::vec2 bottomLeft = glm::min(p1, p2);
	glm::vec2 topRight = glm::max(p1,p2);
	// 0 = top left; 1 = top right; 2 = bottom left; 3 = bottom right;
	std::vector<glm::vec2> coners = {
		glm::vec2(bottomLeft.x,topRight.y),
		glm::vec2(topRight.x, topRight.y),
		glm::vec2(bottomLeft.x, bottomLeft.y),
		glm::vec2(topRight.x,bottomLeft.y)
	};

	return coners;
}
