#include "ScriptSystem.h"

#include "ECS/World.h"

namespace Magma {

void ScriptSystem::Update(TimeStep ts) {
	m_TimeStep = ts;
}

void ScriptSystem::Run(Phase phase) {
	m_EntityWorld->ForEach<ScriptComponent>(
		[this](Entity& entity)
		{
			auto [sc] = GetRequired(entity);
			sc.Instance->Call("OnUpdate", (float)m_TimeStep);
		});
}

void ScriptSystem::Listen(Entity& entity, const std::string& id) {

}

void ScriptSystem::Broadcast(const std::string& id) {

}

void ScriptSystem::OnComponentAdd(Entity& entity) {

}

void ScriptSystem::OnComponentSet(Entity& entity) {

}

void ScriptSystem::OnComponentRemove(Entity& entity) {

}

}