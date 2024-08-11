#pragma once

namespace Demo {

class Template : public Application {
public:
	Template();

	void OnUpdate(TimeStep ts);

private:
	Ref<Camera> camera;
	Ref<CameraController> controller;
};

Template::Template() {
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::Escape)
			Application::Close();
	});
	Events::RegisterListener<KeyPressedEvent>(
	[](const KeyPressedEvent& event) {
		if(event.Key == Key::K)
		VOLCANICORE_LOG_INFO("Test Key K");
	});

	camera = CreateRef<StereographicCamera>(75.0f);
	// camera = CreateRef<OrthographicCamera>(800, 600, 0.1f, 100.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CreateRef<CameraController>(camera);

	VOLCANICORE_LOG_INFO("Template Project Started");
}

void Template::OnUpdate(TimeStep ts) {

}

}