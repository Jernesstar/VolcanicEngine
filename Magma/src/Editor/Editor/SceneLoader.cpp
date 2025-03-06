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

#include <Magma/Script/ScriptClass.h>

#include <Magma/Scene/Component.h>

#include <Lava/App.h>

#include "EditorApp.h"

using namespace Magma::ECS;
using namespace Magma::Physics;
using namespace Lava;

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

	if(entity.Has<TagComponent>()) {
		auto& tag = entity.Get<TagComponent>().Tag;

		serializer.WriteKey("TagComponent")
		.BeginMapping()
			.WriteKey("Tag").Write(tag)
		.EndMapping();
	}
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
	if(entity.Has<TransformComponent>()) {
		const auto& transform = entity.Get<TransformComponent>();

		serializer.WriteKey("TransformComponent")
		.BeginMapping()
			.WriteKey("Transform").BeginMapping()
				.WriteKey("Translation").Write(transform.Translation)
				.WriteKey("Rotation")	.Write(transform.Rotation)
				.WriteKey("Scale")		.Write(transform.Scale)
			.EndMapping()
		.EndMapping(); // TransformComponent
	}
	if(entity.Has<MeshComponent>()) {
		auto asset = entity.Get<MeshComponent>().MeshAsset;
		serializer.WriteKey("MeshComponent")
		.BeginMapping()
			.WriteKey("AssetID").Write((uint64_t)asset.ID)
		.EndMapping();
	}
	if(entity.Has<SkyboxComponent>()) {
		auto asset = entity.Get<SkyboxComponent>().CubemapAsset;
		serializer.WriteKey("SkyboxComponent")
		.BeginMapping()
			.WriteKey("AssetID").Write((uint64_t)asset.ID)
		.EndMapping();
	}
	if(entity.Has<AudioComponent>()) {
		auto asset = entity.Get<AudioComponent>().AudioAsset;
		serializer.WriteKey("AudioComponent")
		.BeginMapping()
			.WriteKey("AssetID").Write((uint64_t)asset.ID)
		.EndMapping();
	}
	if(entity.Has<ScriptComponent>()) {
		const auto& comp = entity.Get<ScriptComponent>();
		auto asset = comp.ModuleAsset;
		auto obj = comp.Instance;

		serializer.WriteKey("ScriptComponent")
		.BeginMapping()
			.WriteKey("ModuleAsset").Write((uint64_t)asset.ID)
			.WriteKey("Class").Write(obj ? obj->GetClass()->Name : "")
			.WriteKey("Instance").BeginMapping()
				// TODO(Implement): Reflection
			.EndMapping()
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

			serializer.WriteKey("ShapeType").Write(shapeType);
		}
		serializer
			.EndMapping() // Body
		.EndMapping(); // RigidBodyComponent
	}
	if(entity.Has<DirectionalLightComponent>()) {
		const auto& light = entity.Get<DirectionalLightComponent>();

		serializer.WriteKey("DirectionalLightComponent")
		.BeginMapping()
			.WriteKey("Light")
			.BeginMapping()
				.WriteKey("Ambient").Write(light.Ambient)
				.WriteKey("Diffuse").Write(light.Diffuse)
				.WriteKey("Specular").Write(light.Specular)
				.WriteKey("Position").Write(light.Position)
				.WriteKey("Direction").Write(light.Direction)
			.EndMapping()
		.EndMapping(); // DirectionalLightComponent
	}
	if(entity.Has<PointLightComponent>()) {
		const auto& light = entity.Get<PointLightComponent>();
		
		serializer.WriteKey("PointLightComponent")
		.BeginMapping()
			.WriteKey("Light")
			.BeginMapping()
				.WriteKey("Ambient").Write(light.Ambient)
				.WriteKey("Diffuse").Write(light.Diffuse)
				.WriteKey("Specular").Write(light.Specular)
				.WriteKey("Position").Write(light.Position)
				.WriteKey("Constant").Write(light.Constant)
				.WriteKey("Linear").Write(light.Linear)
				.WriteKey("Quadratic").Write(light.Quadratic)
			.EndMapping()
		.EndMapping(); // PointLightComponent
	}
	if(entity.Has<SpotlightComponent>()) {
		const auto& light = entity.Get<SpotlightComponent>();
	
		serializer.WriteKey("SpotlightComponent")
		.BeginMapping()
			.WriteKey("Light")
			.BeginMapping()
				.WriteKey("Ambient").Write(light.Ambient)
				.WriteKey("Diffuse").Write(light.Diffuse)
				.WriteKey("Specular").Write(light.Specular)
				.WriteKey("Position").Write(light.Position)
				.WriteKey("Direction").Write(light.Direction)
				.WriteKey("CutoffAngle").Write(light.CutoffAngle)
				.WriteKey("OuterCutoffAngle").Write(light.OuterCutoffAngle)
			.EndMapping()
		.EndMapping(); // SpotlightComponent
	}
	if(entity.Has<ParticleSystemComponent>()) {
		const auto& system = entity.Get<ParticleSystemComponent>();

		serializer.WriteKey("ParticleSystemComponent")
		.BeginMapping()
			.WriteKey("System")
			.BeginMapping()
				.WriteKey("Position").Write(system.Position)
				.WriteKey("MaxParticleCount").Write(system.MaxParticleCount)
				.WriteKey("ParticleLifetime").Write(system.ParticleLifetime)
				.WriteKey("AssetID").Write((uint64_t)system.ImageAsset.ID)
			.EndMapping()
		.EndMapping(); // ParticleSystemComponent
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

	auto& assetManager = App::Get()->GetAssetManager();
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
		entity.Add<TransformComponent>(
			Transform
			{
				.Translation = transformNode["Translation"].as<glm::vec3>(),
				.Rotation	 = transformNode["Rotation"].as<glm::vec3>(),
				.Scale		 = transformNode["Scale"].as<glm::vec3>()
			});
	}

	auto audioComponentNode = components["AudioComponent"];
	if(audioComponentNode) {
		auto id = audioComponentNode["AssetID"].as<uint64_t>();
		entity.Add<AudioComponent>(Asset{ id, AssetType::Audio });
	}

	auto meshComponentNode = components["MeshComponent"];
	if(meshComponentNode) {
		auto id = meshComponentNode["AssetID"].as<uint64_t>();
		entity.Add<MeshComponent>(Asset{ id, AssetType::Mesh });
	}

	auto skyboxComponentNode = components["SkyboxComponent"];
	if(skyboxComponentNode) {
		auto id = skyboxComponentNode["AssetID"].as<uint64_t>();
		entity.Add<SkyboxComponent>(Asset{ id, AssetType::Cubemap });
	}

	auto scriptComponentNode = components["ScriptComponent"];
	if(scriptComponentNode) {
		auto id = scriptComponentNode["ModuleAsset"].as<uint64_t>();
		Asset asset = { id, AssetType::Script };
		auto classNode = scriptComponentNode["Class"];
		std::string className = classNode.as<std::string>();
		if(className != "") {
			auto mod = assetManager.Get<ScriptModule>(asset);
			auto _class = mod->GetClass(className);
			entity.Add<ScriptComponent>(asset, _class->Instantiate(entity));
		}
		else
			entity.Add<ScriptComponent>();
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

	auto directionalLightComponentNode = components["DirectionalLightComponent"];
	if(directionalLightComponentNode) {
		auto lightNode = directionalLightComponentNode["Light"];
		entity.Add<DirectionalLightComponent>(
			lightNode["Ambient"].as<glm::vec3>(),
			lightNode["Diffuse"].as<glm::vec3>(),
			lightNode["Specular"].as<glm::vec3>(),
			lightNode["Position"].as<glm::vec3>(),
			lightNode["Direction"].as<glm::vec3>());
	}

	auto pointLightComponentNode = components["PointLightComponent"];
	if(pointLightComponentNode) {
		auto lightNode = directionalLightComponentNode["Light"];
		entity.Add<PointLightComponent>(
			lightNode["Ambient"].as<glm::vec3>(),
			lightNode["Diffuse"].as<glm::vec3>(),
			lightNode["Specular"].as<glm::vec3>(),
			lightNode["Position"].as<glm::vec3>(),
			lightNode["Constant"].as<float>(),
			lightNode["Linear"].as<float>(),
			lightNode["Quadratic"].as<float>());
	}

	auto spotlightComponentNode = components["SpotlightComponent"];
	if(spotlightComponentNode) {
		auto lightNode = spotlightComponentNode["Light"];
		entity.Add<SpotlightComponent>(
			lightNode["Ambient"].as<glm::vec3>(),
			lightNode["Diffuse"].as<glm::vec3>(),
			lightNode["Specular"].as<glm::vec3>(),
			lightNode["Position"].as<glm::vec3>(),
			lightNode["Direction"].as<glm::vec3>(),
			lightNode["CutoffAngle"].as<float>(),
			lightNode["OuterCutoffAngle"].as<float>());
	}

	auto particleSystemComponentNode = components["ParticleSystemComponent"];
	if(particleSystemComponentNode) {
		auto system = particleSystemComponentNode["System"];
		entity.Add<ParticleSystemComponent>(
			system["Position"].as<glm::vec3>(),
			system["MaxParticleCount"].as<uint64_t>(),
			system["ParticleLifetime"].as<float>(),
			Asset{ system["AssetID"].as<uint64_t>(), AssetType::Texture });
	}
}

}

