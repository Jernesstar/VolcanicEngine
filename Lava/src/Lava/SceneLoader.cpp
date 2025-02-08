#include "SceneLoader.h"

#include <VolcaniCore/Core/Assert.h>
#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Core/UUID.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>
#include <VolcaniCore/Graphics/OrthographicCamera.h>

#include <Magma/Core/AssetManager.h>
#include <Magma/Core/YAMLSerializer.h>

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

}

namespace Lava {

static void DeserializeEntity(YAML::Node entityNode, Scene& scene);
static void SerializeEntity(YAMLSerializer& out, const Entity& entity);

void SceneLoader::Load(Scene& scene, const std::string& path) {
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

void SceneLoader::Save(const Scene& scene, const std::string& path) {
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
		auto asset = entity.Get<MeshComponent>().MeshAsset;
		serializer.WriteKey("MeshComponent")
		.BeginMapping();

		serializer.WriteKey("")

		if(model->Path != "") {
			serializer.WriteKey("Model")
			.BeginMapping()
				.WriteKey("Path").Write(model->Path)
			.EndMapping();
		}
		else {
			serializer.WriteKey("Meshes").BeginSequence();

			for(auto& mesh : *model) {
				serializer.BeginMapping();
				serializer.WriteKey("Mesh").BeginMapping();

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

					auto& mat = mesh->GetMaterial();
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

					if(mat.Emissive)
						serializer.WriteKey("Emissive")
						.BeginMapping()
							.WriteKey("Path").Write(mat.Emissive->GetPath())
						.EndMapping();

					serializer
						.WriteKey("DiffuseColor").Write(mat.DiffuseColor);
					serializer
						.WriteKey("SpecularColor").Write(mat.SpecularColor);
					serializer
						.WriteKey("EmissiveColor").Write(mat.EmissiveColor);

					serializer
					.EndMapping(); // Material
				}
				serializer.EndMapping(); // Mesh
				serializer.EndMapping();
			}
			serializer.EndSequence();
		}

		serializer.EndMapping(); // MeshComponent
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
	serializer.EndMapping(); // Components

	serializer.EndMapping(); // Entity
}

void DeserializeEntity(YAML::Node entityNode, Scene& scene) {
	auto components = entityNode["Components"];

	if(!components) {
		VolcaniCore::UUID id(entityNode["ID"].as<uint64_t>());
		scene.EntityWorld.AddEntity(id);
		return;
	}

	Entity entity;

	// TODO(Fix): TagComponent -> NameComponent
	auto tagComponentNode = components["TagComponent"];
	if(tagComponentNode) {
		auto tag = tagComponentNode["Tag"].as<std::string>();
		entity = scene.EntityWorld.AddEntity(tag);
	}
	else {
		VolcaniCore::UUID id(entityNode["ID"].as<uint64_t>());
		entity = scene.EntityWorld.AddEntity(id);
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

		auto camera = Camera::Create(type, fr);
		camera->SetPositionDirection(pos, dir);
		camera->Resize(w, h);
		camera->SetProjection(near, far);

		entity.Add<CameraComponent>(camera);
	}

	auto meshComponentNode = components["MeshComponent"];
	if(meshComponentNode) {
		auto modelNode = meshComponentNode["Model"];
		if(modelNode) {
			auto path = modelNode["Path"].as<std::string>();
			// auto model = AssetManager::GetOrCreate<Model>(path);
			auto model = Model::Create(path);
			entity.Add<MeshComponent>(model);
		}
		else {
			auto model = entity.Add<MeshComponent>().Mesh;
			for(auto meshNode : meshComponentNode["Meshes"]) {
				auto node = meshNode["Mesh"];
				if(node["Path"]) {
					auto path = node["Path"].as<std::string>();
					// auto mesh = AssetManager::GetOrCreate<Mesh>(path);
					// model->AddMesh(mesh);
					continue;
				}

				auto v = node["Vertices"].as<List<VolcaniCore::Vertex>>();
				auto i = node["Indices"].as<List<uint32_t>>();
				auto diffuseNode = node["Material"]["Diffuse"];
				auto specularNode = node["Material"]["Specular"];

				Material mat;
				if(diffuseNode) {
					auto path = diffuseNode["Path"].as<std::string>();
					mat.Diffuse = Texture::Create(path);
				}
				if(specularNode) {
					auto path = specularNode["Path"].as<std::string>();
					mat.Diffuse = Texture::Create(path);
				}

				model->AddMesh(Mesh::Create(v, i, mat));
			}
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

		entity.Add<TransformComponent>(
			Transform
			{
				.Translation = translationNode.as<glm::vec3>(),
				.Rotation	 = rotationNode.as<glm::vec3>(),
				.Scale		 = scaleNode.as<glm::vec3>()
			});
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