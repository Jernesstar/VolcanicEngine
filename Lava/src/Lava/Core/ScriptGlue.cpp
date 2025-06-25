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
#include <angelscript/add_on/scriptarray/scriptarray.h>

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

#include "Types/GridSet.h"
#include "Types/GridSet3D.h"
#include "Types/Timer.h"

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
	RegisterScriptArray(engine, true);

	RegisterEvents();
	ScriptEngine::RegisterInterface("IApp")
		.AddMethod("void OnLoad()")
		.AddMethod("void OnClose()")
		.AddMethod("void OnUpdate(float ts)");

	ScriptEngine::RegisterInterface("IScreen")
		.AddMethod("void OnLoad()")
		.AddMethod("void OnClose()")
		.AddMethod("void OnUpdate(float ts)");

	ScriptEngine::RegisterInterface("IEntityController")
		.AddMethod("void OnStart()")
		.AddMethod("void OnUpdate(float ts)")
		.AddMethod("void OnKeyEvent(KeyEvent@)")
		.AddMethod("void OnMouseEvent(MouseEvent@)")
		.AddMethod("void OnPhysicsEvent(PhysicsEvent@)")
		.AddMethod("void OnGameEvent(GameEvent@)");

	ScriptEngine::RegisterInterface("IUIController")
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

	GridSet::RegisterInterface();
	GridSet3D::RegisterInterface();
	Timer::RegisterInterface();
}

