#pragma once

#include <glm/vec3.hpp>

#include <VolcaniCore/Core/Math.h>
#include <VolcaniCore/Graphics/StereographicCamera.h>

#include "Core/AssetManager.h"

#include "Physics/RigidBody.h"

#include "Script/ScriptObject.h"

using namespace VolcaniCore;

namespace Magma {

struct Component {
	uint8_t _;

	Component() = default;
	Component(const Component& other) = default;
	virtual ~Component() = default;
};

struct CameraComponent : public Component {
	Ref<Camera> Cam;

	CameraComponent() = default;
	CameraComponent(Ref<Camera> camera)
		: Cam(camera) { }
	CameraComponent(const CameraComponent& other) = default;
};

struct TagComponent : public Component {
	std::string Tag;

	TagComponent()
		: Tag("Empty Tag") { }
	TagComponent(const std::string& tag)
		: Tag(tag) { }
	TagComponent(const TagComponent& other) = default;
};

struct TransformComponent : public Component {
	Vec3 Translation = { 0.0f, 0.0f, 0.0f };
	Vec3 Rotation	 = { 0.0f, 0.0f, 0.0f };
	Vec3 Scale		 = { 1.0f, 1.0f, 1.0f };

	TransformComponent() = default;
	TransformComponent(const Vec3& t, const Vec3& r, const Vec3& s)
		: Translation(t), Rotation(r), Scale(s) { }
	TransformComponent(const Transform& t)
		: Translation(t.Translation), Rotation(t.Rotation), Scale(t.Scale) { }
	TransformComponent(const TransformComponent& other) = default;

	operator Transform() const { return { Translation, Rotation, Scale }; }
};

struct AudioComponent : public Component {
	Asset AudioAsset;

	AudioComponent() = default;
	AudioComponent(const Asset& asset)
		: AudioAsset(asset) { }
	AudioComponent(const AudioComponent& other) = default;
};

struct MeshComponent : public Component {
	Asset MeshAsset;

	MeshComponent() = default;
	MeshComponent(const Asset& asset)
		: MeshAsset(asset) { }
	MeshComponent(const MeshComponent& other) = default;
};

struct SkyboxComponent : public Component {
	Asset CubemapAsset;

	SkyboxComponent() = default;
	SkyboxComponent(const Asset& asset)
		: CubemapAsset(asset) { }
	SkyboxComponent(const SkyboxComponent& other) = default;
};

struct ScriptComponent : public Component {
	Asset ModuleAsset;
	Ref<Script::ScriptObject> Instance;

	ScriptComponent() = default;
	ScriptComponent(const Asset& asset, Ref<Script::ScriptObject> obj)
		: ModuleAsset(asset), Instance(obj) { }
	ScriptComponent(const ScriptComponent& other) = default;
	ScriptComponent(ScriptComponent&& other) = default;
};

struct RigidBodyComponent : public Component {
	Ref<Physics::RigidBody> Body;

	RigidBodyComponent() = default;
	RigidBodyComponent(Ref<Physics::RigidBody> body)
		: Body(body) { }
	RigidBodyComponent(const RigidBodyComponent& other) = default;
};

struct DirectionalLightComponent : public Component {
	Vec3 Ambient;
	Vec3 Diffuse;
	Vec3 Specular;
	Vec3 Position;
	Vec3 Direction;

	DirectionalLightComponent() = default;
	DirectionalLightComponent(const Vec3& a, const Vec3& d, const Vec3& s,
							  const Vec3& pos, const Vec3& dir)
		: Ambient(a), Diffuse(d), Specular(s), Position(pos), Direction(dir) { }
	DirectionalLightComponent(const DirectionalLightComponent& other) = default;
};

struct PointLightComponent : public Component {
	Vec3 Ambient;
	Vec3 Diffuse;
	Vec3 Specular;
	Vec3 Position;
	float Constant;
	float Linear;
	float Quadratic;

	PointLightComponent() = default;
	PointLightComponent(const Vec3& a, const Vec3& d, const Vec3& s,
						const Vec3& pos, float c, float l, float q)
		: Ambient(a), Diffuse(d), Specular(s), Position(pos),
			Constant(c), Linear(l), Quadratic(q) { }
	PointLightComponent(const PointLightComponent& other) = default;
};

struct SpotlightComponent : public Component {
	Vec3 Ambient;
	Vec3 Diffuse;
	Vec3 Specular;
	Vec3 Position;
	Vec3 Direction;
	float CutoffAngle;
	float OuterCutoffAngle;

	SpotlightComponent() = default;
	SpotlightComponent(const Vec3& a, const Vec3& d, const Vec3& s,
						const Vec3& pos, const Vec3& dir,
						float inner, float outer)
		: Ambient(a), Diffuse(d), Specular(s), Position(pos), Direction(dir),
			CutoffAngle(inner), OuterCutoffAngle(outer) { }
	SpotlightComponent(const SpotlightComponent& other) = default;
};

struct ParticleEmitterComponent : public Component {
	Vec3 Position;
	uint64_t MaxParticleCount;
	float ParticleLifetime; // In milliseconds
	float SpawnInterval; // In milliseconds
	Asset MaterialAsset;

	ParticleEmitterComponent() = default;
	ParticleEmitterComponent(const Vec3& pos, uint64_t max, float lifetime,
		float spawnInterval, const Asset& asset)
		: Position(pos), MaxParticleCount(max),
		SpawnInterval(spawnInterval), ParticleLifetime(lifetime),
		MaterialAsset(asset) { }
	ParticleEmitterComponent(const ParticleEmitterComponent& other) = default;
};

}