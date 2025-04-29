#include "ScriptSystem.h"

#include <VolcaniCore/Event/Events.h>

#include "ECS/World.h"

namespace Magma {

struct GameEventListener { };

ScriptSystem::ScriptSystem(ECS::World* world)
	: System(world)
{
	m_KeyPressedCallbackID =
		Events::RegisterListener<KeyPressedEvent>(
			[this](KeyPressedEvent& event)
			{
				m_EntityWorld->ForEach<ScriptComponent>(
					[=](Entity& entity)
					{
						auto obj = entity.Get<ScriptComponent>().Instance;
						obj->Call("OnKeyEvent", event);
					});
			});
	m_KeyReleasedCallbackID =
		Events::RegisterListener<KeyReleasedEvent>(
			[this](KeyReleasedEvent& event)
			{
				m_EntityWorld->ForEach<ScriptComponent>(
					[=](Entity& entity)
					{
						auto obj = entity.Get<ScriptComponent>().Instance;
						obj->Call("OnKeyEvent", event);
					});
			});
	m_KeyCharCallbackID =
		Events::RegisterListener<KeyCharEvent>(
			[this](KeyCharEvent& event)
			{
				m_EntityWorld->ForEach<ScriptComponent>(
					[=](Entity& entity)
					{
						auto obj = entity.Get<ScriptComponent>().Instance;
						obj->Call("OnKeyEvent", event);
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

void ScriptSystem::Broadcast(Entity& entity, asIScriptObject* event) {
	event->AddRef();

	auto eventName = event->GetObjectType()->GetName();
	m_EntityWorld->
	ForEach<ScriptComponent>(
		[=, this](Entity& entity)
		{
			auto eventID = m_EntityWorld->GetNative().lookup(eventName);
			if(!eventID)
				return;
			if(!entity.GetHandle().has<GameEventListener>(eventID))
				return;

			auto sc = entity.Get<ScriptComponent>();
			sc.Instance->Call("OnGameEvent", event);
		});

	event->Release();
}

void ScriptSystem::OnComponentAdd(Entity& entity) {

}

void ScriptSystem::OnComponentSet(Entity& entity) {

}

void ScriptSystem::OnComponentRemove(Entity& entity) {

}

}