#include "JSONSerializer.h"

#include <fstream>

using namespace rapidjson;

namespace VolcaniCore {

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
	m_Writer.StartObject();
	return *this;
}

JSONSerializer& JSONSerializer::EndMapping() {
	m_Writer.EndObject();
	return *this;
}

JSONSerializer& JSONSerializer::WriteKey(const std::string& name) {
	m_Writer.String(name);
	return *this;
}

JSONSerializer& JSONSerializer::Write(uint32_t value) {
	m_Writer.Uint64(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(int32_t value) {
	m_Writer.Int(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(uint64_t value) {
	m_Writer.Uint64(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(int64_t value) {
	m_Writer.Int(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(float value) {
	m_Writer.Double(value);
	return *this;
}

JSONSerializer& JSONSerializer::Write(const glm::vec2& value) {

	return *this;
}

JSONSerializer& JSONSerializer::Write(const glm::vec3& value) {

	return *this;
}

JSONSerializer& JSONSerializer::Write(const glm::vec4& value) {

	return *this;
}

JSONSerializer& JSONSerializer::Write(const std::string& value) {

	return *this;
}

void JSONSerializer::Finalize(const std::string& path) {
	std::ofstream fout(path);
	fout << m_Buffer.GetString();
}

}
