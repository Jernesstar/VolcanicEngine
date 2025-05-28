#include "GridSet.h"

#include <angelscript.h>

#include <Magma/Script/ScriptEngine.h>

using namespace Magma::Script;

namespace Lava {

GridSet::GridSet(uint32_t width, uint32_t height)
	: m_Width(width), m_Height(height)
{
	if(width && height)
		m_Data = new uint8_t[width * height];
}

GridSet::~GridSet() {
	delete[] m_Data;
}

uint8_t* GridSet::At(uint32_t x, uint32_t y) const {
	VOLCANICORE_ASSERT(m_Data);
	VOLCANICORE_ASSERT(x < m_Width && y < m_Height);
	return m_Data + y * m_Width + x;
}


void GridSet::ResizeX(uint32_t width) {

}

void GridSet::ResizeY(uint32_t height) {

}

void GridSet::Clear() {
	memset(m_Data, 0, GetCount() * sizeof(uint8_t));
}

uint32_t GridSet::GetWidth() const {
	return m_Width;
}

uint32_t GridSet::GetHeight() const {
	return m_Height;
}

uint32_t GridSet::GetCount() const {
	return m_Width * m_Height;
}

static void GridSetDefaultCtor(GridSet* ptr) {
	new(ptr) GridSet();
}

static void GridSetDimensionCtor(uint32_t w, uint32_t h, GridSet* ptr) {
	new(ptr) GridSet(w, h);
}

static void GridSetDestruct(GridSet* ptr) {
	ptr->~GridSet();
}

void GridSet::RegisterInterface() {
	auto* engine = ScriptEngine::Get();

	engine->RegisterObjectType("GridSet", sizeof(GridSet),
		asOBJ_VALUE | asOBJ_APP_CLASS_ALLINTS
		| asOBJ_APP_CLASS_MORE_CONSTRUCTORS | asGetTypeTraits<GridSet>());
	engine->RegisterObjectBehaviour("GridSet", asBEHAVE_CONSTRUCT,
		"void f()", asFUNCTION(GridSetDefaultCtor), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("GridSet", asBEHAVE_CONSTRUCT,
		"void f(uint32, uint32)", asFUNCTION(GridSetDimensionCtor),
		asCALL_CDECL_OBJLAST);
	engine->RegisterObjectBehaviour("GridSet", asBEHAVE_DESTRUCT,
		"void f()", asFUNCTION(GridSetDestruct), asCALL_CDECL_OBJLAST);

	engine->RegisterObjectMethod("GridSet", "uint32 get_Width() const property",
		asMETHOD(GridSet, GetWidth), asCALL_THISCALL);
	engine->RegisterObjectMethod("GridSet", "uint32 get_Height() const property",
		asMETHOD(GridSet, GetHeight), asCALL_THISCALL);
	engine->RegisterObjectMethod("GridSet", "uint32 get_Count() const property",
		asMETHOD(GridSet, GetCount), asCALL_THISCALL);
}

}