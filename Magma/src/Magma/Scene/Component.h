#pragma once

#include <glm/vec3.hpp>

#include <VolcaniCore/Graphics/Mesh.h>
#include <VolcaniCore/Graphics/Model.h>
#include <VolcaniCore/Graphics/Camera.h>

#include "Core/AssetManager.h"

#include "Physics/RigidBody.h"

#include "Script/ScriptObject.h"

using namespace VolcaniCore;

namespace Magma {

struct Component {
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
	glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Rotation	  = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Scale		  = { 1.0f, 1.0f, 1.0f };

	TransformComponent() = default;
	TransformComponent(const glm::vec3& t,
					   const glm::vec3& r,
					   const glm::vec3& s)
		: Translation(t), Rotation(r), Scale(s) { }
	TransformComponent(const Transform& t)
		: Translation(t.Translation), Rotation(t.Rotation), Scale(t.Scale) { }
	TransformComponent(const TransformComponent& other) = default;

	operator Transform() const { return { Translation, Rotation, Scale }; }
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

struct RigidBodyComponent : public Component {
	Ref<Physics::RigidBody> Body;

	RigidBodyComponent() = default;
	RigidBodyComponent(Ref<Physics::RigidBody> body)
		: Body(body) { }
	RigidBodyComponent(const RigidBodyComponent& other) = default;
};

struct ScriptComponent : public Component {
	std::string Class;
	Ref<Script::ScriptObject> Instance;

	ScriptComponent() = default;
	ScriptComponent(const std::string& _class)
		: Class(_class) { }
	ScriptComponent(const ScriptComponent& other) = default;
};

struct SoundComponent : public Component {
	Asset SoundAsset;

	SoundComponent() = default;
	SoundComponent(const Asset& asset)
		: SoundAsset(asset) { }
	SoundComponent(const SoundComponent& other) = default;
};

struct DirectionalLightComponent : public Component {
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	glm::vec3 Direction;

	DirectionalLightComponent() = default;
	DirectionalLightComponent(const DirectionalLightComponent& other) = default;
};

struct PointLightComponent : public Component {
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	glm::vec3 Position;
	float Constant;
	float Linear;
	float Quadratic;

	PointLightComponent() = default;
	PointLightComponent(const PointLightComponent& other) = default;
};

struct SpotlightComponent : public Component {
	glm::vec3 Ambient;
	glm::vec3 Diffuse;
	glm::vec3 Specular;
	glm::vec3 Position;
	glm::vec3 Direction;
	float CutoffAngle;
	float OuterCutoffAngle;

	SpotlightComponent() = default;
	SpotlightComponent(const SpotlightComponent& other) = default;
};

struct ParticleSystemComponent : public Component {
	float Lifetime;

	ParticleSystemComponent() = default;
	ParticleSystemComponent(const ParticleSystemComponent& other) = default;
};

}