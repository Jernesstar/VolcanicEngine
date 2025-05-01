#pragma once

#include "ECS/System.h"

#include "Component.h"

using namespace Magma::ECS;

namespace Magma {

class ScriptSystem : public System<ScriptComponent> {
public:
	ScriptSystem(ECS::World* world);
	~ScriptSystem();

	void Update(TimeStep ts) override;
	void Run(Entity& entity, TimeStep ts, Phase phase) override;

	void Listen(Entity& entity, const std::string& id);
	void Broadcast(Entity& entity, asIScriptObject* event);

	void OnComponentAdd(Entity& entity) override;
	void OnComponentSet(Entity& entity) override;
	void OnComponentRemove(Entity& entity) override;

public:
	flecs::entity m_KeyPressedCallbackEntity;
	flecs::entity m_KeyReleasedCallbackEntity;
	flecs::entity m_KeyCharCallbackEntity;

	UUID m_KeyPressedCallbackID;
	UUID m_KeyReleasedCallbackID;
	UUID m_KeyCharCallbackID;
};

}