#include "test.h"

namespace UIObjects {

static Map<std::string, UIObject*> m_Objects;

extern "C" EXPORT UIObject* GetObject(const std::string& id) {
	if(!m_Objects.count(id)) { return nullptr; }
	return m_Objects[id];
}

extern "C" EXPORT void LoadObjects() {
	m_Objects["Button1"] = new Button1;
}

}
