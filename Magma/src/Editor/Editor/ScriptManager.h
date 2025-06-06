#pragma once

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Script/ScriptObject.h>

using namespace VolcaniCore;
using namespace Magma::Script;

namespace Magma {

// Loading, linting, metadata
class ScriptManager {
public:
	ScriptManager() = default;
	~ScriptManager() = default;

	static asIScriptModule* LoadScript(const std::string& path,
		bool metadata = true, bool* error = nullptr, std::string name = "");
	static void SaveScript(asIScriptModule* mod, BinaryWriter& writer);

	static bool FunctionHasMetadata(const std::string& name,
									const std::string& str)
	{
		return
			m_FunctionMetadata[name].Find(
				[&](auto& val) { return val == str; }).Found;
	}
	static bool ClassHasMetadata(const std::string& name,
								 const std::string& str)
	{
		return
			m_ClassMetadata[name].Find(
				[&](auto& val) { return val == str; }).Found;
	}
	static bool FieldHasMetadata(const std::string& _class,
		const std::string& field, const std::string& str)
	{
		return
			m_FieldMetadata[_class + "::" + field].Find(
				[&](auto& val) { return val == str; }).Found;
	}

private:
	inline static Map<std::string, List<std::string>> m_FunctionMetadata;
	inline static Map<std::string, List<std::string>> m_ClassMetadata;
	inline static Map<std::string, List<std::string>> m_FieldMetadata;
};

}