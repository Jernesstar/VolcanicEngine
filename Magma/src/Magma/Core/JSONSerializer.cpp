#include "JSONSerializer.h"

#include <fstream>

using namespace rapidjson;

namespace Magma {

Serializer& JSONSerializer::SetOptions(Serializer::Options options) {
	if(options == Serializer::Options::ArrayOneLine)
		m_Writer.SetFormatOptions(kFormatSingleLineArray);
	return *this;
}

JSONSerializer::JSONSerializer() {
	m_Writer.Reset(m_Buffer);
}

JSONSerializer& JSONSerializer::BeginSequence() {
	m_Writer.StartArray();
	return *this;
}

JSONSerializer& JSONSerializer::EndSequence() {
	m_Writer.EndArray();
	return *this;
}

JSONSerializer& JSONSerializer::BeginMapping() {
	m_Writer.SetFormatOptions(kFormatDefault);
	m_Writer.StartObject();
	return *this;
}

JSONSerializer& JSONSerializer::EndMapping() {
	m_Writer.EndObject();
	return *this;
}

Serializer& JSONSerializer::WriteKey(const std::string& name) {
	m_Writer.String(name);
	return *this;
}

JSONSerializer& JSONSerializer::Write(uint32_t value) {
	m_Writer.Uint64(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(int32_t value) {
	m_Writer.Int64(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(uint64_t value) {
	m_Writer.Uint64(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(int64_t value) {
	m_Writer.Int64(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(bool value) {
	m_Writer.Bool(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(float value) {
	m_Writer.Double(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(const glm::vec2& value) {
	m_Writer.SetFormatOptions(kFormatSingleLineArray);
	m_Writer.StartArray();
	m_Writer.Double(value.x);
	m_Writer.Double(value.y);
	m_Writer.EndArray();
	return *this;
}

JSONSerializer& JSONSerializer::Write(const glm::vec3& value) {
	m_Writer.SetFormatOptions(kFormatSingleLineArray);
	m_Writer.StartArray();
	m_Writer.Double(value.x);
	m_Writer.Double(value.y);
	m_Writer.Double(value.z);
	m_Writer.EndArray();
	return *this;
}

JSONSerializer& JSONSerializer::Write(const glm::vec4& value) {
	m_Writer.SetFormatOptions(kFormatSingleLineArray);
	m_Writer.StartArray();
	m_Writer.Double(value.x);
	m_Writer.Double(value.y);
	m_Writer.Double(value.z);
	m_Writer.Double(value.w);
	m_Writer.EndArray();
	return *this;
}

JSONSerializer& JSONSerializer::Write(const char* value) {
	m_Writer.String(value);
	return *this;
}

void JSONSerializer::Finalize(const std::string& path) {
	std::ofstream fout(path);
	fout << m_Buffer.GetString();
}

}
