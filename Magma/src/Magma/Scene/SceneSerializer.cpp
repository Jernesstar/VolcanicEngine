#include "SceneSerializer.h"
#include "Scene.h"

#include <fstream>

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/UUID.h>
#include <VolcaniCore/Renderer/StereographicCamera.h>
#include <VolcaniCore/Renderer/OrthographicCamera.h>

#include "Core/Serialize.h"

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace YAML {

YAML::Emitter& operator <<(YAML::Emitter& out, const VolcaniCore::Vertex& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq;
	out << v.Position << v.Normal << v.TexCoord_Color;
	out << YAML::EndSeq;
	return out;
}

}

namespace Magma {

static void SerializeEntity(YAML::Emitter& out, Entity& entity);
static void DeserializeEntity(YAML::Node entityNode, Scene* scene);

SceneSerializer::SceneSerializer(Scene* scene)
	: m_Scene(scene) { }

void SceneSerializer::Serialize(const std::string& path) {
	YAML::Emitter out;
	out << YAML::BeginMap; // File

	out << YAML::Key << "Scene" << YAML::BeginMap; // Scene
	out << YAML::Key << "Name" << YAML::Value << m_Scene->Name;

	out << YAML::Key << "Entities" << YAML::BeginSeq; // Entities
	m_Scene->EntityWorld
	.ForEach(
		[&](Entity& entity)
		{
			out << YAML::BeginMap; // Entity
			SerializeEntity(out, entity);
			out << YAML::EndMap; // Entity
		});
	out << YAML::EndSeq; // Entities

	out << YAML::EndMap; // Scene

	out << YAML::EndMap; // File

	std::ofstream fout(path);
	fout << out.c_str();
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

void SerializeEntity(YAML::Emitter& out, Entity& entity) {
	out << YAML::Key << "Entity" << YAML::BeginMap; // Entity
	out << YAML::Key << "ID" << YAML::Value << (uint64_t)entity.GetHandle();

	out << YAML::Key << "Components" << YAML::BeginMap; // Components

	if(entity.Has<CameraComponent>()) {
		auto& camera = entity.Get<CameraComponent>();

		out << YAML::Key << "Camera" << YAML::BeginMap; // Camera
		out << YAML::Key << "Type" << YAML::Value
			<< (camera.CameraType == Camera::Type::Stereo ? "Stereographic"
														  : "Orthographic");
		if(camera.CameraType == Camera::Type::Stereo) {
			out << YAML::Key << "VerticalFOV"
				<< YAML::Value << camera.VerticalFOV;
		}
		if(camera.CameraType == Camera::Type::Ortho) {
			out << YAML::Key << "Rotation"
				<< YAML::Value << camera.Rotation;
		}
		out << YAML::Key << "Position" << YAML::Value << camera.Position;
		out << YAML::Key << "Direction" << YAML::Value << camera.Direction;
		out << YAML::Key << "ViewportWidth"
			<< YAML::Value << camera.ViewportWidth;
		out << YAML::Key << "ViewportHeight"
			<< YAML::Value << camera.ViewportHeight;
		out << YAML::Key << "Near"	   << YAML::Value << camera.Near;
		out << YAML::Key << "Far"	   << YAML::Value << camera.Far;
		out << YAML::EndMap; // Camera
	}

	if(entity.Has<MeshComponent>()) {
		auto mesh = entity.Get<MeshComponent>().Mesh;
		auto& mat = mesh->GetMaterial();

		out << YAML::Key << "MeshComponent" << YAML::BeginMap; // MeshComponent

		out << YAML::Key << "Mesh" << YAML::BeginMap; // Mesh
		if(mesh->Path != "") {
			out << YAML::Key << "Path" << YAML::Value << mesh->Path;
		}
		else {
			out << YAML::Key << "Vertices" << YAML::Flow << mesh->GetVertices();
			out << YAML::Key << "Indices" << YAML::Flow << mesh->GetIndices();

			out << YAML::Key << "Material" << YAML::BeginMap; // Material
			if(mat.Diffuse) {
				out << YAML::Key << "Diffuse" << YAML::BeginMap; // Diffuse
				out << YAML::Key << "Path" << YAML::Value
					<< mat.Diffuse->GetPath();
				out << YAML::EndMap; // Diffuse
			}
			if(mat.Specular) {
				out << YAML::Key << "Specular" << YAML::BeginMap; // Specular
				out << YAML::Key << "Path" << YAML::Value
					<< mat.Specular->GetPath();
				out << YAML::EndMap; // Specular
			}
			out << YAML::EndMap; // Material
		}
		out << YAML::EndMap; // Mesh

		out << YAML::EndMap; // MeshComponent
	}

	if(entity.Has<RigidBodyComponent>()) {
		auto body = entity.Get<RigidBodyComponent>().Body;

		out << YAML::Key << "RigidBodyComponent" << YAML::BeginMap; // RigidBodyComponent
		out << YAML::Key << "Body" << YAML::Value << YAML::BeginMap; // Body
		out << YAML::Key << "Type" << YAML::Value <<
			(body->GetType() == RigidBody::Type::Static ? "Static" : "Dynamic");

		out << YAML::Key << "Shape Type" << YAML::Value;
		switch(body->GetShape()->GetType()) {
			case Shape::Type::Box:
				out << "Box";
				break;
			case Shape::Type::Sphere:
				out << "Sphere";
				break;
			case Shape::Type::Plane:
				out << "Plane";
				break;
			case Shape::Type::Capsule:
				out << "Capsule";
				break;
			case Shape::Type::Mesh:
				out << "Mesh";
				break;
		}
		out << YAML::EndMap; // Body
		out << YAML::EndMap; // RigidBodyComponent
	}

	if(entity.Has<ScriptComponent>()) {
		// std::string& path = entity.Get<ScriptComponent>().Path;

		out << YAML::Key << "ScriptComponent" << YAML::BeginMap; // ScriptComponent
		// out << YAML::Key << "Path" << YAML::Value << path;
		out << YAML::EndMap; // ScriptComponent
	}

	if(entity.Has<TagComponent>()) {
		auto& tag = entity.Get<TagComponent>().Tag;

		out << YAML::Key << "TagComponent" << YAML::BeginMap; // TagComponent
		out << YAML::Key << "Tag" << YAML::Value << tag;
		out << YAML::EndMap; // TagComponent
	}

	if(entity.Has<TransformComponent>()) {
		auto& t = entity.Get<TransformComponent>().Translation;
		auto& r = entity.Get<TransformComponent>().Rotation;
		auto& s = entity.Get<TransformComponent>().Scale;

		out << YAML::Key << "TrasformComponent" << YAML::BeginMap; // TransformComponent

		out << YAML::Key << "Translation" << YAML::Value << t;
		out << YAML::Key << "Rotation"	  << YAML::Value << r;
		out << YAML::Key << "Scale"		  << YAML::Value << s;

		out << YAML::EndMap; // TransformComponent
	}
	out << YAML::EndMap; // Components
 
	out << YAML::EndMap; // Entity
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
		CameraComponent component;

		auto pos    = cameraComponentNode["Position"]	   .as<glm::vec3>();
		auto dir    = cameraComponentNode["Direction"]	   .as<glm::vec3>();
		auto width	= cameraComponentNode["ViewportWidth"] .as<uint32_t>();
		auto height = cameraComponentNode["ViewportHeight"].as<uint32_t>();
		auto near	= cameraComponentNode["Near"]		   .as<float>();
		auto far	= cameraComponentNode["Far"]		   .as<float>();
		float fovOrRotation;
		Camera::Type type;

		if(cameraComponentNode["Type"].as<std::string>() == "Stereographic") {
			fovOrRotation = cameraComponentNode["VerticalFOV"].as<float>();
			type = Camera::Type::Stereo;
		}
		else if(cameraComponentNode["Type"].as<std::string>() == "Orthographic")
		{
			fovOrRotation = cameraComponentNode["Rotation"].as<float>();
			type = Camera::Type::Ortho;
		}

		entity.Add<CameraComponent>(type, pos, dir, width, height, near, far);
	}

	auto meshComponentNode = components["MeshComponent"];
	if(meshComponentNode) {
		auto meshNode = meshComponentNode["Mesh"];

		if(meshNode["Path"]) {
			// TODO(Fix): MeshID/AssetID
			entity.Add<MeshComponent>(meshNode["Path"].as<std::string>());
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
		// auto scriptNode = scriptComponentNode["Path"];
		// std::string path = scriptNode.as<std::string>();

		// ScriptComponent& sc = entity.Add<ScriptComponent>(path);

		ScriptComponent& sc = entity.Add<ScriptComponent>();
	}

	auto transformComponentNode = components["TransformComponent"];
	if(transformComponentNode) {
		auto translationNode = transformComponentNode["Translation"];
		auto rotationNode	 = transformComponentNode["Rotation"];
		auto scaleNode		 = transformComponentNode["Scale"];

		TransformComponent& tc = entity.Add<TransformComponent>();
		tc.Translation = translationNode.as<glm::vec3>();
		tc.Rotation	   = rotationNode	.as<glm::vec3>();
		tc.Scale	   = scaleNode		.as<glm::vec3>();
	}

	auto rigidBodyComponentNode = components["RigidBodyComponent"];
	if(rigidBodyComponentNode) {
		auto rigidBodyNode = rigidBodyComponentNode["Body"];
		auto typeStr   = rigidBodyNode["Type"].as<std::string>();
		auto shapeTypeStr = rigidBodyNode["Shape Type"].as<std::string>();

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