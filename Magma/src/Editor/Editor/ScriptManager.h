#pragma once

#include <Magma/Core/BinaryWriter.h>
#include <Magma/Script/ScriptObject.h>

using namespace VolcaniCore;
using namespace Magma::Script;

namespace Magma {

// Loading, metadata, debugger, linting
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
			s_FunctionMetadata[name]
			.Find([&](auto& val) { return val == str; });
	}
	static bool ClassHasMetadata(const std::string& name,
								 const std::string& str)
	{
		return
			s_ClassMetadata[name].Find([&](auto& val) { return val == str; });
	}
	static bool FieldHasMetadata(const std::string& _class,
		const std::string& field, const std::string& str)
	{
		return
			s_FieldMetadata[_class + "::" + field]
			.Find([&](auto& val) { return val == str; });
	}

	static void StartDebug();
	static void EndDebug();
	static void StepOver();
	static void StepInto();
	static void StepOut();
	static void Continue();

private:
	inline static Map<std::string, List<std::string>> s_FunctionMetadata;
	inline static Map<std::string, List<std::string>> s_ClassMetadata;
	inline static Map<std::string, List<std::string>> s_FieldMetadata;
	inline static Map<std::string, List<std::string>> s_MethodMetadata;
};

}