#include "GridSet.h"

#include <angelscript.h>

#include <Magma/Script/ScriptEngine.h>

using namespace Magma::Script;

namespace Lava {

GridSet::GridSet(uint32_t width, uint32_t height)
	: m_Width(width), m_Height(height)
{
	if(width && height)
		m_Data = new uint8_t[GetCount()];
}

GridSet::~GridSet() {
	delete[] m_Data;
}

GridSet& GridSet::operator =(const GridSet& other) {
	m_Width = other.m_Width;
	m_Height = other.m_Height;
	if(!m_Data && other.m_Data)
		m_Data = new uint8_t[GetCount()];
	if(m_Data && !other.m_Data) {
		delete[] m_Data;
		m_Data = nullptr;
	}
	if(m_Data && other.m_Data)
		memcpy(m_Data, other.m_Data, GetCount());
	return *this;
}

uint8_t* GridSet::Get() const {
	return m_Data;
}

uint8_t* GridSet::At(uint32_t x, uint32_t y) const {
	VOLCANICORE_ASSERT(m_Data);
	VOLCANICORE_ASSERT(m_Width && m_Height);
	VOLCANICORE_ASSERT(x < m_Width && y < m_Height);
	return m_Data + y * m_Width + x;
}

void GridSet::Resize(uint32_t width, uint32_t height) {
	Reallocate(width, height);
}

void GridSet::ResizeX(uint32_t width) {
	Reallocate(width, m_Height);
}

void GridSet::ResizeY(uint32_t height) {
	Reallocate(m_Width, height);
}

void GridSet::Clear() {
	if(m_Data)
		memset(m_Data, 0, GetCount());
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

void GridSet::Reallocate(uint32_t width, uint32_t height) {
	if(!(width * height))
		return;

	uint32_t newDim = width * height;
	uint8_t* newData = new uint8_t[newDim];
	memset(newData, 0, newDim);

	if(m_Data) {
		memcpy(newData, m_Data, GetCount());
		delete[] m_Data;
	}

	m_Data = newData;
	m_Width = width;
	m_Height = height;
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

static uint8_t GetData(uint32_t x, uint32_t y, GridSet* ptr) {
	return *ptr->At(x, y);
}

static void SetData(uint32_t x, uint32_t y, uint8_t data, GridSet* ptr) {
	*ptr->At(x, y) = data;
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

	engine->RegisterObjectMethod("GridSet", "uint32 get_Count() const property",
		asMETHOD(GridSet, GetCount), asCALL_THISCALL);
	engine->RegisterObjectMethod("GridSet", "uint32 get_Width() const property",
		asMETHOD(GridSet, GetWidth), asCALL_THISCALL);
	engine->RegisterObjectMethod("GridSet", "uint32 get_Height() const property",
		asMETHOD(GridSet, GetHeight), asCALL_THISCALL);
	engine->RegisterObjectMethod("GridSet", "void set_Width(uint32) property",
		asMETHOD(GridSet, ResizeX), asCALL_THISCALL);
	engine->RegisterObjectMethod("GridSet", "void set_Height(uint32) property",
		asMETHOD(GridSet, ResizeY), asCALL_THISCALL);
	engine->RegisterObjectMethod("GridSet", "uint8 At(uint32, uint32)",
		asFUNCTION(GetData), asCALL_CDECL_OBJLAST);
	engine->RegisterObjectMethod("GridSet", "void Set(uint32, uint32, uint8)",
		asFUNCTION(SetData), asCALL_CDECL_OBJLAST);
}

}