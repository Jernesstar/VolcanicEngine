#pragma once

#include <glm/vec3.hpp>

#include <VolcaniCore/Graphics/Mesh.h>
#include <VolcaniCore/Graphics/Model.h>
#include <VolcaniCore/Graphics/Camera.h>

#include <Physics/RigidBody.h>

using namespace VolcaniCore;
using namespace Magma::Physics;

namespace Magma::ECS {

struct Component {
	Component() = default;
	Component(const Component& other) = default;
	virtual ~Component() = default;
};

struct CameraComponent : public Component {
	// TODO(Change): AssetID
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
	// TODO(Change): AssetID
	Ref<RigidBody> Body;

	RigidBodyComponent() = default;
	RigidBodyComponent(Ref<RigidBody> body)
		: Body(body) { }
	RigidBodyComponent(RigidBody::Type type) {
		Body = RigidBody::Create(type);
	}
	RigidBodyComponent(RigidBody::Type type, Ref<Shape> shape) {
		Body = RigidBody::Create(type, shape);
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
	TransformComponent(const glm::vec3& t, const glm::vec3& r, const glm::vec3& s)
		: Translation(t), Rotation(r), Scale(s) { }
	TransformComponent(const Transform& t)
		: Translation(t.Translation), Rotation(t.Rotation), Scale(t.Scale) { }
	TransformComponent(const TransformComponent& other) = default;
};

struct ScriptComponent : public Component {
	// std::string Path;
	
	Func<TransformComponent&, void> OnInput;

	ScriptComponent() = default;
	ScriptComponent(const std::string& path)
		// : Path(path)
	{
		// ScriptEngine::LoadScript(path);
	}
	ScriptComponent(const ScriptComponent& other) = default;
};

}