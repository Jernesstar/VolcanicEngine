#pragma once

namespace Demo {

class Template : public Application {
public:
	Template();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<Camera> camera;
	CameraController controller;
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
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	VOLCANICORE_LOG_INFO("Template Project Started");

	RendererAPI::Get()->NewDrawCommand(nullptr)->Clear = true;
	RendererAPI::Get()->NewDrawCommand(nullptr)->Clear = true;
	RendererAPI::Get()->NewDrawCommand(nullptr)->Clear = true;
	RendererAPI::Get()->NewDrawCommand(nullptr)->Clear = true;

	VOLCANICORE_LOG_INFO("Success");
}

void Template::OnUpdate(TimeStep ts) {
}

}