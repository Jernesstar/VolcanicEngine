#include "ScriptGlue.h"

#include <iostream>
#include <fstream>

#include <angelscript.h>
#include <angelscript/add_on/scripthandle/scripthandle.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <angelscript/add_on/scripthelper/scripthelper.h>

#include <Magma/ECS/Entity.h>

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/Component.h>

#include <Magma/UI/UIPage.h>

#include <Magma/Script/ScriptEngine.h>
#include <Magma/Script/ScriptModule.h>
#include <Magma/Script/ScriptClass.h>
#include <Magma/Script/ScriptObject.h>

#include "App.h"

using namespace Magma;
using namespace Magma::UI;
using namespace Magma::ECS;
using namespace Magma::Script;

namespace Lava {

static void print(const std::string& str) {
	std::cout << str << "\n";
}

void ScriptGlue::Init() {
	auto* engine = ScriptEngine::Get();

	RegisterStdString(engine);
	RegisterScriptHandle(engine);

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

	engine->RegisterObjectType("CameraComponent", sizeof(CameraComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<CameraComponent>());

	engine->RegisterObjectType("Entity", sizeof(Entity),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<Entity>());
	engine->RegisterObjectMethod("Entity", "string get_Name() const property",
		asMETHOD(Entity, GetName), asCALL_THISCALL);

	engine->RegisterObjectMethod("Entity", "bool HasCameraComponent() const",
		asMETHODPR(Entity, Has<CameraComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasTagComponent() const",
		asMETHODPR(Entity, Has<TagComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasTransformComponent() const",
		asMETHODPR(Entity, Has<TransformComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasMeshComponent() const",
		asMETHODPR(Entity, Has<MeshComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasSkyboxComponent() const",
		asMETHODPR(Entity, Has<SkyboxComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasRigidBodyComponent() const",
		asMETHODPR(Entity, Has<RigidBodyComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasScriptComponent() const",
		asMETHODPR(Entity, Has<ScriptComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasAudioComponent() const",
		asMETHODPR(Entity, Has<AudioComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasDirectionalLightComponent() const",
		asMETHODPR(Entity, Has<DirectionalLightComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasPointLightComponent() const",
		asMETHODPR(Entity, Has<PointLightComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasSpotlightComponent() const",
		asMETHODPR(Entity, Has<SpotlightComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasParticleSystemComponent() const",
		asMETHODPR(Entity, Has<ParticleSystemComponent>, () const, bool), asCALL_THISCALL);

	engine->RegisterObjectType("Scene", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectMethod("Scene", "Entity GetEntity(const uint64 &in)",
		asMETHODPR(ECS::World, GetEntity, (UUID), Entity), asCALL_THISCALL, 0,
		asOFFSET(Scene, EntityWorld));

	engine->RegisterObjectType("UIElement", 0, asOBJ_REF | asOBJ_NOCOUNT);
	// engine->RegisterObjectProperty("UIElement", "")

	engine->RegisterObjectType("UIPage", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectMethod("UIPage", "UIElement@ Get(const string &in)",
		asMETHODPR(UIPage, Get, (const std::string&) const, UIElement*),
		asCALL_THISCALL);
}

}