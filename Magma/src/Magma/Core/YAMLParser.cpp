#include "YAMLParser.h"

#include <VolcaniCore/Core/Assert.h>

namespace Magma {

YAMLParserNode::YAMLParserNode(YAML::Node node)
	: m_Node(node) { }

// YAMLParserNode YAMLParserNode::operator [](const std::string& node) {
// 	return YAMLParserNode(m_Node[node.c_str()]);
// }

bool YAMLParserNode::IsValid() {
	return bool(m_Node);
}

YAMLParser::YAMLParser(const std::string& path) {
	YAML::Node file;
	try {
		file = YAML::LoadFile(path);
	}
	catch(YAML::ParserException e) {
		VOLCANICORE_ASSERT_ARGS(false, "Could not load file {0}: {1}",
								path, e.what());
	}

	// m_Root = YAMLParserNode(file);
}

}
