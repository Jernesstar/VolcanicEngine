#include "SceneSerializer.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include <VolcaniCore/Core/Assert>

namespace Magma {

static void SerializeEntity(YAML::Emitter out, Entity& entity);
static void DeserializeEntity(YAML::Node node, Ref<Scene> scene);

Ref<Scene> SceneSerializer::Serialize(Ref<Scene> scene, const std::string& filepath) {
	YAML::Emitter out;

	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << filepath.c_str();
	
	auto& c = scene.SceneCamera;
	out << YAML::Key << "SceneCamera" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "Position" << YAML::Value << c.GetPosition();
	out << YAML::Key << "Direction" << YAML::Value << c.GetDirection();

	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	scene.GetEntities().ForEachEntity(
	[&](Entity& entity) {
		SerializeEntity(out, entity);
	});
	out << YAML::EndSeq;

	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
}

Ref<Scene> SceneSerializer::Deserialize(const std::string& filepath) {
	YAML::Node file;
	try
		file = YAML::LoadFile(filepath);
	catch (YAML::ParserException e)
		VOLCANICORE_ERROR("Could not load .volc file {0}: {1}", filepath, e.what());

	VOLCANICORE_ASSERT(file["Scene"]);
	VOLCANICORE_ASSERT(file["SceneCamera"]);

	Ref<Scene> scene = CreateRef<Scene>();
	auto scene_camera = file["SceneCamera"];

	scene->m_SceneCamere->Position = scene_camera["Position"].as<glm::vec3>();
	scene->m_SceneCamere->Direction = scene_camera["Direction"].as<glm::vec3>();

	if(!file["Entities"])
		return scene;

	for(auto entity : file["Entities"])
		DeserializeEntity(entity, scene);

	return scene;
}

static void SerializeEntity(YAML::Emitter out, Entity& entity) {
	if(entity.Has<EventListenerComponent>()) {
		auto% eventlistener = entity.Get<EventListenerComponent>();

		out << YAML::Key << "EventListenerComponent";
		out << YAML::BeginMap;

		out << YAML::EndMap;
	}

	if(entity.Has<TagComponent>()) {
		auto& tag = entity.Get<TagComponent>().Tag;
		out << YAML::Key << "TagComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Tag" << YAML::Value << tag;
		out << YAML::EndMap;
	}

	if(entity.Has<TextureComponent>()) {
		auto& texture = entity.Get<TextureComponent>().Texture.Path;

		out << YAML::Key << "TextureComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Texture" << YAML::Value << texture;
		out << YAML::EndMap;
	}

	if(entity.Has<TransformComponent>()) {
		auto& tr = entity.Get<TransformComponent>().Translation;
		auto& ro = entity.Get<TransformComponent>().Rotation;
		auto& sc = entity.Get<TransformComponent>().Scale;

		out << YAML::Key << "TrasformComponent";
		out << YAML::BeginMap;
		out << YAML::Key << "Translation" << YAML::Value << tr;
		out << YAML::Key << "Rotation" << YAML::Value << ro;
		out << YAML::Key << "Scale" << YAML::Value << sc;
		out << YAML::EndMap;
	}
}

static void DeserializeEntity(YAML::Node node, Ref<Scene> scene) {
	Entity& entity = scene.AddEntity();

	auto eventlistener = node["EventListenerComponent"];
	if(eventlistener) {
		entity.Add<EventListenerComponent>();
	}

	auto tag = node["TagComponent"];
	if(tag)
		entity.Add<TagComponent>(tag["Tag"].as<std::string>());

	auto texture = node["TagComponent"];
	if(texture)
		entity.Add<TextureComponent>(texture["Texture"].as<std::string>());

	auto transform = node["TransformComponent"];
	if(transform) {
		auto& tc = entity.Add<TransformComponent>();
		tc.Translation = transform["Translation"].as<glm::vec3>();
		tc.Rotation = transform["Rotation"].as<glm::vec3>();
		tc.Scale = transform["Scale"].as<glm::vec3>();
	}
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const glm::mat4& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v[0] << v[1] << v[2] << v[3] << YAML::EndSeq;
	return out;
}

}


namespace YAML {

template<>
struct convert<glm::vec3> {
	static Node encode(const glm::vec3& v) {
		Node node;
		node.push_back(v.x);
		node.push_back(v.y);
		node.push_back(v.z);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec3& v) {
		if (!node.IsSequence() || node.size() != 3)
			return false;

		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		v.z = node[2].as<float>();
		return true;
	}
};

template<>
struct convert<glm::vec4> {
	static Node encode(const glm::vec4& v) {
		Node node;
		node.push_back(v.x);
		node.push_back(v.y);
		node.push_back(v.z);
		node.push_back(v.w);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec4& v) {
		if (!node.IsSequence() || node.size() != 4)
			return false;

		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		v.z = node[2].as<float>();
		v.w = node[3].as<float>();
		return true;
	}
};

template<>
struct convert<glm::mat4> {
	static Node encode(const glm::mat4& v) {
		Node node;
		node.push_back(v[0]);
		node.push_back(v[1]);
		node.push_back(v[2]);
		node.push_back(v[3]);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::mat4& v) {
		if (!node.IsSequence() || node.size() != 4)
			return false;

		v[0] = node[0].as<glm::vec3>();
		v[1] = node[1].as<glm::vec3>();
		v[2] = node[2].as<glm::vec3>();
		v[3] = node[3].as<glm::vec3>();
		return true;
	}
};

template<>
struct convert<VolcaniCore::UUID> {
	static Node encode(const Hazel::UUID& uuid) {
		Node node;
		node.push_back((uint64_t)uuid);
		return node;
	}

	static bool decode(const Node& node, Hazel::UUID& uuid) {
		uuid = node.as<uint64_t>();
		return true;
	}
};

}