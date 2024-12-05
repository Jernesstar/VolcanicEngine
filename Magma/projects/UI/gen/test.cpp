#include "test.h"

namespace UIElements {

static Map<std::string, UIClickable*> m_Elements;

extern "C" EXPORT UIClickable* GetElement(const std::string& id) {
	return m_Elements[id];
}

extern "C" EXPORT void LoadElements() {
	m_Elements["Button1"] = new Button1;
}

}
