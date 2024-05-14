#include "SceneSerializer.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/UUID.h>

namespace Magma {

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

static void SerializeEntity(YAML::Emitter& out, Entity& entity);
static void DeserializeEntity(YAML::Node node, Ref<Scene> scene);

void SceneSerializer::Serialize(Ref<Scene> scene, const std::string& filepath) {
	YAML::Emitter out;

	out << YAML::BeginMap; // Scene
	out << YAML::Key << "Scene" << YAML::Value << YAML::BeginMap;
	out << YAML::Key << "Name" << YAML::Value <<  scene->Name;

	auto c = scene->Camera;
	out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap; // Camera
	out << YAML::Key << "Position" << YAML::Value << c->GetPosition();
	out << YAML::Key << "Direction" << YAML::Value << c->GetDirection();
	out << YAML::Key << "Type" << YAML::Value << c->GetType() == CameraType::Stereo ? "Stereo" : "Ortho";
	if(c.Type == CameraType::Stereo) {
		out << YAML::Key << "VerticalFOV" << YAML::Value << c->GetVerticalFOV();
		out << YAML::Key << "NearClip" << YAML::Value << c->GetNearClip();
		out << YAML::Key << "FarClip" << YAML::Value << c->GetFarClip();
		out << YAML::Key << "RotationSpeed" << YAML::Value << c->GetRotationSpeed();
		out << YAML::Key << "ViewportWidth" << YAML::Value << c->GetViewportWidth();
		out << YAML::Key << "ViewportHeight" << YAML::Value << c->GetViewportHeight();
	}
	else if(c.Type == CameraType::Ortho) {
		out << YAML::Key << "GetRotation" << YAML::Value << c->GetRotation();
		out << YAML::Key << "GetLeft" << YAML::Value << c->GetLeft();
		out << YAML::Key << "GetRight" << YAML::Value << c->GetRight();
		out << YAML::Key << "GetBottom" << YAML::Value << c->GetBottom();
		out << YAML::Key << "GetTop" << YAML::Value << c->GetTop();
	}
	out << YAML::EndMap; // Camera

	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq; // Entities
	scene->GetEntitySystem().ForEachEntity(
	[&](Entity& entity) {
		out << YAML::BeginMap; // Entity
		SerializeEntity(out, entity);
		out << YAML::EndMap; // Entity
	});
	out << YAML::EndSeq; // Entities

	out << YAML::EndMap;
	out << YAML::EndMap; // Scene

	std::ofstream fout(filepath);
	fout << out.c_str();
}

Ref<Scene> SceneSerializer::Deserialize(const std::string& filepath) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(filepath);
	}
	catch (YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file {0}: {1}", filepath, e.what());
	}
	auto scene = file["Scene"];
	auto camera = scene["Camera"];

	VOLCANICORE_ASSERT(scene);
	VOLCANICORE_ASSERT(camera);

	Ref<Scene> newScene = CreateRef<Scene>(file["Scene"].as<std::string>());

	auto camera = file["Camera"];
	if(camera["Type"].as<std::string>() == "Stereo") {
		auto verticalFOV = camera["VerticalFOV"].as<float>();
		auto nearClip = camera["NearClip"].as<float>();
		auto farClip = camera["FarClip"].as<float>();
		auto rotationSpeed = camera["RotationSpeed"].as<float>();
		auto viewportWidth = camera["ViewportWidth"].as<float>();
		auto viewportHeight = camera["ViewportHeight"].as<float>();

		newScene->Camera = CreateRef<StereographicCamera>(verticalFOV, nearClip, farClip, rotationSpeed, viewportWidth, viewportHeight);
	}
	if(camera["Type"].as<std::string>() == "Ortho") {
		auto rotation = camera["Rotation"].as<float>();
		auto left = camera["Left"].as<float>();
		auto right = camera["Right"].as<float>();
		auto bottom = camera["Bottom"].as<float>();
		auto top = camera["Top"].as<float>();

		newScene->Camera = CreateRef<OrthographicCameraCamera>(left, right, bottom, top, rotation);
	}

	newScene->Camera->SetPosition(camera["Position"].as<glm::vec3>());
	newScene->Camera->SetDirection(camera["Direction"].as<glm::vec3>());

	if(!scene["Entities"]) return scene;

	for(auto entity : scene["Entities"])
		DeserializeEntity(entity["Entity"], scene);

	return newScene;
}

void SerializeEntity(YAML::Emitter& out, Entity& entity) {
	out << YAML::Key << "Entity";
	out << YAML::BeginMap;

	out << YAML::Key << "ID" << YAML::Value << entity.GetID();

	if(entity.Has<EventListenerComponent>()) {
		auto& eventlistener = entity.Get<EventListenerComponent>();

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
		auto& texture = entity.Get<TextureComponent>().Texture->Path;

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

	out << YAML::EndMap;
}

void DeserializeEntity(YAML::Node node, Ref<Scene> scene) {
	Entity& entity = scene->GetEntitySystem().AddEntity();

	auto eventlistener = node["EventListenerComponent"];
	if(eventlistener) {
		entity.Add<EventListenerComponent>();
	}

	auto tag = node["TagComponent"];
	if(tag)
		entity.Add<TagComponent>(tag["Tag"].as<std::string>());

	auto texture = node["TextureComponent"];
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
struct convert<VolcaniCore::UUID> {
	static Node encode(const VolcaniCore::UUID& uuid) {
		Node node;
		node.push_back((uint64_t)uuid);
		return node;
	}

	static bool decode(const Node& node, VolcaniCore::UUID& uuid) {
		uuid = node.as<uint64_t>();
		return true;
	}
};

}