#pragma once
#include "config.h"
#include <memory>
#include "Camera.h"
#include "Shader.h"
//#include "Engine/Scene.h"

#include "Engine/Events/Event.h"
#include "Engine/Events/KeyEvent.h"
#include "Engine/Events/MouseEvent.h"
#include "Engine/Events/ApplicationEvent.h"
#include "Engine/LayerStack.h"
#include "Engine/ImGui/ImGuiLayer.h"
#include "Engine/UI/UIManager.h"

#include "Window.h"

class Scene;
int main(int argc, char** argv);

enum class returnCode {
	CONTINUE, QUIT
};

struct ApplicationCommandLineArgs
{
	int Count = 0;
	char** Args = nullptr;

	const char* operator[](int index) const
	{
		assert(index < Count);
		return Args[index];
	}
};

struct ApplicationSpecification
{
	std::string Name = "DOE Application";
	std::string WorkingDirectory;
	ApplicationCommandLineArgs CommandLineArgs;
	std::map<std::string, std::shared_ptr<Shader>> ShaderMap;
};

class GameEngine {
public:
	GameEngine();
	~GameEngine();

	void CreateShader(std::string name, const std::string& VertPath, const std::string& FragPath);
	void CreateShader(std::string name, const std::string& VertPath, const std::string& FragPath, const std::string& GeometryShaderPath);

	void close() {m_Running = false;}

	void createModels();
	void sUserInput();
	std::shared_ptr<Shader> ourShader;
	std::shared_ptr<Scene> m_Scene;
	GLFWwindow* makeWindow();
	void calculateFramesRate();
	void mainLoop();
	void OnEvent(Event& e);
	bool OnKeyPressedEvent();
	bool OnKeyPressedEvent(KeyPressedEvent& event);
	inline static GameEngine& Get() { return *s_Instance; }
	inline Window& GetWindow() { return *m_Window; }
	inline Camera* GetCamera() { return m_camera; }
	inline void ResetCamera() {m_camera = new Camera(); }
	void PushLayer(Layer* layer);
	void PushOverlay(Layer* layer);
	bool OnWindowClose(WindowCloseEvent& e);
	bool OnWindowResize(WindowResizeEvent& e);
	std::map<std::string, std::shared_ptr<Shader>> GetShaderMap() const { return ShaderMap; }
private:
	std::map<std::string, std::shared_ptr<Shader>> ShaderMap;
	std::unique_ptr<Window> m_Window;
	int width, height;
	Camera* m_camera;
	Camera* FollowCamera;
	double lastTime, currentTime;
	int numFrames;
	float frameTime;
	ImGuiLayer* m_ImGuiLayer;
	LayerStack m_LayerStack;
	static GameEngine* s_Instance;
	bool m_Running = true;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
	bool firstMouse = true;
	float m_LastFrameTime = 0.0f;
	bool m_Minimized = false;
	float lastX = SCR_WIDTH / 2.0f;
	float lastY = SCR_HEIGHT / 2.0f;

	friend int ::main(int argc, char** argv);
	//for client use only
	GameEngine* CreateApplication(ApplicationCommandLineArgs args);
};