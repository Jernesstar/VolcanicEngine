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

#include "Runtime.h"

using namespace Magma::ECS;
using namespace Magma::Physics;

namespace Magma {

template<>
BinaryReader& BinaryReader::ReadObject(glm::vec3& vec) {
	Read(vec.x);
	Read(vec.y);
	Read(vec.z);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(CameraComponent& comp) {
	uint32_t typeInt;
	Read(typeInt);
	Camera::Type type =
		typeInt == 0 ? Camera::Type::Ortho : Camera::Type::Stereo;

	float rotation_fov;
	Read(rotation_fov);
	glm::vec3 pos, dir;
	Read(pos); Read(dir);
	uint32_t w, h;
	Read(w); Read(h);
	float near, far;
	Read(near); Read(far);

	comp.Cam = Camera::Create(type, rotation_fov);
	comp.Cam->SetPositionDirection(pos, dir);
	comp.Cam->Resize(w, h);
	comp.Cam->SetProjection(near, far);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(TagComponent& comp) {
	Read(comp.Tag);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(TransformComponent& comp) {
	Read(comp.Translation);
	Read(comp.Rotation);
	Read(comp.Scale);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(MeshComponent& comp) {
	uint64_t id;
	Read(id);
	comp.MeshAsset = { id, AssetType::Mesh };

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(SkyboxComponent& comp) {
	uint64_t id;
	Read(id);
	comp.CubemapAsset = { id, AssetType::Cubemap };

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(RigidBodyComponent& comp) {
	uint32_t typeInt;
	Read(typeInt);
	RigidBody::Type type = (RigidBody::Type)typeInt;
	uint32_t shapeTypeInt;
	Read(shapeTypeInt);
	Shape::Type shapeType = (Shape::Type)shapeTypeInt;

	Ref<Shape> shape = Shape::Create(shapeType);
	comp.Body = RigidBody::Create(type, shape);

	return *this;
}

static Entity s_CurrentEntity;

template<>
BinaryReader& BinaryReader::ReadObject(ScriptComponent& comp) {
	std::string className;
	Read(className);
	auto _class =
		Application::As<Lava::Runtime>()->GetApp()->GetScriptClass(className);
	comp.Instance = _class->Instantiate(s_CurrentEntity);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(AudioComponent& comp) {
	uint64_t id;
	Read(id);
	comp.AudioAsset = { id, AssetType::Sound };

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(DirectionalLightComponent& comp) {
	Read(comp.Ambient);
	Read(comp.Diffuse);
	Read(comp.Specular);
	Read(comp.Position);
	Read(comp.Direction);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(PointLightComponent& comp) {
	Read(comp.Ambient);
	Read(comp.Diffuse);
	Read(comp.Specular);
	Read(comp.Position);
	Read(comp.Constant);
	Read(comp.Linear);
	Read(comp.Quadratic);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(SpotlightComponent& comp) {
	Read(comp.Ambient);
	Read(comp.Diffuse);
	Read(comp.Specular);
	Read(comp.Position);
	Read(comp.Direction);
	Read(comp.CutoffAngle);
	Read(comp.OuterCutoffAngle);

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(ParticleSystemComponent& comp) {
	uint64_t id;
	Read(id);
	comp.ImageAsset = { id, AssetType::Texture };

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(Entity& entity) {
	std::string name;
	Read(name);
	entity.SetName(name);

	uint32_t bits;
	Read(bits);
	std::bitset<12> componentBits(bits);

	s_CurrentEntity = entity;

	if(componentBits.test(0))
		Read(entity.Set<CameraComponent>());
	if(componentBits.test(1))
		Read(entity.Set<TagComponent>());
	if(componentBits.test(2))
		Read(entity.Set<TransformComponent>());
	if(componentBits.test(3))
		Read(entity.Set<MeshComponent>());
	if(componentBits.test(4))
		Read(entity.Set<SkyboxComponent>());
	if(componentBits.test(5))
		Read(entity.Set<RigidBodyComponent>());
	if(componentBits.test(6))
		Read(entity.Set<ScriptComponent>());
	if(componentBits.test(7))
		Read(entity.Set<AudioComponent>());
	if(componentBits.test(8))
		Read(entity.Set<DirectionalLightComponent>());
	if(componentBits.test(9))
		Read(entity.Set<PointLightComponent>());
	if(componentBits.test(10))
		Read(entity.Set<SpotlightComponent>());
	if(componentBits.test(11))
		Read(entity.Set<ParticleSystemComponent>());

	return *this;
}

}

namespace Lava {

void SceneLoader::Load(Scene& scene, const std::string& path) {
	namespace fs = std::filesystem;

	BinaryReader reader(path);
	reader.Read(scene.Name);

	uint64_t entityCount;
	reader.Read(entityCount);

	for(uint64_t i = 0; i < entityCount; i++) {
		uint64_t id;
		reader.Read(id);
		Entity entity = scene.EntityWorld.AddEntity(id);
		reader.Read(entity);
	}
}

void SceneLoader::Save(const Scene& scene, const std::string& path) {
	namespace fs = std::filesystem;

	BinaryWriter writer(path);

	writer.Write(scene.Name);

	scene.EntityWorld
	.ForEach(
		[&](const Entity& entity)
		{
			// writer.Write(entity);
		});
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