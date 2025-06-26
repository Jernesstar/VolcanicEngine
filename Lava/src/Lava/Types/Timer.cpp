#include "Timer.h"

using namespace Magma::Script;

namespace Lava {

Timer::Timer(TimerType type, float time)
	: m_Type(type), m_Time(time)
{

}

Timer::~Timer() {
	SetCallback(nullptr);
}

void Timer::SetCallback(asIScriptFunction* callback) {
	if(m_Callback)
		m_Callback->Release();
	m_Callback = callback;
}

void Timer::Tick(float ts) {
	if(!m_Callback)
		return;

	m_Step += ts;
	if(m_Step < m_Time)
		return;

	m_Step = 0.0f;
	asIScriptContext* ctx = ScriptEngine::GetContext();
	// asIScriptObject* obj = ctx->GetThisPointer(0);
	// ScriptFunc func{ m_Callback, ctx, obj };
	// func.CallVoid(ts);

	if(m_Type == TimerType::Single)
		SetCallback(nullptr);
}

static void TimerCtor(TimerType type, float time, Timer* ptr) {
	new(ptr) Timer(type, time);
}

static void TimerDestruct(Timer* ptr) {
	ptr->~Timer();
}

void Timer::RegisterInterface() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterEnum("TimerType");
	engine->RegisterEnumValue("TimerType", "Single", 0);
	engine->RegisterEnumValue("TimerType", "Repeat", 1);

	engine->RegisterObjectType("Timer", sizeof(Timer),
		asOBJ_VALUE | asGetTypeTraits<Timer>());
	engine->RegisterObjectBehaviour("Timer", asBEHAVE_CONSTRUCT,
		"void f(TimerType, float)", asFUNCTION(TimerCtor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("Timer", asBEHAVE_DESTRUCT,
		"void f()", asFUNCTION(TimerDestruct), asCALL_CDECL_OBJLAST);

	engine->RegisterFuncdef("void TimerCallback(float ts)");
	engine->RegisterObjectMethod("Timer", "void SetCallback(TimerCallback @cb)",
		asMETHOD(Timer, SetCallback), asCALL_THISCALL);
	engine->RegisterObjectMethod("Timer", "void Tick(float ts)",
		asMETHOD(Timer, Tick), asCALL_THISCALL);
}

}