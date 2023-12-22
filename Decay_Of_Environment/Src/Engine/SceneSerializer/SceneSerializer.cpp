#include "SceneSerializer.h"
#include "yaml-cpp/yaml.h"
#include <fstream>
#include "Engine/ECS/Entity.h"

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
			{
				return false;
			}
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}

	};
}

SceneSerializer::SceneSerializer(EntityManager* manager)
	:manager(manager)
{

}



YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
	// [1,2,3]
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

static void SerializeEntity(YAML::Emitter& out, std::shared_ptr<Entity> entity)
{
	out << YAML::BeginMap;
	out << YAML::Key << "Entity";
	out << YAML::Value << entity->id();
	out << YAML::Key << "TagComponent";
	out << YAML::BeginMap;
	out << YAML::Key << "Tag" << YAML::Value << entity->tag();
	out << YAML::EndMap;

	if (entity->hasComponent<CTransform>())
	{
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap;
		auto& tc = entity->getComponent<CTransform>();
		out << YAML::Key << "Translation" << YAML::Value << tc.pos;
		out << YAML::Key << "Rotation" << YAML::Value << tc.rotation;
		out << YAML::Key << "Scale" << YAML::Value << tc.scale;
		out << YAML::EndMap;

	}
	if (entity->hasComponent<CCamera>())
	{
		out << YAML::Key << "CameraComponent";
		out << YAML::BeginMap;

		auto& CameraComponent = entity->getComponent<CCamera>();
		auto& camera = CameraComponent.camera;

		out << YAML::Key << "Camera" << YAML::Value;
		out << YAML::BeginMap;
		out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera->GetProjectionType();
		out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera->GetPerspectiveVerticalFOV();
		out << YAML::Key << "PerspectiveNear" << YAML::Value << camera->GetPerspectiveNearClip();
		out << YAML::Key << "PerspectiveFar" << YAML::Value << camera->GetPerspectiveFarClip();
		out << YAML::EndMap;
		out << YAML::EndMap;

	}
	if (entity->hasComponent<CModel>())
	{
		out << YAML::Key << "ModelComponent";
		out << YAML::BeginMap;

		auto& ModelComponent = entity->getComponent<CModel>();
		auto& model = ModelComponent.m_model;
		out << YAML::Key << "Path" << YAML::Value << model->GetPath();

		out << YAML::EndMap;
	}
	out << YAML::EndMap;

}
void SceneSerializer::Serialize(const std::string& filepath)
{
	
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	for (auto& entity : manager->getEntities())
	{
		SerializeEntity(out, entity);
	}
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();



}

void SceneSerializer::SerializeRuntime(const std::string& filepath)
{

}

bool SceneSerializer::Deserialize(const std::string& filepath)
{
	std::ifstream stream(filepath);
	std::stringstream StrStream;
	StrStream << stream.rdbuf();

	YAML::Node data = YAML::Load(StrStream.str());
	if (!data["Scene"])
		return false;

	std::string SceneName = data["Scene"].as<std::string>();

	auto entities = data["Entities"];
	if (entities)
	{
		for (auto entity : entities)
		{
			uint64_t uuid = entity["Entity"].as<uint64_t>();

			std::string name;
			auto tagComponent = entity["TagComponent"];
			if (tagComponent)
			{
				name = tagComponent["Tag"].as<std::string>();
				std::cout << "Entity with id: " << uuid << " name: " << name << std::endl;
			}
			auto DeserializedEntity = manager->addEntity(name);

			auto transformComponent = entity["TransformComponent"];
			if(transformComponent)
			{
				auto& tc = DeserializedEntity->addComponent<CTransform>();
				tc.pos = transformComponent["Translation"].as<glm::vec3>();
				tc.rotation = transformComponent["Rotation"].as<glm::vec3>();
				tc.scale = transformComponent["Scale"].as<glm::vec3>();

			}
			auto cameraComponent = entity["CameraComponent"];

			if (cameraComponent)
			{
				auto& cc = DeserializedEntity->addComponent<CCamera>();

				auto cameraProps = cameraComponent["Camera"];

				cc.camera->SetProjectionType((Camera::ProjectionType)cameraProps["ProjectionType"].as<int>());
				cc.camera->SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
				cc.camera->SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
				cc.camera->SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
				
			}
			auto ModelComponent = entity["ModelComponent"];
			if (ModelComponent)
			{
				auto& mc = DeserializedEntity->addComponent<CModel>(std::make_shared<Model>(ModelComponent["Path"].as<std::string>(), DeserializedEntity->id()));
			}
		}
	}
}

bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
{
	return false;
}

