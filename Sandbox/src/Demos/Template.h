#pragma once

namespace Demo {

#pragma once

#include <cstring>

#include "Core/Defines.h"

class Template : public Application {
public:
	Template();

	void OnUpdate(TimeStep ts);

private:
	Ref<Camera> camera;
	CameraController controller;

	Buffer<glm::mat4> buffer;
};

Template::Template() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::K)
				VOLCANICORE_LOG_INFO("Test Key K");
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.1f, 100.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CameraController{ camera };

	VOLCANICORE_LOG_INFO("Template Project Started");

	buffer = Buffer<glm::mat4>(5);

	while(buffer.GetCount() < 4) {
		buffer.Add(glm::mat4(1.0f));
		VOLCANICORE_LOG_INFO("Count: %i", buffer.GetCount());
	}

	Buffer<glm::mat4> partition;
	partition = buffer.Partition(1);
	partition.Add(glm::mat4(1.0f));

	Buffer<glm::mat4> copy;
	copy = buffer.Copy();
	for(auto& transform : copy) {
		VOLCANICORE_LOG_INFO("[");
		for(uint32_t y = 0; y < 4; y++)
			VOLCANICORE_LOG_INFO("\t[%f, %f, %f, %f]", transform[y][0], transform[y][1], transform[y][2], transform[y][3]);
		VOLCANICORE_LOG_INFO("]\n");
	}
}

void Template::OnUpdate(TimeStep ts) {

}

}