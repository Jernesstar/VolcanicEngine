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
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	glm::vec3 Direction = { 0.0f, 0.0f, -1.0f };
	uint32_t ViewportWidth = 0;
	uint32_t ViewportHeight = 0;
	float Near = 0.01f;
	float Far  = 1000.0f;
	union
	{
		float VerticalFOV, Rotation;
	};

	CameraComponent() = default;
	CameraComponent(Camera::Type type, const glm::vec3& pos,
					const glm::vec3& dir, uint32_t width, uint32_t height,
					float near, float far)
		: CameraType(type), Position(pos), Direction(dir),
		  ViewportWidth(width), ViewportHeight(height), Near(near), Far(far) { }

	CameraComponent(const CameraComponent& other) = default;
};

struct MeshComponent : public Component {
	// TODO(Fix): MeshID
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