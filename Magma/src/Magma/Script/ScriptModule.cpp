#include "ScriptModule.h"

#include <angelscript.h>
#include <angelscript/add_on/scriptbuilder/scriptbuilder.h>

#include <VolcaniCore/Core/FileUtils.h>

#include <Magma/Core/BinaryReader.h>
#include <Magma/Core/BinaryWriter.h>

#include "ScriptEngine.h"

namespace Magma::Script {

class ByteCodeStream : public asIBinaryStream {
public:
	ByteCodeStream(const std::string& path, bool option) {
		if(option == 0)
			m_Reader = new BinaryReader(path);
		else
			m_Writer = new BinaryWriter(path);
	}
	~ByteCodeStream() {
		if(m_Reader)
			delete m_Reader;
		else
			delete m_Writer;
	}

	int Read(void* data, uint32_t size) override {
		VOLCANICORE_ASSERT(m_Reader);
		m_Reader->ReadData(data, (uint64_t)size);
		return 0;
	}

	int Write(const void* data, uint32_t size) override {
		VOLCANICORE_ASSERT(m_Writer);
		m_Writer->WriteData(data, (uint64_t)size);
		return 0;
	}

private:
	BinaryReader* m_Reader = nullptr;
	BinaryWriter* m_Writer = nullptr;
};

ScriptModule::ScriptModule(const std::string& name)
	: Name(name)
{
}

ScriptModule::~ScriptModule() {

}

void ScriptModule::Load(const std::string& path) {
	namespace fs = std::filesystem;

	auto* engine = ScriptEngine::Get();

	if(!fs::exists(path)) {
		VOLCANICORE_LOG_ERROR("File '%s' does not exist", path.c_str());
		m_HasErrors = true;
		return;
	}

	CScriptBuilder builder;
	bool loadMetadata = true;
	if(fs::path(path).extension() == ".as") {
		int r;
		r = builder.StartNewModule(engine, Name.c_str());
		if(r < 0) {
			VOLCANICORE_LOG_ERROR("StartNewModule failed'%s'", Name.c_str());
			m_HasErrors = true;
			return;
		}
		builder.DefineWord("EDITOR");
		r = builder.AddSectionFromFile(path.c_str());
		if(r < 0) {
			VOLCANICORE_LOG_ERROR("AddSectionFromFile failed");
			m_HasErrors = true;
			return;
		}
		r = builder.BuildModule();
		if(r < 0) {
			VOLCANICORE_LOG_ERROR("BuildModule failed");
			m_HasErrors = true;
			return;
		}
		m_Handle = builder.GetModule();
	}
	else {
		m_Handle = engine->GetModule(Name.c_str(), asGM_ALWAYS_CREATE);
		ByteCodeStream stream(path, 0);
		m_Handle->LoadByteCode(&stream);
		loadMetadata = false;
	}

	for(uint32_t i = 0; i < m_Handle->GetObjectTypeCount(); i++) {
		asITypeInfo* type = m_Handle->GetObjectTypeByIndex(i);
		std::string name = type->GetName();

		auto scriptClass = CreateRef<ScriptClass>(name, type);
		scriptClass->m_Module = this;
		m_Classes[name] = scriptClass;
	}

	if(!loadMetadata)
		return;

	for(auto [_, _class] : m_Classes) {
		auto type = _class->m_Type;
		auto id = type->GetTypeId();
		for(auto str : builder.GetMetadataForType(id))
			_class->m_Metadata.Add(str);

		for(uint32_t i = 0; i < type->GetPropertyCount(); i++)
			for(auto str : builder.GetMetadataForTypeProperty(id, i)) {
				const char* name;
				type->GetProperty(i, &name);
				_class->m_FieldMetadata[name].Add(str);
			}
	}
}

void ScriptModule::Save(const std::string& path) {
	ByteCodeStream stream(path, 1);
	m_Handle->SaveByteCode(&stream, true);
}

Ref<ScriptClass> ScriptModule::GetClass(const std::string& name) const {
	if(!m_Classes.count(name)) {
		VOLCANICORE_LOG_WARNING("Could not find class '%s'", name.c_str());
		return nullptr;
	}
	return m_Classes.at(name);
}

}