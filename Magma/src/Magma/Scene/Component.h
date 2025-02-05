#pragma once

#include <glm/vec3.hpp>

#include <VolcaniCore/Graphics/Mesh.h>
#include <VolcaniCore/Graphics/Model.h>
#include <VolcaniCore/Graphics/Camera.h>

#include "Physics/RigidBody.h"

// #include "Script/ScriptObject.h"
// #include "Script/ScriptClass.h"

using namespace VolcaniCore;

namespace Magma::ECS {

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

struct MeshComponent : public Component {
	// TODO(Change): AssetID
	Ref<Model> Mesh;

	MeshComponent() {
		Mesh = Model::Create("");
	}
	MeshComponent(Ref<Model> mesh)
		: Mesh(mesh) { }
	MeshComponent(Ref<VolcaniCore::Mesh> mesh) {
		Mesh = Model::Create("");
		Mesh->AddMesh(mesh);
	}
	MeshComponent(const std::string& path) {
		Mesh = Model::Create(path);
	}
	MeshComponent(const MeshComponent& other) = default;
};

struct RigidBodyComponent : public Component {
	Ref<Physics::RigidBody> Body;

	RigidBodyComponent() = default;
	RigidBodyComponent(Ref<Physics::RigidBody> body)
		: Body(body) { }
	RigidBodyComponent(Physics::RigidBody::Type type) {
		Body = Physics::RigidBody::Create(type);
	}
	RigidBodyComponent(Physics::RigidBody::Type type, Ref<Physics::Shape> shape)
	{
		Body = Physics::RigidBody::Create(type, shape);
	}
	RigidBodyComponent(const RigidBodyComponent& other) = default;
};

struct TagComponent : public Component {
	std::string Tag;

	TagComponent()
		: Tag("Unnamed Entity") { }
	TagComponent(const std::string_view& tag)
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
};

struct ScriptComponent : public Component {
	// Ref<Script::ScriptObject> Instance;

	ScriptComponent() = default;
	ScriptComponent(const std::string& _class)
	{
		// ScriptEngine::LoadScript(path);
	}
	ScriptComponent(const ScriptComponent& other) = default;
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
	glm::vec3 Direction;
	float CutoffAngle;
	float OuterCutoffAngle;

	SpotlightComponent() = default;
	SpotlightComponent(const SpotlightComponent& other) = default;
};

struct SkyboxComponent : public Component {
	// Asset Cubemap;

	SkyboxComponent() = default;
	SkyboxComponent(const SkyboxComponent& other) = default;
};

}