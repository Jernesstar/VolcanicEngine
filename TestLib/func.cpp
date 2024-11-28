#include <cstdint>

#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
	#define EXPORT __declspec(dllexport)
	#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
	#define EXPORT __attribute__((visibility("default")))
	#define IMPORT
#else
	#define EXPORT
	#define IMPORT
	#pragma warning Unknown dynamic link import/export semantics.
#endif

namespace TestLib {

extern "C" EXPORT uint32_t Add(uint32_t x, uint32_t y) {
	return x + y;
}

}