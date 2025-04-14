#include "ScriptSystem.h"

#include <VolcaniCore/Event/Events.h>

#include "ECS/World.h"

namespace Magma {

struct GameEventListener { };

ScriptSystem::ScriptSystem(ECS::World* world)
	: System(world)
{
	// m_EntityWorld->GetNative()
	// .observer()
	// .event<GameEvent>()
	// .run(
	// 	[this](flecs::iter& it)
	// 	{
	// 		m_EntityWorld->GetNative()
	// 		.query<ScriptComponent>()
	// 		.each(
	// 			[this, event=m_Event](flecs::entity e, ScriptComponent& sc)
	// 			{
	// 				auto eventID =
	// 					m_EntityWorld->GetNative().lookup(event.ID.c_str());
	// 				if(!eventID)
	// 					return;
	// 				if(!e.has<GameEventListener>(eventID))
	// 					return;

	// 				sc.Instance->Call("OnGameEvent", &event);
	// 			});
	// 	});

	m_KeyPressedCallbackID =
	Events::RegisterListener<KeyPressedEvent>(
		[this](KeyPressedEvent& event)
		{
			m_EntityWorld->GetNative()
			.query<ScriptComponent>()
			.each(
				[=](ScriptComponent& sc)
				{
					sc.Instance->Call("OnKeyEvent", event);
				});
		});
	m_KeyReleasedCallbackID =
		Events::RegisterListener<KeyReleasedEvent>(
			[this](KeyReleasedEvent& event)
			{
				m_EntityWorld->GetNative()
				.query<ScriptComponent>()
				.each(
					[=](ScriptComponent& sc)
					{
						sc.Instance->Call("OnKeyEvent", event);
					});
			});
	m_KeyCharCallbackID =
		Events::RegisterListener<KeyCharEvent>(
			[this](KeyCharEvent& event)
			{
				m_EntityWorld->GetNative()
				.query<ScriptComponent>()
				.each(
					[=](ScriptComponent& sc)
					{
						sc.Instance->Call("OnKeyEvent", event);
					});
			});
}

ScriptSystem::~ScriptSystem() {
	Events::UnregisterListener<KeyPressedEvent>(m_KeyPressedCallbackID);
	Events::UnregisterListener<KeyReleasedEvent>(m_KeyReleasedCallbackID);
	Events::UnregisterListener<KeyCharEvent>(m_KeyCharCallbackID);
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
	// If already exists, returns existing
	flecs::entity eventID = m_EntityWorld->GetNative().entity(id.c_str());
	entity.GetHandle().add<GameEventListener>(eventID);
	VOLCANICORE_LOG_INFO("Registered entity '%s' for '%s' GameEvent",
		entity.GetName().c_str(), id.c_str());
}

void ScriptSystem::Broadcast(Entity& entity, const std::string& id) {
	// entity.GetHandle().enqueue<GameEvent>({ id });
	// m_Event = GameEvent{ id };
}

void ScriptSystem::OnComponentAdd(Entity& entity) {

}

void ScriptSystem::OnComponentSet(Entity& entity) {

}

void ScriptSystem::OnComponentRemove(Entity& entity) {

}

}