#include "RenderSystem.h"

#include "VolcaniCore/Renderer/Renderer.h"
#include "VolcaniCore/Renderer/Renderer3D.h"

namespace Magma::ECS {

void RenderSystem::Submit(Entity& entity) {
	auto& [mc, tc] = GetRequired(entity);

	auto mesh = mc.Mesh;
	Transform t{ tc.Translation, tc.Rotation, tc.Scale };

	Renderer3D::DrawMesh(mesh, t);
}

void RenderSystem::Update(TimeStep ts) {

}

void RenderSystem::Run(Phase phase) {

}

}