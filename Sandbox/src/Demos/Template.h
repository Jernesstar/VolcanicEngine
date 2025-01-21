#pragma once

#include <VolcaniCore/Core/List.h>

namespace Demo {

class Template : public Application {
public:
	Template();

	void OnUpdate(TimeStep ts) override;

private:
	List<std::string> list;

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
	list = { "1", "2", "3", "4" };

	list.Add(std::string("5"));
	VOLCANICORE_LOG_INFO("%s", list.Pop().c_str());
	VOLCANICORE_LOG_INFO("%s", list.Emplace("15").c_str());

	List<std::string> another = list;
	for(auto& val : list)
		VOLCANICORE_LOG_INFO(val.c_str());
}

void Template::OnUpdate(TimeStep ts) {
}

}