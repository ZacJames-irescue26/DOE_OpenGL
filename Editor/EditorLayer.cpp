#include "EditorLayer.h"
#include "imgui.h"
#include "yaml-cpp/yaml.h"
#include "yaml-cpp/emitter.h"
#include "Engine/Engine.h"
#include "ImGuizmo.h"
#include "glad/glad.h"

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

extern const std::filesystem::path s_AssetPath;
const int RADIOSITY_TEXTURE_SIZE = 32;
EditorLayer::EditorLayer()
{

}




void EditorLayer::OnAttach()
{
	m_EntityManager = new EntityManager();
	FramebufferSpecification FBSpec;
	FBSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::DEPTH24STENCIL8 };
	FBSpec.Width = 1280;
	FBSpec.Height = 720;
	m_FrameBuffer = std::make_shared<OpenGLFramebuffer>(FBSpec);
	ShaderMap = GameEngine::Get().GetShaderMap();
	m_camera = GameEngine::Get().GetCamera();
	FramebufferSpecification ShadowSpec;
	ShadowSpec.Attachments = {FramebufferTextureFormat::DEPTH24STENCIL8};
	ShadowSpec.Width = 2048;
	ShadowSpec.Height = 2048;
	m_ShadowFramebuffer = std::make_shared<OpenGLFramebuffer>(ShadowSpec);

	auto plane = m_EntityManager->addEntity("plane");
	plane->addComponent<CTransform>(glm::vec3(1.0f, -0.5f, 0.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-90.0f, 0.0f, 0.0f));
	plane->addComponent<CModel>(std::make_shared<Model>("assets/models/TestGround.obj", plane->id()));
	auto cube = m_EntityManager->addEntity("cube");
	cube->addComponent<CTransform>(glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	cube->addComponent<CModel>(std::make_shared<Model>("assets/models/normalTest/normaltest.obj", cube->id()));
	auto light = m_EntityManager->addEntity("light");
	light->addComponent<CTransform>(glm::vec3(1.0f, -0.5f, 0.0f), glm::vec3(0.0, 0.0, 0.0), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	light->addComponent<CModel>(std::make_shared<Model>("assets/models/normalTest/normaltest.obj", light->id()));
	
	CreateShadowMap();
	m_EntityManager->update();
	
	initVoxelization();
	initVoxelVisualization(1280, 720);
	
}






void EditorLayer::OnDetach()
{

}




void EditorLayer::CreateShadowMap()
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

	/*DebugShadowMapShader->use();
	DebugShadowMapShader->setInt("depthMap", 0);*/

}

// ----------------------
// Voxelization.
// ----------------------
void EditorLayer::initVoxelization()
{
		
	voxelizationShader = ShaderMap.find("voxelization")->second;

	assert(voxelizationShader != nullptr);

	const std::vector<GLfloat> texture3D(4 * voxelTextureSize * voxelTextureSize * voxelTextureSize, 0.0f);
	voxelTexture = new Texture3D(texture3D, voxelTextureSize, voxelTextureSize, voxelTextureSize, true);
}

