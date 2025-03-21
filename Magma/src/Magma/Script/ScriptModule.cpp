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
	m_Ctx = ScriptEngine::Get()->CreateContext();
}

ScriptModule::~ScriptModule() {
	m_Ctx->Release();
}

void ScriptModule::Load(const std::string& path) {
	namespace fs = std::filesystem;

	auto* engine = ScriptEngine::Get();

	if(!fs::exists(path)) {
		VOLCANICORE_LOG_WARNING("File '%s' does not exist", path.c_str());
		return;
	}

	if(fs::path(path).extension() == ".as") {
		CScriptBuilder builder;
		builder.StartNewModule(engine, Name.c_str());
		builder.AddSectionFromFile(path.c_str());
		builder.BuildModule();
		m_Handle = builder.GetModule();
	}
	else {
		m_Handle = engine->GetModule(Name.c_str(), asGM_ALWAYS_CREATE);
		ByteCodeStream stream(path, 0);
		m_Handle->LoadByteCode(&stream);
	}

	for(uint32_t i = 0; i < m_Handle->GetObjectTypeCount(); i++) {
		asITypeInfo* type = m_Handle->GetObjectTypeByIndex(i);
		std::string name = type->GetName();
		auto scriptClass = CreateRef<ScriptClass>(name, type);
		scriptClass->m_Module = this;
		m_Classes[name] = scriptClass;
	}
}

void ScriptModule::Save(const std::string& path) {
	ByteCodeStream stream(path, 1);
	m_Handle->SaveByteCode(&stream, true);
}

Ref<ScriptClass> ScriptModule::GetClass(const std::string& name) const {
	if(!m_Classes.count(name))
		return nullptr;
	return m_Classes.at(name);
}

}