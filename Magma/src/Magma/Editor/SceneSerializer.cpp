#include "SceneSerializer.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/UUID.h>
#include <VolcaniCore/Renderer/StereographicCamera.h>
#include <VolcaniCore/Renderer/OrthographicCamera.h>

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace YAML {

YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec3& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec4& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

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
static void DeserializeEntity(YAML::Node entityNode, Ref<Scene> scene);

void SceneSerializer::Serialize(Ref<Scene> scene, const std::string& filePath) {
	YAML::Emitter out;
	out << YAML::BeginMap; // File

	out << YAML::Key << "Scene" << YAML::BeginMap; // Scene
	out << YAML::Key << "Name" << YAML::Value <<  scene->Name;

	auto cam = scene->GetCamera();
	out << YAML::Key << "Camera" << YAML::BeginMap; // Camera
	out << YAML::Key << "Type" << YAML::Value
		<< (cam->GetType() == Camera::Type::Stereo ? "Stereographic"
												   : "Orthographic");
	if(cam->GetType() == Camera::Type::Stereo) {
		auto c = cam->As<StereographicCamera>();
		out << YAML::Key << "VerticalFOV" << YAML::Value << c->GetVerticalFOV();
	}
	if(cam->GetType() == Camera::Type::Ortho) {
		auto c = cam->As<OrthographicCamera>();
		out << YAML::Key << "Rotation" << YAML::Value << c->GetRotation();
	}
	out << YAML::Key << "Position" << YAML::Value << cam->GetPosition();
	out << YAML::Key << "Direction" << YAML::Value << cam->GetDirection();
	out << YAML::Key << "ViewportWidth"
		<< YAML::Value << cam->GetViewportWidth();
	out << YAML::Key << "ViewportHeight"
		<< YAML::Value << cam->GetViewportHeight();
	out << YAML::Key << "Near"	   << YAML::Value << cam->GetNear();
	out << YAML::Key << "Far"	   << YAML::Value << cam->GetFar();
	out << YAML::EndMap; // Camera

	out << YAML::Key << "Entities" << YAML::BeginSeq; // Entities
	scene->GetEntityWorld()
	.ForEach(
	[&](Entity& entity) {
		out << YAML::BeginMap; // Entity
		SerializeEntity(out, entity);
		out << YAML::EndMap; // Entity
	});
	out << YAML::EndSeq; // Entities

	out << YAML::EndMap; // Scene

	out << YAML::EndMap; // File

	std::ofstream fout(filePath);
	fout << out.c_str();
}

Ref<Scene> SceneSerializer::Deserialize(const std::string& filePath) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(filePath);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file {0}: {1}",
										filePath, e.what());
	}
	auto scene = file["Scene"];
	auto camera = scene["Camera"];

	// TODO(Implement): MAGMA_ASSERT

	VOLCANICORE_ASSERT(scene);
	VOLCANICORE_ASSERT(camera);

	Ref<Scene> newScene = CreateRef<Scene>(scene["Name"].as<std::string>());
	auto width	= camera["ViewportWidth"] .as<uint32_t>();
	auto height = camera["ViewportHeight"].as<uint32_t>();
	auto near	= camera["Near"]		  .as<float>();
	auto far	= camera["Far"]			  .as<float>();

	if(camera["Type"].as<std::string>() == "Stereographic") {
		auto fov = camera["VerticalFOV"].as<float>();
		newScene->SetCamera(
			CreateRef<StereographicCamera>(fov, width, height, near, far));
	}
	else if(camera["Type"].as<std::string>() == "Orthographic") {
		auto ro = camera["Rotation"].as<float>();
		newScene->SetCamera(
			CreateRef<OrthographicCamera>(width, height, near, far, ro));
	}

	newScene->GetCamera()
		->SetPositionDirection(camera["Position"].as<glm::vec3>(),
							   camera["Direction"].as<glm::vec3>());

	for(auto node : scene["Entities"])
		DeserializeEntity(node["Entity"], newScene);

	return newScene;
}

