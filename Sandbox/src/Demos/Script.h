#pragma once

#include <angelscript.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>

namespace Demo {

void RunScriptEngine();

class Script : public Application {
public:
	Script();

	void OnUpdate(TimeStep ts) override;

private:
	Ref<Camera> camera;
	CameraController controller;
};

Script::Script() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 0.0f, 0.0f, 3.0f });
	controller = CameraController{ camera };

	RunScriptEngine();
	VOLCANICORE_LOG_INFO("Success");
}

void Script::OnUpdate(TimeStep ts) {

}

void MessageCallback(const asSMessageInfo *msg, void *param) {
	const char* type = "ERR ";
	if(msg->type == asMSGTYPE_WARNING)
		type = "WARN";
	else if(msg->type == asMSGTYPE_INFORMATION)
		type = "INFO";

	VOLCANICORE_LOG_INFO("%s (%d, %d) : %s : %s\n", msg->section, msg->row, msg->col, type, msg->message);
}

void RunScriptEngine() {
	asIScriptEngine *engine = asCreateScriptEngine();
	engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	// Create a context that will execute the script.
	asIScriptContext *ctx = engine->CreateContext();

	RegisterStdString(engine);
	// ...

	// We must release the contexts when no longer using them
	ctx->Release();

	engine->ShutDownAndRelease();
}

}