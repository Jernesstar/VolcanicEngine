#pragma once

#include <glm/vec3.hpp>

#include <VolcaniCore/Object/Mesh.h>
#include <VolcaniCore/Object/Model.h>
#include <VolcaniCore/Renderer/Camera.h>

#include <Physics/RigidBody.h>

namespace Magma::ECS {

struct Component {
	Component() = default;
	Component(const Component& other) = default;
	virtual ~Component() = default;
};

struct CameraComponent : public Component {
	Camera::Type CameraType = Camera::Type::Stereo;
	uint32_t ViewportWidth = 100;
	uint32_t ViewportHeight = 100;
	float Near = 0.01f;
	float Far  = 1000.0f;
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Direction = { 0.0f, 0.0f, -1.0f };

	CameraComponent() = default;
	CameraComponent(const CameraComponent& other) = default;
};

struct MeshComponent : public Component {
	Ref<VolcaniCore::Mesh> Mesh;

	MeshComponent() = default;
	MeshComponent(Ref<VolcaniCore::Mesh> mesh)
		: Mesh(mesh) { }
	MeshComponent(const std::string& path) {
		Mesh = Mesh::Create(path);
	}
	MeshComponent(MeshPrimitive primitive, const Material& material) {
		Mesh = Mesh::Create(primitive, material);
	}
	MeshComponent(const std::vector<VolcaniCore::Vertex>& vertices,
				  const std::vector<uint32_t> indices, const Material& material)
	{
		Mesh = Mesh::Create(vertices, indices, material);
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
	TransformComponent(const glm::vec3& t, const glm::vec3& r,
					   const glm::vec3& s)
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