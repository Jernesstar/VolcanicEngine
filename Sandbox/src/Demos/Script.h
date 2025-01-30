#pragma once

#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Script/ScriptEngine.h>
#include <Magma/Script/ScriptModule.h>
#include <Magma/Script/ScriptClass.h>

using namespace Magma::Script;

namespace Demo {

class Script : public Application {
public:
	Script();
	~Script();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<Camera> camera;
	CameraController controller;

	Ref<ScriptModule> mod;
	Screen screen;
};

Script::Script()
	: screen("Test", "Test", "Test")
{
	Ref<ScriptObject> obj;

	Events::RegisterListener<KeyPressedEvent>(
		[&](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
			if(event.Key == Key::R)
				mod->Reload();
		});
	Events::RegisterListener<MouseButtonPressedEvent>(
		[&](const MouseButtonPressedEvent& event)
		{
			obj->Call("OnClick");
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	ScriptEngine::Init();

	ScriptEngine::RegisterInterface("IUIObject")
		.AddMethod("void OnClick()")
		.AddMethod("void OnHover()")
		.AddMethod("void OnMouseUp()")
		.AddMethod("void OnMouseDown()");

	mod = CreateRef<ScriptModule>("TestScript");
	mod->Reload("Sandbox/assets/scripts/script.as");

	Ref<ScriptClass> scriptClass = mod->GetScriptClass("UI::Button");
	// scriptClass->SetInstanceMethod({ "Screen @screen" });

	// obj = scriptClass->Instantiate(screen);
}

Script::~Script() {
	ScriptEngine::Shutdown();
}

void Script::OnUpdate(TimeStep ts) {

}

}