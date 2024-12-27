#pragma once

#include <VolcaniCore/Core/Time.h>

using namespace VolcaniCore;

namespace Lava {

class App {
public:
	template<typename TApp>
	static TApp* As() { return (TApp*)s_Instance; }

public:
	App() {
		s_Instance = this;
	}
	virtual ~App() = default;

	virtual void OnLoad();
	virtual void OnClose();
	virtual void OnUpdate(TimeStep ts);

	// template<typename TComponent>
	// void OnSerialize(const TComponent& component, YAMLSerializer& serializer);

	// template<typename TComponent>
	// TComponent& OnDeserialize();

private:
	inline static s_Instance;
};

}