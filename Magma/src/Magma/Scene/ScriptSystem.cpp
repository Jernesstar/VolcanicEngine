#include "ScriptSystem.h"

#include "ECS/World.h"

namespace Magma {

void ScriptSystem::Update(TimeStep ts) {

}

void ScriptSystem::Run(Phase phase) {
	m_EntityWorld
	->ForEach<ScriptComponent, TransformComponent>(
		[this](Entity& entity)
		{
			auto [sc] = GetRequired(entity);
			auto& tr = entity.Get<TransformComponent>();
			// sc.Instance->Call("Update");
		});
}

}