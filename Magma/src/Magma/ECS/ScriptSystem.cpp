#include "ScriptSystem.h"

#include "World.h"

namespace Magma::ECS {

void ScriptSystem::Update(TimeStep ts) {

}

void ScriptSystem::Run(Phase phase) {
	// m_EntityWorld->GetNative()
	// .query_builder(m_EntityWorld->GetNative(), m_Query)
	// .with<TransformComponent>()
	// .build()
	// .each(
	// 	[this](flecs::entity id)
	// 	{
	// 		Entity entity{ id };
	// 		auto [sc] = GetRequired(entity);
	// 		auto& tr = entity.Get<TransformComponent>();
	// 		sc.OnInput(tr);
	// 	});
}

}