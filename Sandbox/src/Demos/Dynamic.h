#pragma once

#include <Magma/Core/DLL.h>

#include <TestLib/TestObject.h>

using namespace TestLib;

namespace Demo {

class DLL : public Application {
public:
	DLL();

	void OnUpdate(TimeStep ts);

private:
	Ref<Magma::DLL> m_DLL;
};

DLL::DLL() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});

	m_DLL = CreateRef<Magma::DLL>("build\\TestLib\\lib\\TestLib.dll");

	auto load = m_DLL->GetFunction<void>("Load");
	auto get = m_DLL->GetFunction<TestObject*, std::string>("Load");

	VOLCANICORE_LOG_INFO("DLL Project Successful");
	load();
	auto* object0 = get("Object0");
	auto* object1 = get("Object1");
	auto* object2 = get("Object2");
	// VOLCANICORE_LOG_INFO(object0->ToString().c_str());
	// VOLCANICORE_LOG_INFO(object1->ToString().c_str());
	// VOLCANICORE_LOG_INFO(object2->ToString().c_str());
}

void DLL::OnUpdate(TimeStep ts) {

}

}