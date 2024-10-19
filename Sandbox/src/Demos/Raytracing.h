#pragma once

#include <Ash/Ray.h>

namespace Demo {

class RaytracingApp : public Application {
public:
	RaytracingApp();

	void OnUpdate(TimeStep ts);

private:

};

RaytracingApp::RaytracingApp() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	Ash::Ray r;
	auto val = r(0.5f);
}

void RaytracingApp::OnUpdate(TimeStep ts) {

}

}