#include "ScriptGlue.h"

#include <iostream>
#include <fstream>

#include <angelscript.h>
#include <angelscript/add_on/scripthandle/scripthandle.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <angelscript/add_on/scripthelper/scripthelper.h>
#include <angelscript/add_on/scriptmath/scriptmath.h>

#include <Magma/Core/AssetManager.h>

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

static void RegisterGlobalFunctions();
static void RegisterTypes();
static void RegisterInput();
static void RegisterAssetManager();
static void RegisterECS();

void ScriptGlue::RegisterInterface() {
	auto* engine = ScriptEngine::Get();

	RegisterStdString(engine);
	RegisterScriptHandle(engine);
	RegisterScriptMath(engine);

	RegisterGlobalFunctions();
	RegisterTypes();
	RegisterInput();
	RegisterAssetManager();
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

	engine->RegisterObjectType("SceneClass", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("SceneClass", "Entity GetEntity(uint64)",
		asMETHODPR(ECS::World, GetEntity, (UUID), Entity), asCALL_THISCALL, 0,
		asOFFSET(Scene, EntityWorld));

	engine->RegisterObjectType("UIElement", 0, asOBJ_REF);
	// engine->RegisterObjectProperty("UIElement", "");

	engine->RegisterObjectType("UIPageClass", 0, asOBJ_REF | asOBJ_NOHANDLE);
	// engine->RegisterObjectMethod("UIPageClass", "UIElement Get(const string &in)",
	// 	asMETHODPR(UIPage, Get, (const std::string&) const, UIElement*),
	// 	asCALL_THISCALL);
}

static void print(const std::string& str) {
	std::cout << str << "\n";
}

void RegisterGlobalFunctions() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterGlobalFunction(
		"void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
}

static void Vec3DefaultConstructor(Vec3* ptr) {
	new(ptr) Vec3();
}
static void Vec4DefaultConstructor(Vec4* ptr) {
	new(ptr) Vec4();
}

static void Vec3CopyConstructor(const Vec3& other, Vec3* ptr) {
	new(ptr) Vec3(other);
}
static void Vec4CopyConstructor(const Vec4& other, Vec4* ptr) {
	new(ptr) Vec4(other);
}

static void Vec3ConvConstructor(float v, Vec3* ptr) {
	new(ptr) Vec3(v);
}
static void Vec4ConvConstructor(float v, Vec4* ptr) {
	new(ptr) Vec4(v);
}

static void Vec3InitConstructor(float r, float g, float b, Vec3* ptr) {
	new(ptr) Vec3(r, g, b);
}
static void Vec4InitConstructor(float r, float g, float b, float a, Vec4* ptr) {
	new(ptr) Vec4(r, g, b, a);
}

static void Vec3ListConstructor(float* list, Vec3* ptr) {
	new(ptr) Vec3(list[0], list[1], list[2]);
}
static void Vec4ListConstructor(float* list, Vec4* ptr) {
	new(ptr) Vec4(list[0], list[1], list[2], list[3]);
}

void RegisterTypes() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterObjectType("Vec3", sizeof(Vec3),
		asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS
		| asGetTypeTraits<Vec3>());
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,
		"void f()", asFUNCTION(Vec3DefaultConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,
		"void f(const Vec3 &in)",
		asFUNCTION(Vec3CopyConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,
		"void f(float)",
		asFUNCTION(Vec3ConvConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,
		"void f(float, float, float)",
		asFUNCTION(Vec3InitConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_LIST_CONSTRUCT,
		"void f(const int &in) {float, float, float}",
		asFUNCTION(Vec3ListConstructor), asCALL_CDECL_OBJLAST);

	engine->RegisterGlobalFunction(
		"Vec3 normalize(const Vec3 &in)", asFUNCTION(glm::normalize),
		asCALL_CDECL);
}

void RegisterInput() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterEnum("Mouse");
	engine->RegisterEnumValue("Mouse", "Left", 0);
	engine->RegisterEnumValue("Mouse", "Right", 1);
	engine->RegisterEnumValue("Mouse", "Middle", 2);

	engine->RegisterEnum("Key");
	engine->RegisterEnumValue("Key", "A", 65);
	engine->RegisterEnumValue("Key", "B", 66);
	engine->RegisterEnumValue("Key", "C", 67);
	engine->RegisterEnumValue("Key", "D", 68);
	engine->RegisterEnumValue("Key", "E", 69);
	engine->RegisterEnumValue("Key", "F", 70);
	engine->RegisterEnumValue("Key", "G", 71);
	engine->RegisterEnumValue("Key", "H", 72);
	engine->RegisterEnumValue("Key", "I", 73);
	engine->RegisterEnumValue("Key", "J", 74);
	engine->RegisterEnumValue("Key", "K", 75);
	engine->RegisterEnumValue("Key", "L", 76);
	engine->RegisterEnumValue("Key", "M", 77);
	engine->RegisterEnumValue("Key", "N", 78);
	engine->RegisterEnumValue("Key", "O", 79);
	engine->RegisterEnumValue("Key", "P", 80);
	engine->RegisterEnumValue("Key", "Q", 81);
	engine->RegisterEnumValue("Key", "R", 82);
	engine->RegisterEnumValue("Key", "S", 83);
	engine->RegisterEnumValue("Key", "T", 84);
	engine->RegisterEnumValue("Key", "U", 85);
	engine->RegisterEnumValue("Key", "V", 86);
	engine->RegisterEnumValue("Key", "W", 87);
	engine->RegisterEnumValue("Key", "X", 88);
	engine->RegisterEnumValue("Key", "Y", 89);
	engine->RegisterEnumValue("Key", "Z", 90);

	engine->RegisterEnumValue("Key", "Ctrl", 224 + 230);
	engine->RegisterEnumValue("Key", "Shift", 225 + 229);

	engine->RegisterEnumValue("Key", "Right", 262);
	engine->RegisterEnumValue("Key", "Left",  263);
	engine->RegisterEnumValue("Key", "Down",  264);
	engine->RegisterEnumValue("Key", "Up",    265);
}

static uint64_t GetAssetID(Asset* asset) {
	return (uint64_t)asset->ID;
}

void RegisterAssetManager() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterObjectType("Asset", sizeof(Asset),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Asset>());
	engine->RegisterObjectMethod("Asset", "uint64 get_ID() const property",
		asFUNCTION(GetAssetID), asCALL_CDECL_OBJLAST);

	engine->RegisterEnum("AssetType");
	engine->RegisterEnumValue("AssetType", "Mesh",	  0);
	engine->RegisterEnumValue("AssetType", "Texture", 1);
	engine->RegisterEnumValue("AssetType", "Cubemap", 2);
	engine->RegisterEnumValue("AssetType", "Font",	  3);
	engine->RegisterEnumValue("AssetType", "Audio",	  4);
	engine->RegisterEnumValue("AssetType", "Script",  5);
	engine->RegisterEnumValue("AssetType", "Shader",  6);
	engine->RegisterEnumValue("AssetType", "None",	  7);

	engine->RegisterObjectProperty("Asset", "AssetType Type",
		asOFFSET(Asset, Type));
	engine->RegisterObjectProperty("Asset", "bool Primary",
		asOFFSET(Asset, Primary));

	engine->RegisterObjectType("AssetManagerClass", 0,
		asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("AssetManagerClass",
		"bool IsLoaded(Asset) const",
		asMETHOD(AssetManager, IsLoaded), asCALL_THISCALL);
	engine->RegisterObjectMethod("AssetManagerClass", "bool Load(Asset)",
		asMETHOD(AssetManager, Load), asCALL_THISCALL);
	engine->RegisterObjectMethod("AssetManagerClass", "bool Unload(Asset)",
		asMETHOD(AssetManager, Unload), asCALL_THISCALL);

	engine->RegisterObjectType("Sound", 0, asOBJ_REF);

	engine->RegisterObjectMethod("AssetManagerClass",
		"Sound @+ GetSound(Asset)",
		asMETHODPR(AssetManager, Get<Sound>, (Asset), Ref<Sound>),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Sound", "void Play()", asMETHOD(Sound, Play),
		asCALL_THISCALL);
}

static asIScriptObject* GetScriptInstance(ScriptComponent* sc) {
	return sc->Instance->GetHandle();
}

static Physics::RigidBody* GetRigidBody(RigidBodyComponent* rc) {
	return rc->Body.get();
}

void RegisterECS() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterObjectType("CameraComponent", sizeof(CameraComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<CameraComponent>());
	// engine->RegisterObjectProperty("CameraComponent")

	engine->RegisterObjectType("TagComponent", sizeof(TagComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<TagComponent>());
	engine->RegisterObjectProperty("TagComponent", "string Tag",
		asOFFSET(TagComponent, Tag));

	engine->RegisterObjectType("TransformComponent", sizeof(TransformComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<TransformComponent>());
	engine->RegisterObjectProperty("TransformComponent", "Vec3 Translation",
		asOFFSET(TransformComponent, Translation));
	engine->RegisterObjectProperty("TransformComponent", "Vec3 Rotation",
		asOFFSET(TransformComponent, Rotation));
	engine->RegisterObjectProperty("TransformComponent", "Vec3 Scale",
		asOFFSET(TransformComponent, Scale));

	engine->RegisterObjectType("AudioComponent", sizeof(AudioComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<AudioComponent>());
	engine->RegisterObjectProperty("AudioComponent", "Asset AudioAsset",
		asOFFSET(AudioComponent, AudioAsset));

	engine->RegisterObjectType("MeshComponent", sizeof(MeshComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<MeshComponent>());
	engine->RegisterObjectProperty("MeshComponent", "Asset MeshAsset",
		asOFFSET(MeshComponent, MeshAsset));

	engine->RegisterObjectType("SkyboxComponent", sizeof(SkyboxComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<SkyboxComponent>());
	engine->RegisterObjectProperty("SkyboxComponent", "Asset CubemapAsset",
		asOFFSET(SkyboxComponent, CubemapAsset));

	engine->RegisterObjectType("ScriptComponent", sizeof(ScriptComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<ScriptComponent>());
	engine->RegisterObjectProperty("ScriptComponent", "Asset ModuleAsset",
		asOFFSET(ScriptComponent, ModuleAsset));
	engine->RegisterObjectMethod("ScriptComponent", "IEntity@ get_Instance()",
		asFUNCTION(GetScriptInstance), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("RigidBodyComponent", sizeof(RigidBodyComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<RigidBodyComponent>());

	engine->RegisterObjectType("RigidBody", 0,
		asOBJ_REF | asGetTypeTraits<Physics::RigidBody>());
		engine->RegisterObjectType("TransformComponent", sizeof(TransformComponent),
		asOBJ_VALUE | asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asOBJ_POD
		| asGetTypeTraits<TransformComponent>());

	engine->RegisterObjectProperty("Transform", "Vec3 Translation",
		asOFFSET(Transform, Translation));
	engine->RegisterObjectProperty("Transform", "Vec3 Rotation",
		asOFFSET(Transform, Rotation));
	engine->RegisterObjectProperty("Transform", "Vec3 Scale",
		asOFFSET(Transform, Scale));

	engine->RegisterObjectProperty("RigidBody", "")
	engine->RegisterObjectMethod("RigidBody", "void ApplyForce(Vec3 force)",
		asMETHOD(Physics::DynamicBody::ApplyForce));

	// engine->RegisterObjectMethod("RigidBodyComponent", "RigidBody@ get_Body()",
	// 	asFUNCTION(GetRigidBody), asCALL_CDECL_OBJLAST);

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
		| asGetTypeTraits<Entity>());
	engine->RegisterObjectMethod("Entity", "string get_Name() const property",
		asMETHOD(Entity, GetName), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool Alive() const",
		asMETHOD(Entity, IsAlive), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "void Kill()",
		asMETHOD(Entity, Kill), asCALL_THISCALL);

	engine->RegisterObjectMethod("Entity", "bool HasCameraComponent() const",
		asMETHODPR(Entity, Has<CameraComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasTagComponent() const",
		asMETHODPR(Entity, Has<TagComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasTransformComponent() const",
		asMETHODPR(Entity, Has<TransformComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasAudioComponent() const",
		asMETHODPR(Entity, Has<AudioComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasMeshComponent() const",
		asMETHODPR(Entity, Has<MeshComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasSkyboxComponent() const",
		asMETHODPR(Entity, Has<SkyboxComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasScriptComponent() const",
		asMETHODPR(Entity, Has<ScriptComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasRigidBodyComponent() const",
		asMETHODPR(Entity, Has<RigidBodyComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasDirectionalLightComponent() const",
		asMETHODPR(Entity, Has<DirectionalLightComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasPointLightComponent() const",
		asMETHODPR(Entity, Has<PointLightComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasSpotlightComponent() const",
		asMETHODPR(Entity, Has<SpotlightComponent>, () const, bool),
		asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool HasParticleSystemComponent() const",
		asMETHODPR(Entity, Has<ParticleSystemComponent>, () const, bool),
		asCALL_THISCALL);

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

	engine->RegisterObjectMethod("Entity",
		"CameraComponent& SetCameraComponent()",
		asMETHODPR(Entity, Set<CameraComponent>, (),
			CameraComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"TagComponent& SetTagComponent()",
		asMETHODPR(Entity, Set<TagComponent>, (),
			TagComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"TransformComponent& SetTransformComponent()",
		asMETHODPR(Entity, Set<TransformComponent>, (),
			TransformComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"AudioComponent& SetAudioComponent()",
		asMETHODPR(Entity, Set<AudioComponent>, (),
			AudioComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"MeshComponent& SetMeshComponent()",
		asMETHODPR(Entity, Set<MeshComponent>, (),
			MeshComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"SkyboxComponent& SetSkyboxComponent()",
		asMETHODPR(Entity, Set<SkyboxComponent>, (),
			SkyboxComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"ScriptComponent& SetScriptComponent()",
		asMETHODPR(Entity, Set<ScriptComponent>, (),
			ScriptComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"RigidBodyComponent& SetRigidBodyComponent()",
		asMETHODPR(Entity, Set<RigidBodyComponent>, (),
			RigidBodyComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"DirectionalLightComponent& SetDirectionalLightComponent()",
		asMETHODPR(Entity, Set<DirectionalLightComponent>, (),
			DirectionalLightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"PointLightComponent& SetPointLightComponent()",
		asMETHODPR(Entity, Set<PointLightComponent>, (),
			PointLightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"SpotlightComponent& SetSpotlightComponent()",
		asMETHODPR(Entity, Set<SpotlightComponent>, (),
			SpotlightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"ParticleSystemComponent& SetParticleSystemComponent()",
		asMETHODPR(Entity, Set<ParticleSystemComponent>, (),
			ParticleSystemComponent&), asCALL_THISCALL);
}

}