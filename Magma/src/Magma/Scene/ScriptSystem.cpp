#include "ScriptSystem.h"

#include <VolcaniCore/Event/Events.h>

#include "ECS/World.h"

namespace Magma {

ScriptSystem::ScriptSystem(ECS::World* world)
	: System(world)
{
	m_KeyPressedCallbackEntity = m_EntityWorld->GetNative().entity("KeyPressed");
	m_KeyPressedCallbackEntity.observe<KeyPressedEvent>([](auto){});

	m_KeyPressedCallbackID =
		Events::RegisterListener<KeyPressedEvent>(
			[this](KeyPressedEvent& event)
			{
				m_KeyPressedCallbackEntity.enqueue<KeyPressedEvent>(event);
			});
	// m_KeyReleasedCallbackID =
	// 	Events::RegisterListener<KeyReleasedEvent>(
	// 		[this](KeyReleasedEvent& event)
	// 		{
	// 			m_KeyPressedCallbackEntity.enqueue<KeyReleasedEvent>(event);
	// 		});
}

ScriptSystem::~ScriptSystem() {
	Events::UnregisterListener<KeyPressedEvent>(m_KeyPressedCallbackID);
	// Events::UnregisterListener<KeyReleasedEvent>(m_KeyReleasedCallbackID);
	// Events::UnregisterListener<KeyCharEvent>(m_KeyCharCallbackID);
}

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
	VOLCANICORE_LOG_INFO("Entity '%s' registered for %s GameEvent",
		entity.GetName().c_str(), id.c_str());
	entity.GetHandle().observe<GameEvent>(
		[this, &handle=entity](GameEvent& event)
		{
			auto& component = handle.Get<ScriptComponent>();
			component.Instance->Call("OnGameEvent", &event);
		});
}

void ScriptSystem::Broadcast(Entity& entity, const std::string& id) {
	entity.GetHandle().enqueue<GameEvent>({ id });
}

void ScriptSystem::OnComponentAdd(Entity& entity) {
	VOLCANICORE_LOG_INFO("Registered '%s'", entity.GetName().c_str());
	entity.GetHandle().observe<KeyPressedEvent>(
		[this, &handle=entity](KeyPressedEvent& event)
		{
			auto sc = handle.Get<ScriptComponent>();
			if(!sc.Instance)
				return;

			sc.Instance->Call("OnKeyEvent", dynamic_cast<KeyEvent*>(&event));
		});
}

void ScriptSystem::OnComponentSet(Entity& entity) {

}

void ScriptSystem::OnComponentRemove(Entity& entity) {

}

}