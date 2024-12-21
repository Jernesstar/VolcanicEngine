#include "ScriptSystem.h"

#include "World.h"

namespace Magma::ECS {

void ScriptSystem::Update(TimeStep ts) {

}

void ScriptSystem::Run(Phase phase) {
	m_EntityWorld
	->ForEach<ScriptComponent, TransformComponent>(
		[this](Entity& entity)
		{
			auto [sc] = GetRequired(entity);
			auto& tr = entity.Get<TransformComponent>();
			sc.OnInput(tr);
		});
}

}