void ScriptGlue::Copy(Ref<ScriptObject> src, Ref<ScriptObject> dst) {
	if(src->GetClass()->Name != dst->GetClass()->Name)
		return;

	for(uint32_t i = 0; i < src->GetHandle()->GetPropertyCount(); i++) {
		ScriptField ours = dst->GetProperty(i);
		ScriptField field = src->GetProperty(i);
		if(!field.Data)
			continue;
		if(field.Is(ScriptQualifier::ScriptObject))
			continue;

		std::string fieldType;
		if(field.Type)
			fieldType = field.Type->GetName();

		size_t size = 0;
		if(field.TypeID > 0 && field.TypeID <= 11) // (Void, Double]
			size = ScriptEngine::Get()->GetSizeOfPrimitiveType(field.TypeID);
		else if(fieldType == "Asset")
			size = sizeof(Asset);
		else if(fieldType == "Vec3")
			size = sizeof(Vec3);

		if(size) {
			void* us = dst->GetHandle()->GetAddressOfProperty(i);
			void* them = src->GetHandle()->GetAddressOfProperty(i);
			memcpy(us, them, size);
			continue;
		}

		if(fieldType == "string")
			*ours.As<std::string>() = *field.As<std::string>();
		else if(fieldType == "array")
			*ours.As<CScriptArray>() = *field.As<CScriptArray>();
		else if(fieldType == "GridSet")
			*ours.As<Lava::GridSet>() = *field.As<Lava::GridSet>();
	}
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

static Vec3 AddVec3(const Vec3& v1, const Vec3& v2) {
	return v1 + v2;
}

static Vec3 SubVec3(const Vec3& v1, const Vec3& v2) {
	return v1 - v2;
}

static Vec3 NegateVec3(const Vec3& dest) {
	return -dest;
}

static Vec3 MultiplyVec3(float r, const Vec3& dest) {
	return r * dest;
}

static Vec3 FloatDivideVec3(float r, const Vec3& dest) {
	return r / dest;
}

static Vec3 FloatDividedByVec3(float r, const Vec3& dest) {
	return dest / r;
}

void RegisterTypes() {
	auto* engine = ScriptEngine::Get();

	// engine->SetDefaultNamespace("Math");
	engine->RegisterObjectType("Vec3", sizeof(Vec3),
		asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLFLOATS
		| asGetTypeTraits<Vec3>());
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,
		"void f()", asFUNCTION(Vec3DefaultConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,
		"void f(const Vec3 &in)", asFUNCTION(Vec3CopyConstructor),
		asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,
		"void f(float)", asFUNCTION(Vec3ConvConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT,
		"void f(float, float, float)", asFUNCTION(Vec3InitConstructor),
		asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Vec3", asBEHAVE_LIST_CONSTRUCT,
		"void f(const int &in) { float, float, float }",
		asFUNCTION(Vec3ListConstructor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectProperty("Vec3", "float x", asOFFSET(Vec3, x));
	engine->RegisterObjectProperty("Vec3", "float y", asOFFSET(Vec3, y));
	engine->RegisterObjectProperty("Vec3", "float z", asOFFSET(Vec3, z));

	engine->RegisterObjectMethod("Vec3", "Vec3 &opAddAssign(const Vec3 &in)",
		asMETHODPR(Vec3, operator+=, (const Vec3 &), Vec3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vec3", "Vec3 &opSubAssign(const Vec3 &in)",
		asMETHODPR(Vec3, operator-=, (const Vec3 &), Vec3&), asCALL_THISCALL);
	engine->RegisterObjectMethod("Vec3", "Vec3 opAdd(const Vec3 &in) const",
		asFUNCTION(AddVec3), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vec3", "Vec3 opSub(const Vec3 &in) const",
		asFUNCTION(SubVec3), asCALL_CDECL_OBJFIRST);
	engine->RegisterObjectMethod("Vec3", "Vec3 opNeg() const",
		asFUNCTION(NegateVec3), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Vec3", "Vec3 opMul(float r) const",
		asFUNCTION(MultiplyVec3), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Vec3", "Vec3 opMul_r(float r) const",
		asFUNCTION(MultiplyVec3), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Vec3", "Vec3 opDiv(float r) const",
		asFUNCTION(FloatDivideVec3), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Vec3", "Vec3 opDiv_r(float r) const",
		asFUNCTION(FloatDividedByVec3), asCALL_CDECL_OBJLAST);

	engine->RegisterGlobalFunction("float radians(float deg)",
		asFUNCTIONPR(glm::radians, (float), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("Vec3 radians(const Vec3 &in)",
		asFUNCTIONPR(glm::radians, (const Vec3&), Vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("float degrees(float deg)",
		asFUNCTIONPR(glm::degrees, (float), float), asCALL_CDECL);
	engine->RegisterGlobalFunction("Vec3 degrees(const Vec3 &in)",
		asFUNCTIONPR(glm::degrees, (const Vec3&), Vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("Vec3 sin(const Vec3 &in)",
		asFUNCTIONPR(glm::sin, (const Vec3&), Vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("Vec3 cos(const Vec3 &in)",
		asFUNCTIONPR(glm::cos, (const Vec3&), Vec3), asCALL_CDECL);
	engine->RegisterGlobalFunction("Vec3 normalize(const Vec3 &in)",
		asFUNCTIONPR(glm::normalize, (const Vec3&), Vec3), asCALL_CDECL);

	// engine->SetDefaultNamespace("");
}

static KeyPressedEvent* KeyPressedEventCast(KeyEvent* event) {
	if(event->Type != EventType::KeyPressed)
		return nullptr;
	return static_cast<KeyPressedEvent*>(event);
}

static KeyReleasedEvent* KeyReleasedEventCast(KeyEvent* event) {
	if(event->Type != EventType::KeyReleased)
		return nullptr;
	return static_cast<KeyReleasedEvent*>(event);
}

static KeyCharEvent* KeyCharacterEventCast(KeyEvent* event) {
	if(event->Type != EventType::KeyChar)
		return nullptr;
	return static_cast<KeyCharEvent*>(event);
}

static MouseButtonPressedEvent* MousePressedEventCast(MouseEvent* event) {
	if(event->Type != EventType::MouseButtonPressed)
		return nullptr;
	return static_cast<MouseButtonPressedEvent*>(event);
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
	engine->RegisterEnumValue("Key", "Enter", 257);

	engine->RegisterEnumValue("Key", "Right", 262);
	engine->RegisterEnumValue("Key", "Left",  263);
	engine->RegisterEnumValue("Key", "Down",  264);
	engine->RegisterEnumValue("Key", "Up",    265);

	engine->SetDefaultNamespace("Input");
	engine->RegisterGlobalFunction("bool KeyPressed(Key key)",
		asFUNCTION(Input::KeyPressed), asCALL_CDECL);
	engine->RegisterGlobalFunction("bool MousePressed(Mouse button)",
		asFUNCTION(Input::MouseButtonPressed), asCALL_CDECL);
	engine->SetDefaultNamespace("");

	engine->RegisterEnum("EventType");
	engine->RegisterEnumValue("EventType", "KeyPressed",	0);
	engine->RegisterEnumValue("EventType", "KeyReleased",	1);
	engine->RegisterEnumValue("EventType", "KeyCharacter",	2);
	engine->RegisterEnumValue("EventType", "MouseMoved",	3);
	engine->RegisterEnumValue("EventType", "MouseScrolled", 4);
	engine->RegisterEnumValue("EventType", "MousePressed",	5);
	engine->RegisterEnumValue("EventType", "MouseReleased", 6);

	engine->RegisterObjectType("KeyEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("KeyEvent", "const EventType Type",
		asOFFSET(KeyEvent, Type));
	engine->RegisterObjectProperty("KeyEvent", "const Key Key",
		asOFFSET(KeyEvent, Key));

	engine->RegisterObjectType("KeyPressedEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("KeyPressedEvent", "const EventType Type",
		asOFFSET(KeyPressedEvent, Type));
	engine->RegisterObjectProperty("KeyPressedEvent", "const Key Key",
		asOFFSET(KeyPressedEvent, Key));
	engine->RegisterObjectProperty("KeyPressedEvent", "const bool IsRepeat",
		asOFFSET(KeyPressedEvent, IsRepeat));

	engine->RegisterObjectType("KeyReleasedEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("KeyReleasedEvent", "const EventType Type",
		asOFFSET(KeyReleasedEvent, Type));
	engine->RegisterObjectProperty("KeyReleasedEvent", "const Key Key",
		asOFFSET(KeyReleasedEvent, Key));

	engine->RegisterObjectType("KeyCharacterEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("KeyCharacterEvent", "const EventType Type",
		asOFFSET(KeyCharEvent, Type));
	engine->RegisterObjectProperty("KeyCharacterEvent", "const Key Key",
		asOFFSET(KeyCharEvent, Key));
	engine->RegisterObjectMethod("KeyCharacterEvent", "string get_Char() const property",
		asMETHOD(KeyCharEvent, ToString), asCALL_THISCALL);

	engine->RegisterObjectMethod("KeyEvent", "KeyPressedEvent@ opCast()",
		asFUNCTION(KeyPressedEventCast), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("KeyEvent", "KeyReleasedEvent@ opCast()",
		asFUNCTION(KeyReleasedEventCast), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("KeyEvent", "KeyCharacterEvent@ opCast()",
		asFUNCTION(KeyCharacterEventCast), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("MouseEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	// engine->RegisterObjectProperty("MouseEvent", "const EventType Type",
	// 	asOFFSET(MouseEvent, Type));

	// engine->RegisterObjectType("MousePressedEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	// engine->RegisterObjectProperty("MousePressedEvent", "const EventType Type",
	// 	asOFFSET(MouseButtonPressedEvent, Type));
	// engine->RegisterObjectProperty("MousePressedEvent", "const Mouse Button",
	// 	asOFFSET(MouseButtonPressedEvent, Button));

	engine->RegisterObjectType("PhysicsEvent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	// engine->RegisterObjectProperty("PhysicsEvent", "PhysicsEventType Type")

	// TODO: Replace with class
	ScriptEngine::RegisterInterface("GameEvent")
		.AddMethod("string GetID() const");
}

static void AssetDefaultCtor(Asset* ptr) {
	new (ptr) Asset{ };
}

static void AssetInitCtor(uint64_t id, AssetType type, Asset* ptr) {
	new (ptr) Asset{ id, type };
}

static uint64_t GetAssetID(Asset* asset) {
	return (uint64_t)asset->ID;
}

static bool AssetIsValid(Asset* asset) {
	return AssetManager::Get()->IsValid(*asset);
}

static bool AssetIsLoaded(Asset* asset) {
	return AssetManager::Get()->IsLoaded(*asset);
}

static std::string AssetGetName(Asset* asset) {
	return AssetManager::Get()->GetAssetName(*asset);
}

static Sound* GetSound(Asset asset, AssetManager* manager) {
	manager->Load(asset);
	return manager->Get<Sound>(asset).get();
}

static void PlaySound(Sound* sound) {
	sound->Play();
}

void RegisterAssetManager() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterEnum("AssetType");
	engine->RegisterEnumValue("AssetType", "Mesh",	  0);
	engine->RegisterEnumValue("AssetType", "Texture", 1);
	engine->RegisterEnumValue("AssetType", "Cubemap", 2);
	engine->RegisterEnumValue("AssetType", "Font",	  3);
	engine->RegisterEnumValue("AssetType", "Audio",	  4);
	engine->RegisterEnumValue("AssetType", "Script",  5);
	engine->RegisterEnumValue("AssetType", "Shader",  6);
	engine->RegisterEnumValue("AssetType", "None",	  7);

	engine->RegisterObjectType("Asset", sizeof(Asset),
		asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<Asset>());
	engine->RegisterObjectBehaviour("Asset", asBEHAVE_CONSTRUCT,
		"void f()", asFUNCTION(AssetDefaultCtor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Asset", asBEHAVE_CONSTRUCT,
		"void f(uint64, AssetType)", asFUNCTION(AssetInitCtor),
		asCALL_CDECL_OBJLAST);

	engine->RegisterObjectProperty("Asset", "const AssetType Type",
		asOFFSET(Asset, Type));
	engine->RegisterObjectProperty("Asset", "const bool Primary",
		asOFFSET(Asset, Primary));
	engine->RegisterObjectMethod("Asset", "uint64 get_ID() const property",
		asFUNCTION(GetAssetID), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Asset", "bool get_IsValid() const property",
		asFUNCTION(AssetIsValid), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Asset", "bool get_IsLoaded() const property",
		asFUNCTION(AssetIsLoaded), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Asset", "string get_Name() const property",
		asFUNCTION(AssetGetName), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("AssetManagerClass", 0,
		asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("AssetManagerClass", "bool Load(Asset)",
		asMETHOD(AssetManager, Load), asCALL_THISCALL);
	engine->RegisterObjectMethod("AssetManagerClass", "bool Unload(Asset)",
		asMETHOD(AssetManager, Unload), asCALL_THISCALL);
	engine->RegisterObjectMethod("AssetManagerClass",
		"Asset GetNamedAsset(const string &in)",
		asMETHOD(AssetManager, GetNamedAsset), asCALL_THISCALL);
	engine->RegisterObjectMethod("AssetManagerClass",
		"Asset GetNativeAsset(const string &in)",
		asMETHOD(AssetManager, GetNativeAsset), asCALL_THISCALL);

	engine->RegisterObjectType("Sound", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectMethod("Sound", "void Play(float volume)",
		asMETHOD(Sound, Play), asCALL_THISCALL);
	engine->RegisterObjectMethod("Sound", "void Play()",
		asFUNCTION(PlaySound), asCALL_CDECL_OBJLAST);
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
	sc->Instance->GetHandle()->AddRef();
	return sc->Instance->GetHandle();
}

static Physics::RigidBody* GetRigidBody(RigidBodyComponent* rc) {
	return rc->Body.get();
}

static void RigidBodyApplyForce(Vec3 force, RigidBody* body) {
	if(body->GetType() != RigidBody::Type::Dynamic)
		return;

	body->As<Physics::DynamicBody>()->ApplyForce(force);
}

static CameraComponent* AddCameraComponent(Entity* entity) {
	entity->Add<CameraComponent>();
	return &entity->Set<CameraComponent>();
}

static TagComponent* AddTagComponent(Entity* entity) {
	entity->Add<TagComponent>();
	return &entity->Set<TagComponent>();
}

static TransformComponent* AddTransformComponent(Entity* entity) {
	entity->Add<TransformComponent>();
	return &entity->Set<TransformComponent>();
}

static AudioComponent* AddAudioComponent(Entity* entity) {
	entity->Add<AudioComponent>();
	return &entity->Set<AudioComponent>();
}

static MeshComponent* AddMeshComponent(Entity* entity) {
	entity->Add<MeshComponent>();
	return &entity->Set<MeshComponent>();
}

static SkyboxComponent* AddSkyboxComponent(Entity* entity) {
	entity->Add<SkyboxComponent>();
	return &entity->Set<SkyboxComponent>();
}

static ScriptComponent* AddScriptComponent(Entity* entity) {
	entity->Add<ScriptComponent>();
	return &entity->Set<ScriptComponent>();
}

static RigidBodyComponent* AddRigidBodyComponent(Entity* entity) {
	entity->Add<RigidBodyComponent>();
	return &entity->Set<RigidBodyComponent>();
}

static DirectionalLightComponent* AddDirectionalLightComponent(Entity* entity) {
	entity->Add<DirectionalLightComponent>();
	return &entity->Set<DirectionalLightComponent>();
}

static PointLightComponent* AddPointLightComponent(Entity* entity) {
	entity->Add<PointLightComponent>();
	return &entity->Set<PointLightComponent>();
}

static SpotlightComponent* AddSpotlightComponent(Entity* entity) {
	entity->Add<SpotlightComponent>();
	return &entity->Set<SpotlightComponent>();
}

static ParticleEmitterComponent* AddParticleEmitterComponent(Entity* entity) {
	entity->Add<ParticleEmitterComponent>();
	return &entity->Set<ParticleEmitterComponent>();
}

static const CameraComponent* GetCameraComponent(const Entity* entity) {
	return &entity->Get<CameraComponent>();
}

static const TagComponent* GetTagComponent(const Entity* entity) {
	return &entity->Get<TagComponent>();
}

static const TransformComponent* GetTransformComponent(const Entity* entity) {
	return &entity->Get<TransformComponent>();
}

static const AudioComponent* GetAudioComponent(const Entity* entity) {
	return &entity->Get<AudioComponent>();
}

static const MeshComponent* GetMeshComponent(const Entity* entity) {
	return &entity->Get<MeshComponent>();
}

static const SkyboxComponent* GetSkyboxComponent(const Entity* entity) {
	return &entity->Get<SkyboxComponent>();
}

static const ScriptComponent* GetScriptComponent(const Entity* entity) {
	return &entity->Get<ScriptComponent>();
}

static const RigidBodyComponent* GetRigidBodyComponent(const Entity* entity) {
	return &entity->Get<RigidBodyComponent>();
}

static const DirectionalLightComponent* GetDirectionalLightComponent(const Entity* entity) {
	return &entity->Get<DirectionalLightComponent>();
}

static const PointLightComponent* GetPointLightComponent(const Entity* entity) {
	return &entity->Get<PointLightComponent>();
}

static const SpotlightComponent* GetSpotlightComponent(const Entity* entity) {
	return &entity->Get<SpotlightComponent>();
}

static const ParticleEmitterComponent* GetParticleEmitterComponent(const Entity* entity) {
	return &entity->Get<ParticleEmitterComponent>();
}

static CameraComponent* SetCameraComponent(Entity* entity) {
	return &entity->Set<CameraComponent>();
}

static TagComponent* SetTagComponent(Entity* entity) {
	return &entity->Set<TagComponent>();
}

static TransformComponent* SetTransformComponent(Entity* entity) {
	return &entity->Set<TransformComponent>();
}

static AudioComponent* SetAudioComponent(Entity* entity) {
	return &entity->Set<AudioComponent>();
}

static MeshComponent* SetMeshComponent(Entity* entity) {
	return &entity->Set<MeshComponent>();
}

static SkyboxComponent* SetSkyboxComponent(Entity* entity) {
	return &entity->Set<SkyboxComponent>();
}

static ScriptComponent* SetScriptComponent(Entity* entity) {
	return &entity->Set<ScriptComponent>();
}

static RigidBodyComponent* SetRigidBodyComponent(Entity* entity) {
	return &entity->Set<RigidBodyComponent>();
}

static DirectionalLightComponent* SetDirectionalLightComponent(Entity* entity) {
	return &entity->Set<DirectionalLightComponent>();
}

static PointLightComponent* SetPointLightComponent(Entity* entity) {
	return &entity->Set<PointLightComponent>();
}

static SpotlightComponent* SetSpotlightComponent(Entity* entity) {
	return &entity->Set<SpotlightComponent>();
}

static ParticleEmitterComponent* SetParticleEmitterComponent(Entity* entity) {
	return &entity->Set<ParticleEmitterComponent>();
}

static void SetParticleEmitterMaxParticles(uint32_t maxCount,
	ParticleEmitterComponent* comp)
{
	comp->MaxParticleCount = maxCount;

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
	engine->RegisterObjectProperty("MeshComponent", "Asset MeshSourceAsset",
		asOFFSET(MeshComponent, MeshSourceAsset));
	engine->RegisterObjectProperty("MeshComponent", "Asset MaterialAsset",
		asOFFSET(MeshComponent, MaterialAsset));

	engine->RegisterObjectType("SkyboxComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("SkyboxComponent", "Asset CubemapAsset",
		asOFFSET(SkyboxComponent, CubemapAsset));

	engine->RegisterObjectType("ScriptComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("ScriptComponent", "Asset ModuleAsset",
		asOFFSET(ScriptComponent, ModuleAsset));
	engine->RegisterObjectMethod("ScriptComponent",
		"IEntityController@ get_Instance() property",
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
		"Transform GetTransform() const",
		asMETHOD(Physics::RigidBody, GetTransform), asCALL_THISCALL);
	engine->RegisterObjectMethod("RigidBody", "void ApplyForce(const Vec3 &in)",
		asFUNCTION(RigidBodyApplyForce), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("RigidBodyComponent",
		"RigidBody@ get_Body() property", asFUNCTION(GetRigidBody),
		asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("DirectionalLightComponent", 0,
		asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("DirectionalLightComponent", "Vec3 Ambient",
		asOFFSET(DirectionalLightComponent, Ambient));
	engine->RegisterObjectProperty("DirectionalLightComponent", "Vec3 Diffuse",
		asOFFSET(DirectionalLightComponent, Diffuse));
	engine->RegisterObjectProperty("DirectionalLightComponent", "Vec3 Specular",
		asOFFSET(DirectionalLightComponent, Specular));
	engine->RegisterObjectProperty("DirectionalLightComponent", "Vec3 Position",
		asOFFSET(DirectionalLightComponent, Position));
	engine->RegisterObjectProperty("DirectionalLightComponent", "Vec3 Direction",
		asOFFSET(DirectionalLightComponent, Direction));

	engine->RegisterObjectType("PointLightComponent", 0,
		asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectType("SpotlightComponent", 0,
		asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectType("ParticleEmitterComponent", 0,
		asOBJ_REF | asOBJ_NOCOUNT);
	engine->RegisterObjectProperty("ParticleEmitterComponent",
		"uint32 MaxParticles",
		asOFFSET(ParticleEmitterComponent, MaxParticleCount), asCALL_THISCALL);
	engine->RegisterObjectMethod("ParticleEmitterComponent",
		"void set_MaxParticles(uint32) property",
		asFUNCTION(SetParticleEmitterMaxParticles), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectType("Entity", sizeof(Entity),
		asOBJ_VALUE | asOBJ_POD | asOBJ_APP_CLASS_ALLINTS |
		asGetTypeTraits<Entity>());
	engine->RegisterObjectMethod("Entity", "string get_Name() const property",
		asMETHOD(Entity, GetName), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "void set_Name(const string &in) property",
		asMETHOD(Entity, SetName), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool get_IsAlive() const property",
		asMETHOD(Entity, IsAlive), asCALL_THISCALL);
	engine->RegisterObjectMethod("Entity", "bool get_IsValid() const property",
		asMETHOD(Entity, IsValid), asCALL_THISCALL);
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
	engine->RegisterObjectMethod("Entity", "bool HasParticleEmitterComponent() const",
		asMETHODPR(Entity, Has<ParticleEmitterComponent>, () const, bool),
		asCALL_THISCALL);

	engine->RegisterObjectMethod("Entity",
		"CameraComponent@ AddCameraComponent()",
		asFUNCTION(AddCameraComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"TagComponent@ AddTagComponent()",
		asFUNCTION(AddTagComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"TransformComponent@ AddTransformComponent()",
		asFUNCTION(AddTransformComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"AudioComponent@ AddAudioComponent()",
		asFUNCTION(AddAudioComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"MeshComponent@ AddMeshComponent()",
		asFUNCTION(AddMeshComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"SkyboxComponent@ AddSkyboxComponent()",
		asFUNCTION(AddSkyboxComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"ScriptComponent@ AddScriptComponent()",
		asFUNCTION(AddScriptComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"RigidBodyComponent@ AddRigidBodyComponent()",
		asFUNCTION(AddRigidBodyComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"DirectionalLightComponent@ AddDirectionalLightComponent()",
		asFUNCTION(AddDirectionalLightComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"PointLightComponent@ AddPointLightComponent()",
		asFUNCTION(AddPointLightComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"SpotlightComponent@ AddSpotlightComponent()",
		asFUNCTION(AddSpotlightComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"ParticleEmitterComponent@ AddParticleEmitterComponent()",
		asFUNCTION(AddParticleEmitterComponent), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("Entity",
		"const CameraComponent@ GetCameraComponent() const",
		asFUNCTION(GetCameraComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const TagComponent@ GetTagComponent() const",
		asFUNCTION(GetTagComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const TransformComponent@ GetTransformComponent() const",
		asFUNCTION(GetTransformComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const AudioComponent@ GetAudioComponent() const",
		asFUNCTION(GetAudioComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const MeshComponent@ GetMeshComponent() const",
		asFUNCTION(GetMeshComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const SkyboxComponent@ GetSkyboxComponent() const",
		asFUNCTION(GetSkyboxComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const ScriptComponent@ GetScriptComponent() const",
		asFUNCTION(GetScriptComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const RigidBodyComponent@ GetRigidBodyComponent() const",
		asFUNCTION(GetRigidBodyComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const DirectionalLightComponent@ GetDirectionalLightComponent() const",
		asFUNCTION(GetDirectionalLightComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const PointLightComponent@ GetPointLightComponent() const",
		asFUNCTION(GetPointLightComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const SpotlightComponent@ GetSpotlightComponent() const",
		asFUNCTION(GetSpotlightComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"const ParticleEmitterComponent@ GetParticleEmitterComponent() const",
		asFUNCTION(GetParticleEmitterComponent), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("Entity",
		"CameraComponent@ SetCameraComponent()",
		asFUNCTION(SetCameraComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"TagComponent@ SetTagComponent()",
		asFUNCTION(SetTagComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"TransformComponent@ SetTransformComponent()",
		asFUNCTION(SetTransformComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"AudioComponent@ SetAudioComponent()",
		asFUNCTION(SetAudioComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"MeshComponent@ SetMeshComponent()",
		asFUNCTION(SetMeshComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"SkyboxComponent@ SetSkyboxComponent()",
		asFUNCTION(SetSkyboxComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"ScriptComponent@ SetScriptComponent()",
		asFUNCTION(SetScriptComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"RigidBodyComponent@ SetRigidBodyComponent()",
		asFUNCTION(SetRigidBodyComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"DirectionalLightComponent@ SetDirectionalLightComponent()",
		asFUNCTION(SetDirectionalLightComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"PointLightComponent@ SetPointLightComponent()",
		asFUNCTION(SetPointLightComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"SpotlightComponent@ SetSpotlightComponent()",
		asFUNCTION(SetSpotlightComponent), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("Entity",
		"ParticleEmitterComponent@ SetParticleEmitterComponent()",
		asFUNCTION(SetParticleEmitterComponent), asCALL_CDECL_OBJLAST);
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
		"void BroadcastEvent(Entity, GameEvent@)",
		asMETHOD(ScriptSystem, Broadcast), asCALL_THISCALL);

	engine->RegisterObjectType("SceneClass", 0, asOBJ_REF | asOBJ_NOHANDLE);
	engine->RegisterObjectMethod("SceneClass", "Entity NewEntity()",
		asMETHODPR(ECS::World, AddEntity, (), Entity),
		asCALL_THISCALL, 0, asOFFSET(Scene, EntityWorld));
	engine->RegisterObjectMethod("SceneClass", "Entity NewEntity(const string &in)",
		asMETHODPR(ECS::World, AddEntity, (const std::string&), Entity),
		asCALL_THISCALL, 0, asOFFSET(Scene, EntityWorld));
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
	engine->RegisterObjectMethod("UIPageClass", "void ResetLayers()",
		asMETHOD(UIPage, SetLayer), asCALL_THISCALL);
	engine->RegisterObjectMethod("UIPageClass", "void SetLayer(const string &in)",
		asMETHOD(UIPage, SetLayer), asCALL_THISCALL);
	engine->RegisterObjectMethod("UIPageClass", "void PushLayer(const string &in)",
		asMETHOD(UIPage, PushLayer), asCALL_THISCALL);
	engine->RegisterObjectMethod("UIPageClass", "void PopLayer()",
		asMETHOD(UIPage, PopLayer), asCALL_THISCALL);
	// engine->RegisterObjectMethod("UIPageClass", "UIElement Get(const string &in)",
	// 	asMETHODPR(UIPage, Get, (const std::string&) const, UIElement*),
	// 	asCALL_THISCALL);
}

}