#include "RenderSystem.h"

#include "VolcaniCore/Graphics/Renderer.h"
#include "VolcaniCore/Graphics/Renderer3D.h"

namespace Magma::ECS {

void RenderSystem::Update(TimeStep ts) {
	// m_Query
	// .each(
	// 	[this](flecs::entity id)
	// 	{
	// 		Entity entity{ id };
	// 		auto [mc, tc] = GetRequired(entity);

	// 		Renderer3D::DrawMesh(mc.Mesh,
	// 			{ tc.Translation, tc.Rotation, tc.Scale });
	// 	});
}

void RenderSystem::Run(Phase phase) {

}

}