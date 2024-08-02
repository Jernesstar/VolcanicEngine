#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <VolcaniCore/Core/Defines.h>
#include <VolcaniCore/Events/EventCallback.h>
#include <VolcaniCore/Events/EventSystem.h>

#include <VolcaniCore/Object/Mesh.h>
#include <VolcaniCore/Object/Texture.h>

#include <Physics/RigidBody.h>

using namespace VolcaniCore;
using namespace Magma::Physics;

namespace Magma {

struct Component {
	Component() = default;
	Component(const Component& other) = default;
	virtual ~Component() = default;
};

struct MeshComponent : public Component {
	Ref<VolcaniCore::Mesh> Mesh;

	MeshComponent(Ref<VolcaniCore::Mesh> mesh)
		: Mesh(mesh) { }
	MeshComponent(MeshPrimitive primitive, Material material) {
		Mesh = Mesh::Create(primitive, material);
	}
};

struct RigidBodyComponent : public Component {
	RigidBody Body;

	RigidBodyComponent()
		: Body(RigidBodyType::Static, Shape(ShapeType::Box)) { }
	RigidBodyComponent(const RigidBody& body)
		: Body(body) { }
	RigidBodyComponent(const RigidBodyComponent& other) = default;
};

struct ScriptComponent : public Component {
	std::string Path;

	ScriptComponent()
		: Path("") { }
	ScriptComponent(const std::string_view& path)
		: Path(path)
	{
		// ScriptLibrary::LoadScript(path);
	}
	ScriptComponent(const ScriptComponent& other) = default;
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
	TransformComponent(const TransformComponent& other) = default;
};

}