namespace Magma {

template<>
BinaryWriter& BinaryWriter::WriteObject(const glm::vec3& vec) {
	Write(vec.x);
	Write(vec.y);
	Write(vec.z);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const CameraComponent& comp) {
	auto camera = comp.Cam;

	Write((uint32_t)camera->GetType());
	if(camera->GetType() == Camera::Type::Stereo)
		Write(camera->As<StereographicCamera>()->GetVerticalFOV());
	else
		Write(camera->As<OrthographicCamera>()->GetRotation());

	Write(camera->GetPosition());
	Write(camera->GetDirection());
	Write(camera->GetViewportWidth());
	Write(camera->GetViewportHeight());
	Write(camera->GetNear());
	Write(camera->GetFar());

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const TagComponent& comp) {
	Write(comp.Tag);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const TransformComponent& comp) {
	Write(comp.Translation);
	Write(comp.Rotation);
	Write(comp.Scale);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const MeshComponent& comp) {
	Write((uint64_t)comp.MeshAsset.ID);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const SkyboxComponent& comp) {
	Write((uint64_t)comp.CubemapAsset.ID);
	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const AudioComponent& comp) {
	Write((uint64_t)comp.AudioAsset.ID);

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const ScriptComponent& comp) {
	Write((uint64_t)comp.ModuleAsset.ID);
	Write(comp.Instance ? comp.Instance->GetClass()->Name : std::string(""));

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const RigidBodyComponent& comp) {
	auto body = comp.Body;

	Write(body->GetType());
	Write(body->GetShape()->GetType());

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const DirectionalLightComponent& comp) {
	Write(comp.Ambient);
	Write(comp.Diffuse);
	Write(comp.Specular);
	Write(comp.Position);
	Write(comp.Direction);

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const PointLightComponent& comp) {
	Write(comp.Ambient);
	Write(comp.Diffuse);
	Write(comp.Specular);
	Write(comp.Position);
	Write(comp.Constant);
	Write(comp.Linear);
	Write(comp.Quadratic);

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const SpotlightComponent& comp) {
	Write(comp.Ambient);
	Write(comp.Diffuse);
	Write(comp.Specular);
	Write(comp.Position);
	Write(comp.Direction);
	Write(comp.CutoffAngle);
	Write(comp.OuterCutoffAngle);

	return *this;
}

template<>
BinaryWriter& BinaryWriter::WriteObject(const ParticleSystemComponent& comp) {
	Write(comp.Position);
	Write(comp.MaxParticleCount);
	Write(comp.ParticleLifetime);
	Write((uint64_t)comp.ImageAsset.ID);

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
	componentBits |= (entity.Has<AudioComponent>()			<< 7);
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
	if(entity.Has<AudioComponent>())
		Write(entity.Get<AudioComponent>());
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
		(fs::path(projectPath) / "Visual" / "Scene" / scene.Name
		).string() + ".magma.scene";
	auto binPath =
		(fs::path(exportPath) / "Scene" / scene.Name).string() + ".bin";

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