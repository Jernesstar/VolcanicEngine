#include "SceneSerializer.h"
#include "Scene.h"

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/UUID.h>
#include <VolcaniCore/Renderer/StereographicCamera.h>
#include <VolcaniCore/Renderer/OrthographicCamera.h>

#include "Core/AssetManager.h"
#include "Core/YAMLSerializer.h"

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace Magma {

static void SerializeEntity(YAMLSerializer& out, Entity& entity);
static void DeserializeEntity(YAML::Node entityNode, Scene* scene);

template<>
Serializer& Serializer::Write(const VolcaniCore::Vertex& value) {
	SetOptions(Serializer::Options::ArrayOneLine);
	BeginSequence();
	Write(value.Position);
	Write(value.Normal);
	Write(value.TexCoord_Color);
	EndSequence();
	return *this;
}

SceneSerializer::SceneSerializer(Scene* scene)
	: m_Scene(scene) { }

void SceneSerializer::Serialize(const std::string& path) {
	YAMLSerializer serializer;
	serializer.BeginMapping(); // File

	serializer
	.WriteKey("Scene").BeginMapping()
		.WriteKey("Name").Write(m_Scene->Name);

		serializer.WriteKey("Entities").BeginSequence(); // Entities
		m_Scene->EntityWorld
		.ForEach(
			[&](Entity& entity)
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

void SceneSerializer::Deserialize(const std::string& path) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file {0}: {1}",
								path, e.what());
	}
	auto scene = file["Scene"];

	VOLCANICORE_ASSERT(scene);

	m_Scene->Name = scene["Name"].as<std::string>();

	for(auto node : scene["Entities"])
		DeserializeEntity(node["Entity"], m_Scene);
}

void SerializeEntity(YAMLSerializer& serializer, Entity& entity) {
	serializer.WriteKey("Entity").BeginMapping(); // Entity

	serializer.WriteKey("ID").Write((uint64_t)entity.GetHandle());

	serializer.WriteKey("Components")
	.BeginMapping(); // Components

	if(entity.Has<TagComponent>()) {
		auto& tag = entity.Get<TagComponent>().Tag;

		serializer.WriteKey("TagComponent")
		.BeginMapping()
			.WriteKey("Tag").Write(tag.c_str()) // TODO(Fix): Only works on const char*
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

	if(entity.Has<MeshComponent>()) {
		auto mesh = entity.Get<MeshComponent>().Mesh;
		auto& mat = mesh->GetMaterial();

		serializer.WriteKey("MeshComponent")
		.BeginMapping()
			.WriteKey("Mesh").BeginMapping();

		if(mesh->Path != "")
			serializer.WriteKey("Path").Write(mesh->Path);
		else {
			serializer.WriteKey("Vertices")
			.SetOptions(Serializer::Options::ArrayOneLine)
			.Write(mesh->GetVertices());

			serializer.WriteKey("Indices")
			.SetOptions(Serializer::Options::ArrayOneLine)
			.Write(mesh->GetIndices());

			serializer.WriteKey("Material")
			.BeginMapping();

			if(mat.Diffuse)
				serializer.WriteKey("Diffuse")
				.BeginMapping()
					.WriteKey("Path").Write(mat.Diffuse->GetPath())
				.EndMapping();

			if(mat.Specular)
				serializer.WriteKey("Specular")
				.BeginMapping()
					.WriteKey("Path").Write(mat.Specular->GetPath())
				.EndMapping();

			serializer
			.EndMapping(); // Material
		}
		serializer
		.EndMapping() // Mesh
		.EndMapping(); // MeshComponent
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
		// std::string& path = entity.Get<ScriptComponent>().Path;

		serializer.WriteKey("ScriptComponent")
		.BeginMapping()
			.WriteKey("Script").BeginMapping()
			// .WriteKey("Path").Write(path)
			.EndMapping()
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
	serializer.EndMapping(); // Components

	serializer.EndMapping(); // Entity
}

void DeserializeEntity(YAML::Node entityNode, Scene* scene) {
	auto components = entityNode["Components"];

	if(!components) {
		VolcaniCore::UUID id(entityNode["ID"].as<uint64_t>());
		scene->EntityWorld.AddEntity(id);
		return;
	}

	Entity entity;

	auto tagComponentNode = components["TagComponent"];
	if(tagComponentNode) {
		auto tag = tagComponentNode["Tag"].as<std::string>();
		entity = scene->EntityWorld.AddEntity(tag);
	}
	else {
		VolcaniCore::UUID id(entityNode["ID"].as<uint64_t>());
		entity = scene->EntityWorld.AddEntity(id);
	}

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

		entity.Add<CameraComponent>(type, pos, dir, w, h, near, far, fr);
	}

	auto meshComponentNode = components["MeshComponent"];
	if(meshComponentNode) {
		auto meshNode = meshComponentNode["Mesh"];

		if(meshNode["Path"]) {
			auto path = meshNode["Path"].as<std::string>();
			entity.Add<MeshComponent>(AssetManager::GetOrCreate<Mesh>(path));
		}
		else {
			auto v = meshNode["Vertices"].as<std::vector<VolcaniCore::Vertex>>();
			auto i = meshNode["Indices"].as<std::vector<uint32_t>>();
			auto diffuseNode = meshNode["Material"]["Diffuse"];
			auto specularNode = meshNode["Material"]["Specular"];

			Material mat;
			if(diffuseNode) {
				auto path = diffuseNode["Path"].as<std::string>();
				mat.Diffuse = Texture::Create(path);
			}
			if(specularNode) {
				auto path = specularNode["Path"].as<std::string>();
				mat.Diffuse = Texture::Create(path);
			}
			entity.Add<MeshComponent>(v, i, mat);
		}
	}

	auto scriptComponentNode = components["ScriptComponent"];
	if(scriptComponentNode) {
		// auto scriptNode = scriptComponentNode["Script"];
		// std::string path = scriptNode.as<std::string>();

		// ScriptComponent& sc = entity.Add<ScriptComponent>(path);

		ScriptComponent& sc = entity.Add<ScriptComponent>();
	}

	auto transformComponentNode = components["TransformComponent"];
	if(transformComponentNode) {
		auto transformNode = transformComponentNode["Transform"];
		auto translationNode = transformNode["Translation"];
		auto rotationNode	 = transformNode["Rotation"];
		auto scaleNode		 = transformNode["Scale"];

		TransformComponent& tc = entity.Add<TransformComponent>();
		tc.Translation = translationNode.as<glm::vec3>();
		tc.Rotation	   = rotationNode	.as<glm::vec3>();
		tc.Scale	   = scaleNode		.as<glm::vec3>();
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

		if(shapeType != Shape::Type::Mesh) {
			Ref<Shape> shape = Shape::Create(shapeType);
			entity.Add<RigidBodyComponent>(type, shape);
		}
		else {
			entity.Add<RigidBodyComponent>(type);
		}
	}
}

}


namespace YAML {

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
		if(!node.IsSequence() || node.size() != 3)
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
		node.push_back(vertex.TexCoord_Color);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, VolcaniCore::Vertex& vertex) {
		if(!node.IsSequence() || node.size() != 3)
			return false;

		vertex.Position		  = node[0].as<glm::vec3>();
		vertex.Normal		  = node[1].as<glm::vec3>();
		vertex.TexCoord_Color = node[2].as<glm::vec4>();
		return true;
	}
};

}