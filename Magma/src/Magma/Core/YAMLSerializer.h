#pragma once

#include <glm/gtc/matrix_access.hpp>

#include "Serializer.h"

#define YAML_CPP_STATIC_DEFINE
#include <yaml-cpp/yaml.h>

namespace Magma {

class YAMLSerializer : public Serializer {
public:
	YAMLSerializer() = default;
	~YAMLSerializer() = default;

	Serializer& SetOptions(Serializer::Options options) override;

	YAMLSerializer& BeginSequence() override;
	YAMLSerializer& EndSequence() override;
	YAMLSerializer& BeginMapping() override;
	YAMLSerializer& EndMapping() override;

	Serializer& WriteKey(const std::string& name) override;

	YAMLSerializer& Write(uint32_t value) override;
	YAMLSerializer& Write(int32_t value) override;
	YAMLSerializer& Write(uint64_t value) override;
	YAMLSerializer& Write(int64_t value) override;

	YAMLSerializer& Write(bool value) override;
	YAMLSerializer& Write(float value) override;

	YAMLSerializer& Write(const glm::vec2& value) override;
	YAMLSerializer& Write(const glm::vec3& value) override;
	YAMLSerializer& Write(const glm::vec4& value) override;

	YAMLSerializer& Write(const std::string& value) override;

	void Finalize(const std::string& path) override;

private:
	YAML::Emitter m_Out;
};

}


namespace YAML {

template<typename T>
struct convert<List<T>> {
	static Node encode(const List<T>& list) {
		Node node;
		for(auto& val : list)
			node.push_back(val);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, List<T>& v) {
		if(!node.IsSequence())
			return false;
		for(uint64_t i = 0; i < node.size(); i++)
			v.Add(node[i].as<T>());
		return true;
	}
};

template<>
struct convert<glm::vec2> {
	static Node encode(const glm::vec2& v) {
		Node node;
		node.push_back(v.x);
		node.push_back(v.y);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec2& v) {
		if(!node.IsSequence() || node.size() != 2)
			return false;

		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		return true;
	}
};

template<>
struct convert<glm::vec3> {
	static Node encode(const glm::vec3& v) {
		Node node;
		node.push_back(v.x);
		node.push_back(v.y);
		node.push_back(v.z);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec3& v) {
		if(!node.IsSequence() || node.size() != 3)
			return false;

		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		v.z = node[2].as<float>();
		return true;
	}
};

template<>
struct convert<glm::vec4> {
	static Node encode(const glm::vec4& v) {
		Node node;
		node.push_back(v.x);
		node.push_back(v.y);
		node.push_back(v.z);
		node.push_back(v.w);
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::vec4& v) {
		if(!node.IsSequence() || node.size() != 4)
			return false;

		v.x = node[0].as<float>();
		v.y = node[1].as<float>();
		v.z = node[2].as<float>();
		v.w = node[3].as<float>();
		return true;
	}
};

template<>
struct convert<glm::mat2> {
	static Node encode(const glm::mat2& mat) {
		Node node;
		node.push_back(glm::row(mat, 0));
		node.push_back(glm::row(mat, 1));
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::mat2& mat) {
		if(!node.IsSequence() || node.size() != 2)
			return false;

		mat = glm::row(mat, 0, node[0].as<glm::vec2>());
		mat = glm::row(mat, 1, node[1].as<glm::vec2>());
		return true;
	}
};

template<>
struct convert<glm::mat3> {
	static Node encode(const glm::mat3& mat) {
		Node node;
		node.push_back(glm::row(mat, 0));
		node.push_back(glm::row(mat, 1));
		node.push_back(glm::row(mat, 2));
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::mat3& mat) {
		if(!node.IsSequence() || node.size() != 3)
			return false;

		mat = glm::row(mat, 0, node[0].as<glm::vec3>());
		mat = glm::row(mat, 1, node[1].as<glm::vec3>());
		mat = glm::row(mat, 2, node[2].as<glm::vec3>());
		return true;
	}
};

template<>
struct convert<glm::mat4> {
	static Node encode(const glm::mat4& mat) {
		Node node;
		node.push_back(glm::row(mat, 0));
		node.push_back(glm::row(mat, 1));
		node.push_back(glm::row(mat, 2));
		node.push_back(glm::row(mat, 3));
		node.SetStyle(EmitterStyle::Flow);
		return node;
	}

	static bool decode(const Node& node, glm::mat4& mat) {
		if(!node.IsSequence() || node.size() != 4)
			return false;

		mat = glm::row(mat, 0, node[0].as<glm::vec4>());
		mat = glm::row(mat, 1, node[1].as<glm::vec4>());
		mat = glm::row(mat, 2, node[2].as<glm::vec4>());
		mat = glm::row(mat, 3, node[2].as<glm::vec4>());
		return true;
	}
};

}