#pragma once

namespace Demo {

class Template : public Application {
public:
	Template();

	void OnUpdate(TimeStep ts) override;

private:
	Buffer<uint32_t> buffer;

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
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CameraController{ camera };

	buffer = Buffer<uint32_t>({ 1, 2, 3, 4, 5 });
	VOLCANICORE_LOG_INFO("Template Project Started");
	uint32_t* data = new uint32_t[]{ 6, 7, 8, 9, 10, 11 };
	buffer.Set(data, 6, 1);
	for(uint32_t val : buffer)
		VOLCANICORE_LOG_INFO("%i", val);
	VOLCANICORE_LOG_INFO("%i", buffer[4]);
}

void Template::OnUpdate(TimeStep ts) {

}

}