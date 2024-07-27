#include "Scene.h"

#include <VolcaniCore/Renderer/Renderer3D.h>

namespace Magma {

Scene::Scene(const std::string& name)
	: Name(name)
{
	auto world = m_EntitySystem.GetWorld();

	flecs::system physicsUpdateSys =
		world
		.system<RigidBodyComponent, TransformComponent>("Update")
		.kind(flecs::OnUpdate)
		.each(
		[](const RigidBodyComponent& r, TransformComponent& t) {
			// t = r.Body.GetTransform();
		});
	flecs::system renderSys =
		world
		.system<TransformComponent, MeshComponent>("Render")
		.kind(flecs::OnUpdate)
		.each(
		[](TransformComponent& t, MeshComponent& m) {
			Renderer3D::DrawMesh(m.Mesh, t);
		});
}

void Scene::OnUpdate(TimeStep ts) {
	// m_PhysicsSystem.OnUpdate(ts);

	auto world = m_EntitySystem.GetWorld();
	world.progress((float)ts);
}

void Scene::OnRender() {

}

}