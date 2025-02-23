#include "ScriptModule.h"

#include <angelscript.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
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

void ScriptModule::Reload(const std::string& path) {
	namespace fs = std::filesystem;

	auto* engine = ScriptEngine::Get();

	if(path != "") {
		m_Path = path;

		if(fs::path(path).extension() == ".as") {
			CScriptBuilder builder;
			builder.StartNewModule(engine, Name.c_str());
			builder.AddSectionFromFile(m_Path.c_str());
			builder.BuildModule();
			m_Handle = builder.GetModule();
		}
		else {
			m_Handle = engine->GetModule(Name.c_str(), asGM_ALWAYS_CREATE);
			ByteCodeStream stream(path, 0);
			m_Handle->LoadByteCode(&stream);
		}

		return;
	}

	if(m_Path != "")
		return;

	// TODO(Implement): Hot reload
}

void ScriptModule::Save(const std::string& path) {
	ByteCodeStream stream(path, 1);
	m_Handle->SaveByteCode(&stream, true);
}

Ref<ScriptClass> ScriptModule::GetScriptClass(const std::string& name) {
	asITypeInfo* type = m_Handle->GetTypeInfoByDecl(name.c_str());
	if(!type)
		return nullptr;

	auto scriptClass = CreateRef<ScriptClass>(name, type);
	scriptClass->m_Module = this;

	return scriptClass;
}

}