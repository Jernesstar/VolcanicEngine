#include "ScriptManager.h"

#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>

#include <VolcaniCore/Core/FileUtils.h>

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

	for(uint32_t i = 0; i < handle->GetObjectTypeCount(); i++) {
		asITypeInfo* type = handle->GetObjectTypeByIndex(i);

		// auto& list = ScriptManager::GetClassMetadata(id);
		// for(auto str : builder.GetMetadataForType(id))
		// 	_class->m_Metadata.Add(str);

		// for(uint32_t i = 0; i < type->GetPropertyCount(); i++)
		// 	for(auto str : builder.GetMetadataForTypeProperty(id, i)) {
		// 		const char* name;
		// 		type->GetProperty(i, &name);
		// 		_class->m_FieldMetadata[name].Add(str);
		// 	}
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

}