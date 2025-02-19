#include "SceneLoader.h"

#include <bitset>

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/FileUtils.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/UUID.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>
#include <VolcaniCore/Graphics/OrthographicCamera.h>

#include <Magma/Core/YAMLSerializer.h>

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Core/BinaryReader.h>

#include <Magma/Scene/Component.h>

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace Magma {

template<>
Serializer& Serializer::Write(const VolcaniCore::Vertex& value) {
	SetOptions(Serializer::Options::ArrayOneLine);
	BeginSequence();
		Write(value.Position);
		Write(value.Normal);
		Write(value.TexCoord);
	EndSequence();
	return *this;
}

template<>
Serializer& Serializer::Write(const Asset& value) {
	BeginMapping();
		WriteKey("ID").Write((uint64_t)value.ID);
		WriteKey("Type").Write((uint32_t)value.Type);
	EndMapping();
	return *this;
}

}

namespace Magma {

static void DeserializeEntity(YAML::Node entityNode, Scene& scene);
static void SerializeEntity(YAMLSerializer& out, const Entity& entity);

void SceneLoader::EditorLoad(Scene& scene, const std::string& path) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file %s: %s",
								path.c_str(), e.what());
	}
	auto sceneNode = file["Scene"];

	VOLCANICORE_ASSERT(sceneNode);

	scene.Name = sceneNode["Name"].as<std::string>();

	for(auto node : sceneNode["Entities"])
		DeserializeEntity(node["Entity"], scene);
}

void SceneLoader::EditorSave(const Scene& scene, const std::string& path) {
	YAMLSerializer serializer;
	serializer.BeginMapping(); // File

	serializer
	.WriteKey("Scene").BeginMapping()
		.WriteKey("Name").Write(scene.Name);

		serializer.WriteKey("Entities").BeginSequence(); // Entities
		scene.EntityWorld
		.ForEach(
			[&](const Entity& entity)
			{
				serializer.BeginMapping(); // Entity
				SerializeEntity(serializer, entity);
				serializer.EndMapping(); // Entity
			});
		serializer.EndSequence(); // Entities

	serializer.EndMapping(); // Scene

	serializer.EndMapping(); // File

	serializer.Finalize(path);
}

