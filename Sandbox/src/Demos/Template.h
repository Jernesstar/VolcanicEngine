#pragma once

namespace Demo {

class Template : public Application {
public:
	Template();

	void OnUpdate(TimeStep ts) override;

private:
	Buffer<void> buffer;

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

	buffer = Buffer<void>(List<uint32_t>{ 1, 2, 3, 4, 5 });
	VOLCANICORE_LOG_INFO("Template Project Started");

	uint32_t* data = new uint32_t[]{ 6, 7, 8, 9, 10, 11, 12 };
	buffer.Set(data, 7, 1);

	uint32_t* bufferData = buffer.Get<uint32_t>();
	for(uint64_t i = 0; i < buffer.GetCount(); i++)
		VOLCANICORE_LOG_INFO("%i", i[bufferData]);
}

void Template::OnUpdate(TimeStep ts) {

}

}