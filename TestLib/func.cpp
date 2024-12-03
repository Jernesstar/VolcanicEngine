#include <VolcaniCore/Core/Defines.h>

#include "TestObject.h"

using namespace VolcaniCore;

namespace TestLib {

static Map<std::string, TestObject*> m_Objects;

extern "C" EXPORT void Load() {
	m_Objects["Object0"] = new TestObject();
	m_Objects["Object1"] = new InheritedA();
	m_Objects["Object2"] = new InheritedB();
}

extern "C" EXPORT TestObject* Get(const std::string& id) {
	return m_Objects[id];
}

}