void SerializeEntity(YAMLSerializer& serializer, const Entity& entity) {
	serializer.WriteKey("Entity").BeginMapping(); // Entity

	serializer.WriteKey("Name").Write(entity.GetName());
	serializer.WriteKey("ID").Write((uint64_t)entity.GetHandle());

	serializer.WriteKey("Components")
	.BeginMapping(); // Components

	if(entity.Has<CameraComponent>()) {
		auto& camera = entity.Get<CameraComponent>().Cam;
		auto type = camera->GetType();
		auto s = type == Camera::Type::Ortho ? "Orthographic" : "Stereographic";

		serializer.WriteKey("CameraComponent")
		.BeginMapping()
			.WriteKey("Camera").BeginMapping()
			.WriteKey("Type").Write(s);

		if(type == Camera::Type::Stereo)
			serializer
			.WriteKey("VerticalFOV")
			.Write(camera->As<StereographicCamera>()->GetVerticalFOV());
		else if(type == Camera::Type::Ortho)
			serializer
			.WriteKey("Rotation")
			.Write(camera->As<OrthographicCamera>()->GetRotation());

		serializer
			.WriteKey("Position").Write(camera->GetPosition())
			.WriteKey("Direction").Write(camera->GetDirection())
			.WriteKey("ViewportWidth").Write(camera->GetViewportWidth())
			.WriteKey("ViewportHeight").Write(camera->GetViewportHeight())
			.WriteKey("Near").Write(camera->GetNear())
			.WriteKey("Far").Write(camera->GetFar())
		.EndMapping()
		.EndMapping();
	}

	if(entity.Has<TagComponent>()) {
		auto& tag = entity.Get<TagComponent>().Tag;

		serializer.WriteKey("TagComponent")
		.BeginMapping()
			.WriteKey("Tag").Write(tag)
		.EndMapping();
	}

	if(entity.Has<TransformComponent>()) {
		auto& t = entity.Get<TransformComponent>().Translation;
		auto& r = entity.Get<TransformComponent>().Rotation;
		auto& s = entity.Get<TransformComponent>().Scale;

		serializer.WriteKey("TransformComponent")
		.BeginMapping()
			.WriteKey("Transform").BeginMapping()
				.WriteKey("Translation").Write(t)
				.WriteKey("Rotation")	.Write(r)
				.WriteKey("Scale")		.Write(s)
			.EndMapping()
		.EndMapping(); // TransformComponent
	}

	if(entity.Has<MeshComponent>()) {
		auto asset = entity.Get<MeshComponent>().MeshAsset;
		serializer.WriteKey("MeshComponent")
		.BeginMapping()
			.WriteKey("Asset").Write(asset)
		.EndMapping();
	}

	if(entity.Has<SkyboxComponent>()) {
		auto asset = entity.Get<SkyboxComponent>().CubemapAsset;
		serializer.WriteKey("SkyboxComponent")
		.BeginMapping()
			.WriteKey("Asset").Write(asset)
		.EndMapping();
	}

	if(entity.Has<RigidBodyComponent>()) {
		auto body = entity.Get<RigidBodyComponent>().Body;
		auto type = body->GetType();
		auto t = type == RigidBody::Type::Static ? "Static" : "Dynamic";

		serializer.WriteKey("RigidBodyComponent")
		.BeginMapping()
			.WriteKey("Body").BeginMapping()
				.WriteKey("Type").Write(t);

		if(body->HasShape()) {
			std::string shapeType;
			switch(body->GetShape()->GetType()) {
				case Shape::Type::Box:
					shapeType = "Box";
					break;
				case Shape::Type::Sphere:
					shapeType = "Sphere";
					break;
				case Shape::Type::Plane:
					shapeType = "Plane";
					break;
				case Shape::Type::Capsule:
					shapeType = "Capsule";
					break;
				case Shape::Type::Mesh:
					shapeType = "Mesh";
					break;
			}

			serializer
				.WriteKey("ShapeType").Write(shapeType);
		}
		serializer
			.EndMapping() // Body
		.EndMapping(); // RigidBodyComponent
	}

	if(entity.Has<ScriptComponent>()) {
		std::string _class = entity.Get<ScriptComponent>().Class;

		serializer.WriteKey("ScriptComponent")
		.BeginMapping()
			.WriteKey("Script").BeginMapping()
				.WriteKey("Class").Write(_class)
			.EndMapping()
		.EndMapping();
	}

	if(entity.Has<DirectionalLightComponent>()) {

	}

	if(entity.Has<PointLightComponent>()) {

	}

	if(entity.Has<SpotlightComponent>()) {

	}

	if(entity.Has<ParticleSystemComponent>()) {

	}

	serializer.EndMapping(); // Components

	serializer.EndMapping(); // Entity
}

