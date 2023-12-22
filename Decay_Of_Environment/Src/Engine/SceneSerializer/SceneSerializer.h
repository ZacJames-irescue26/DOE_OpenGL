#pragma once
#include "Engine\ECS\EntityManager.h"

class SceneSerializer
{
public:
	SceneSerializer(EntityManager* manager);

	void Serialize(const std::string& filepath);
	void SerializeRuntime(const std::string& filepath);

	bool Deserialize(const std::string& filepath);
	bool DeserializeRuntime(const std::string& filepath);

private:
	EntityManager* manager;
};