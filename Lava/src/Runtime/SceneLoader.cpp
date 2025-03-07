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

#include <Lava/App.h>

using namespace Magma::ECS;
using namespace Magma::Physics;
using namespace Lava;

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
	auto type = (Camera::Type)typeInt;

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
	comp.Cam->SetProjection(near, far);
	comp.Cam->Resize(w, h);

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
	uint64_t id;
	Read(id);
	Asset asset{ id, AssetType::Script };
	Read(className);
	comp.ModuleAsset = asset;
	if(className != "") {
		auto& assetManager = App::Get()->GetAssetManager();
		auto mod = assetManager.Get<ScriptModule>(asset);
		auto _class = mod->GetClass(className);
		comp.Instance = _class->Instantiate(s_CurrentEntity);
	}

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(AudioComponent& comp) {
	uint64_t id;
	Read(id);
	comp.AudioAsset = { id, AssetType::Audio };

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

	s_CurrentEntity = entity;

	uint32_t bits;
	Read(bits);
	std::bitset<12> componentBits(bits);

	VOLCANICORE_LOG_INFO(name.c_str());
	VOLCANICORE_LOG_INFO(componentBits.to_string().c_str());

	// if(componentBits[0])
	// 	Read(entity.Set<CameraComponent>());
	// if(componentBits[1])
	// 	Read(entity.Set<TagComponent>());
	// if(componentBits[2])
	// 	Read(entity.Set<TransformComponent>());
	// if(componentBits[3])
	// 	Read(entity.Set<MeshComponent>());
	// if(componentBits[4])
	// 	Read(entity.Set<SkyboxComponent>());
	// if(componentBits[5])
	// 	Read(entity.Set<RigidBodyComponent>());
	// if(componentBits[6])
	// 	Read(entity.Set<ScriptComponent>());
	// if(componentBits[7])
	// 	Read(entity.Set<AudioComponent>());
	// if(componentBits[8])
	// 	Read(entity.Set<DirectionalLightComponent>());
	// if(componentBits[9])
	// 	Read(entity.Set<PointLightComponent>());
	// if(componentBits[10])
	// 	Read(entity.Set<SpotlightComponent>());
	// if(componentBits[11])
	// 	Read(entity.Set<ParticleSystemComponent>());

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