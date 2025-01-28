#include "ScriptObject.h"

namespace Magma {

ScriptObject::ScriptObject() {
	m_RefCount = 1;
}

ScriptObject::~ScriptObject() {
	if(m_Handle)
		m_Handle->Release();
}

uint32_t ScriptObject::AddRef() {
	return ++m_RefCount;
}

uint32_t ScriptObject::Release() {
	if(--m_RefCount == 0) {
		delete this;
		return 0;
	}

	return m_RefCount;
}

void ScriptObject::DestroyAndRelease() {
	// Since there might be other object's still referencing this one, we
	// cannot just delete it. Here we will release all other references that
	// this object holds, so it doesn't end up holding circular references.

	if(m_Handle) {
		m_Handle->Release();
		m_Handle = nullptr;
	}
	Release();
}

}