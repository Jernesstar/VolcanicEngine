#pragma once

namespace Magma::ECS {

class World;

class EventHandler {
public:
	EventHandler(World* world);

	template<typename TEvent>
	void Register();

	

private:
	World* m_World;
};

}