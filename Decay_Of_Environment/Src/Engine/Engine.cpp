#include "pch.h"
#include "Engine.h"
#include "Shader.h"
#include "Input.h"
#include <string>

#define BIND_EVENT_FN(x) std::bind(&GameEngine::x, this, std::placeholders::_1)
GameEngine* GameEngine::s_Instance = nullptr;

GameEngine::GameEngine()
{
	s_Instance = this;
	m_Window = std::unique_ptr<Window>(Window::Create());
	m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));
	m_Window->SetVSync(false);
	lastTime = glfwGetTime();
	numFrames = 0;
	frameTime = 16.0f;

	

	//m_camera = new Camera(m_Window->GetWidth(), m_Window->GetHeight(), glm::vec3(0.0f, 0.0f, 2.0f));
	m_camera = new Camera();
	CreateShader("BaseShader", "assets/shaders/Vert.txt", "assets/shaders/frag.txt");
	CreateShader("selectionBoxShader", "assets/shaders/SelectionBoxVert.txt", "assets/shaders/SelectionBoxFrag.txt");
	CreateShader("UIShader", "assets/shaders/UIVert.txt", "assets/shaders/UIFrag.txt");
	CreateShader("LightShader", "assets/shaders/Light.vert", "assets/shaders/Light.frag");

	CreateShader("Lighting", "assets/shaders/Lighting.vert", "assets/shaders/Lighting.frag");
	CreateShader("SimpleDepthShader", "assets/shaders/SimpleDepthShader.vert", "assets/shaders/SimpleDepthShader.frag");
	CreateShader("DebugDepthShader", "assets/shaders/debug_quad_depth.vert", "assets/shaders/debug_quad_depth.frag");
	CreateShader("CubeMapShader", "assets/shaders/CubeMap.vert", "assets/shaders/CubeMap.frag");
	CreateShader("HDR", "assets/shaders/HDR.vert", "assets/shaders/HDR.frag");
	CreateShader("ShadowCubeMap", "assets/shaders/ShadowCubeMap.vert", "assets/shaders/ShadowCubeMap.frag", "assets/shaders/ShadowCubeMap.geom");
	CreateShader("PBRShader", "assets/shaders/PBR.vert", "assets/shaders/PBR.frag");
	CreateShader("IBLCubeMap", "assets/shaders/IBLCubemap.vert", "assets/shaders/IBLEquirectangular_to_cubemap.frag");
	CreateShader("IrradianceShader", "assets/shaders/IBLCubemap.vert", "assets/shaders/irradiance_convolution.frag");
	CreateShader("Anim_Model", "assets/shaders/anim_model.vert", "assets/shaders/anim_model.frag");
	CreateShader("EditorShader", "assets/shaders/Editor.vert", "assets/shaders/Editor.frag");
	
	CreateShader("voxelization", "assets/shaders/Voxelization/voxelization.vert", "assets/shaders/Voxelization/voxelization.frag", "assets/shaders/Voxelization/voxelization.geom");
	CreateShader("DebugVoxelization", "assets/shaders/Voxelization/Visualization/voxel_visualization.vert", "assets/shaders/Voxelization/Visualization/voxel_visualization.frag");
	CreateShader("WorldPosition", "assets/shaders/Voxelization/Visualization/world_position.vert", "assets/shaders/Voxelization/Visualization/world_position.frag");
	CreateShader("Voxel_cone", "assets/shaders/VoxelConeTracing/voxel_cone_tracing.vert", "assets/shaders/VoxelConeTracing/voxel_cone_tracing.frag");
	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 128.0 / 255.0f, 128.0 / 255.0, 1.0f);

	m_ImGuiLayer = new ImGuiLayer();
	PushOverlay(m_ImGuiLayer);
}
void GameEngine::CreateShader(std::string name, const std::string& VertPath, const std::string& FragPath)
{
	ShaderMap.insert({ name, make_shared<Shader>(VertPath, FragPath) });
	
}

void GameEngine::CreateShader(std::string name, const std::string& VertPath, const std::string& FragPath, const std::string& GeometryShaderPath)
{
	ShaderMap.insert({ name, make_shared<Shader>(VertPath, FragPath, GeometryShaderPath) });
}

GameEngine::~GameEngine()
{
	glfwTerminate();
}

//returnCode GameEngine::processInput()
//{
//	m_camera->Inputs(static_cast<GLFWwindow*>(GetWindow().GetNativeWindow()));
//
//	if (glfwGetKey(static_cast<GLFWwindow*>(GetWindow().GetNativeWindow()), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//		return returnCode::QUIT;
//	}
//	return returnCode::CONTINUE;
//}
void GameEngine::PushLayer(Layer* layer)
{
	m_LayerStack.PushLayer(layer);
	layer->OnAttach();
}

void GameEngine::PushOverlay(Layer* layer)
{
	m_LayerStack.PushOverlay(layer);
	layer->OnAttach();
}


void GameEngine::OnEvent(Event& event)
{
	EventDispatcher dispatcher(event);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

	
	
	for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); )
	{
		(*--it)->OnEvent(event);
		if (event.Handled)
			break;
	}
}


bool GameEngine::OnWindowClose(WindowCloseEvent& e)
{
	m_Running = false;
	return true;
}

bool GameEngine::OnWindowResize(WindowResizeEvent& e)
{
	if (e.GetWidth() == 0 || e.GetHeight() == 0)
	{
		m_Minimized = true;
	}
	m_Minimized = false;

	glViewport(0, 0, e.GetWidth(), e.GetHeight());

	return false;
}

void GameEngine::mainLoop()
{
	while(m_Running)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwPollEvents();

		float time = (float)glfwGetTime();
		Timestep timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;
		if (!m_Minimized)
		{
			for (Layer* layer : m_LayerStack)
			{
				m_camera->Inputs(static_cast<GLFWwindow*>(GameEngine::Get().GetWindow().GetNativeWindow()), timestep);
				layer->OnUpdate(timestep);

			}
		}
		m_ImGuiLayer->Begin();
		for (Layer* layer : m_LayerStack)
			layer->OnImGuiRender();
		m_ImGuiLayer->End();
		//update

		//draw
		m_Window->OnUpdate();
	}
}


