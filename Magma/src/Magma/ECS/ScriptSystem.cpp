#include "ScriptSystem.h"

#include "VolcaniCore/Renderer/Renderer.h"
#include "VolcaniCore/Renderer/Renderer3D.h"

namespace Magma::ECS {

void ScriptSystem::Update(TimeStep ts) {

	
	m_Query
	.each(
		[this](flecs::entity id)
		{
			Entity entity{ id };
			auto& [sc] = GetRequired(entity);

		});
}

void ScriptSystem::Run(Phase phase) {

}

}