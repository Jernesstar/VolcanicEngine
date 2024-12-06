#include "test.h"

namespace UIObjects {

static Map<std::string, UIObject*> m_Objects;

extern "C" EXPORT UIObject* GetObject(const std::string& id) {
	return m_Objects[id];
}

extern "C" EXPORT void LoadObjects() {
	m_Elements["Button1"] = new Button1;
}

}