void DeserializeEntity(YAML::Node entityNode, Scene& scene) {
	Entity entity =
		scene.EntityWorld.AddEntity(entityNode["ID"].as<uint64_t>());
	auto name = entityNode["Name"].as<std::string>();
	if(name != "")
		entity.SetName(name);

	auto components = entityNode["Components"];

	auto cameraComponentNode = components["CameraComponent"];
	if(cameraComponentNode) {
		auto cameraNode = cameraComponentNode["Camera"];
		auto pos  = cameraNode["Position"]		.as<glm::vec3>();
		auto dir  = cameraNode["Direction"]		.as<glm::vec3>();
		auto w	  = cameraNode["ViewportWidth"] .as<uint32_t>();
		auto h	  = cameraNode["ViewportHeight"].as<uint32_t>();
		auto near = cameraNode["Near"]			.as<float>();
		auto far  = cameraNode["Far"]			.as<float>();
		auto typeStr = cameraNode["Type"].as<std::string>();
		float fr;
		Camera::Type type;

		if(typeStr == "Stereographic") {
			type = Camera::Type::Stereo;
			fr = cameraNode["VerticalFOV"].as<float>();
		}
		else if(typeStr == "Orthographic") {
			type = Camera::Type::Ortho;
			fr = cameraNode["Rotation"].as<float>();
		}

		auto camera = Camera::Create(type, fr);
		camera->SetPositionDirection(pos, dir);
		camera->Resize(w, h);
		camera->SetProjection(near, far);

		entity.Add<CameraComponent>(camera);
	}

	auto tagComponentNode = components["TagComponent"];
	if(tagComponentNode) {
		auto tag = tagComponentNode["Tag"].as<std::string>();
		entity.Add<TagComponent>(tag);
	}

	auto transformComponentNode = components["TransformComponent"];
	if(transformComponentNode) {
		auto transformNode = transformComponentNode["Transform"];
		auto translationNode = transformNode["Translation"];
		auto rotationNode	 = transformNode["Rotation"];
		auto scaleNode		 = transformNode["Scale"];

		entity.Add<TransformComponent>(
			Transform
			{
				.Translation = translationNode.as<glm::vec3>(),
				.Rotation	 = rotationNode.as<glm::vec3>(),
				.Scale		 = scaleNode.as<glm::vec3>()
			});
	}

	auto meshComponentNode = components["MeshComponent"];
	if(meshComponentNode) {
		auto assetNode = meshComponentNode["Asset"];
		auto id = assetNode["ID"].as<uint64_t>();
		entity.Add<MeshComponent>(Asset{ id, AssetType::Mesh });
	}

	auto skyboxComponentNode = components["SkyboxComponent"];
	if(skyboxComponentNode) {
		auto assetNode = skyboxComponentNode["Asset"];
		auto id = assetNode["ID"].as<uint64_t>();
		entity.Add<SkyboxComponent>(Asset{ id, AssetType::Cubemap });
	}

	auto scriptComponentNode = components["ScriptComponent"];
	if(scriptComponentNode) {
		auto classNode = scriptComponentNode["Class"];
		std::string _class = classNode.as<std::string>();
		entity.Add<ScriptComponent>(_class);
	}

	auto rigidBodyComponentNode = components["RigidBodyComponent"];
	if(rigidBodyComponentNode) {
		auto rigidBodyNode = rigidBodyComponentNode["Body"];
		auto typeStr	   = rigidBodyNode["Type"].as<std::string>();
		auto shapeTypeStr  = rigidBodyNode["Shape Type"].as<std::string>();

		RigidBody::Type type = (typeStr == "Static") ? RigidBody::Type::Static
							 						 : RigidBody::Type::Dynamic;
		Shape::Type shapeType;
		if(shapeTypeStr == "Box")	  shapeType = Shape::Type::Box;
		if(shapeTypeStr == "Sphere")  shapeType = Shape::Type::Sphere;
		if(shapeTypeStr == "Plane")	  shapeType = Shape::Type::Plane;
		if(shapeTypeStr == "Capsule") shapeType = Shape::Type::Capsule;
		if(shapeTypeStr == "Mesh")	  shapeType = Shape::Type::Mesh;

		Ref<RigidBody> body;
		if(shapeType == Shape::Type::Mesh)
			body = RigidBody::Create(type);
		else {
			Ref<Shape> shape = Shape::Create(shapeType);
			body = RigidBody::Create(type, shape);
		}

		entity.Add<RigidBodyComponent>(body);
	}

	auto soundComponent = components["SoundComponent"];
	if(soundComponent) {

	}

	auto directionalLightComponentNode = components["DirectionalLightComponent"];
	if(directionalLightComponentNode) {

	}

	auto pointLightComponentNode = components["PointLightComponent"];
	if(pointLightComponentNode) {

	}

	auto spotlightComponentNode = components["SpotlightComponent"];
	if(spotlightComponentNode) {

	}

	auto particleSystemComponent = components["ParticleSystemComponent"];
	if(particleSystemComponent) {

	}
}

}

