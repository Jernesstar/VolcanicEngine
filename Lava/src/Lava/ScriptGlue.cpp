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

static void RegisterStaticFunctions();
static void RegisterTypes();
static void RegisterECS();

void ScriptGlue::RegisterInterface() {
	auto* engine = ScriptEngine::Get();

	RegisterStdString(engine);
	RegisterScriptHandle(engine);
	RegisterScriptMath(engine);

	RegisterStaticFunctions();
	RegisterECS();

	ScriptEngine::RegisterInterface("IApp")
		.AddMethod("void OnLoad()")
		.AddMethod("void OnClose()")
		.AddMethod("void OnUpdate(float ts)");

	ScriptEngine::RegisterInterface("IScreen")
		.AddMethod("void OnLoad()")
		.AddMethod("void OnClose()")
		.AddMethod("void OnUpdate(float ts)");

	ScriptEngine::RegisterInterface("IEntity")
		.AddMethod("void OnUpdate(float ts)")
		// .AddMethod("void OnEvent()")
		;

	ScriptEngine::RegisterInterface("IUIObject")
		.AddMethod("void OnClick()")
		.AddMethod("void OnHover()")
		.AddMethod("void OnMouseUp()")
		.AddMethod("void OnMouseDown()");

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

void RegisterStaticFunctions() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterGlobalFunction(
		"void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
}

void RegisterTypes() {
	// Vec
	// 
}

void RegisterECS() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterObjectType("CameraComponent", sizeof(CameraComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<CameraComponent>());
	engine->RegisterObjectType("TagComponent", sizeof(TagComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<TagComponent>());
	engine->RegisterObjectType("TransformComponent", sizeof(TransformComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<TransformComponent>());
	engine->RegisterObjectType("AudioComponent", sizeof(AudioComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<AudioComponent>());
	engine->RegisterObjectType("MeshComponent", sizeof(MeshComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<MeshComponent>());
	engine->RegisterObjectType("SkyboxComponent", sizeof(SkyboxComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<SkyboxComponent>());
	engine->RegisterObjectType("ScriptComponent", sizeof(ScriptComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<ScriptComponent>());
	engine->RegisterObjectType("RigidBodyComponent", sizeof(RigidBodyComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<RigidBodyComponent>());
	engine->RegisterObjectType("DirectionalLightComponent", sizeof(DirectionalLightComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<DirectionalLightComponent>());
	engine->RegisterObjectType("PointLightComponent", sizeof(PointLightComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<PointLightComponent>());
	engine->RegisterObjectType("SpotlightComponent", sizeof(SpotlightComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<SpotlightComponent>());
	engine->RegisterObjectType("ParticleSystemComponent", sizeof(ParticleSystemComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<ParticleSystemComponent>());

	engine->RegisterObjectType("Entity", sizeof(Entity),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asOBJ_APP_CLASS_ALLINTS | asGetTypeTraits<Entity>());
	engine->RegisterObjectMethod("Entity", "string get_Name() const property",
		asMETHOD(Entity, GetName), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool Alive() const",
		asMETHOD(Entity, IsAlive), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "void Kill()",
		asMETHOD(Entity, Kill), asCALL_THISCALL);

	engine->RegisterObjectMethod("Entity", "bool HasCameraComponent() const",
		asMETHODPR(Entity, Has<CameraComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasTagComponent() const",
		asMETHODPR(Entity, Has<TagComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasTransformComponent() const",
		asMETHODPR(Entity, Has<TransformComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasAudioComponent() const",
		asMETHODPR(Entity, Has<AudioComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasMeshComponent() const",
		asMETHODPR(Entity, Has<MeshComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasSkyboxComponent() const",
		asMETHODPR(Entity, Has<SkyboxComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasScriptComponent() const",
		asMETHODPR(Entity, Has<ScriptComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasRigidBodyComponent() const",
		asMETHODPR(Entity, Has<RigidBodyComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasDirectionalLightComponent() const",
		asMETHODPR(Entity, Has<DirectionalLightComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasPointLightComponent() const",
		asMETHODPR(Entity, Has<PointLightComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasSpotlightComponent() const",
		asMETHODPR(Entity, Has<SpotlightComponent>, () const, bool), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasParticleSystemComponent() const",
		asMETHODPR(Entity, Has<ParticleSystemComponent>, () const, bool), asCALL_THISCALL);

	engine->RegisterObjectMethod("Entity",
		"CameraComponent& GetCameraComponent()",
		asMETHODPR(Entity, Get<CameraComponent>, () const,
			const CameraComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"TagComponent& GetTagComponent()",
		asMETHODPR(Entity, Get<TagComponent>, () const,
			const TagComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"TransformComponent& GetTransformComponent()",
		asMETHODPR(Entity, Get<TransformComponent>, () const,
			const TransformComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"AudioComponent& GetAudioComponent()",
		asMETHODPR(Entity, Get<AudioComponent>, () const,
			const AudioComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"MeshComponent& GetMeshComponent()",
		asMETHODPR(Entity, Get<MeshComponent>, () const,
			const MeshComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"SkyboxComponent& GetSkyboxComponent()",
		asMETHODPR(Entity, Get<SkyboxComponent>, () const,
			const SkyboxComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"ScriptComponent& GetScriptComponent()",
		asMETHODPR(Entity, Get<ScriptComponent>, () const,
			const ScriptComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"RigidBodyComponent& GetRigidBodyComponent()",
		asMETHODPR(Entity, Get<RigidBodyComponent>, () const,
			const RigidBodyComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"DirectionalLightComponent& GetDirectionalLightComponent()",
		asMETHODPR(Entity, Get<DirectionalLightComponent>, () const,
			const DirectionalLightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"PointLightComponent& GetPointLightComponent()",
		asMETHODPR(Entity, Get<PointLightComponent>, () const,
			const PointLightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"SpotlightComponent& GetSpotlightComponent()",
		asMETHODPR(Entity, Get<SpotlightComponent>, () const,
			const SpotlightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"ParticleSystemComponent& GetParticleSystemComponent()",
		asMETHODPR(Entity, Get<ParticleSystemComponent>, () const,
			const ParticleSystemComponent&), asCALL_THISCALL);
}

}