void EditorLayer::voxelize(bool clearVoxelization)
{
	if (clearVoxelization) {
		GLfloat clearColor[4] = { 0, 0, 0, 0 };
		voxelTexture->Clear(clearColor);
	}

	

	voxelizationShader->use();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Settings.
	glViewport(0, 0, voxelTextureSize, voxelTextureSize);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	// Texture.
	voxelTexture->Activate(voxelizationShader->ID, "texture3D", 0);
	glBindImageTexture(0, voxelTexture->textureID, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RGBA8);

	// Lighting.
	//uploadLighting(renderingScene, material->program);

	// Render.
	for (auto& e : m_EntityManager->getEntities())
	{
		Renderer::DrawEntityModel(voxelizationShader, e);
	}
	if (automaticallyRegenerateMipmap || regenerateMipmapQueued) 
	{
		glGenerateMipmap(GL_TEXTURE_3D);
		regenerateMipmapQueued = false;
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

// ----------------------
// Voxelization visualization.
// ----------------------
void EditorLayer::initVoxelVisualization(unsigned int viewportWidth, unsigned int viewportHeight)
{
	//"DebugVoxelization"
		//"WorldPosition"
	worldPosShader = ShaderMap.find("WorldPosition")->second;
	DebugVoxelization = ShaderMap.find("DebugVoxelization")->second;


	assert(worldPosShader != nullptr);
	assert(DebugVoxelization != nullptr);

	vvfbo1 = new FBO(viewportHeight, viewportWidth);
	vvfbo2 = new FBO(viewportHeight, viewportWidth);


	// Rendering cube.
	cubeModel = new Model("assets/models/cube.obj");
	//assert(cubeModel->meshes.size() == 1);
	
}

void EditorLayer::renderVoxelVisualization(Camera& cam)
{
	// -------------------------------------------------------
	// Render cube to FBOs.
	// -------------------------------------------------------
	
	
	worldPosShader->use();
	worldPosShader->setMat4("V", cam.GetViewMatrix());
	worldPosShader->setMat4("P", cam.GetProjectionMatrix());
	worldPosShader->setVec3("cameraPosition", cam.Position);

	// Settings.
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	// Back.
	glCullFace(GL_FRONT);
	glBindFramebuffer(GL_FRAMEBUFFER, vvfbo1->frameBuffer);
	glViewport(0, 0, vvfbo1->width, vvfbo1->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 model = glm::mat4(0.0f);
	worldPosShader->setMat4("model", model);
	cubeModel->Draw(*worldPosShader);

	// Front.
	glCullFace(GL_BACK);
	glBindFramebuffer(GL_FRAMEBUFFER, vvfbo2->frameBuffer);
	glViewport(0, 0, vvfbo2->width, vvfbo2->height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	model = glm::mat4(0.0f);
	worldPosShader->setMat4("model", model);
	cubeModel->Draw(*worldPosShader);

	// -------------------------------------------------------
	// Render 3D texture to screen.
	// -------------------------------------------------------
	DebugVoxelization->use();
	DebugVoxelization->setMat4("V", cam.GetViewMatrix());
	DebugVoxelization->setMat4("P", cam.GetProjectionMatrix());
	DebugVoxelization->setVec3("cameraPosition", cam.Position);
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Settings.
	glUniform1i(glGetUniformLocation(DebugVoxelization->ID, "state"), 0);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Activate textures.
	vvfbo1->ActivateAsTexture(DebugVoxelization->ID, "textureBack", 0);
	vvfbo2->ActivateAsTexture(DebugVoxelization->ID, "textureFront", 1);
	voxelTexture->Activate(DebugVoxelization->ID, "texture3D", 2);

	// Render.
	glViewport(0, 0, GameEngine::Get().GetWindow().GetWidth(), GameEngine::Get().GetWindow().GetHeight());
	//m_FrameBuffer->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
	model = glm::mat4(0.0f);
	DebugVoxelization->setMat4("model", model);
	Renderer::RenderQuad(quadVAO);
	//m_FrameBuffer->Unbind();
}

void EditorLayer::uploadRenderingSettings(const GLuint glProgram) const
{
	glUniform1i(glGetUniformLocation(glProgram, "settings.shadows"), shadows);
	glUniform1i(glGetUniformLocation(glProgram, "settings.indirectDiffuseLight"), indirectDiffuseLight);
	glUniform1i(glGetUniformLocation(glProgram, "settings.indirectSpecularLight"), indirectSpecularLight);
	glUniform1i(glGetUniformLocation(glProgram, "settings.directLight"), directLight);
}

void EditorLayer::uploadLighting(Scene& renderingScene, const GLuint program) const
{
	// Point lights.
	//for (unsigned int i = 0; i < renderingScene.pointLights.size(); ++i) renderingScene.pointLights[i].Upload(program, i);

	// Number of point lights.
	glUniform1i(glGetUniformLocation(program, "numberOfLights"), 0);
}

void EditorLayer::Upload(GLuint program, bool useProgram) {
	if (useProgram) glUseProgram(program);

	// Vec3s.
	glUniform3fv(glGetUniformLocation(program, diffuseColorName), 1, glm::value_ptr(diffuseColor));
	glUniform3fv(glGetUniformLocation(program, specularColorName), 1, glm::value_ptr(specularColor));

	// Floats.
	glUniform1f(glGetUniformLocation(program, emissivityName), emissivity);
	glUniform1f(glGetUniformLocation(program, specularReflectanceName), specularReflectivity);
	glUniform1f(glGetUniformLocation(program, diffuseReflectanceName), diffuseReflectivity);
	glUniform1f(glGetUniformLocation(program, specularDiffusionName), specularDiffusion);
	glUniform1f(glGetUniformLocation(program, transparencyName), transparency);
	glUniform1f(glGetUniformLocation(program, refractiveIndexName), refractiveIndex);
}

void EditorLayer::OnUpdate(Timestep ts)
{
		
		
		
		
		
	m_EntityManager->update();
	Camera& Editorcamera = *GameEngine::Get().GetCamera();
	Editorcamera.RecalculateProjection();
	if (FramebufferSpecification spec = m_FrameBuffer->GetSpecification();
		m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f && // zero sized framebuffer is invalid
		(spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
	{
		m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
	}
	
	m_ShadowFramebuffer->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
	glm::mat4 lightSpaceMatrix;
	Renderer::StartDirectionalLightShadow(lightPos, SHADOW_WIDTH, SHADOW_HEIGHT, depthMapFBO, lightSpaceMatrix);
	std::shared_ptr<Shader> SimpleDepthShader = ShaderMap.find("SimpleDepthShader")->second;
	Renderer::BeginScene(m_camera, SimpleDepthShader);
	for (auto& e : m_EntityManager->getEntities())
	{
		SimpleDepthShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		if (e->hasComponent<CLight>() || e->hasComponent<CAnimation>())
		{
			continue;
		}
		Renderer::DrawEntityModel(SimpleDepthShader, e);
	}

	Renderer::EndScene();
	/*glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
	m_ShadowFramebuffer->Unbind();

	m_FrameBuffer->Bind();
	

	glClearColor(0, 128.0 / 255.0f, 128.0 / 255.0, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_FrameBuffer->ClearAttachment(1, -1);
	auto EditorShader = ShaderMap.find("EditorShader")->second;
	Renderer::BeginScene(&Editorcamera, EditorShader);
	for (auto& e : m_EntityManager->getEntities())
	{
		EditorShader->setVec3("camPos", Editorcamera.Position);
		EditorShader->setMat4("lightSpaceMatrix", lightSpaceMatrix);
		EditorShader->setVec3("lightPos", lightPos);
		EditorShader->setVec4("lightColor", lightColor);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_ShadowFramebuffer->GetDepthAttachment());
		EditorShader->setInt("shadowMap", 1);
		if (e->hasComponent<CModel>())
		{
			Renderer::DrawEntityModel(EditorShader, e);
		}
	}

	auto [mx, my] = ImGui::GetMousePos();
	mx -= m_ViewportBounds[0].x;
	my -= m_ViewportBounds[0].y;
	glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
	my = viewportSize.y - my;
	int mouseX = (int)mx;
	int mouseY = (int)my;
	if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
	{

		int PixelData = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
		m_HoveredEntity = PixelData == -1 ? nullptr : m_EntityManager->GetEntityByID(PixelData);
	}
	m_FrameBuffer->Unbind();
	Renderer::EndScene();
	






	
}


void EditorLayer::OnImGuiRender()
{

	// Note: Switch this to true to enable dockspace
	static bool dockspaceOpen = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
	// because it would be confusing to have two docking targets within each others.
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
	// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive, 
	// all active windows docked into it will lose their parent and become undocked.
	// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise 
	// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();
	float minWinSizeX = style.WindowMinSize.x;
	style.WindowMinSize.x = 370.0f;
	if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	{
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	style.WindowMinSize.x = minWinSizeX;

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			// Disabling fullscreen would allow the window to be moved to the front of other windows, 
			// which we can't undo at the moment without finer window depth/z control.
			//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);1
			if (ImGui::MenuItem("New", "Ctrl+N"))
				NewScene();

			if (ImGui::MenuItem("Open...", "Ctrl+O"))
				OpenScene();

			if (ImGui::MenuItem("Save", "Ctrl+S"))
				SaveScene();

			if (ImGui::MenuItem("Exit")) GameEngine::Get().close();
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	ImGui::Begin("Stats");
	std::string name = "None";
	if (m_HoveredEntity)
		name = m_HoveredEntity->tag();
	ImGui::Text("Hovered Entity: %s", name.c_str());
	ImGui::Text("Shadow Map:");
	uint64_t ShadowtextureID = m_ShadowFramebuffer->GetDepthAttachment();
	ImGui::Image(reinterpret_cast<void*>(ShadowtextureID), ImVec2{ 160, 160 });
	ImGui::SliderFloat("Light Position: x", &lightPos.x, -10.0f, 50.0f);
	ImGui::SliderFloat("Light Position: y", &lightPos.y, -10.0f, 50.0f);
	ImGui::SliderFloat("Light Position: z", &lightPos.z, -10.0f, 50.0f);

	ImGui::Image(reinterpret_cast<void*>(ShadowtextureID), ImVec2{ 160, 160 });
	ImGui::End();
	
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
	ImGui::Begin("Viewport");

	auto MouseViewportOffset = ImGui::GetCursorPos();
	

	auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
	auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
	auto viewportOffset = ImGui::GetWindowPos();
	m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
	m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

	m_ViewportFocused = ImGui::IsWindowFocused();
	m_ViewportHovered = ImGui::IsWindowHovered();

	ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
	m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };

	uint64_t textureID = m_FrameBuffer->GetColorAttachmentRendererID(0);
	ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
		{
			const wchar_t* path = (const wchar_t*)payload->Data;
			OpenScene(std::filesystem::path(s_AssetPath) / path);
		}
		ImGui::EndDragDropTarget();
	}


	auto selectedEntity = m_SceneHierarchyPanel.GetSelectedEntity();
	if (selectedEntity && m_GuizmoType != -1)
	{
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowHeight = (float)ImGui::GetWindowHeight();
		ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
		Camera* EditorCamera = GameEngine::Get().GetCamera();

		const glm::mat4& cameraProjection = EditorCamera->GetProjectionMatrix();
		glm::mat4 m_ViewMatrix = EditorCamera->GetViewMatrix();
		

		auto& tc = selectedEntity->getComponent<CTransform>();
		glm::mat4 transform = tc.GetTransform();
		
		ImGuizmo::Manipulate(glm::value_ptr(m_ViewMatrix), glm::value_ptr(cameraProjection), 
		(ImGuizmo::OPERATION)m_GuizmoType,
		ImGuizmo::LOCAL, 
		glm::value_ptr(transform));

		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, rotation, scale;
			Math::DecomposeTransform(transform, translation, rotation, scale);
			glm::vec3 deltaRotation = rotation - tc.rotation;
			tc.pos = glm::vec3(transform[3]);
			/*tc.rotation += deltaRotation;*/
			tc.scale = scale;
		}

	}
	
	ImGui::End();
	ImGui::PopStyleVar();
	m_SceneHierarchyPanel.OnImGuiRender(m_EntityManager);
	m_CotentBrowserPanel.OnImGuiRender();
	ImGui::End();
}

void EditorLayer::OnEvent(Event& e)
{
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
	dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
}

bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
{
	// Shortcuts
	if (e.IsRepeat())
		return false;

	bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
	bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

	switch (e.GetKeyCode())
	{
	case Key::N:
	{
		if (control)
			NewScene();

		break;
	}
	case Key::O:
	{
		if (control)
			OpenScene();

		break;
	}
	case Key::S:
	{
		if (control)
		{
			/*if (shift)
				SaveSceneAs();
			else*/
				SaveScene();
		}

		break;

	case Key::Q:
		m_GuizmoType = -1;
		break;
	case Key::W:
		m_GuizmoType = ImGuizmo::OPERATION::TRANSLATE;
		break;
	case Key::E:
		m_GuizmoType = ImGuizmo::OPERATION::ROTATE;
		break;
	case Key::R:
		m_GuizmoType = ImGuizmo::OPERATION::SCALE;
		break;
	}
	}
}

bool EditorLayer::OnMouseButtonPressed(MouseButtonEvent& e)
{
	if (e.GetMouseButton() == Mouse::ButtonLeft && !ImGuizmo::IsOver() && !Input::IsKeyPressed(Key::LeftAlt))
	{
		if (m_ViewportHovered)
			m_SceneHierarchyPanel.SetSelectedEntity(m_HoveredEntity);
	}
	return false;
}

void EditorLayer::NewScene()
{
	m_EntityManager->Reset();
	GameEngine::Get().ResetCamera();
	m_SceneHierarchyPanel.OnImGuiRender(m_EntityManager);
	m_SceneHierarchyPanel.SetContext();
}

void EditorLayer::OpenScene()
{
	std::string filepath = FileDialogs::OpenFile("DOE Scene(*.DOE)\0*.DOE\0");
	if (!filepath.empty())
	{
		OpenScene(filepath);
	}
}

void EditorLayer::OpenScene(const std::filesystem::path& path)
{
	m_EntityManager->Reset();
	GameEngine::Get().ResetCamera();
	//m_SceneHierarchyPanel.OnImGuiRender(m_EntityManager);
	m_SceneHierarchyPanel.SetContext();
	SceneSerializer serializer(m_EntityManager);
	serializer.Deserialize(path.string());
}

void EditorLayer::SaveScene()
{
	std::string filepath = FileDialogs::SaveFile("DOE Scene(*.DOE)\0*.DOE\0");
	if (!filepath.empty())
	{
		SceneSerializer serializer(m_EntityManager);
		serializer.Serialize(filepath);
	}
}

