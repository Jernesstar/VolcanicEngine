#pragma once

#include <VolcaniCore/Core/List.h>

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

	VOLCANICORE_LOG_INFO("Template Project Started");
	List<std::string> list = { "1", "2", "3", "4" };

	list.Add(std::string("5"));
	VOLCANICORE_LOG_INFO("%s", list.Pop().c_str());
	VOLCANICORE_LOG_INFO("%s", list.Emplace("15").c_str());

	for(auto& val : list)
		VOLCANICORE_LOG_INFO(val.c_str());
}

void Template::OnUpdate(TimeStep ts) {

}

}