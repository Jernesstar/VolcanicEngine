#include "Parser.h"

namespace Magma {

template<>
int64_t Parser::As() {
	return GetInt();
}

template<>
uint64_t Parser::As() {
	return GetUInt();
}

template<>
float Parser::As() {
	return GetFloat();
}

}