void SerializeEntity(YAML::Emitter& out, Entity& entity) {
	out << YAML::Key << "Entity" << YAML::BeginMap; // Entity
	out << YAML::Key << "ID" << YAML::Value << (uint64_t)entity.GetHandle();

	out << YAML::Key << "Components" << YAML::BeginMap; // Components

	if(entity.Has<MeshComponent>()) {
		auto mesh = entity.Get<MeshComponent>().Mesh;
		auto& mat = mesh->GetMaterial();

		out << YAML::Key << "MeshComponent" << YAML::BeginMap; // MeshComponent

		out << YAML::Key << "Mesh" << YAML::BeginMap; // Mesh
		out << YAML::Key << "Vertices" << YAML::Flow << mesh->GetVertices();
		out << YAML::Key << "Indices" << YAML::Flow << mesh->GetIndices();

		out << YAML::Key << "Material" << YAML::BeginMap; // Material
		if(mat.Diffuse) {
			out << YAML::Key << "Diffuse" << YAML::BeginMap; // Diffuse
			out << YAML::Key << "Path" << YAML::Value << mat.Diffuse->GetPath();
			out << YAML::EndMap; // Diffuse
		}
		if(mat.Specular) {
			out << YAML::Key << "Specular" << YAML::BeginMap; // Specular
			out << YAML::Key << "Path" << YAML::Value << mat.Specular->GetPath();
			out << YAML::EndMap; // Specular
		}
		out << YAML::EndMap; // Material
		out << YAML::EndMap; // Mesh

		out << YAML::EndMap; // MeshComponent
	}

	if(entity.Has<RigidBodyComponent>()) {
		auto& body = entity.Get<RigidBodyComponent>().Body;

		out << YAML::Key << "RigidBodyComponent" << YAML::BeginMap; // RigidBodyComponent
		out << YAML::Key << "Body" << YAML::Value << YAML::BeginMap; // Body
		out << YAML::Key << "Type" << YAML::Value <<
			(body->GetType() == RigidBody::Type::Static ? "Static" : "Dynamic");

		out << YAML::Key << "Shape Type" << YAML::Value;
		switch(body->GetShapeType()) {
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
			case Shape::Type::ConvexMesh:
				out << "ConvexMesh";
				break;
			case Shape::Type::TriangleMesh:
				out << "TriangleMesh";
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

void DeserializeEntity(YAML::Node entityNode, Ref<Scene> scene) {
	VolcaniCore::UUID id(entityNode["ID"].as<uint64_t>());
	Entity entity = scene->GetEntityWorld().AddEntity(id);

	auto components = entityNode["Components"];
	if(!components) return;

	auto meshComponentNode = components["MeshComponent"];
	if(meshComponentNode) {
		auto meshNode = meshComponentNode["Mesh"];
		auto verts = meshNode["Vertices"].as<std::vector<VolcaniCore::Vertex>>();
		auto indices = meshNode["Indices"].as<std::vector<uint32_t>>();
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

		entity.Add<MeshComponent>(verts, indices, mat);
	}

	auto scriptComponentNode = components["ScriptComponent"];
	if(scriptComponentNode) {
		// auto scriptNode = scriptComponentNode["Path"];
		// std::string path = scriptNode.as<std::string>();

		// ScriptComponent& sc = entity.Add<ScriptComponent>(path);

		ScriptComponent& sc = entity.Add<ScriptComponent>();
	}

	auto tagComponentNode = components["TagComponent"];
	if(tagComponentNode) {
		auto tag = tagComponentNode["Tag"].as<std::string>();

		entity.Add<TagComponent>(tag);
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
		if(shapeTypeStr == "Box")		   shapeType = Shape::Type::Box;
		if(shapeTypeStr == "Sphere")	   shapeType = Shape::Type::Sphere;
		if(shapeTypeStr == "Plane")		   shapeType = Shape::Type::Plane;
		if(shapeTypeStr == "Capsule")	   shapeType = Shape::Type::Capsule;
		if(shapeTypeStr == "ConvexMesh")   shapeType = Shape::Type::ConvexMesh;
		if(shapeTypeStr == "TriangleMesh") shapeType = Shape::Type::TriangleMesh;

		if(shapeType != Shape::Type::ConvexMesh
		&& shapeType != Shape::Type::TriangleMesh)
		{
			Shape shape(shapeType);
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