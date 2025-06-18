#include "ScriptManager.h"

#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>

#include <VolcaniCore/Core/FileUtils.h>

#include "Editor/Editor.h"
#include "Project/ScriptEditorPanel.h"

namespace fs = std::filesystem;

namespace Magma {

asIScriptModule* ScriptManager::LoadScript(const std::string& path,
	bool metadata, bool* error, std::string name)
{
	auto* engine = ScriptEngine::Get();

	if(!fs::exists(path)) {
		VOLCANICORE_LOG_ERROR(
			"GetScriptData: File '%s' does not exist", path.c_str());
		if(error) *error = true;
		return nullptr;
	}

	if(name == "")
		name = fs::path(path).filename().stem().string();
	CScriptBuilder builder;
	int r;
	r = builder.StartNewModule(engine, name.c_str());
	if(r < 0) {
		VOLCANICORE_LOG_ERROR("StartNewModule failed'%s'", name.c_str());
		if(error) *error = true;
		return nullptr;
	}
	builder.DefineWord("EDITOR");
	r = builder.AddSectionFromFile(path.c_str());
	if(r < 0) {
		VOLCANICORE_LOG_ERROR("AddSectionFromFile failed");
		if(error) *error = true;
		return nullptr;
	}
	r = builder.BuildModule();
	if(r < 0) {
		VOLCANICORE_LOG_ERROR("BuildModule failed");
		if(error) *error = true;
		return nullptr;
	}
	asIScriptModule* handle = builder.GetModule();

	if(error) *error = false;

	if(!metadata)
		return handle;

	for(uint32_t i = 0; i < handle->GetFunctionCount(); i++) {
		asIScriptFunction* func = handle->GetFunctionByIndex(i);

	}

	for(uint32_t i = 0; i < handle->GetObjectTypeCount(); i++) {
		asITypeInfo* type = handle->GetObjectTypeByIndex(i);
		int id = type->GetTypeId();
		std::string className = type->GetName();

		auto& list = s_ClassMetadata[className];
		for(auto str : builder.GetMetadataForType(id))
			list.Add(str);

		for(uint32_t i = 0; i < type->GetPropertyCount(); i++) {
			for(auto str : builder.GetMetadataForTypeProperty(id, i)) {
				const char* name;
				type->GetProperty(i, &name);
				s_FieldMetadata[className + "::" + name].Add(str);
			}
		}

		for(uint32_t i = 0; i < type->GetMethodCount(); i++) {
			asIScriptFunction* method = type->GetMethodByIndex(i);
			for(auto str : builder.GetMetadataForTypeMethod(id, method)) {
				std::string name = method->GetName();
				s_MethodMetadata[className + "::" + name].Add(str);
			}
		}
	}

	return handle;
}

class ByteCodeWriter : public asIBinaryStream {
public:
	ByteCodeWriter(BinaryWriter* writer)
		: m_Writer(writer) { }
	~ByteCodeWriter() = default;

	int Read(void* data, uint32_t size) override {
		return 1;
	}

	int Write(const void* data, uint32_t size) override {
		m_Writer->WriteData(data, (uint64_t)size);
		return 0;
	}

private:
	BinaryWriter* m_Writer = nullptr;
};

void ScriptManager::SaveScript(asIScriptModule* mod, BinaryWriter& writer) {
	ByteCodeWriter stream(&writer);
	mod->SaveByteCode(&stream, true);
}

void ScriptManager::RunCodeAnalysis() {
	auto panel =
		Editor::GetProjectTab()->
			GetPanel("ScriptEditor")->As<ScriptEditorPanel>();
}

static asIScriptContext* s_Context = nullptr;
static asIScriptFunction* s_LastFunction = nullptr;
static asIScriptModule* s_LastModule = nullptr;
static int s_StackLevel = 0;
static bool s_Suspended = false;

static void DebugLineCallback(asIScriptContext* ctx) {
	if(ctx->GetState() != asEXECUTION_ACTIVE)
		return;

	const char* path = nullptr;
	int lineNbr = ctx->GetLineNumber(s_StackLevel, 0, &path);
	if(!path)
		return;

	if(s_Suspended) {
		ctx->Suspend();
	}

	auto panel =
		Editor::GetProjectTab()->
			GetPanel("ScriptEditor")->As<ScriptEditorPanel>();
	panel->OpenFile(path, s_Suspended);

	auto* file = panel->GetFile(path);
	VOLCANICORE_ASSERT(file);

	bool isBreakpoint = file->Breakpoints.contains(lineNbr);
	if(!isBreakpoint)
		return;

	asIScriptFunction* func = ctx->GetFunction();
	VOLCANICORE_LOG_INFO("File: %s, Function: %s, Line: %d",
		path, func->GetName(), lineNbr);

	s_Suspended = true;
	ctx->Suspend();

	Editor::GetProjectTab()->OnPause();
}

static void DebugExceptionCallback(asIScriptContext* ctx) {

}

void ScriptManager::StartDebug() {
	s_Context = ScriptEngine::GetContext();
	s_Context->SetLineCallback(
		asFUNCTION(DebugLineCallback), nullptr, asCALL_CDECL);
}

void ScriptManager::EndDebug() {
	if(s_Context)
		s_Context->ClearLineCallback();
	s_Context = nullptr;
	s_LastModule = nullptr;
	s_LastFunction = nullptr;
	s_StackLevel = 0;
	s_Suspended = false;
}

void ScriptManager::StepOver() {
	s_Context->Execute();
	s_Context->Suspend();
}

void ScriptManager::StepInto() {
	if(s_StackLevel < s_Context->GetCallstackSize())
		s_StackLevel++;
}

void ScriptManager::StepOut() {
	if(s_StackLevel)
		s_StackLevel--;
}

void ScriptManager::Continue() {
	s_Suspended = false;
	Editor::GetProjectTab()->OnResume();
}

}