#pragma once

#include <iostream>

#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>

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
	Ref<ScriptClass> scriptClass;
	Ref<ScriptObject> obj;
};

static void print(const std::string& str) {
	std::cout << str << "\n";
}

Script::Script() {
	Events::RegisterListener<KeyPressedEvent>(
		[&](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});
	Events::RegisterListener<KeyPressedEvent>(
		[&](const KeyPressedEvent& event)
		{
			obj->Call("OnClick");
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	ScriptEngine::Init();

	RegisterStdString(ScriptEngine::Get());

	ScriptEngine::Get()->RegisterGlobalFunction(
		"void print(const string &in)", asFUNCTION(print), asCALL_CDECL);

	ScriptEngine::RegisterInterface("IUIObject")
		.AddMethod("void OnClick()")
		.AddMethod("void OnHover()")
		.AddMethod("void OnMouseUp()")
		.AddMethod("void OnMouseDown()");

	mod = CreateRef<ScriptModule>("TestScript");
	mod->Load("Sandbox/assets/scripts/script.as");

	scriptClass = mod->GetScriptClass("Button");

	obj = scriptClass->Instantiate();
}

Script::~Script() {
	obj.reset();
	mod.reset();
	ScriptEngine::Shutdown();
}

void Script::OnUpdate(TimeStep ts) {

}

}