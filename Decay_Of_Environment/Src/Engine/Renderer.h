#pragma once
#include "config.h"
#include "Shader.h"
#include "ECS\Entity.h"
#include "Camera.h"

class Renderer
{
public:
	
	static void BeginScene(Camera* camera, std::shared_ptr<Shader> shader);
	static void DrawEntityModel(std::shared_ptr<Shader> shader, std::shared_ptr<Entity> entity);
	static void JustDraw(std::shared_ptr<Shader> shader, std::shared_ptr<Entity> entity);
	static void DrawPPBRModel(std::shared_ptr<Shader> shader, std::shared_ptr<Entity> entity);
	static void EndScene();
	static uint32_t LoadCubeMap(std::vector<std::string> faces);
	static void DrawCubeMap(uint32_t textureId, std::shared_ptr<Shader> CubeMapShader, Camera* camera, uint32_t VAO, bool IsEnvironmentMap);
	static void RenderQuad(unsigned int quadVAO);
	static void StartDirectionalLightShadow(glm::vec3 lightPos, float SHADOW_WIDTH, float SHADOW_HEIGHT, unsigned int depthMapFBO, glm::mat4& lightSpaceMatrix);
	static void StartPointLightShadow(glm::vec3 lightPos, float SHADOW_WIDTH, float SHADOW_HEIGHT, unsigned int depthMapFBO, glm::mat4& lightSpaceMatrix);
	static unsigned int LoadHDR(const std::string& path);
	void Radiosity(std::shared_ptr<Shader> shader, std::shared_ptr<Entity> entity);
	
	void DrawCube();
	
	
	struct Statistics
	{
		uint32_t DrawCalls = 0;
	};
	static void ResetStats();
	static Statistics GetStats();
	static Statistics stats;

	

	//rendering quad data
	//drawing cube data
	unsigned int cubeVAO = 0;
	unsigned int cubeVBO = 0;
};