#include "Scene.h"

namespace Magma {

Scene::Scene() {
	auto world = m_PhysicsSystem.GetWorld();

	flecs::system physicsUpdateSys =
		world
		.kind(flecs::OnUpdate)
		.system<const RigidBodyComponent, TransformComponent>("Update")
		.each(
		[](const RigidBodyComponent& r, TransformComponent& t) {
			t = r.RigidBody.GetTransform();
		});
	flecs::system renderSys =
		world
		.kind(flecs::OnUpdate)
		.system<const TransformComponent, const MeshComponent>("Render")
		.each(
		[](const TransformComponent& t, const MeshComponent& m) {
			Renderer3D::DrawMesh(m.GetMesh(), t.GetTransform())
		});
}

void Scene::OnUpdate(TimeStep ts) {
	// m_PhysicsSystem.OnUpdate(ts);

	auto world = m_PhysicsSystem.GetWorld();
	world.progress((float)ts);
}

void Scene::OnRender() {

}

}