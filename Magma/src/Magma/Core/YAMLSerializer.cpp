#include "YAMLSerializer.h"

#include <fstream>

namespace YAML {

YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec2& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec3& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec4& v) {
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
	return out;
}

}

namespace Magma {

Serializer& YAMLSerializer::SetOptions(Serializer::Options options) {
	if(options == Serializer::Options::ArrayOneLine)
		m_Out << YAML::Flow;
	return *this;
}

YAMLSerializer& YAMLSerializer::BeginSequence() {
	m_Out << YAML::BeginSeq;
	return *this;
}

YAMLSerializer& YAMLSerializer::EndSequence() {
	m_Out << YAML::EndSeq;
	return *this;
}

YAMLSerializer& YAMLSerializer::BeginMapping() {
	m_Out << YAML::BeginMap;
	return *this;
}

YAMLSerializer& YAMLSerializer::EndMapping() {
	m_Out << YAML::EndMap;
	return *this;
}

Serializer& YAMLSerializer::WriteKey(const std::string& name) {
	m_Out << YAML::Key << name;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(uint32_t value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(int32_t value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(uint64_t value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(int64_t value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(bool value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(float value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(const glm::vec2& value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(const glm::vec3& value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(const glm::vec4& value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(const std::string& value) {
	m_Out << YAML::Value << value;
	return *this;
}

void YAMLSerializer::Finalize(const std::string& path) {
	std::ofstream fout(path);
	fout << m_Out.c_str();
}

}