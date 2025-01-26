#pragma once

#include <angelscript.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>

#include <VolcaniCore/Core/FileUtils.h>

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

void PrintString(std::string& str)
{
	VOLCANICORE_LOG_INFO(str.c_str());
}

void RunScriptEngine() {
	asIScriptEngine* engine = asCreateScriptEngine();
	engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

	RegisterStdString(engine);
	engine->RegisterGlobalFunction("void Print(string& in)", asFUNCTION(PrintString), asCALL_CDECL);

	CScriptBuilder builder;
	int r = builder.StartNewModule(engine, "MyModule");
	if( r < 0 )
	{
		// If the code fails here it is usually because there
		// is no more memory to allocate the module
		printf("Unrecoverable error while starting a new module.\n");
		return;
	}
	r = builder.AddSectionFromFile("Sandbox/assets/scripts/script.as");
	if( r < 0 )
	{
		// The builder wasn't able to load the file. Maybe the file
		// has been removed, or the wrong name was given, or some
		// preprocessing commands are incorrectly written.
		printf("Please correct the errors in the script and try again.\n");
		return;
	}
	r = builder.BuildModule();
	if( r < 0 )
	{
		// An error occurred. Instruct the script writer to fix the 
		// compilation errors that were listed in the output stream.
		printf("Please correct the errors in the script and try again.\n");
		return;
	}

	// Create a context that will execute the script.
	asIScriptContext* ctx = engine->CreateContext();

	asIScriptFunction* func =
		engine->GetModule("MyModule")->GetFunctionByDecl("float calc(float, float)");

	ctx->Prepare(func);
	ctx->SetArgFloat(0, 3.14159265359f);
	ctx->SetArgFloat(1, 2.71828182846f);
	r = ctx->Execute();

	if(r == asEXECUTION_FINISHED) {
		float returnValue = ctx->GetReturnFloat();
		VOLCANICORE_LOG_INFO("The script function returned: %f", returnValue);
	}

	// We must release the contexts when no longer using them
	ctx->Release();

	engine->ShutDownAndRelease();
}

}