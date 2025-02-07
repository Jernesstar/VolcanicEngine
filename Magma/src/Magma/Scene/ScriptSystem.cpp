#include "ScriptSystem.h"

#include "ECS/World.h"

namespace Magma {

void ScriptSystem::Update(TimeStep ts) {
	m_TimeStep = ts;
}

void ScriptSystem::Run(Phase phase) {
	m_EntityWorld->ForEach<ScriptComponent, TransformComponent>(
		[this](Entity& entity)
		{
			auto [sc] = GetRequired(entity);
			auto& tr = entity.Get<TransformComponent>();
			// sc.Instance->Call("OnUpdate", (float)m_TimeStep);
		});
}

void ScriptSystem::OnComponentAdd(Entity& entity) {
	auto& sc = entity.Get<ScriptComponent>();
	// sc.Instance = scriptClass->Instantiate();
}

void ScriptSystem::OnComponentSet(Entity& entity) {

}

void ScriptSystem::OnComponentRemove(Entity& entity) {

}

}