#include "ScriptObject.h"

namespace Magma::Script {

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
	if(m_Handle) {
		m_Handle->Release();
		m_Handle = nullptr;
	}

	Release();
}

}