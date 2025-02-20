#include "ScriptGlue.h"

#include <iostream>

#include <Magma/ECS/Entity.h>

#include <Magma/Scene/Component.h>

#include <Magma/Script/ScriptEngine.h>
#include <Magma/Script/ScriptModule.h>
#include <Magma/Script/ScriptClass.h>
#include <Magma/Script/ScriptObject.h>

using namespace Magma;
using namespace Magma::ECS;
using namespace Magma::Script;

namespace Lava {

static void print(const std::string& str) {
	std::cout << str << "\n";
}

void ScriptGlue::Init() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterObjectType("App", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterGlobalFunction(
		"void print(const string &in)", asFUNCTION(print), asCALL_CDECL);

	ScriptEngine::RegisterInterface("IApp")
		.AddMethod("void OnLoad()")
		.AddMethod("void OnClose()")
		.AddMethod("void OnUpdate(float ts)");

	ScriptEngine::RegisterInterface("IScreen")
		.AddMethod("void OnLoad()")
		.AddMethod("void OnClose()")
		.AddMethod("void OnUpdate(float ts)");

	ScriptEngine::RegisterInterface("IUIObject")
		.AddMethod("void OnClick()")
		.AddMethod("void OnHover()")
		.AddMethod("void OnMouseUp()")
		.AddMethod("void OnMouseDown()");

	engine->RegisterObjectType("Component", sizeof(Component),
		asOBJ_VALUE | asGetTypeTraits<Component>());

	engine->RegisterObjectType("Entity", sizeof(Entity),
		asOBJ_VALUE | asGetTypeTraits<Entity>());
	engine->RegisterObjectMethod("Entity", "bool HasCameraComponent() const",
		asMETHODPR(Entity, Has<CameraComponent>, () const, bool), asCALL_THISCALL);

	engine->RegisterObjectType("Scene", 0, asOBJ_REF | asOBJ_NOCOUNT);
	// engine->RegisterObjectMethod("Scene", "Entity @+ GetEntity(long id)", );
}

}