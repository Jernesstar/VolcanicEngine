#include "YAMLSerializer.h"

#include <fstream>

namespace VolcaniCore {

void YAMLSerializer::Finalize(const std::string& path) {
	std::ofstream fout(path);
	fout << m_Out.c_str();
}

}