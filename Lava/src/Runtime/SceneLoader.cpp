#include "SceneLoader.h"

#include <bitset>

#include <angelscript/add_on/scriptarray/scriptarray.h>

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

#include <Lava/Core/App.h>
#include <Lava/Types/GridSet.h>
#include <Lava/Types/GridSet3D.h>
#include <Lava/Types/Timer.h>

#undef near
#undef far

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
BinaryReader& BinaryReader::ReadObject(AudioComponent& comp) {
	uint64_t id;
	Read(id);
	comp.AudioAsset = { id, AssetType::Audio };

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

static Entity s_CurrentEntity;

template<>
BinaryReader& BinaryReader::ReadObject(ScriptComponent& comp) {
	uint64_t id;
	Read(id);
	Asset asset = { id, AssetType::Script };
	comp.ModuleAsset = asset;

	std::string className;
	Read(className);

	auto* assetManager = AssetManager::Get();
	assetManager->Load(asset);

	auto mod = assetManager->Get<ScriptModule>(asset);
	auto _class = mod->GetClass(className);
	comp.Instance = _class->Instantiate(s_CurrentEntity);

	auto obj = comp.Instance;
	for(uint32_t i = 0; i < obj->GetHandle()->GetPropertyCount(); i++) {
		int typeID;
		Read(typeID);
		if(typeID == -1)
			continue;

		ScriptField field = obj->GetProperty(i);
		std::string typeName;
		if(field.Type)
			typeName = field.Type->GetName();

		if(field.TypeID == asTYPEID_BOOL)
			Read(*field.As<bool>());
		else if(field.TypeID == asTYPEID_INT8)
			Read(*field.As<int8_t>());
		else if(field.TypeID == asTYPEID_INT16)
			Read(*field.As<int16_t>());
		else if(field.TypeID == asTYPEID_INT32)
			Read(*field.As<int32_t>());
		else if(field.TypeID == asTYPEID_INT64)
			Read(*field.As<int64_t>());
		else if(field.TypeID == asTYPEID_UINT8)
			Read(*field.As<uint8_t>());
		else if(field.TypeID == asTYPEID_UINT16)
			Read(*field.As<uint16_t>());
		else if(field.TypeID == asTYPEID_UINT32)
			Read(*field.As<uint32_t>());
		else if(field.TypeID == asTYPEID_UINT64)
			Read(*field.As<uint64_t>());
		else if(field.TypeID == asTYPEID_FLOAT)
			Read(*field.As<float>());
		else if(field.TypeID == asTYPEID_DOUBLE)
			Read(*field.As<double>());
		else if(typeName == "string")
			Read(*field.As<std::string>());
		else if(typeName == "array") {
			auto* array = field.As<CScriptArray>();
			auto subTypeID = array->GetArrayObjectType()->GetSubTypeId();
			auto* subType = ScriptEngine::Get()->GetTypeInfoById(subTypeID);
			uint64_t size = 0;
			if(subType)
				size = subType->GetSize();
			else
				size = ScriptEngine::Get()->GetSizeOfPrimitiveType(subTypeID);

			uint32_t count;
			Read(count);
			Buffer<void> data(size, count);
			ReadData(data.Get(), (uint64_t)size * count);

			array->Reserve(count);
			for(uint32_t i = 0; i < count; i++)
				array->InsertLast((char*)data.Get() + size * i);
		}
		else if(typeName == "Asset") {
			uint64_t id;
			Read(id);
			asset.ID = id;
			uint8_t type;
			Read(type);
			asset.Type = (AssetType)type;
			Read(asset.Primary);
		}
		else if(typeName == "Vec3")
			Read(*field.As<glm::vec3>());
		else if(typeName == "GridSet") {
			auto* grid = field.As<GridSet>();
			uint32_t width;
			uint32_t height;
			Read(width);
			Read(height);
			grid->Resize(width, height);
			if(width && height)
				ReadData(grid->Get(), grid->GetCount());
		}
	}

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
	Read(comp.Bloom);

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
BinaryReader& BinaryReader::ReadObject(ParticleEmitterComponent& comp) {
	Read(comp.Position);
	Read(comp.MaxParticleCount);
	Read(comp.ParticleLifetime);

	uint64_t id;
	Read(id);
	comp.MaterialAsset = { id, AssetType::Texture };

	return *this;
}

template<>
BinaryReader& BinaryReader::ReadObject(Entity& entity) {
	std::string name;
	Read(name);
	if(name != "")
		entity.SetName(name);

	s_CurrentEntity = entity;

	uint16_t bits;
	Read(bits);
	std::bitset<12> componentBits(bits);

	if(componentBits.test(0))
		Read(entity.Set<CameraComponent>());
	if(componentBits.test(1))
		Read(entity.Set<TagComponent>());
	if(componentBits.test(2))
		Read(entity.Set<TransformComponent>());
	if(componentBits.test(3))
		Read(entity.Set<AudioComponent>());
	if(componentBits.test(4))
		Read(entity.Set<MeshComponent>());
	if(componentBits.test(5))
		Read(entity.Set<SkyboxComponent>());
	if(componentBits.test(6))
		Read(entity.Set<ScriptComponent>());
	if(componentBits.test(7))
		Read(entity.Set<RigidBodyComponent>());
	if(componentBits.test(8))
		Read(entity.Set<DirectionalLightComponent>());
	if(componentBits.test(9))
		Read(entity.Set<PointLightComponent>());
	if(componentBits.test(10))
		Read(entity.Set<SpotlightComponent>());
	if(componentBits.test(11))
		Read(entity.Set<ParticleEmitterComponent>());

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