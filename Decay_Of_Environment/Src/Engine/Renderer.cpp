#include "Renderer.h"
#include "pch.h"
#include "Engine.h"
Renderer::Statistics Renderer::stats;

void Renderer::BeginScene(Camera* camera, std::shared_ptr<Shader> shader)
{
	shader->use();
	camera->Matrix(*shader, "camMatrix");
}



void Renderer::DrawEntityModel(std::shared_ptr<Shader> shader, std::shared_ptr<Entity> entity)
{
	shader->use();
	if (!entity->hasComponent<CTransform>())
	{
		return;
	}
	if (!entity->hasComponent<CModel>())
	{
		return;
	}
	auto transform = glm::mat4(1.0f);
	transform = glm::translate(transform, entity->getComponent<CTransform>().pos);
	//transform = transform * glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y), glm::radians(Rotation.z));
	auto& Rotation = entity->getComponent<CTransform>().rotation;
	transform = glm::rotate(transform, glm::radians(Rotation.x), glm::vec3(1, 0, 0));//rotation x = 0.0 degrees
	transform = glm::rotate(transform, glm::radians(Rotation.y), glm::vec3(0, 1, 0));//rotation y = 0.0 degrees
	transform = glm::rotate(transform, glm::radians(Rotation.z), glm::vec3(0, 0, 1));//rotation z = 0.0 degrees
	transform = glm::scale(transform, entity->getComponent<CTransform>().scale);
	shader->setMat4("model", transform);
	entity->getComponent<CModel>().m_model->Draw(*shader);
	
	stats.DrawCalls++;


}

void Renderer::JustDraw(std::shared_ptr<Shader> shader, std::shared_ptr<Entity> entity)
{
	if (!entity->hasComponent<CModel>())
	{
		return;
	}
	entity->getComponent<CModel>().m_model->Draw(*shader);
}

void Renderer::DrawPPBRModel(std::shared_ptr<Shader> shader, std::shared_ptr<Entity> entity)
{
	shader->use();
	if (!entity->hasComponent<CTransform>())
	{
		return;
	}
	if (!entity->hasComponent<CModel>())
	{
		return;
	}
	auto transform = glm::mat4(1.0f);
	transform = glm::translate(transform, entity->getComponent<CTransform>().pos);
	transform = glm::scale(transform, entity->getComponent<CTransform>().scale);
	auto& Rotation = entity->getComponent<CTransform>().rotation;
	transform = transform * glm::eulerAngleXYZ(glm::radians(Rotation.x), glm::radians(Rotation.y), glm::radians(Rotation.z));
	shader->setMat3("normalMatrix", glm::transpose(glm::inverse(glm::mat3(transform))));
	shader->setMat4("model", transform);
	entity->getComponent<CModel>().m_model->Draw(*shader);
	stats.DrawCalls++;

}

void Renderer::EndScene()
{

}

uint32_t Renderer::LoadCubeMap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void Renderer::DrawCubeMap(uint32_t textureId, std::shared_ptr<Shader> CubeMapShader, Camera* camera, uint32_t VAO, bool IsEnvironmentMap)
{



	CubeMapShader->use();
	glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	glm::mat4 projectionMatrix = camera->GetProjectionMatrix();
	glm::mat4 ViewMatrix = glm::mat4(glm::mat3(camera->GetViewMatrix()));
	CubeMapShader->setMat4("projection", projectionMatrix);
	CubeMapShader->setMat4("view", ViewMatrix);
	CubeMapShader->setBool("isEnvironmentMap", IsEnvironmentMap);
	// skybox cube
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // set depth function back to defaul
}

void Renderer::RenderQuad(unsigned int quadVAO)
{
	

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void Renderer::ResetStats()
{
	memset(&stats, 0, sizeof(Statistics));
}

Renderer::Statistics Renderer::GetStats()
{
	return stats;
}

void Renderer::StartDirectionalLightShadow(glm::vec3 lightPos, float SHADOW_WIDTH, float SHADOW_HEIGHT, unsigned int depthMapFBO, glm::mat4& lightSpaceMatrix)
{
	// 1. first render to depth map
	glm::mat4 lightProjection, lightView;
	float near_plane = 0.1f, far_plane = 75.0f;
	lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	//lightProjection = glm::perspective(45.0f, (float)(GameEngine::Get().GetWindow().GetWidth() / GameEngine::Get().GetWindow().GetHeight()), 0.1f, 1000.0f);
	lightView = glm::lookAt(lightPos * 20.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix = lightProjection * lightView;


	//// render scene from light's point of view
	//glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	//glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	//glClear(GL_DEPTH_BUFFER_BIT);
	//glCullFace(GL_FRONT);
}
void Renderer::StartPointLightShadow(glm::vec3 lightPos, float SHADOW_WIDTH, float SHADOW_HEIGHT, unsigned int depthMapFBO, glm::mat4& lightSpaceMatrix)
{
	// 1. first render to depth map
	float farPlane = 100.0f;
	glm::mat4 lightProjection, lightView;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
	//lightProjection = glm::perspective(90.0f, 1.0f, 0.1f, farPlane);
	lightView = glm::lookAt(lightPos * 10.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));;
	lightSpaceMatrix = lightProjection * lightView;


	// render scene from light's point of view
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);
}

unsigned int Renderer::LoadHDR(const std::string& path)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
	unsigned int hdrTexture;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Failed to load HDR image." << std::endl;
	}
	return hdrTexture;
}



void Renderer::DrawCube()
{
	
		// initialize (if necessary)
		if (cubeVAO == 0)
		{
			float vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
				 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
				-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
				-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
				// front face
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
				-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
				-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
				// left face
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
				-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
				// right face
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
				 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
				 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
				 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
				 // bottom face
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				  1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				  1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
				 -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
				 -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
				 // top face
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				  1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				  1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
				  1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
				 -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
				 -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
			};
			glGenVertexArrays(1, &cubeVAO);
			glGenBuffers(1, &cubeVBO);
			// fill buffer
			glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
			// link vertex attributes
			glBindVertexArray(cubeVAO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}
		// render Cube
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
}


