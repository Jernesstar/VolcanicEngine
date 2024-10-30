#include "JSONSerializer.h"

#include <fstream>

namespace VolcaniCore {

JSONSerializer& JSONSerializer::Write(int32_t value) {

	return *this;
}

void JSONSerializer::Finalize(const std::string& path) {

}

}