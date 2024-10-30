#include "YAMLSerializer.h"

#include <fstream>

namespace YAML {

// YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec2& v) {
// 	out << YAML::Flow;
// 	out << YAML::BeginSeq << v.x << v.y << YAML::EndSeq;
// 	return out;
// }

// YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec3& v) {
// 	out << YAML::Flow;
// 	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
// 	return out;
// }

// YAML::Emitter& operator <<(YAML::Emitter& out, const glm::vec4& v) {
// 	out << YAML::Flow;
// 	out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
// 	return out;
// }

}

namespace VolcaniCore {

Serializer& YAMLSerializer::BeginSequence() {
	m_Out << YAML::BeginSeq;
	return *this;
}

Serializer& YAMLSerializer::EndSequence() {
	m_Out << YAML::EndSeq;
	return *this;
}

Serializer& YAMLSerializer::BeginMapping() {
	m_Out << YAML::BeginMap;
	return *this;
}

Serializer& YAMLSerializer::EndMapping() {
	m_Out << YAML::EndMap;
	return *this;
}

YAMLSerializer& YAMLSerializer::WriteKey(const std::string& name) {
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

YAMLSerializer& YAMLSerializer::Write(float value) {
	m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(const glm::vec2& value) {
	// m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(const glm::vec3& value) {
	// m_Out << YAML::Value << value;
	return *this;
}

YAMLSerializer& YAMLSerializer::Write(const glm::vec4& value) {
	// m_Out << YAML::Value << value;
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

namespace YAML {

// template<>
// struct convert<glm::vec2> {
// 	static Node encode(const glm::vec2& v) {
// 		Node node;
// 		node.push_back(v.x);
// 		node.push_back(v.y);
// 		node.SetStyle(EmitterStyle::Flow);
// 		return node;
// 	}

// 	static bool decode(const Node& node, glm::vec2& v) {
// 		if(!node.IsSequence() || node.size() != 3)
// 			return false;

// 		v.x = node[0].as<float>();
// 		v.y = node[1].as<float>();
// 		return true;
// 	}
// };

// template<>
// struct convert<glm::vec3> {
// 	static Node encode(const glm::vec3& v) {
// 		Node node;
// 		node.push_back(v.x);
// 		node.push_back(v.y);
// 		node.push_back(v.z);
// 		node.SetStyle(EmitterStyle::Flow);
// 		return node;
// 	}

// 	static bool decode(const Node& node, glm::vec3& v) {
// 		if(!node.IsSequence() || node.size() != 3)
// 			return false;

// 		v.x = node[0].as<float>();
// 		v.y = node[1].as<float>();
// 		v.z = node[2].as<float>();
// 		return true;
// 	}
// };

// template<>
// struct convert<glm::vec4> {
// 	static Node encode(const glm::vec4& v) {
// 		Node node;
// 		node.push_back(v.x);
// 		node.push_back(v.y);
// 		node.push_back(v.z);
// 		node.push_back(v.w);
// 		node.SetStyle(EmitterStyle::Flow);
// 		return node;
// 	}

// 	static bool decode(const Node& node, glm::vec4& v) {
// 		if(!node.IsSequence() || node.size() != 4)
// 			return false;

// 		v.x = node[0].as<float>();
// 		v.y = node[1].as<float>();
// 		v.z = node[2].as<float>();
// 		v.w = node[3].as<float>();
// 		return true;
// 	}
// };

}