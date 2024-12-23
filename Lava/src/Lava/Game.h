#pragma once

#include <VolcaniCore/Core/Time.h>

using namespace VolcaniCore;

namespace Lava {

class Game {
public:
	Game();
	virtual ~Game() = default;

	virtual void OnLoad();
	virtual void OnClose();
	virtual void OnUpdate(TimeStep ts);

	// template<typename TComponent>
	// void OnSerialize(const TComponent& component, YAMLSerializer& serializer);

	// template<typename TComponent>
	// TComponent& OnDeserialize();

private:
	
};

}