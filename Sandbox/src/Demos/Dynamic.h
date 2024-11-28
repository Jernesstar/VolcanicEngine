#pragma once

#include <Magma/Core/DLL.h>

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

	auto func = m_DLL->GetFunction<uint32_t, uint32_t, uint32_t>("Add");

	VOLCANICORE_LOG_INFO("DLL Project Successful");
	if(func)
		VOLCANICORE_LOG_INFO("1 + 1 = %i", func(1, 1));
	else
		VOLCANICORE_LOG_INFO("We are unable to do math");
}

void DLL::OnUpdate(TimeStep ts) {

}

}