#pragma once

#include <Magma/Script/ScriptEngine.h>

namespace Lava {

enum class TimerType { Single, Repeat };

class Timer {
public:
	static void RegisterInterface();

public:
	Timer(TimerType type, float time);
	~Timer();

	void SetCallback(asIScriptFunction* callback);
	void Tick(float ts);

private:
	TimerType m_Type;
	float m_Time, m_Step;
	asIScriptFunction* m_Callback;

};

}