namespace Magma {

template<>
BinaryWriter& BinaryWriter::WriteObject(const CameraComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const TagComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const TransformComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const MeshComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const SkyboxComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const RigidBodyComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const ScriptComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const SoundComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const DirectionalLightComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const PointLightComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const SpotlightComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const ParticleSystemComponent& comp) {

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const Entity& entity) {
	Write((uint64_t)entity.GetHandle());
	Write(entity.GetName());

	std::bitset<12> componentBits;
	componentBits |= (entity.Has<CameraComponent>()			<< 0);
	componentBits |= (entity.Has<TagComponent>()			<< 1);
	componentBits |= (entity.Has<TransformComponent>()		<< 2);
	componentBits |= (entity.Has<MeshComponent>()			<< 3);
	componentBits |= (entity.Has<SkyboxComponent>()			<< 4);
	componentBits |= (entity.Has<RigidBodyComponent>()		<< 5);
	componentBits |= (entity.Has<ScriptComponent>()			<< 6);
	componentBits |= (entity.Has<SoundComponent>()			<< 7);
	componentBits |= (entity.Has<DirectionalLightComponent>() << 8);
	componentBits |= (entity.Has<PointLightComponent>()		<< 9);
	componentBits |= (entity.Has<SpotlightComponent>()		<< 10);
	componentBits |= (entity.Has<ParticleSystemComponent>()	<< 11);

	Write(componentBits.to_ulong());

	if(entity.Has<CameraComponent>())
		Write(entity.Get<CameraComponent>());
	if(entity.Has<TagComponent>())
		Write(entity.Get<TagComponent>());
	if(entity.Has<TransformComponent>())
		Write(entity.Get<TransformComponent>());
	if(entity.Has<MeshComponent>())
		Write(entity.Get<MeshComponent>());
	if(entity.Has<SkyboxComponent>())
		Write(entity.Get<SkyboxComponent>());
	if(entity.Has<RigidBodyComponent>())
		Write(entity.Get<RigidBodyComponent>());
	if(entity.Has<ScriptComponent>())
		Write(entity.Get<ScriptComponent>());
	if(entity.Has<SoundComponent>())
		Write(entity.Get<SoundComponent>());
	if(entity.Has<DirectionalLightComponent>())
		Write(entity.Get<DirectionalLightComponent>());
	if(entity.Has<PointLightComponent>())
		Write(entity.Get<PointLightComponent>());
	if(entity.Has<SpotlightComponent>())
		Write(entity.Get<SpotlightComponent>());
	if(entity.Has<ParticleSystemComponent>())
		Write(entity.Get<ParticleSystemComponent>());

	return *this;
}

}

namespace Magma {

void SceneLoader::RuntimeSave(const Scene& scene,
							  const std::string& projectPath,
							  const std::string& exportPath)
{
	namespace fs = std::filesystem;

	auto scenePath =
		(fs::path(projectPath) / "Visual" / "Scene" / "Schema" / scene.Name
		).string() + ".magma.scene";
	auto binPath =
		(fs::path(exportPath) / "Scene" / "Data" / scene.Name).string() + ".bin";

	BinaryWriter writer(binPath);

	writer.Write(scene.Name);

	uint64_t entityCount = 0;
	uint64_t idx = writer.GetPosition();
	writer.Advance(sizeof(uint64_t));

	scene.EntityWorld
	.ForEach(
		[&](const Entity& entity)
		{
			entityCount++;
			writer.Write(entity);
		});

	writer.SetPosition(idx);
	writer.Write(entityCount);
}

}


namespace YAML {

template<typename T>
struct convert<List<T>> {
	static Node encode(const List<T>& list) {
		Node node;
		for(auto& val : list)
			node.push_back(val);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, List<T>& v) {
		if(!node.IsSequence())
			return false;
		for(uint64_t i = 0; i < node.size(); i++)
			v.Add(node[i].as<T>());
		return true;
	}
};

template<>
struct convert<glm::vec2> {
	static Node encode(const glm::vec2& v) {
		Node node;
		node.push_back(v.x);
		node.push_back(v.y);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec2& v) {
		if(!node.IsSequence() || node.size() != 2)
			return false;

		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		return true;
	}
};

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
		if(!node.IsSequence() || node.size() != 3)
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
		if(!node.IsSequence() || node.size() != 4)
			return false;

		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		v.z = node[2].as<float>();
		v.w = node[3].as<float>();
		return true;
	}
};

template<>
struct convert<VolcaniCore::Vertex> {
	static Node encode(const VolcaniCore::Vertex& vertex) {
		Node node;
		node.push_back(vertex.Position);
		node.push_back(vertex.Normal);
		node.push_back(vertex.TexCoord);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, VolcaniCore::Vertex& vertex) {
		if(!node.IsSequence() || node.size() != 3)
			return false;

		vertex.Position = node[0].as<glm::vec3>();
		vertex.Normal	= node[1].as<glm::vec3>();
		vertex.TexCoord = node[2].as<glm::vec2>();
		return true;
	}
};

}