#include "ScriptGlue.h"

#include <iostream>
#include <fstream>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <angelscript.h>
#include <angelscript/add_on/scripthandle/scripthandle.h>
#include <angelscript/add_on/scriptstdstring/scriptstdstring.h>
#include <angelscript/add_on/scripthelper/scripthelper.h>
#include <angelscript/add_on/scriptmath/scriptmath.h>

#include <VolcaniCore/Core/Input.h>

#include <Magma/Core/AssetManager.h>

#include <Magma/Scene/Scene.h>
#include <Magma/Scene/Component.h>
#include <Magma/Scene/PhysicsSystem.h>
#include <Magma/Scene/ScriptSystem.h>

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
using namespace Magma::Physics;

namespace Lava {

static void RegisterGlobalFunctions();
static void RegisterTypes();
static void RegisterEvents();
static void RegisterAssetManager();
static void RegisterECS();
static void RegisterScene();
static void RegisterUI();

void ScriptGlue::RegisterInterface() {
	auto* engine = ScriptEngine::Get();

	RegisterStdString(engine);
	RegisterScriptHandle(engine);
	RegisterScriptMath(engine);

	RegisterEvents();
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
		// KeyPressed, KeyReleased
		.AddMethod("void OnKeyEvent(KeyEvent@)")
		// MousePressed, MouseReleased
		.AddMethod("void OnMouseEvent(MouseEvent@)")
		// MouseClicked, Collided
		.AddMethod("void OnPhysicsEvent(PhysicsEvent@)")
		// PlayerDied, LevelComplete, Collided
		.AddMethod("void OnGameEvent(GameEvent@)")
		;

	ScriptEngine::RegisterInterface("IUIObject")
		.AddMethod("void OnClick()")
		.AddMethod("void OnHover()")
		.AddMethod("void OnMouseUp()")
		.AddMethod("void OnMouseDown()");

	RegisterGlobalFunctions();
	RegisterTypes();
	RegisterAssetManager();
	RegisterECS();

	RegisterScene();
	RegisterUI();
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

static Vec3 NormalizeVec3(const Vec3& vec) {
	return glm::normalize(vec);
}
static Vec4 NormalizeVec4(const Vec4& vec) {
	return glm::normalize(vec);
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
		"void f(const int &in) { float, float, float }",
		asFUNCTION(Vec3ListConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectProperty("Vec3", "float x", asOFFSET(Vec3, x));
	engine->RegisterObjectProperty("Vec3", "float y", asOFFSET(Vec3, y));
	engine->RegisterObjectProperty("Vec3", "float z", asOFFSET(Vec3, z));

	engine->RegisterGlobalFunction("Vec3 normalize(const Vec3 &in)",
		asFUNCTION(NormalizeVec3), asCALL_CDECL);
}

static KeyPressedEvent* KeyPressedEventCast(KeyEvent* event) {
	if(event->Type != EventType::KeyPressed)
		return nullptr;
	return dynamic_cast<KeyPressedEvent*>(event);
}

static KeyReleasedEvent* KeyReleasedEventCast(KeyEvent* event) {
	if(event->Type != EventType::KeyReleased)
		return nullptr;
	return dynamic_cast<KeyReleasedEvent*>(event);
}

static KeyCharEvent* KeyCharacterEventCast(KeyEvent* event) {
	if(event->Type != EventType::KeyChar)
		return nullptr;
	return dynamic_cast<KeyCharEvent*>(event);
}

static MouseButtonPressedEvent* MousePressedEventCast(MouseEvent* event) {
	if(event->Type != EventType::MouseButtonPressed)
		return nullptr;
	return dynamic_cast<MouseButtonPressedEvent*>(event);
}

void RegisterEvents() {
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

	engine->RegisterEnumValue("Key", "Space", 32);
	engine->RegisterEnumValue("Key", "Ctrl", 224 + 230);
	engine->RegisterEnumValue("Key", "Shift", 225 + 229);

	engine->RegisterEnumValue("Key", "Right", 262);
	engine->RegisterEnumValue("Key", "Left",  263);
	engine->RegisterEnumValue("Key", "Down",  264);
	engine->RegisterEnumValue("Key", "Up",    265);

	engine->RegisterGlobalFunction("bool KeyPressed(Key key)",
		asFUNCTION(Input::KeyPressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool MousePressed(Mouse button)",
		asFUNCTION(Input::MouseButtonPressed), asCALL_CDECL);

	engine->RegisterEnum("EventType");
	engine->RegisterEnumValue("EventType", "KeyPressed",	0);
	engine->RegisterEnumValue("EventType", "KeyReleased",	1);
	engine->RegisterEnumValue("EventType", "KeyCharacter",	2);
	engine->RegisterEnumValue("EventType", "MouseMoved",	3);
	engine->RegisterEnumValue("EventType", "MouseScrolled", 4);
	engine->RegisterEnumValue("EventType", "MousePressed",	5);
	engine->RegisterEnumValue("EventType", "MouseReleased", 6);

	engine->RegisterObjectType("KeyPressedEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("KeyPressedEvent", "EventType Type",
		asOFFSET(KeyPressedEvent, Type));
	engine->RegisterObjectProperty("KeyPressedEvent", "const Key Key",
		asOFFSET(KeyPressedEvent, Key));
	engine->RegisterObjectProperty("KeyPressedEvent", "const bool IsRepeat",
		asOFFSET(KeyPressedEvent, IsRepeat));

	engine->RegisterObjectType("KeyReleasedEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("KeyReleasedEvent", "EventType Type",
		asOFFSET(KeyReleasedEvent, Type));
	engine->RegisterObjectProperty("KeyReleasedEvent", "const Key Key",
		asOFFSET(KeyReleasedEvent, Key));

	engine->RegisterObjectType("KeyCharacterEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("KeyCharacterEvent", "EventType Type",
		asOFFSET(KeyCharEvent, Type));
	engine->RegisterObjectProperty("KeyCharacterEvent", "const Key Key",
		asOFFSET(KeyCharEvent, Key));
	engine->RegisterObjectMethod("KeyCharacterEvent", "string get_Char() const property",
		asMETHOD(KeyCharEvent, ToString), asCALL_THISCALL);

	engine->RegisterObjectType("KeyEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("KeyEvent", "EventType Type",
		asOFFSET(KeyEvent, Type));
	engine->RegisterObjectProperty("KeyEvent", "const Key Key",
		asOFFSET(KeyEvent, Key));
	engine->RegisterObjectMethod("KeyEvent", "KeyPressedEvent@ opCast()",
		asFUNCTION(KeyPressedEventCast), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("KeyEvent", "KeyReleasedEvent@ opCast()",
		asFUNCTION(KeyReleasedEventCast), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("KeyEvent", "KeyCharacterEvent@ opCast()",
		asFUNCTION(KeyCharacterEventCast), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("MouseEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);

	engine->RegisterObjectType("PhysicsEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	// engine->RegisterObjectProperty("PhysicsEvent", "PhysicsEventType Type")

	engine->RegisterObjectType("GameEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	// engine->RegisterObjectProperty("GameEvent", "string ID",
	// 	asOFFSET(GameEvent, ID));
}

static uint64_t GetAssetID(Asset* asset) {
	return (uint64_t)asset->ID;
}

static AssetType GetAssetType(Asset* asset) {
	return asset->Type;
}

static void AssetDefaultCtor(Asset* ptr) {
	new (ptr) Asset{ };
}

static void AssetInitCtor(uint64_t id, AssetType type, Asset* ptr) {
	new (ptr) Asset{ id, type };
}

static Sound* GetSound(Asset asset, AssetManager* manager) {
	manager->Load(asset);
	return manager->Get<Sound>(asset).get();
}

void RegisterAssetManager() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterObjectType("Asset", sizeof(Asset),
		asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLINTS
		| asGetTypeTraits<Asset>());
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

	engine->RegisterObjectMethod("Asset", "AssetType get_Type() const property",
		asFUNCTION(GetAssetType), asCALL_CDECL_OBJLAST);
	// engine->RegisterObjectMethod("Asset", "bool get_Primary() const property",
	// 	asFUNCTION(GetAssetPrimaryFlag), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Asset", asBEHAVE_CONSTRUCT,
		"void f()", asFUNCTION(AssetDefaultCtor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Asset", asBEHAVE_CONSTRUCT,
		"void f(uint64, AssetType)", asFUNCTION(AssetInitCtor), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("AssetManagerClass", 0,
		asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("AssetManagerClass", "bool IsLoaded(Asset) const",
		asMETHOD(AssetManager, IsLoaded), asCALL_THISCALL);
	engine->RegisterObjectMethod("AssetManagerClass", "bool Load(Asset)",
		asMETHOD(AssetManager, Load), asCALL_THISCALL);
	engine->RegisterObjectMethod("AssetManagerClass", "bool Unload(Asset)",
		asMETHOD(AssetManager, Unload), asCALL_THISCALL);
	// engine->RegisterObjectMethod("AssetManagerClass",
	// 	"Asset GetNamedAsset(const string &in)",
	// 	asMETHOD(AssetManager, GetNamedAsset), asCALL_THISCALL);

	engine->RegisterObjectType("Sound", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectMethod("Sound", "void Play()", asMETHOD(Sound, Play),
		asCALL_THISCALL);

	engine->RegisterObjectMethod("AssetManagerClass",
		"Sound@ GetSound(Asset)", asFUNCTION(GetSound), asCALL_CDECL_OBJLAST);
}

static Vec3 GetCameraPosition(CameraComponent* cc) {
	return cc->Cam->GetPosition();
}

static void SetCameraPosition(const Vec3& vec, CameraComponent* cc) {
	cc->Cam->SetPosition(vec);
}

static Vec3 GetCameraDirection(CameraComponent* cc) {
	return cc->Cam->GetDirection();
}

static void SetCameraDirection(const Vec3& vec, CameraComponent* cc) {
	cc->Cam->SetDirection(vec);
}

static uint32_t GetCameraWidth(CameraComponent* cc) {
	return cc->Cam->GetViewportWidth();
}

static void SetCameraWidth(uint32_t width, CameraComponent* cc) {
	cc->Cam->Resize(width, 0);
}

static uint32_t GetCameraHeight(CameraComponent* cc) {
	return cc->Cam->GetViewportHeight();
}

static void SetCameraHeight(uint32_t height, CameraComponent* cc) {
	cc->Cam->Resize(0, height);
}

static asIScriptObject* GetScriptInstance(ScriptComponent* sc) {
	return sc->Instance->GetHandle();
}

static Physics::RigidBody* GetRigidBody(RigidBodyComponent* rc) {
	return rc->Body.get();
}

void RegisterECS() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterObjectType("CameraComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectMethod("CameraComponent",
		"Vec3 get_Position() const property", asFUNCTION(GetCameraPosition),
		asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("CameraComponent",
		"void set_Position(const Vec3 &in) property",
		asFUNCTION(SetCameraPosition), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("CameraComponent",
		"Vec3 get_Direction() const property", asFUNCTION(GetCameraDirection),
		asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("CameraComponent",
		"void set_Direction(const Vec3 &in) property",
		asFUNCTION(SetCameraDirection), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("CameraComponent",
		"uint32 get_Width() const property", asFUNCTION(GetCameraWidth),
		asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("CameraComponent",
		"void set_Width(uint32) property", asFUNCTION(SetCameraWidth),
		asCALL_CDECL_OBJLAST);	
	engine->RegisterObjectMethod("CameraComponent",
		"uint32 get_Height() const property", asFUNCTION(GetCameraHeight),
		asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("CameraComponent",
		"void set_Height(uint32) property", asFUNCTION(SetCameraHeight),
		asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("TagComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("TagComponent", "string Tag",
		asOFFSET(TagComponent, Tag));

	engine->RegisterObjectType("TransformComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("TransformComponent", "Vec3 Translation",
		asOFFSET(TransformComponent, Translation));
	engine->RegisterObjectProperty("TransformComponent", "Vec3 Rotation",
		asOFFSET(TransformComponent, Rotation));
	engine->RegisterObjectProperty("TransformComponent", "Vec3 Scale",
		asOFFSET(TransformComponent, Scale));

	engine->RegisterObjectType("AudioComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("AudioComponent", "Asset AudioAsset",
		asOFFSET(AudioComponent, AudioAsset));

	engine->RegisterObjectType("MeshComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("MeshComponent", "Asset MeshAsset",
		asOFFSET(MeshComponent, MeshAsset));

	engine->RegisterObjectType("SkyboxComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("SkyboxComponent", "Asset CubemapAsset",
		asOFFSET(SkyboxComponent, CubemapAsset));

	engine->RegisterObjectType("ScriptComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("ScriptComponent", "Asset ModuleAsset",
		asOFFSET(ScriptComponent, ModuleAsset));
	engine->RegisterObjectMethod("ScriptComponent", "IEntity@ get_Instance()",
		asFUNCTION(GetScriptInstance), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("RigidBodyComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);

	engine->RegisterObjectType("RigidBody", 0, asOBJ_REF | asOBJ_NOCOUNT);

	engine->RegisterObjectType("Transform", sizeof(Transform),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Transform>());
	engine->RegisterObjectProperty("Transform", "Vec3 Translation",
		asOFFSET(Transform, Translation));
	engine->RegisterObjectProperty("Transform", "Vec3 Rotation",
		asOFFSET(Transform, Rotation));
	engine->RegisterObjectProperty("Transform", "Vec3 Scale",
		asOFFSET(Transform, Scale));

	engine->RegisterObjectMethod("RigidBody",
		"Transform get_PhysicsTransform() const property",
		asMETHOD(Physics::RigidBody, GetTransform), asCALL_THISCALL);
	// engine->RegisterObjectMethod("RigidBody", "void ApplyForce(Vec3 force)",
	// 	asMETHOD(Physics::DynamicBody::ApplyForce));

	engine->RegisterObjectMethod("RigidBodyComponent", "RigidBody@ get_Body()",
		asFUNCTION(GetRigidBody), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("DirectionalLightComponent", 0,
		asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectType("PointLightComponent", 0,
		asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectType("SpotlightComponent", 0,
		asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectType("ParticleSystemComponent", 0,
		asOBJ_REF | asOBJ_NOCOUNT);

	engine->RegisterObjectType("Entity", sizeof(Entity),
		asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLINTS |
		asGetTypeTraits<Entity>());
	engine->RegisterObjectMethod("Entity", "string get_Name() const property",
		asMETHOD(Entity, GetName), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool get_Alive() const property",
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
		"const CameraComponent@ GetCameraComponent()",
		asMETHODPR(Entity, Get<CameraComponent>, () const,
			const CameraComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const TagComponent@ GetTagComponent()",
		asMETHODPR(Entity, Get<TagComponent>, () const,
			const TagComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const TransformComponent@ GetTransformComponent()",
		asMETHODPR(Entity, Get<TransformComponent>, () const,
			const TransformComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const AudioComponent@ GetAudioComponent()",
		asMETHODPR(Entity, Get<AudioComponent>, () const,
			const AudioComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const MeshComponent@ GetMeshComponent()",
		asMETHODPR(Entity, Get<MeshComponent>, () const,
			const MeshComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const SkyboxComponent@ GetSkyboxComponent()",
		asMETHODPR(Entity, Get<SkyboxComponent>, () const,
			const SkyboxComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const ScriptComponent@ GetScriptComponent()",
		asMETHODPR(Entity, Get<ScriptComponent>, () const,
			const ScriptComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const RigidBodyComponent@ GetRigidBodyComponent()",
		asMETHODPR(Entity, Get<RigidBodyComponent>, () const,
			const RigidBodyComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const DirectionalLightComponent@ GetDirectionalLightComponent()",
		asMETHODPR(Entity, Get<DirectionalLightComponent>, () const,
			const DirectionalLightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const PointLightComponent@ GetPointLightComponent()",
		asMETHODPR(Entity, Get<PointLightComponent>, () const,
			const PointLightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const SpotlightComponent@ GetSpotlightComponent()",
		asMETHODPR(Entity, Get<SpotlightComponent>, () const,
			const SpotlightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"const ParticleSystemComponent@ GetParticleSystemComponent()",
		asMETHODPR(Entity, Get<ParticleSystemComponent>, () const,
			const ParticleSystemComponent&), asCALL_THISCALL);

	engine->RegisterObjectMethod("Entity",
		"CameraComponent@ SetCameraComponent()",
		asMETHODPR(Entity, Set<CameraComponent>, (),
			CameraComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"TagComponent@ SetTagComponent()",
		asMETHODPR(Entity, Set<TagComponent>, (),
			TagComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"TransformComponent@ SetTransformComponent()",
		asMETHODPR(Entity, Set<TransformComponent>, (),
			TransformComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"AudioComponent@ SetAudioComponent()",
		asMETHODPR(Entity, Set<AudioComponent>, (),
			AudioComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"MeshComponent@ SetMeshComponent()",
		asMETHODPR(Entity, Set<MeshComponent>, (),
			MeshComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"SkyboxComponent@ SetSkyboxComponent()",
		asMETHODPR(Entity, Set<SkyboxComponent>, (),
			SkyboxComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"ScriptComponent@ SetScriptComponent()",
		asMETHODPR(Entity, Set<ScriptComponent>, (),
			ScriptComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"RigidBodyComponent@ SetRigidBodyComponent()",
		asMETHODPR(Entity, Set<RigidBodyComponent>, (),
			RigidBodyComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"DirectionalLightComponent@ SetDirectionalLightComponent()",
		asMETHODPR(Entity, Set<DirectionalLightComponent>, (),
			DirectionalLightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"PointLightComponent@ SetPointLightComponent()",
		asMETHODPR(Entity, Set<PointLightComponent>, (),
			PointLightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"SpotlightComponent@ SetSpotlightComponent()",
		asMETHODPR(Entity, Set<SpotlightComponent>, (),
			SpotlightComponent&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity",
		"ParticleSystemComponent@ SetParticleSystemComponent()",
		asMETHODPR(Entity, Set<ParticleSystemComponent>, (),
			ParticleSystemComponent&), asCALL_THISCALL);
}

static HitInfo PhysicsRaycastScreen(uint32_t x, uint32_t y, PhysicsSystem* sys) {

	return HitInfo();
}

static HitInfo PhysicsRaycast(const Vec3& start, const Vec3& dir, PhysicsSystem* sys) {

	return HitInfo();
}

void RegisterScene() {
	auto* engine = ScriptEngine::Get();

	// engine->RegisterObjectType("HitInfo", sizeof(Physics::HitInfo),
	// 	asOBJ_VALUE | asGetTypeTraits<Physics::HitInfo>());
	// engine->RegisterObjectProperty("HitInfo", "const bool HasHit",
	// 	asOFFSET(Physics::HitInfo, HasHit));
	// engine->RegisterObjectProperty("HitInfo", "RigidBody@ Actor",
	// 	asOFFSET(Physics::HitInfo, Actor));

	engine->RegisterObjectType("PhysicsSystem", 0, asOBJ_REF | asOBJ_NOCOUNT);
	// engine->RegisterObjectMethod("PhysicsSystem",
	// 	"HitInfo Raycast(const Vec3 &in, const Vec3 &in)",
	// 	asFUNCTION(PhysicsRaycast), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("ScriptSystem", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectMethod("ScriptSystem",
		"void ListenForEvent(Entity, const string &in)",
		asMETHOD(ScriptSystem, Listen), asCALL_THISCALL);
	engine->RegisterObjectMethod("ScriptSystem",
		"void BroadcastEvent(const string &in)",
		asMETHOD(ScriptSystem, Broadcast), asCALL_THISCALL);

	engine->RegisterObjectType("SceneClass", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("SceneClass", "Entity FindEntity(const string &in)",
		asMETHODPR(ECS::World, GetEntity, (const std::string&), Entity),
		asCALL_THISCALL, 0, asOFFSET(Scene, EntityWorld));
	engine->RegisterObjectMethod("SceneClass", "Entity GetEntity(uint64)",
		asMETHODPR(ECS::World, GetEntity, (UUID), Entity), asCALL_THISCALL, 0,
		asOFFSET(Scene, EntityWorld));

	engine->RegisterObjectMethod("SceneClass", "PhysicsSystem@ GetPhysicsSystem()",
		asMETHODPR(ECS::World, Get<PhysicsSystem>, (), PhysicsSystem*),
		asCALL_THISCALL, 0, asOFFSET(Scene, EntityWorld));
	engine->RegisterObjectMethod("SceneClass", "ScriptSystem@ GetScriptSystem()",
		asMETHODPR(ECS::World, Get<ScriptSystem>, (), ScriptSystem*),
		asCALL_THISCALL, 0, asOFFSET(Scene, EntityWorld));
}

void RegisterUI() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterObjectType("UIElement", 0, asOBJ_REF | asOBJ_NOCOUNT);
	// engine->RegisterObjectProperty("UIElement", "");

	engine->RegisterObjectType("UIPageClass", 0, asOBJ_REF | asOBJ_NOHANDLE);
	// engine->RegisterObjectMethod("UIPageClass", "UIElement Get(const string &in)",
	// 	asMETHODPR(UIPage, Get, (const std::string&) const, UIElement*),
	// 	asCALL_THISCALL);
}

}