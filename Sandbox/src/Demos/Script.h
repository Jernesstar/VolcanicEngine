#pragma once

#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Script/ScriptEngine.h>

using namespace Magma::Script;

namespace Demo {

void RunScriptEngine();

class Script : public Application {
public:
	Script();
	~Script();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<Camera> camera;
	CameraController controller;
};

Script::Script() {
	Events::RegisterListener<KeyPressedEvent>(
		[&](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
			if(event.Key == Key::K)
				RunScriptEngine();
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	ScriptEngine::Init();
	VOLCANICORE_LOG_INFO("Success");
}

Script::~Script() {
	ScriptEngine::Shutdown();
}

void Script::OnUpdate(TimeStep ts) {

}

void RunScriptEngine() {


}

}