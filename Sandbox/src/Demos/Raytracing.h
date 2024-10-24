#pragma once

#include <Ash/Ray.h>

namespace Demo {

class Raytracing : public Application {
public:
	Raytracing();

	void OnUpdate(TimeStep ts);

private:

};

Raytracing::Raytracing() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	Ash::Ray r;
	auto val = r(0.5f);
}

void Raytracing::OnUpdate(TimeStep ts) {

}

}